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
#include "adc.h"
#include "gpio.h"

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
/* 8 个 LED 对应的 GPIO 引脚号 (复用 CubeMX 在 main.h 中生成的宏) */
const uint16_t LED_Pins[8] = {
  LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin,
  LED5_Pin, LED6_Pin, LED7_Pin, LED8_Pin
};
/* 8 个 LED 对应的 GPIO 端口 */
GPIO_TypeDef* LED_Ports[8] = {
  LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port,
  LED5_GPIO_Port, LED6_GPIO_Port, LED7_GPIO_Port, LED8_GPIO_Port
};
uint8_t current_led = 0;   /* 当前点亮的 LED 编号 0-7 */
uint8_t direction = 0;     /* 流水方向: 0=正向, 1=反向 */
uint8_t last_btn = 1;      /* 按键上一次状态(内部上拉, 未按=1) */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_AllOff(void);
void LED_On(uint8_t index);
uint32_t Get_Delay(void);
uint8_t Key_Scan(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 关闭所有 LED */
void LED_AllOff(void)
{
  for (int i = 0; i < 8; i++)
  {
    HAL_GPIO_WritePin(LED_Ports[i], LED_Pins[i], GPIO_PIN_RESET);
  }
}

/* 点亮指定编号的 LED (0-7), 其余熄灭 */
void LED_On(uint8_t index)
{
  LED_AllOff();
  HAL_GPIO_WritePin(LED_Ports[index], LED_Pins[index], GPIO_PIN_SET);
}

/* 读取电位器, 返回延时时间(毫秒), 范围 50-500ms */
uint32_t Get_Delay(void)
{
  HAL_ADC_Start(&hadc1);                          /* 启动 ADC 转换 */
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); /* 等待转换完成 */
  uint32_t val = HAL_ADC_GetValue(&hadc1);        /* 读取结果 0-4095 */
  /* 映射到 50ms - 500ms: 电位器值越大, 延时越长, 流水越慢 */
  return 50 + (val * 450) / 4095;
}

/* 按键扫描(含软件消抖), 返回 1 表示本次按下有效 */
uint8_t Key_Scan(void)
{
  uint8_t btn = HAL_GPIO_ReadPin(DIR_BTN_GPIO_Port, DIR_BTN_Pin);
  if (btn == 0 && last_btn == 1)            /* 检测到下降沿 */
  {
    HAL_Delay(20);                          /* 消抖延时 20ms */
    if (HAL_GPIO_ReadPin(DIR_BTN_GPIO_Port, DIR_BTN_Pin) == 0)
    {
      last_btn = 0;
      return 1;                             /* 确认按下, 返回有效 */
    }
  }
  else if (btn == 1)
  {
    last_btn = 1;                           /* 按键已释放, 恢复状态 */
  }
  return 0;
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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* 1. 扫描按键, 切换流水方向 */
    if (Key_Scan())
    {
      direction = !direction;
    }

    /* 2. 点亮当前 LED */
    LED_On(current_led);

    /* 3. 根据电位器获取延时时间 */
    uint32_t delay_ms = Get_Delay();
    HAL_Delay(delay_ms);

    /* 4. 更新 LED 编号 (到端点循环) */
    if (direction == 0)
    {
      current_led = (current_led + 1) % 8;   /* 正向: 0->1->...->7->0 */
    }
    else
    {
      current_led = (current_led + 7) % 8;   /* 反向: 0->7->6->...->1->0 */
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
