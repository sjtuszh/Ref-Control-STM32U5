#ifndef REF_CONTROL_STM32U5_APP_COMM_HPP
#define REF_CONTROL_STM32U5_APP_COMM_HPP

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void AppCommInit(void);
uint8_t AppUsbCdcIsReady(void);
void AppUsbCdcWrite(const char* text);
void AppUsbLogPrintf(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // REF_CONTROL_STM32U5_APP_COMM_HPP
