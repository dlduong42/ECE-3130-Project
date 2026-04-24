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
#include "string.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

static void MX_USART2_UART_Init(void);
void Delay(unsigned int);
void Write_String_LCD(char*);
void Write_Char_LCD(uint8_t);
void Write_Instr_LCD(uint8_t);
void LCD_nibble_write(uint8_t, uint8_t);
void Write_SR_LCD(uint8_t);
void PrintText(uint8_t*);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// USART2 buffer
// receives 1 byte
uint8_t rxdata[1];
// accumulates bytes;
uint8_t rxbuffer[100];

// text index
uint8_t rxindex = 0;

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
  SystemClock_Config();
  MX_GPIO_Init();
	MX_USART2_UART_Init();
	Write_String_LCD("Input Morse Code");
	
  while (1)
  {
		/* 
		USART2 Receiver Initialization
		 
		&huart2 = pointer to which UART we're using
		
		rx_data is where the byte (our data from text) will be stored
		
		1 = how many bytes to receive before we trigger the HAL_UART_RxCpltCallback
		
		HAL_MAX_DELAY = the timing windows to receive 1 data, if we had a set value
		1000 (1 sec) we have 1 sec to receive all the text or else the data is lost;
		
		HAL_OK = Basically confirms we received 1 byte so when we have it say its equal
		to it, it means that if we successfully receive it  then the == HAL_OK is
		true, triggering the if statement
	*/
		if (HAL_UART_Receive(&huart2, rxdata, 1, HAL_MAX_DELAY) == HAL_OK)
		{
			char received = rxdata[0];
			
			if (received == '\n' || received == '\r')
			{
				if (rxindex > 0)
				{
					rxbuffer[rxindex] = '\0';
					rxindex = 0;
					PrintText(rxbuffer);
				}
			}
			else
			{
				if (rxindex < 100)
				{
					rxbuffer[rxindex++] = received;
				}
			}
		}
  }
  /* USER CODE END 3 */
}

//Function Helper

void PrintText(uint8_t* input)
{
	
	Write_Instr_LCD(0x01);		// clears the LCD before a new string is added
	Delay(5);
	int j = 16;								// this is to check if there are too many characters for the LCD row
	
	// iterates through the string inputed and prints it on the LCD
	for(int i = 0;input[i] != '\0'; i++)
	{													// if there are more than enough characters for the first row of the LCD,
		if(i == j){							// this command will move to the next line
			Write_Instr_LCD(0xC0);
			Delay(2);
		}
														// if there are more than enough characters for the second row of the LCD, this command
		else if(i == (j + 16)){	// will clear the LCD after five seconds and print the rest on the next screen
			Delay(5000);
			Write_Instr_LCD(0x01);
			j = j + 32;
		}
		// writes the current character on the LCD
		Write_Char_LCD(input[i]);
	}
}


void LCD_nibble_write(uint8_t temp, uint8_t s)
{
	if (s==0) /*writing instruction*/
	{
		temp=temp&0xF0;
		temp=temp|0x02; /*RS=1 (bit 0) for data EN=high (bit1)*/
		Write_SR_LCD(temp);
		temp=temp&0xFD; /*RS=1 (bit 0) for data EN=high (bit1)*/
		Write_SR_LCD(temp);
	}

	else if (s==1) /*writing data*/
	{
		temp=temp&0xF0;
		temp=temp|0x03; /*RS=1 (bit 0) for data EN=high (bit1)*/
		Write_SR_LCD(temp);
		temp=temp&0xFD; /*RS=1 (bit 0) for data EN=high (bit1)*/
		Write_SR_LCD(temp);
	}
	
}	

	void Write_Char_LCD(uint8_t code)
{
	
	LCD_nibble_write(code&0xF0,1);
	
	code=code<<4;
	LCD_nibble_write(code,1);
	Delay(1);
	
}
		
void Write_String_LCD(char *temp)
{
	int i=0;
	while(temp[i]!=0)
	{
		Write_Char_LCD(temp[i]);
		i=i+1;
	}	
}

void Write_Instr_LCD(uint8_t code)
{
	LCD_nibble_write(code&0xF0,0);
	
	code=code<<4;
	LCD_nibble_write(code,0);
	Delay(2);
}

void Write_SR_LCD(uint8_t temp)
{
int i;
uint8_t mask=0b10000000;
	
for(i=0; i<8; i++) {
        if((temp&mask)==0)
        GPIOB->ODR&=~(1<<5);
        else
        GPIOB->ODR|=(1<<5);

        /*	Sclck */
        GPIOA->ODR&=~(1<<5); GPIOA->ODR|=(1<<5);
        Delay(1);

        mask=mask>>1;
        }

    /*Latch*/
    GPIOA->ODR|=(1<<10); 
		Delay(1);
    GPIOA->ODR&=~(1<<10);
		Delay(1);
}

