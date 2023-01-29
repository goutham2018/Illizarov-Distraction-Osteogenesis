#include "sys_init.h"
#include "sys_func.h"
#include "uart.h"
#include "rtc.h"
#include "flash.h"
#include "motor_func.h"

/* Private typedef -----------------------------------------------------------*/
static GPIO_InitTypeDef  GPIO_InitStruct;
TIM_Encoder_InitTypeDef Encoder;

/* Private variables ---------------------------------------------------------*/
uint32_t data_index2 = 0,data_index1=0;
uint64_t emk[EMK_Length][3]={0};//, local_db[EMK_Length][3]={0};
uint64_t buffer[EMK_Size];

extern uint32_t data_index;
extern uint32_t EnterStandBy,ExitStandBy;

extern int state,mode;
extern char* SysState[3];
extern char* SysMode[3];

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle_int;
TIM_HandleTypeDef    TimHandle_Enc1;
TIM_HandleTypeDef    TimHandle_Enc2;
TIM_HandleTypeDef    TimHandle_Enc3;
TIM_HandleTypeDef    TimHandle_Enc4;

void System_Initialise(void)
{
	/* Configure UART Peripheral*/
	uart_config();
	#ifdef DEBUG_MODE
		printf("\nUART configured successfully ");
	#endif
	
	/* Configure RTC */
	rtc_init();
	
	/* Configure the system Power */
	SystemPower_Config();
	
	/* Initialize the User push-button to generate external interrupts */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
	
	sys_config();
	
	/* Reset motors */
	Motor[A].StopMotor();
	Motor[B].StopMotor();
	Motor[C].StopMotor();
	Motor[D].StopMotor();
	
	/* Read EMK and copy to local database */
	
				flash_read(DATA_BASE_START_ADDR, EMK_Size, buffer);
				data_index = 0;
				while(data_index < EMK_Size){
					for (data_index1 = 0; data_index1 < EMK_Length; data_index1++){
						for (data_index2 = 0; data_index2 < 3; data_index2++){
							emk[data_index1][data_index2] =  buffer[data_index];
							data_index++;
						}
					}
				}
//				for(data_index1=0;data_index1<EMK_Length;data_index1++)
//					for(data_index2=0;data_index2<3;data_index2++)
//						local_db[data_index1][data_index2]=emk[data_index1][data_index2];
	
	/* Reset Encoders */
		
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc2,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc3,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc4,0);
				
#ifndef FIRMWARE_VERSION
#warning Please re-define Firmware version
#endif
#ifdef DEBUG_MODE
	if(!ExitStandBy){
		printf("\nSystem Initialized in Debug Mode");
		printf(" Firmware Version %s", FIRMWARE_VERSION);  //Firmware version is defined in main.h
		printf("\n Currently the device is running in %s", SysMode[mode] );	
	}
	else
		printf("\nRecovered from StandBy");
#endif
}

