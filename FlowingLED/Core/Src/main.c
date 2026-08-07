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
#include "tim.h"

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
volatile uint8_t led_brightness[8] = {0}; /* 软件 PWM 亮度 0-100 */
uint8_t current_led = 0;                  /* 当前渐亮的 LED 编号 0-7 */
uint8_t direction = 0;                    /* 流水方向: 0=正向, 1=反向 */
uint8_t last_btn = 1;                     /* 按键上一次稳定状态 */
uint32_t step_delay_ms = 50;              /* 当前 LED 的保持时间 */
uint32_t last_anim_tick = 0;
uint32_t last_adc_tick = 0;
uint32_t led_hold_start = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SoftwarePwm_Update(void);
void Update_Potentiometer(void);
void Update_Animation(void);
uint8_t Key_Scan(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 软件 PWM 输出，TIM5 每 100us 调用一次 */
void SoftwarePwm_Update(void)
{
  static uint8_t pwm = 0;
  pwm = (pwm >= 99U) ? 0U : (uint8_t)(pwm + 1U);
  for (uint8_t i = 0; i < 8; i++)
  {
    if (pwm < led_brightness[i])
      LED_Ports[i]->BSRR = LED_Pins[i];
    else
      LED_Ports[i]->BSRR = (uint32_t)LED_Pins[i] << 16U;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5)
    SoftwarePwm_Update();
}

void Update_Potentiometer(void)
{
  if (HAL_ADC_Start(&hadc1) == HAL_OK &&
      HAL_ADC_PollForConversion(&hadc1, 2) == HAL_OK)
  {
    uint32_t val = HAL_ADC_GetValue(&hadc1);
    step_delay_ms = 50U + (val * 450U) / 4095U;
  }
  HAL_ADC_Stop(&hadc1);
}

uint8_t Key_Scan(void)
{
  static uint32_t debounce_tick = 0;
  uint32_t now = HAL_GetTick();
  uint8_t btn = (uint8_t)HAL_GPIO_ReadPin(DIR_BTN_GPIO_Port, DIR_BTN_Pin);

  if (btn != last_btn && (now - debounce_tick) >= 20U)
  {
    debounce_tick = now;
    last_btn = btn;
    return (btn == GPIO_PIN_RESET) ? 1U : 0U;
  }
  return 0U;
}

void Update_Animation(void)
{
  uint32_t now = HAL_GetTick();
  if ((now - last_anim_tick) < 5U)
    return;
  last_anim_tick = now;

  for (uint8_t i = 0; i < 8; i++)
  {
    if (i != current_led && led_brightness[i] > 0U)
      led_brightness[i] = (led_brightness[i] > 8U) ?
                          (uint8_t)(led_brightness[i] - 8U) : 0U;
  }

  if (led_brightness[current_led] < 100U)
  {
    led_brightness[current_led] =
        (led_brightness[current_led] <= 90U) ?
        (uint8_t)(led_brightness[current_led] + 10U) : 100U;
    led_hold_start = now;
  }
  else if ((now - led_hold_start) >= step_delay_ms)
  {
    current_led = direction ? (uint8_t)((current_led + 7U) % 8U) :
                               (uint8_t)((current_led + 1U) % 8U);
    led_brightness[current_led] = 0U;
    led_hold_start = now;
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
  MX_ADC1_Init();
  MX_TIM5_Init();
  if (HAL_TIM_Base_Start_IT(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (Key_Scan())
    {
      direction = !direction;
    }

    if ((HAL_GetTick() - last_adc_tick) >= 20U)
    {
      last_adc_tick = HAL_GetTick();
      Update_Potentiometer();
    }
    Update_Animation();
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