void Delay(unsigned int n)
{
	int i;
	if(n!=0)
	{
    for (; n > 0; n--)
        for (i = 0; i < 136; i++) ;
}
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  // TODO: Initialize any input or output pins you'll use here
	
		uint32_t temp;
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
			

temp = GPIOA->MODER;
temp &= ~(0x03<<(2*1)); 
temp|=(0x01<<(2*1)); 
GPIOA->MODER = temp;
temp=GPIOA->OTYPER;
temp &=~(0x01<<1); 
GPIOA->OTYPER=temp;

temp=GPIOA->PUPDR;
temp&=~(0x03<<(2*1));
GPIOA->PUPDR=temp;

	
// Initialization of 7 seg	
	temp = GPIOA->MODER;
	temp &= ~(0x03<<(2*5));
	temp|=(0x01<<(2*5));
	GPIOA->MODER = temp; 
			 
	temp=GPIOA->OTYPER;
	temp &=~(0x01<<5);
	GPIOA->OTYPER=temp;
	
	temp=GPIOA->PUPDR;
	temp&=~(0x03<<(2*5));
	GPIOA->PUPDR=temp;

	
	temp = GPIOB->MODER;
	temp &= ~(0x03<<(2*5));
	temp|=(0x01<<(2*5));
	GPIOB->MODER = temp; 
			 
	temp=GPIOB->OTYPER;
	temp &=~(0x01<<5);
	GPIOB->OTYPER=temp;
	
	temp=GPIOB->PUPDR;
	temp&=~(0x03<<(2*5));
	GPIOB->PUPDR=temp;
	
	temp = GPIOC->MODER;
	temp &= ~(0x03<<(2*10));
	temp|=(0x01<<(2*10));
	GPIOC->MODER = temp; 
			 
	temp=GPIOC->OTYPER;
	temp &=~(0x01<<10);
	GPIOC->OTYPER=temp;
	
	temp=GPIOC->PUPDR;
	temp&=~(0x03<<(2*10));
	GPIOC->PUPDR=temp;	
	
	 temp = GPIOA->MODER;
			 temp &= ~(0x03<<(2*10));
			 temp|=(0x01<<(2*10));
			 GPIOA->MODER = temp; 
			 
			 temp=GPIOA->OTYPER;
			 temp &=~(0x01<<10);
	     GPIOA->OTYPER=temp;
	
			 temp=GPIOA->PUPDR;
	     temp&=~(0x03<<(2*10));
	     GPIOA->PUPDR=temp;

	/* LCD controller reset sequence */
	Delay(20);
	LCD_nibble_write(0x30,0);
	Delay(5);
	LCD_nibble_write(0x30,0);
	Delay(1);
	LCD_nibble_write(0x30,0);
	Delay(1);
	LCD_nibble_write(0x20,0);
	Delay(1);
	
		
	Write_Instr_LCD(0x28); /* set 4 bit data LCD - two line display - 5x8 font*/
	Write_Instr_LCD(0x0E); /* ;turn on display, turn on cursor , turn off blinking 		*/	
	Write_Instr_LCD(0x01); /* clear display screen and return to home position	*/		
	Write_Instr_LCD(0x06); /* ;move cursor to right (entry mode set instruction) 		*/	

/*configure input*/
	/* row0 to 3 are PB11, PB10, PB9, PB8 */ 
	temp = GPIOB->MODER;
	temp &= ~(0x03<<(2*11)); 
	temp &= ~(0x03<<(2*10)); temp &= ~(0x03<<(2*9)); 
	temp &= ~(0x03<<(2*8)); 
	GPIOB->MODER = temp;
	temp=GPIOB->OTYPER;
	temp &=~(0x01<<11); 
	temp &=~(0x01<<10); 
	temp &=~(0x01<<9); 
	temp &=~(0x01<<8); 
	GPIOB->OTYPER=temp;

	temp=GPIOB->PUPDR;
	temp&=~(0x03<<(2*11)); 
	temp&=~(0x03<<(2*10)); 
	temp&=~(0x03<<(2*9)); 
	temp&=~(0x03<<(2*8)); 
	GPIOB->PUPDR=temp;

	/* Col 0 to 3 are PB1, PB2, PB3, PB4*/
	/*configure output*/ 
	temp = GPIOB->MODER;
	temp &= ~(0x03<<(2*1)); 
	temp|=(0x01<<(2*1)); 
	temp &= ~(0x03<<(2*2)); 
	temp|=(0x01<<(2*2)); 
	temp &= ~(0x03<<(2*3)); 
	temp|=(0x01<<(2*3));
	temp &= ~(0x03<<(2*4)); 
	temp|=(0x01<<(2*4));
	GPIOB->MODER = temp;

	temp=GPIOB->OTYPER;
	temp &=~(0x01<<1); 
	temp &=~(0x01<<2); 
	temp &=~(0x01<<3); 
	temp &=~(0x01<<4); 
	GPIOB->OTYPER=temp;

	temp=GPIOB->PUPDR;
	temp&=~(0x03<<(2*1));
	temp&=~(0x03<<(2*2)); 
	temp&=~(0x03<<(2*3)); 
	temp&=~(0x03<<(2*4));
	GPIOB->PUPDR=temp;


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
