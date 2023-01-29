#include "motor_func.h"

extern uint32_t SelectMotor;
Motor_func Motor[4];	

	void Motor_func :: StartMotor()
	{
		switch (Motor[SelectMotor].Data.Direction){
			case POS:
						/* Depending on the direction, turn on the motor */				

						/* Turn on the motor in CW direction */
						HAL_GPIO_WritePin(Motor[SelectMotor].Pin.Port, Motor[SelectMotor].Pin.p1, GPIO_PIN_SET );
						HAL_GPIO_WritePin(Motor[SelectMotor].Pin.Port, Motor[SelectMotor].Pin.p2, GPIO_PIN_RESET);
						break;
			case REV:
					
						/* Turn on the motor in Anti-CW direction */
						HAL_GPIO_WritePin(Motor[SelectMotor].Pin.Port, Motor[SelectMotor].Pin.p1, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(Motor[SelectMotor].Pin.Port, Motor[SelectMotor].Pin.p2, GPIO_PIN_SET  );
						break;
				}
	}
	void Motor_func :: StopMotor()
	{

						/* Turn off the motor */				
				HAL_GPIO_WritePin(Motor[SelectMotor].Pin.Port, Motor[SelectMotor].Pin.p1 , GPIO_PIN_RESET);
				HAL_GPIO_WritePin(Motor[SelectMotor].Pin.Port, Motor[SelectMotor].Pin.p2 , GPIO_PIN_RESET);

	}

void motor_init()
{
					Motor[A] = Motor_func(MOTOR_1_PORT,MOTOR_1_PIN_A,MOTOR_1_PIN_B);
//				Motor[B].Motor_func(MOTOR_2_PIN_A,MOTOR_2_PIN_B);
//				Motor[C].Motor_func(MOTOR_3_PIN_A,MOTOR_3_PIN_B);
//				Motor[D].Motor_func(MOTOR_4_PIN_A,MOTOR_4_PIN_B);
}
	
