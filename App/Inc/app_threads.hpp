#ifndef REF_CONTROL_STM32U5_APP_THREADS_HPP
#define REF_CONTROL_STM32U5_APP_THREADS_HPP

#ifdef __cplusplus
extern "C" {
#endif

void OledThreadMain(void);
void LedThreadMain(void);
void SensorThreadMain(void);
void KeyThreadMain(void);

#ifdef __cplusplus
}
#endif

#endif // REF_CONTROL_STM32U5_APP_THREADS_HPP
