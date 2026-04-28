#include "app_data.hpp"
#include "tx_api.h"

namespace {

TX_MUTEX g_app_data_mutex;
bool g_app_data_initialized = false;
AppTscSnapshot g_tsc_snapshot = {};

} // namespace

void AppDataInit() {
    if (g_app_data_initialized) {
        return;
    }

    if (tx_mutex_create(&g_app_data_mutex, const_cast<char*>("app_data_mutex"), TX_NO_INHERIT) == TX_SUCCESS) {
        g_app_data_initialized = true;
    }
}

void AppDataSetTscSnapshot(const AppTscSnapshot* snapshot) {
    if (!g_app_data_initialized || snapshot == nullptr) {
        return;
    }

    if (tx_mutex_get(&g_app_data_mutex, TX_WAIT_FOREVER) == TX_SUCCESS) {
        g_tsc_snapshot = *snapshot;
        tx_mutex_put(&g_app_data_mutex);
    }
}

void AppDataGetTscSnapshot(AppTscSnapshot* snapshot) {
    if (!g_app_data_initialized || snapshot == nullptr) {
        return;
    }

    if (tx_mutex_get(&g_app_data_mutex, TX_WAIT_FOREVER) == TX_SUCCESS) {
        *snapshot = g_tsc_snapshot;
        tx_mutex_put(&g_app_data_mutex);
    } else {
        *snapshot = {};
    }
}
