#include "sys_init.h"
#include "sys_func.h"
#include "uart.h"
#include "rtc.h"
#include "flash.h"



/* Private typedef -----------------------------------------------------------*/
static GPIO_InitTypeDef  GPIO_InitStruct;
TIM_Encoder_InitTypeDef Encoder;

/* Private variables ---------------------------------------------------------*/
uint32_t data_index2 = 0,data_index1=0;
uint64_t emk[EMK_Length][3]={0}, local_db[EMK_Length][3]={0};
uint64_t buffer[EMK_Size];
char POR_SET=0;

extern uint32_t data_index;
extern uint32_t EnterStandBy,ExitStandBy,SelectMotor;

extern int state,mode;
extern char* SysState[3];
extern char* SysMode[3]; 

extern union Device_Details{
	struct DeviceDetails{
		uint64_t FirmwareID[10];
		uint64_t No_of_Mot;
		uint64_t ID[10];
		uint64_t RTC_Data[7];
		uint64_t BatStat;		
	}Device;
	uint64_t Buffer[11];
}DeviceData;

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle_int;
TIM_HandleTypeDef    TimHandle_Enc1;
TIM_HandleTypeDef    TimHandle_Enc2;
TIM_HandleTypeDef    TimHandle_Enc3;
TIM_HandleTypeDef    TimHandle_Enc4;
#define IDLE 0
#define CONTROL 1
#define COMMUNICATION 2

void System_Initialise(void)
{
	/* Configure UART Peripheral*/
	uart_config();
	#ifdef DEBUG_MODE
		sprintf(dispvar,"\nUART configured successfully ");
		serial_printf();
	#endif
	
	/* Configure RTC */
	rtc_init();
	
	/* Configure the system Power */
	SystemPower_Config();
	
	/* Initialize the User push-button to generate external interrupts */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
	
	sys_config();
	
	#ifdef DEBUG_MODE
		DeviceData.Device.No_of_Mot = 1;
		motor_init();	
	#else
		if(flash_read(DEVICE_DATA_BACKUP_ADDR, 11, buffer) != OK)
		{		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
				Error_Handler(303);	 // See "error codes.txt"
		}
		if(POR_SET == SET)
			motor_init();
		else 
			for(SelectMotor=0 ; SelectMotor < MotorMax ; SelectMotor++)
				Motor[SelectMotor].SetAddr();
	#endif
		 
	/* Reset motors */
	for(SelectMotor=0 ; SelectMotor < MotorMax ; SelectMotor++)
	{
		Motor[SelectMotor].ReadMem();
		if(Motor[SelectMotor].MemVar.isAlive)
		{
			Motor[SelectMotor].TempVar.preEnc 	= RESET;
			Motor[SelectMotor].TempVar.factor 	= RESET;
			Motor[SelectMotor].Data.Position 		= RESET;
			Motor[SelectMotor].Data.Error 	 		= RESET;
			Motor[SelectMotor].StopMotor();
		}
	}
	
	/* Read EMK and copy to local database */	
	if(flash_read(DATA_BASE_START_ADDR, EMK_Size, buffer) != OK)
	{ 
		  sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
			Error_Handler(302);	 // See "error codes.txt"
	}
	data_index = 0;
	while(data_index < EMK_Size){
		for (data_index1 = 0; data_index1 < EMK_Length; data_index1++){
			for (data_index2 = 0; data_index2 < 3; data_index2++){
				emk[data_index1][data_index2] =  buffer[data_index];
				data_index++;
			}
		}
	}

	/* Reset Encoders */
		
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc2,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc3,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc4,0);
				
#ifndef FIRMWARE_VERSION
#error Please define Firmware version
#endif
#ifdef DEBUG_MODE
	if(!ExitStandBy){
		sprintf(dispvar,"\n System Initialized in Debug Mode");
		serial_printf();
		sprintf(dispvar,"\n Firmware Version %s", FIRMWARE_VERSION);  //Firmware version is defined in main.h
		serial_printf();
		sprintf(dispvar,"\n Currently the device is running in %s", SysMode[mode] );	
		serial_printf();
	}
	else
		sprintf(dispvar,"\n Resumed from StandBy");
		serial_printf();
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

	
	/* Configure Timer interrrupt */

	/* Timer 7 interrupt is configured at 1m second */

	TimHandle_int.Instance = TIM7;

	TimHandle_int.Init.Period      = 14400;
	TimHandle_int.Init.Prescaler   = 9;
	TimHandle_int.Init.CounterMode = TIM_COUNTERMODE_UP;

	if (HAL_TIM_Base_Init(&TimHandle_int) != HAL_OK)
	{
		/* Initialization Error */
		sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
		Error_Handler(304);	// See "error codes.txt"
	}

	/* Set interrupt priority*/	
	HAL_NVIC_SetPriority(TIM7_IRQn, 3, 1);

	/* Enable the TIMx global Interrupt */
	HAL_NVIC_EnableIRQ(TIM7_IRQn);	
			
	/* GPIO Configuration for motor control */
	motor_pin_config();		
	
	/* Configure Encoders 
	
	Encoder timers are configured at 100u second */	
	
	Motor[A].EncTimInit(&TimHandle_Enc1, TIM2, 
											ENCODER_1_PIN_A_PORT, ENCODER_1_PIN_A, 
											ENCODER_1_PIN_B_PORT, ENCODER_1_PIN_B, 
											GPIO_AF1_TIM2, GPIO_AF1_TIM2);
	
	Motor[B].EncTimInit(&TimHandle_Enc2, TIM3, 
											ENCODER_2_PIN_A_PORT, ENCODER_2_PIN_A, 
											ENCODER_2_PIN_B_PORT, ENCODER_2_PIN_B, 
											GPIO_AF2_TIM3, GPIO_AF2_TIM3);
											
	Motor[C].EncTimInit(&TimHandle_Enc3, TIM4, 
											ENCODER_3_PIN_A_PORT, ENCODER_3_PIN_A, 
											ENCODER_3_PIN_B_PORT, ENCODER_3_PIN_B, 
											GPIO_AF2_TIM4, GPIO_AF2_TIM4);
											
	Motor[D].EncTimInit(&TimHandle_Enc4, TIM5, 
											ENCODER_4_PIN_A_PORT, ENCODER_4_PIN_A, 
											ENCODER_4_PIN_B_PORT, ENCODER_4_PIN_B, 
											GPIO_AF2_TIM5, GPIO_AF2_TIM5);
	/*-----------------------------------------------------------------*/
	
	for(SelectMotor=0 ; SelectMotor < MotorMax ; SelectMotor++)
		Motor[SelectMotor].EnableIRQ();
		

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

#ifndef DEBUG_MODE
	else
    {
			POR_SET=SET;	
			STATE = IDLE;	
		}
#endif
}

void GPIO_Init(GPIO_TypeDef* temp_port, uint32_t temp_pin, uint32_t temp_mode, uint32_t temp_pull, uint32_t temp_speed, uint32_t temp_af)
{
	GPIO_InitStruct.Mode  		= temp_mode;
	GPIO_InitStruct.Pull  		= temp_pull;
	GPIO_InitStruct.Speed 		= temp_speed;
	GPIO_InitStruct.Alternate	= temp_af;

	GPIO_InitStruct.Pin = temp_pin;
	HAL_GPIO_Init(temp_port, &GPIO_InitStruct);				
}

/*--------------------------------------------------------------------------*/
