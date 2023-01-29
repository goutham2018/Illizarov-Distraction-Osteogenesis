#ifndef __SERIAL_RECEIVE__
#define __SERIAL_RECEIVE__

#include "main.h"
#include "uart.h"
#include "flash.h"
#include "rtc.h"

#define REQ 0xAA
#define CFM 0xBB
#define IND 0xCC
#define NIL 0x00

/*---------------------- Configuration Instructions --------------------------*/

#define SET_DEVICE_ID  				0x00
				/* OpCode2 for Configuring Device*/
				#define SET_DEVICE_ID					0x00
#define CONFIG_DEVICE       					0x01
				/* OpCode2 for Configuring Device*/
				#define SET_RTC									0x00
				#define SET_NUMBER_OF_MOTORS		0x01
#define CONFIG_PATIENT_DETAIL     			0x02

				/* OpCode2 for Configuring Device*/
				#define SAVE_TO_FLASH 					0x00
				#define PATIENT_ID	  					0x01
				#define DOB_GENDER    					0x02
				#define PATIENT_NAME  					0x03
				#define MOTHER_NAME   					0x04
				#define DURATION      					0x05
				#define AMT_DIST      					0x06
				#define INTERVAL_DIST 					0x07
				#define NXT_APP_DATE  					0x08

#define CHANGE_DATABASE     			0x03

/*------------------------------------------------------------------------*/

/*---------------------------- Read Instructions -----------------------------*/

#define VIEW_SYSTEM_DETAILS  			0x10

				/* OpCode2 for Viewing System Details */
				#define VIEW_ALL_DETAILS 	    	0x00
				#define VIEW_FIRMWARE_VERSION		0x01
				#define VIEW_DEVICE_ID			    0x02
				#define READ_RTC							  0x03
				#define VIEW_BATTERY_STATUS		  0x04
				#define VIEW_MODE								0x05	
				#define VIEW_NO_OF_MOT					0x06

#define VIEW_PATIENT_DETAILS 			0x11

				/* OpCode2 for Viewing Patient Details */
				#define READ_FLASH		 					0x00
				#define READ_PATIENT_ID		  		0x01
				#define READ_DOB_GENDER	 		   	0x02
				#define READ_PATIENT_NAME 		 	0x03
				#define READ_MOTHER_NAME		   	0x04
				#define READ_DURATION  	 		   	0x05
				#define READ_AMT_DIST      			0x06
				#define READ_INTERVAL_DIST 			0x07
				#define READ_NXT_APP_DATE  			0x08

#define VIEW_PROGRESS        			0x12

/*------------------------------------------------------------------------*/

/*------------------------- Command Instructions -------------------------*/
	
#define MODE_COMMANDS   					0x20

				/* OpCode2 for Modes */
				#define ENTER_TEST_MODE		 			0x00
				#define EXIT_TEST_MODE			 		0x01
				#define ENTER_MANUAL_MODE			  0x02
				#define ENTER_AUTOMATIC_MODE		0x03
				#define EXIT_AUTOMATIC_MODE     0x04
#define CLEAR_DATA								0x21

				/* OpCode2 for Clearing data */
				#define CLEAR_ALL_DATA		 			0x00
				#define CLEAR_SYSTEM_DATA		 		0x01
				#define CLEAR_PATIENT_DETAILS   0x02
				#define CLEAR_PATIENT_LOG_DATA 	0x03
				#define CLEAR_EMK_DTATBASE		  0x04

#define EMERGENCY_STOP						0x22
#define TERMINATE_COMMUNICATION		0x23

/*------------------------------------------------------------------------*/

/*--------------------------- Mode Instructions --------------------------*/

#define TEST_MODE_COMMANDS				0x30

				/* OpCode2 for Test Mode */
				#define TEST_RTC_ALARM		 			0x00
				#define TEST_MOTOR_ENCODER_1		0x01
				#define TEST_MOTOR_ENCODER_2  	0x02
				#define TEST_MOTOR_ENCODER_3 		0x03
				#define TEST_MOTOR_ENCODER_4		0x04
				#define TEST_BUZZER							0x05

#define MANUAL_MODE_COMMANDS			0x31

				/* OpCode2 for Manual Mode */
				#define MANUAL_MODE_PARAMETERS	0x00
				#define START_MOTOR							0x01

#define AUTOMATIC_MODE_COMMANDS		0x32

				/* OpCode2 for Automatic Mode */
				#define SET_RTC_FIRST_ALARM			0x00
				#define START_DISTRACTION_PROC	0x01
				#define STOP_DISTRACTION_PROC		0x02

/*------------------------------------------------------------------------*/

/*------------------------------ Default Errors --------------------------*/

#define DEFAULT_ERROR							0x40

				/* Error Code for default Errors */
				#define ERR_EXIT_TEST_MODE			0x00
				#define ERR_EXIT_MANUAL_MODE		0x01
				#define ERR_WRONG_INSTRUCTION		0x02

/*------------------------------------------------------------------------*/

#endif
