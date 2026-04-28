#include "app_sensor.hpp"
#include "app_data.hpp"
#include "tsc.h"

namespace {

constexpr uint32_t APP_TSC_TOUCH_THRESHOLD = 120U;
constexpr uint32_t APP_TSC_BASELINE_FILTER_SHIFT = 4U;

struct TscChannelConfig {
    uint32_t channel_mask;
    uint32_t group_index;
};

constexpr uint32_t kShieldMask = TSC_GROUP1_IO1;
constexpr uint32_t kSamplingMask = TSC_GROUP1_IO3 | TSC_GROUP2_IO4 | TSC_GROUP3_IO4 | TSC_GROUP4_IO4;

const TscChannelConfig kTscChannels[APP_TSC_CHANNEL_COUNT] = {
    {TSC_GROUP2_IO1, TSC_GROUP2_IDX},
    {TSC_GROUP2_IO2, TSC_GROUP2_IDX},
    {TSC_GROUP2_IO3, TSC_GROUP2_IDX},
    {TSC_GROUP3_IO2, TSC_GROUP3_IDX},
    {TSC_GROUP3_IO3, TSC_GROUP3_IDX},
    {TSC_GROUP4_IO1, TSC_GROUP4_IDX},
    {TSC_GROUP4_IO2, TSC_GROUP4_IDX},
    {TSC_GROUP4_IO3, TSC_GROUP4_IDX},
};

AppTscSnapshot g_snapshot = {};
bool g_sensor_initialized = false;

void ShortDelay() {
    for (volatile uint32_t i = 0; i < 200U; ++i) {
        __NOP();
    }
}

uint32_t AcquireSingleChannel(const TscChannelConfig& channel_config) {
    TSC_IOConfigTypeDef io_config = {};
    io_config.ChannelIOs = channel_config.channel_mask;
    io_config.ShieldIOs = kShieldMask;
    io_config.SamplingIOs = kSamplingMask;

    if (HAL_TSC_IODischarge(&htsc, ENABLE) != HAL_OK) {
        return 0U;
    }
    ShortDelay();
    if (HAL_TSC_IODischarge(&htsc, DISABLE) != HAL_OK) {
        return 0U;
    }

    if (HAL_TSC_IOConfig(&htsc, &io_config) != HAL_OK) {
        return 0U;
    }

    if (HAL_TSC_Start(&htsc) != HAL_OK) {
        return 0U;
    }

    if (HAL_TSC_PollForAcquisition(&htsc) != HAL_OK) {
        (void)HAL_TSC_Stop(&htsc);
        return 0U;
    }

    const uint32_t value = HAL_TSC_GroupGetValue(&htsc, channel_config.group_index);
    (void)HAL_TSC_Stop(&htsc);
    return value;
}

void UpdateChannelState(AppTscChannelData& channel_data, uint32_t raw_value) {
    channel_data.raw = raw_value;

    if (channel_data.baseline == 0U) {
        channel_data.baseline = raw_value;
    }

    channel_data.delta = static_cast<int32_t>(raw_value) - static_cast<int32_t>(channel_data.baseline);
    channel_data.touched = (channel_data.delta > static_cast<int32_t>(APP_TSC_TOUCH_THRESHOLD)) ? 1U : 0U;

    if (channel_data.touched == 0U) {
        channel_data.baseline =
            ((channel_data.baseline * ((1UL << APP_TSC_BASELINE_FILTER_SHIFT) - 1UL)) + raw_value) >>
            APP_TSC_BASELINE_FILTER_SHIFT;
        channel_data.delta = static_cast<int32_t>(raw_value) - static_cast<int32_t>(channel_data.baseline);
    }
}

} // namespace

void AppSensorInit() {
    g_snapshot = {};
    g_sensor_initialized = true;
}

void AppSensorUpdate() {
    if (!g_sensor_initialized) {
        AppSensorInit();
    }

    g_snapshot.scan_counter++;
    g_snapshot.touched_count = 0U;

    for (uint32_t i = 0; i < APP_TSC_CHANNEL_COUNT; ++i) {
        const uint32_t raw_value = AcquireSingleChannel(kTscChannels[i]);
        UpdateChannelState(g_snapshot.channels[i], raw_value);

        if (g_snapshot.channels[i].touched != 0U) {
            g_snapshot.touched_count++;
        }
    }

    AppDataSetTscSnapshot(&g_snapshot);
}
