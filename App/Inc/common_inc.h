//
// Created by 22320 on 2026/4/8.
//

#ifndef REF_CONTROL_STM32U5_COMMON_INC_H
#define REF_CONTROL_STM32U5_COMMON_INC_H

#ifdef __cplusplus
extern "C"{
#endif

// ####### 应用线程入口函数声明 #######
// 这些函数由 ThreadX 线程入口调用，用于拆分不同业务职责。
void OledThreadMain(void);
void LedThreadMain(void);


#ifdef __cplusplus
}

#endif

#endif //REF_CONTROL_STM32U5_COMMON_INC_H