void sys_config(void)
{
	/* Enable Peripheral clock */
	__HAL_RCC_GPIOA_CLK_ENABLE() ;
	__HAL_RCC_GPIOB_CLK_ENABLE() ;
	__HAL_RCC_TIM2_CLK_ENABLE() ;
	__HAL_RCC_TIM3_CLK_ENABLE() ;
	__HAL_RCC_TIM4_CLK_ENABLE() ;
	__HAL_RCC_TIM5_CLK_ENABLE() ;
	__HAL_RCC_TIM7_CLK_ENABLE() ;

			
	/* Configure Encoders 
	
	
	Encoder timers are configured at 100u second */	
				Encoder.EncoderMode = TIM_ENCODERMODE_TI12;
				Encoder.IC1Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC2Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC1Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC2Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC1Prescaler=TIM_ICPSC_DIV1;
				Encoder.IC2Prescaler=TIM_ICPSC_DIV1;
	

				/*------------------------ Encoder 1 ------------------------------*/
				/* Encoder 1 Timer Configuration */
				TimHandle_Enc1.Instance = TIM2;
				TimHandle_Enc1.Init.Period            = 14400;
				TimHandle_Enc1.Init.Prescaler         = 0;
				TimHandle_Enc1.Init.CounterMode       = TIM_COUNTERMODE_UP;				
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc1, &Encoder) != HAL_OK)
					/* Starting Error */
					Error_Handler(301);
				HAL_TIM_Encoder_Start(&TimHandle_Enc1,TIM_CHANNEL_ALL);
				
				/* Encoder 1 GPIO Configuration */

				GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF1_TIM2 ;

				GPIO_InitStruct.Pin = ENCODER_1_PIN_A;
				HAL_GPIO_Init(ENCODER_1_PIN_A_PORT, &GPIO_InitStruct);				

				GPIO_InitStruct.Alternate = GPIO_AF1_TIM2 ;

				GPIO_InitStruct.Pin = ENCODER_1_PIN_B;
				HAL_GPIO_Init(ENCODER_1_PIN_B_PORT, &GPIO_InitStruct);
				/*-----------------------------------------------------------------*/
				/*------------------------ Encoder 2 ------------------------------*/
				/* Encoder 2 Timer Configuration */
				TimHandle_Enc2.Instance = TIM3;
	
				TimHandle_Enc2.Init.Period            = 14400;
				TimHandle_Enc2.Init.Prescaler         = 0;
				TimHandle_Enc2.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc2, &Encoder) != HAL_OK)
					/* Starting Error */
					Error_Handler(302);
				HAL_TIM_Encoder_Start(&TimHandle_Enc2,TIM_CHANNEL_ALL);

				/* Encoder 2 GPIO Configuration */
				GPIO_InitStruct.Alternate = GPIO_AF2_TIM3 ;

				GPIO_InitStruct.Pin = ENCODER_2_PIN_A | ENCODER_2_PIN_B;
				HAL_GPIO_Init(ENCODER_2_PORT, &GPIO_InitStruct);	
				/*-----------------------------------------------------------------*/
				/*------------------------ Encoder 3 ------------------------------*/
				/* Encoder 3 Timer Configuration */
				TimHandle_Enc3.Instance = TIM4;

				TimHandle_Enc3.Init.Period            = 14400;
				TimHandle_Enc3.Init.Prescaler         = 0;
				TimHandle_Enc3.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc3, &Encoder) != HAL_OK)
					/* Starting Error */
					Error_Handler(303);
				HAL_TIM_Encoder_Start(&TimHandle_Enc3,TIM_CHANNEL_ALL);

				/* Encoder 3 GPIO Configuration */
				GPIO_InitStruct.Alternate = GPIO_AF2_TIM4 ;

				GPIO_InitStruct.Pin = ENCODER_3_PIN_A | ENCODER_3_PIN_A;
				HAL_GPIO_Init(ENCODER_3_PORT, &GPIO_InitStruct);
				/*-----------------------------------------------------------------*/
				/*------------------------ Encoder 4 ------------------------------*/
				/* Encoder 4 Timer Configuration */
				TimHandle_Enc4.Instance = TIM5;

				TimHandle_Enc4.Init.Period            = 14400;
				TimHandle_Enc4.Init.Prescaler         = 0;
				TimHandle_Enc4.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc4, &Encoder) != HAL_OK)
					/* Starting Error */
					Error_Handler(304);
				HAL_TIM_Encoder_Start(&TimHandle_Enc4,TIM_CHANNEL_ALL);

				/* Encoder 4 GPIO Configuration */

				GPIO_InitStruct.Alternate = GPIO_AF2_TIM5 ;

				GPIO_InitStruct.Pin = ENCODER_4_PIN_A | ENCODER_4_PIN_B;
				HAL_GPIO_Init(ENCODER_4_PORT, &GPIO_InitStruct);
				/*-----------------------------------------------------------------*/

				/* Configure Timer interrrupt */
		
				/* Timer 7 interrupt is configured at 1m second */
		
				TimHandle_int.Instance = TIM7;

				TimHandle_int.Init.Period      = 14400;
				TimHandle_int.Init.Prescaler   = 9;
				TimHandle_int.Init.CounterMode = TIM_COUNTERMODE_UP;

				if (HAL_TIM_Base_Init(&TimHandle_int) != HAL_OK)
					/* Initialization Error */
					Error_Handler(304);	
			
				/* Set interrupt priority*/	
				HAL_NVIC_SetPriority(TIM7_IRQn, 3, 0);

				/* Enable the TIMx global Interrupt */
				HAL_NVIC_EnableIRQ(TIM7_IRQn);

				/* GPIO Configuration for motor control */
				/* Motor 1 GPIO Configuration */

				GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
				GPIO_InitStruct.Pull  = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

				GPIO_InitStruct.Pin = MOTOR_1_PIN_A | MOTOR_1_PIN_B;
				HAL_GPIO_Init(MOTOR_1_PORT, &GPIO_InitStruct);				

				
				/* Motor Initialization  */
				motor_init();
				
}

void SystemPower_Config(void)
{
	/* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
	
  /* Check and handle if the system was resumed from StandBy mode */ 
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
  {
		ExitStandBy = SET;
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); 
  }
//	else
//		STATE = IDLE;	
}
/*--------------------------------------------------------------------------*/
