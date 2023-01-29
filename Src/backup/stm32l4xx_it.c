/**
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/Src/stm32l4xx_it.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    26-February-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_it.h"
#include "sys_init.h"
#include "uart.h"
#include "sys_func.h"



/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup TIM_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Handler declaration -------------------------------------------------------*/
extern TIM_HandleTypeDef    TimHandle_int;
extern TIM_HandleTypeDef    TimHandle_Enc1;
extern TIM_HandleTypeDef    TimHandle_Enc2;
extern TIM_HandleTypeDef    TimHandle_Enc3;
extern TIM_HandleTypeDef    TimHandle_Enc4;
extern UART_HandleTypeDef UartHandle;
extern RTC_HandleTypeDef RtcHandle;
/* Private variables ---------------------------------------------------------*/
extern double tOn1, tOn2, pOn1, pOn2, pS1, pS2;
extern float PBC_PCC,TBC_PCC;
char isHigh =0;
extern char isDemandChanged, dir, control;
extern uint32_t tOn, pOn, pS, demand, pSa;
extern int32_t error;
extern uint32_t local_demand;
extern uint32_t local_time;
int32_t present_encoder_count=0,previous_encoder_count=0,offset=0;
int32_t speed=0,speed_count=0,local_time_ms=0,lut_index=0;
uint32_t override=0;
extern uint8_t iteration;
extern int pulse;
extern uint32_t SelectMotor;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
}
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc1);
}
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc2);
}
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc3);
}
void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc4);
}
void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
}
void TIM7_IRQHandler(void)
{
			static int speed_time=0;
			present_encoder_count = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);
//			if(dir == rev && present_encoder_count >= 14400)
//			{
//				present_encoder_count -= 14400;
//				override = 1;
//			}
			speed_time++;
			if(speed_time%10==0)
			{
				/*Take speed every 10ms*/
				speed_time=0;
				speed = present_encoder_count - previous_encoder_count ;
				/* Wait for 10ms after speed becomes 0 ie., until the motor settles */
				if(speed == 0)
					speed_count++;		// If speed is 0, increment speed count
				else
					speed_count = RESET;	// Else reset speed count 
			}	
			local_time++;
	
			/* If control is PBC, turn off the motor when the present_encoder_count is greater than or equal to pOn */
			if(control == PBC && ((dir == POS && present_encoder_count >= pOn) || (dir == REV && present_encoder_count <= pOn)))
			{
				Motor[SelectMotor].StopMotor();
				isHigh = SET;			// Calculate PBC_PCC for adaptation
				
				/* For observation only. Not used in algorithm */
				if(pulse == SET)
					pulse = RESET;      
			}
			
			/* If control is TBC, turn off the motor when the local_time is greater than or equal to tOn */
			if(control == TBC && local_time>=tOn)
			{	
				Motor[SelectMotor].StopMotor();
				
				isHigh = SET;			// Calculate PBC_PCC for adaptation

			}
	
			/* Wait until motor settles */
			if(speed_count == 10 ) 
			{
				pS = present_encoder_count ;
								
				if(isHigh)		// If the control was PBC, calculate PBC_PCC for adaptation
				{
					if(control == PBC){
						if(absolute1(pS - pSa) != 0)
							PBC_PCC = (((float)Weight1*PBC_PCC)+((float)Weight2*PBC_PCC*((float)local_demand/(float)(absolute1(pS - pSa)))))/((float)Weight1+(float)Weight2);
						isHigh = RESET;}
					
					if(control == TBC && (absolute1(pS - pSa)) != 0 && local_demand > 20){
						if(absolute1(pS - pSa) != 0)
							TBC_PCC = (((float)Weight1*TBC_PCC)+((float)Weight2*TBC_PCC*((float)local_demand/(float)(absolute1(pS - pSa)))))/((float)Weight1+(float)Weight2);
						isHigh = RESET;}
				}
				
				error = demand - pS;
				local_demand = absolute1(error);
				pSa = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);

				if(local_demand <= 6){						// If error is less than 6 pulses, exit the control loop			
					// enter idle state
				#ifdef DEBUG_MODE
					printf("\n Demand = %d \n Final Position = %d \n Error = %d\n Itearations = %d", demand, pSa, error, iteration);
				#endif
				}
				else if(local_demand <= TBC_THRESHOLD){			// If error is less than threshold, use Time Based Control
					control = TBC;
					local_time = RESET;
					isDemandChanged = SET;}
				else { 														// If error is greater than threshold, use Position Based Control
					control = PBC;
					isDemandChanged = SET;}
				
				/* Set direction depending on the sign of the error */
				if(error < 0)
					dir = REV;
				else
					dir = POS;
				
				/* Disable Timer Interrupt */
				HAL_TIM_Base_Stop(&TimHandle_int);
				
				/* Reset Speed count*/
				speed_count = RESET;
			}
			
			
			
//			if(override == 1)
//			{
//				dir = pos;
//				override = 0;
//			}
			previous_encoder_count = present_encoder_count;
			HAL_TIM_IRQHandler(&TimHandle_int);
}

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */

void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xx.s).                                               */
/******************************************************************************/
/**
  * @brief  This function handles external lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
}
/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
//void TIMx_IRQHandler(void)
//{
////  HAL_TIM_IRQHandler(&TimHandle);
//}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
