/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MyLcdLib.h"
#include <stdio.h>
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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



void my_delay_us(uint16_t us);
void setPinToGPIO_output(GPIO_TypeDef *port, uint16_t pin);

void my_delay_us(uint16_t us) {
	HAL_TIM_Base_Start(&htim2);
	
	htim2.Instance->CNT = 0;
	while(htim2.Instance->CNT < us) {
		// waiting
	}
	
	HAL_TIM_Base_Stop(&htim2);
}

void setPinToGPIO_output(GPIO_TypeDef *port, uint16_t pin) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin*/
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void setPinToGPIO_input(GPIO_TypeDef *port, uint16_t pin) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin*/
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void DHT11_Start1() {
  setPinToGPIO_output(GPIOB, GPIO_PIN_5);
	
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0); HAL_Delay(20);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);

  setPinToGPIO_input(GPIOB, GPIO_PIN_5);

  // ============================
  // chua co thuat toan check loi
  // ============================

  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) { } // doi = 0
  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) { } // doi = 1
  while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) { } // doi = 0

  // ============================
  // 5bytes (40bits bat dau duoc truyen)
  // ============================
  
}

uint8_t DHT11_Start(void) {
    uint16_t timeout = 0;

    setPinToGPIO_output(GPIOB, GPIO_PIN_5);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0); 
    HAL_Delay(20);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);

    setPinToGPIO_input(GPIOB, GPIO_PIN_5);

    // Ch? chân PB5 v? 0 (Timeout 100us)
    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) {
        my_delay_us(1);
        if(++timeout > 100) return 0; // Thoát ngay n?u b? k?t
    }

    // Ch? chân PB5 lên 1 (Timeout 100us)
    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
        my_delay_us(1);
        if(++timeout > 100) return 0; // Thoát ngay n?u b? k?t
    }

    // Ch? chân PB5 v? 0 d? b?t d?u nh?n d? li?u (Timeout 100us)
    timeout = 0;
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) {
        my_delay_us(1);
        if(++timeout > 100) return 0; // Thoát ngay n?u b? k?t
    }

    return 1; // Kh?i t?o thành công
}

uint8_t DHT11_Read_byte1() {
  uint8_t res = 0;

  for(int i = 0; i < 8; ++i) {
    while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {};
    my_delay_us(50);
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) {
      // bit 1
      res = (res << 1) | (1 << 0);
			while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) {};
    } else {
      // bit 0
      res = (res << 1) & ~(1 << 0);
    }
  }

  return res;
}

uint8_t DHT11_Read_byte(void) {
    uint8_t res = 0;

    for(int i = 0; i < 8; ++i) {
        uint16_t timeout = 0;

        // Ch? tín hi?u lên 1
        while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
            my_delay_us(1);
            if(++timeout > 100) break;
        }

        my_delay_us(50);

        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) {
            res = (res << 1) | 1;
            
            // Ch? tín hi?u v? 0
            timeout = 0;
            while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1) {
                my_delay_us(1);
                if(++timeout > 100) break;
            }
        } else {
            res = (res << 1);
        }
    }

    return res;
}

volatile uint8_t int_Tem = 0
          , int_Hum = 0
          , float_Tem = 0
          , float_Hum = 0
          , checkSum = 0;

void DHT11_Handler1() {
  DHT11_Start();

  // Data format: 
  //          8bit integral RH data 
  //        + 8bit decimal RH data 
  //        + 8bit integral T data 
  //        + 8bit decimal T data 
  //        + 8bit check sum. 
  
  int_Hum   = DHT11_Read_byte();
  float_Hum = DHT11_Read_byte();
  int_Tem   = DHT11_Read_byte();
  float_Tem = DHT11_Read_byte();
  checkSum  = DHT11_Read_byte();

}

void DHT11_Handler(void) {
    // Ch? d?c d? li?u khi DHT11 ph?n h?i thành công (tr? v? 1)
    if (DHT11_Start() == 1) {
        int_Hum   = DHT11_Read_byte();
        float_Hum = DHT11_Read_byte();
        int_Tem   = DHT11_Read_byte();
        float_Tem = DHT11_Read_byte();
        checkSum  = DHT11_Read_byte();
    }
}


// Ham xu ly ngat tu dong khi TIM3 dem het thoi gian (ARR)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        // Viet code thuc thi dinh ky khi ngat tai day
        // Vi du: Dao trang thai 1 chan GPIO hoac tang bien dem
    }
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	
	// Bat Ngat Timer 3
  HAL_TIM_Base_Start_IT(&htim3);
	
	
	LCD_Config_t myLcd = {
    {GPIOA, GPIO_PIN_0},
    {GPIOA, GPIO_PIN_1},
    {GPIOA, GPIO_PIN_2},
    {GPIOA, GPIO_PIN_8},
    {GPIOA, GPIO_PIN_9},
    {GPIOA, GPIO_PIN_10},
    {GPIOA, GPIO_PIN_11},
    {GPIOA, GPIO_PIN_12},
    {GPIOB, GPIO_PIN_0},
    {GPIOB, GPIO_PIN_1},
    {GPIOB, GPIO_PIN_10}
  };
	
	LCD_init(&myLcd);
	
	
	char ndoStr[16];
	char dAmStr[16];
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		DHT11_Handler();
		
		sprintf(ndoStr, "NDo: %u.%u C  ", int_Tem, float_Tem);
    sprintf(dAmStr, "DAm: %u.%u %%  ", int_Hum, float_Hum);
		
		LCD_SetCursor(&myLcd, 0, 0);
    LCD_Print(&myLcd, ndoStr);
		
		LCD_SetCursor(&myLcd, 1, 0);
    LCD_Print(&myLcd, dAmStr);
		
		HAL_Delay(1000);
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 63;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 63999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA8
                           PA9 PA10 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
