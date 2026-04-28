/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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
#include "ux_device_cdc_acm.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN PV */
static UX_SLAVE_CLASS_CDC_ACM* g_cdc_acm = UX_NULL;
static volatile UINT g_cdc_acm_ready = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  USBD_CDC_ACM_Activate
  *         This function is called when insertion of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Activate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Activate */
  g_cdc_acm = (UX_SLAVE_CLASS_CDC_ACM*)cdc_acm_instance;
  g_cdc_acm_ready = 1U;
  /* USER CODE END USBD_CDC_ACM_Activate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_Deactivate
  *         This function is called when extraction of a CDC ACM device.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_Deactivate(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_Deactivate */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  g_cdc_acm = UX_NULL;
  g_cdc_acm_ready = 0U;
  /* USER CODE END USBD_CDC_ACM_Deactivate */

  return;
}

/**
  * @brief  USBD_CDC_ACM_ParameterChange
  *         This function is invoked to manage the CDC ACM class requests.
  * @param  cdc_acm_instance: Pointer to the cdc acm class instance.
  * @retval none
  */
VOID USBD_CDC_ACM_ParameterChange(VOID *cdc_acm_instance)
{
  /* USER CODE BEGIN USBD_CDC_ACM_ParameterChange */
  UX_PARAMETER_NOT_USED(cdc_acm_instance);
  /* USER CODE END USBD_CDC_ACM_ParameterChange */

  return;
}

/* USER CODE BEGIN 1 */

UINT UsbCdcAcmIsReady(void)
{
  return ((g_cdc_acm != UX_NULL) && (g_cdc_acm_ready != 0U)) ? 1U : 0U;
}

void UsbCdcAcmWrite(const char* text)
{
  ULONG actual_length = 0U;
  ULONG length = 0U;
  const char* ptr = text;

  if ((g_cdc_acm == UX_NULL) || (text == UX_NULL))
  {
    return;
  }

  while (*ptr != '\0')
  {
    ++length;
    ++ptr;
  }

  if (length == 0U)
  {
    return;
  }

  (void)ux_device_class_cdc_acm_write(g_cdc_acm, (UCHAR*)text, length, &actual_length);
}

/* USER CODE END 1 */
