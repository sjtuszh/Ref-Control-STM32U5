#ifndef REF_CONTROL_STM32U5_APP_DATA_HPP
#define REF_CONTROL_STM32U5_APP_DATA_HPP

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_TSC_CHANNEL_COUNT 8U

typedef struct {
    uint32_t raw;
    uint32_t baseline;
    int32_t delta;
    uint8_t touched;
} AppTscChannelData;

typedef struct {
    uint32_t scan_counter;
    uint32_t touched_count;
    AppTscChannelData channels[APP_TSC_CHANNEL_COUNT];
} AppTscSnapshot;

void AppDataInit(void);
void AppDataSetTscSnapshot(const AppTscSnapshot* snapshot);
void AppDataGetTscSnapshot(AppTscSnapshot* snapshot);

#ifdef __cplusplus
}
#endif

#endif // REF_CONTROL_STM32U5_APP_DATA_HPP
