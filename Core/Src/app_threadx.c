/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_data.hpp"
#include "app_threads.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD tx_app_thread;
/* USER CODE BEGIN PV */
TX_THREAD tx_oled_thread;
TX_THREAD tx_led_thread;
TX_THREAD tx_sensor_thread;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */
  CHAR *oled_thread_pointer;
  CHAR *led_thread_pointer;
  CHAR *sensor_thread_pointer;

  /* USER CODE END App_ThreadX_MEM_POOL */
  CHAR *pointer;

  /* Allocate the stack for tx app thread  */
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* Create tx app thread.  */
  if (tx_thread_create(&tx_app_thread, "tx app thread", tx_app_thread_entry, 0, pointer,
                       TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                       TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN App_ThreadX_Init */
  AppDataInit();

  if (tx_byte_allocate(byte_pool, (VOID**) &oled_thread_pointer,
                       APP_OLED_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  if (tx_thread_create(&tx_oled_thread, "oled refresh thread", tx_oled_thread_entry, 0, oled_thread_pointer,
                       APP_OLED_THREAD_STACK_SIZE, APP_OLED_THREAD_PRIORITY, APP_OLED_THREAD_PREEMPTION_THRESHOLD,
                       APP_OLED_THREAD_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  if (tx_byte_allocate(byte_pool, (VOID**) &led_thread_pointer,
                       APP_LED_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  if (tx_thread_create(&tx_led_thread, "led status thread", tx_led_thread_entry, 0, led_thread_pointer,
                       APP_LED_THREAD_STACK_SIZE, APP_LED_THREAD_PRIORITY, APP_LED_THREAD_PREEMPTION_THRESHOLD,
                       APP_LED_THREAD_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  if (tx_byte_allocate(byte_pool, (VOID**) &sensor_thread_pointer,
                       APP_SENSOR_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  if (tx_thread_create(&tx_sensor_thread, "sensor thread", tx_sensor_thread_entry, 0, sensor_thread_pointer,
                       APP_SENSOR_THREAD_STACK_SIZE, APP_SENSOR_THREAD_PRIORITY,
                       APP_SENSOR_THREAD_PREEMPTION_THRESHOLD, APP_SENSOR_THREAD_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  /* USER CODE END App_ThreadX_Init */

  return ret;
}
/**
  * @brief  Function implementing the tx_app_thread_entry thread.
  * @param  thread_input: Hardcoded to 0.
  * @retval None
  */
void tx_app_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_app_thread_entry */
  (void)thread_input;

  // 预留一个总控线程入口，后续可用于事件分发、健康监测或线程间协调。
  while (1)
  {
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
  }
  /* USER CODE END tx_app_thread_entry */
}

void tx_oled_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_oled_thread_entry */
  (void)thread_input;
  OledThreadMain();
  /* USER CODE END tx_oled_thread_entry */
}

void tx_led_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_led_thread_entry */
  (void)thread_input;
  LedThreadMain();
  /* USER CODE END tx_led_thread_entry */
}

void tx_sensor_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_sensor_thread_entry */
  (void)thread_input;
  SensorThreadMain();
  /* USER CODE END tx_sensor_thread_entry */
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */

  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
