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
#include "dma.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "app.h"
#include "reader.h"
#include "parser.h"
#include "encoder.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 64 // Must be several MAX_CMD_LENGTH
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t dmaBuf[RX_BUFFER_SIZE];
static uint8_t readBuf[RX_BUFFER_SIZE];
static volatile uint16_t rxSize = 0;
static volatile bool rxDataReady = false;
static volatile bool uart_hdmarx_locked = false;
static volatile bool need_rearm = false;
static uint16_t rearm_retries = 0;
static const uint16_t max_rearm_retries = 100;
static const char * const uart_rearm_stuck = "UART DMA rearm stuck\n";
static int32_t position = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void unlock_uart_hdma();
static void init_debugmcu();
static void rearm_uart_dma_rx();
static void process_command();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance != LPUART1)
		return;

	if ((huart->ErrorCode & HAL_UART_ERROR_ORE) != 0 && huart->hdmarx->Lock == HAL_LOCKED)
		uart_hdmarx_locked = true;
	else
		need_rearm = true;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	(void)huart;
	if (huart->Instance != LPUART1)
		return;

	if (HAL_UARTEx_GetRxEventType(huart) == HAL_UART_RXEVENT_HT)
		return;

	memcpy(readBuf, dmaBuf, Size);
	rxSize = Size;
	rxDataReady = true;
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
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  init_debugmcu();

  reader_init(&hlpuart1, dmaBuf, RX_BUFFER_SIZE);

  encoder_init();
  /* USER CODE END 2 */

  /* Initialize led */
  BSP_LED_Init(LED_GREEN);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  const uint32_t positionInterval = 100;
  uint32_t positionStart = 0;
  while (1)
  {
	if (rxDataReady)
		process_command();

	if (need_rearm)
		rearm_uart_dma_rx();

	if (uart_hdmarx_locked)
		unlock_uart_hdma();

	uint32_t now = HAL_GetTick();
	if (now - positionStart >= positionInterval)
	{
		position = encoder_get_position();
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void process_command()
{
	rxDataReady = false;
	reader_read(readBuf, rxSize);
	RawCmd rawCmd;
	Command command;
	while(reader_get_command(&rawCmd))
	{
		command = parse(&rawCmd);
	}

	need_rearm = true;
}

static void unlock_uart_hdma()
{
	uart_hdmarx_locked = false;
	reader_reset();
	if (HAL_UART_AbortReceive(&hlpuart1) != HAL_OK)
	{
	    __HAL_UNLOCK(&hlpuart1);
	    if (hlpuart1.hdmarx)
	    {
	        __HAL_UNLOCK(hlpuart1.hdmarx);
	        hlpuart1.hdmarx->State = HAL_DMA_STATE_READY;
	    }
	    hlpuart1.RxState = HAL_UART_STATE_READY;
	}
	need_rearm = true;
}

static void rearm_uart_dma_rx()
{
	need_rearm = false;
	if (HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, dmaBuf, RX_BUFFER_SIZE) != HAL_OK)
	{
		if (++rearm_retries >= max_rearm_retries)
		{
			debug_print_str(uart_rearm_stuck);
			HAL_Delay(1);
			rearm_retries = 0;
		}
		need_rearm = true;
	}
	else
	{
		rearm_retries = 0;
	}
}

static void init_debugmcu()
{
	// 1. Включаем тактирование DBGMCU
	SET_BIT(RCC->APB1ENR1, (1UL << 22));

	// 2. Включаем трассировку
	DBGMCU->CR |= 0x00300000;

	// 3. Настраиваем PB3 как SWO (AF0)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef gpio = {0};
	gpio.Pin = GPIO_PIN_3;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Alternate = GPIO_AF0_TRACE;
	HAL_GPIO_Init(GPIOB, &gpio);
}
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
