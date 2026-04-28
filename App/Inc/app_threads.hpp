// ####### 应用线程头文件 #######
// 这里声明各个业务线程的主函数入口。

#ifndef REF_CONTROL_STM32U5_APP_THREADS_HPP
#define REF_CONTROL_STM32U5_APP_THREADS_HPP

#ifdef __cplusplus
extern "C" {
#endif

void OledThreadMain(void);
void LedThreadMain(void);

#ifdef __cplusplus
}
#endif

#endif // REF_CONTROL_STM32U5_APP_THREADS_HPP
