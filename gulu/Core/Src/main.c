/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DHT.h"
#include "stdio.h"
#include "string.h"
#include "jx_uart_send.h"
#include "oled.h"
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
enum DemoStatus
{
    Colourful_Egg,Weather_Humidity,Time
};
enum DemoStatus status;


DHT_DataTypedef DHT11_Data;

int Temperature=16, Humidity=0, WebTemperature=0;
int HH=0,MM=0;
int num=0;

uint8_t RxBuff[1];      //interrupt data buffer
uint8_t DataBuff[500]; //data receved 
int RxLength=0;         //length of receved data
uint8_t Rx2Buff[1];      //interrupt data buffer
uint8_t Data2Buff[500]; //data receved 
int Rx2Length=0;         //length of receved data
int flag = 0;
int quest = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_NVIC_SetPriority(SysTick_IRQn,0,0);
  // DHT_GetData(&DHT11_Data); 
	// Temperature = (int)DHT11_Data.Temperature;
  HAL_UART_Receive_IT(&huart1, (uint8_t *)RxBuff, 1); //打开串口中断接收
  HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx2Buff, 1); // only for esp receve
	HAL_UART_Receive_IT(&huart3, (uint8_t *)RxBuff, 1); //only for speech receve
  
	
  status = Time;
	
  OLED_Init();
	   
	OLED_Display_On();

	OLED_Clear();
  //test
  //OLED_ShowNum(2,10,99,2,16);
	unsigned char hello[] = "hello Gulu";
	OLED_ShowString(20,10,hello,16);
  //Humidity = (int)DHT11_Data.Humidity;
  //printf("STM Starting\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  
  while (1)
  {
    unsigned char buf[1];
    switch (status)
    {
    case Colourful_Egg:
      if(num++==0)
        //tell esp
        {
          printf("$1");
        }
      break;
    case Weather_Humidity:
			//100 000
      num = (num++)%10000;
      if(num==1)
      {
        //tell esp
				//DHT_GetData(&DHT11_Data); // No decimal part,so turn to int
        printf("$2");
        
				//printf("send to esp for weather\n");
				//_uart_Weather(Temperature,WebTemperature);
        // printf("Temperature is: %d\n", Temperature);
        // printf("Humidity is: %d\n", Humidity);
      }
      break;
    case Time:
    //should be 10000
    num = (num++)%10000;
      if(num==1)
      {
        printf("$3");
				//just for test!!!,it's ok to send hex
				//_uart_Time(HH,MM);
				//printf("send to esp for time\n");
      }
      break;
    default:
      break;
    }
		
    if(quest==12){
      _uart_Weather(Temperature,WebTemperature);
      quest=0;
    }
    if(quest==13){
			_uart_Time(HH,MM);
      quest=0;
    }

    HAL_Delay(1);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
    if(UartHandle->Instance == USART3)
    {
      //printf("RXLen=%d\r\n",RxLength); 
      // for(int i=0;i<RxLength;i++)
      // printf("UART DataBuff[%d] = 0x%x\r\n",i,DataBuff[i]);                            
      // memset(DataBuff,0,sizeof(DataBuff));  
      RxLength++; 
      DataBuff[RxLength-1]=RxBuff[0];
      //When receved message(weather or time) from esp,then update
      //when ask for message, we will tell esp,then esp quest the web and return.
      if(RxBuff[0]==0X11)            //end
      {
        status = Colourful_Egg;
        num=0;
        printf("$1");
      }
      if(RxBuff[0]==0X12)            //end
      {
        quest=12;
        status = Weather_Humidity;
        num=0;
        printf("$2");
      }
      if(RxBuff[0]==0X13)            //end
      {
        quest=13;
        status = Time;
        num=0;
        printf("$3");
      }
      
      HAL_UART_Receive_IT(&huart3, (uint8_t *)RxBuff, 1); //每接收一个数据，就打开1次串口中断接收，否则只会接收1个数据就停止接收
    }

    if(UartHandle->Instance == USART2)
    {
      Rx2Length++; 
      Data2Buff[Rx2Length-1]=Rx2Buff[0];
      if(Data2Buff[0]=='$')
      {
        flag = 1;
				//Rx2Length=1;
      }
      if(Rx2Buff[0]=='&')
      {
        flag = 0;
        //printf("RxLength==%d\n",RxLength);
        //Time
        if(Data2Buff[Rx2Length-4]==':'){
          
            HH = Data2Buff[Rx2Length-6]-'0';
            HH *=10;
            HH += Data2Buff[Rx2Length-5]-'0';
            MM = Data2Buff[Rx2Length-3]-'0';
            MM*=10;
            MM += Data2Buff[Rx2Length-2]-'0';
            //printf("receved esp Time %d:%d\n",HH,MM);
            UpdateOledTime(HH,MM);
        }
        //weather
        if(Data2Buff[Rx2Length-4]=='$'){
            WebTemperature = Data2Buff[Rx2Length-3]-'0';
            WebTemperature *=10;
            WebTemperature += Data2Buff[Rx2Length-2]-'0';
            printf("receved esp Temperature %d\n",WebTemperature);
            UpdateOledWeather(Temperature,WebTemperature);
        }
//          Rx2Length=0; 
//          Rx2Buff[0]=0;
//          memset(Data2Buff,0,sizeof(Data2Buff));
      }
      if(flag==0)
      {
        Rx2Length=0; 
        Rx2Buff[0]=0;
        memset(Data2Buff,0,sizeof(Data2Buff));
      }
      HAL_UART_Receive_IT(&huart2, (uint8_t *)Rx2Buff, 1);
    }
     
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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
