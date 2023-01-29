#include "serial_receive.h"
#include "sys_func.h"
#include <string.h>
extern TIM_HandleTypeDef    TimHandle_int;
extern TIM_HandleTypeDef    TimHandle_Enc1;
extern TIM_HandleTypeDef    TimHandle_Enc2;
extern TIM_HandleTypeDef    TimHandle_Enc3;
extern TIM_HandleTypeDef    TimHandle_Enc4;
extern UART_HandleTypeDef UartHandle;
extern RTC_HandleTypeDef RtcHandle;
//int TempMode;
//int32_t count;
//uint32_t TestModeAlarm;
//ExitTestModeFlag=RESET;
//ExitAutomaticModeFlag=RESET;

//extern int mode;
//char* SysMode[3]={"TEST","MANUAL","AUTOMATIC"};
//enum MODE{TEST=0, MANUAL, AUTOMATIC};

union Patient_Data{
	struct PatientDetails{
		uint64_t ID;
		uint64_t DoB[3];
		uint64_t Gender;
		uint64_t Name[30];
		uint64_t MotherName[30];
		uint64_t Duration;
		uint64_t Amt_of_Distraction;
		uint64_t Interval;
		uint64_t NextAppDate[3];
	}Patient;
	uint64_t Buffer[75];
}PatientData;

union Device_Details{
	struct DeviceDetails{
		uint64_t FirmwareID[10];
		uint64_t No_of_Mot;
		uint64_t ID[10];
		uint64_t RTC_Data[7];
		uint64_t BatStat;		
	}Device;
	uint64_t Buffer[11];
}DeviceData;

union UART_Data{
	struct TxBuffer{
		uint8_t Confirmation;
		uint8_t OpCode1;
		uint8_t OpCode2;
		uint8_t ErrorByte;
		uint8_t DataLength;
		uint8_t Data[32];
	}Transmit;
	uint8_t Buffer[37];
}UART;


extern char* SysState[3];
extern char* SysMode[3];

/* Variable declaration*/
extern uint8_t aRxBuffer;

extern uint32_t ExitStandBy;
int receive_count=0;
int RxLength=0;
int serial_data[37]={0} ;

extern int state;
extern int mode;

char isDemandChanged = 0, dir, control = PBC;
uint8_t iteration = 0;
uint32_t local_demand=0,previous_demand=0, demand = 0,SelectMotor;
float PBC_PCC = 1,TBC_PCC = 1;
extern int32_t present_encoder_count,previous_encoder_count;
uint32_t local_time;
int SerialReceiveReady=0;
uint32_t ReInit=0;
int pulse=0;


/* Buffer used for displaying Time */
uint8_t aShowTime[3] = {0};
/* Buffer used for displaying Time */
uint8_t aShowDate[4] = {0};


void serial_receive(void)
{
	
	UART.Transmit.Confirmation = CFM;
	UART.Transmit.OpCode1 = serial_data[1];
	UART.Transmit.OpCode2 = serial_data[2];		
	switch (UART.Transmit.OpCode1){
/*---------------------------------------------------------------*/
		case SET_DEVICE_ID :
			UART.Transmit.DataLength = 0;
			for(int i =1 ; i <= RxLength ; i++)
				DeviceData.Device.ID[i-1] = serial_data[i+3];
			UART.Transmit.ErrorByte = flash_write(DEVICE_CONFIG_ADDR, (COUNTOF(DeviceData.Device.ID)), DeviceData.Device.ID,0);
			#ifdef DEBUG_MODE	
				if(UART.Transmit.ErrorByte == 0)
					sprintf(dispvar,"\n Device ID is %s", (char *)DeviceData.Device.ID);
				else if(UART.Transmit.ErrorByte == 1)
					sprintf(dispvar,"\n Flash write Failed");				
				serial_printf(); 
			#endif							
			break;		
/*-----------------------Config RTC------------------------------*/
		case CONFIG_DEVICE :
			UART.Transmit.DataLength = 0;
			switch (UART.Transmit.OpCode2){
				
/*--------------------- 			SET RTC			 ----------------------*/
				case SET_RTC :
					UART.Transmit.ErrorByte = set_date_time(serial_data[4],serial_data[5],serial_data[6],
											serial_data[7],serial_data[8],serial_data[9],serial_data[10]);
		//			temp = RTC_AlarmConfig(RTC_ALARM_A,serial_data[8],serial_data[9]+1,0);
		//			EnterStandBy=1;
					#ifdef DEBUG_MODE	
						if(UART.Transmit.ErrorByte == 0)
							sprintf(dispvar,"\nRTC Configured Successfully");
						else if(UART.Transmit.ErrorByte == 1)
							sprintf(dispvar,"\nRTC Date Setting Failed");
						else if(UART.Transmit.ErrorByte == 2)
							sprintf(dispvar,"\nRTC Time Setting Failed");
						serial_printf(); 
					#endif							
					break;
/*--------------------- SET NUMBER OF MOTORS --------------------*/
				case SET_NUMBER_OF_MOTORS:
					UART.Transmit.DataLength = 0;
					DeviceData.Device.No_of_Mot = serial_data[4];
					UART.Transmit.ErrorByte = flash_write(DEVICE_CONFIG_ADDR+10 /*device Id offset*/, 1,&DeviceData.Device.No_of_Mot,(COUNTOF(DeviceData.Device.ID)));//?why such a large count?
					/* Motor initialization */
					motor_init();
					#ifdef DEBUG_MODE	
						if(UART.Transmit.ErrorByte == 0)
							sprintf(dispvar,"\n No of Motors is %d", (int)DeviceData.Device.No_of_Mot);
						else if(UART.Transmit.ErrorByte == 1)
							sprintf(dispvar,"\n Flash write Failed");				
						serial_printf(); 
					#endif							
					break;		
/*--------------------------------------------------------------*/
			}
			break;
						
/*---------------------------------------------------------------*/	
/*-----------------------Config Patient Details -----------------*/
				
		case CONFIG_PATIENT_DETAIL :
			UART.Transmit.DataLength = 0;
			switch (UART.Transmit.OpCode2){
				
	/*---------------------Config Patient ID ----------------------*/
				case PATIENT_ID:				// Patient ID is 3 bytes out of which 2 bytes are reserved, PatientDetail[0-2]
					for(int i =1 ; i <= RxLength ; i++){
						PatientData.Patient.ID = serial_data[i+3];
					}
					UART.Transmit.ErrorByte = 0;
					#ifdef DEBUG_MODE	
					 if(UART.Transmit.ErrorByte == 0){
						sprintf(dispvar,"\n Patient ID = %d",(uint32_t)PatientData.Patient.ID);
					}
					serial_printf(); 
					#endif	
					break;
	/*---------------------- DOB and Gender------------------------*/					
				case DOB_GENDER:				// DoB and Gender is 4 bytes dd mm yy '(M/F/O)' , PatientDetail[3-6]
					for(int i =1 ; i <= RxLength-1 ; i++){
						PatientData.Patient.DoB[i-1] = (serial_data[i+3]);
					}
					PatientData.Patient.Gender = serial_data[7];
					UART.Transmit.ErrorByte = 0;				
					#ifdef DEBUG_MODE	
 					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"/nDate of Birth %d : %d : %d (ddmmyy)",
							(uint32_t)PatientData.Patient.DoB[0],(uint32_t)PatientData.Patient.DoB[1],
							(uint32_t)PatientData.Patient.DoB[2]);
					serial_printf();
					if(PatientData.Patient.Gender == 'M')
						sprintf(dispvar,"\nGender Male");
					else if(PatientData.Patient.Gender == 'F')
						sprintf(dispvar,"\nGender Female");
					else if(PatientData.Patient.Gender == 'O')
						sprintf(dispvar,"\nGender Others");
					serial_printf(); 
					#endif	
					break;
	/*---------------------- Patient Name------------------------*/					
				case PATIENT_NAME:			// Patient Name is 30 bytes, PatientDetail[7-37] first byte defines the length
					for(int i =1 ; i <= RxLength ; i++){
						PatientData.Patient.Name[i-1]= (serial_data[i+3]);
					}
					UART.Transmit.ErrorByte = 0;						
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient Name %s",(char *)PatientData.Patient.Name);
						serial_printf(); 
					#endif							
					break;
	/*---------------------- Mother's Name------------------------*/					
				case MOTHER_NAME:				// Mother's Name is 30 bytes, PatientDetail[38-68] first byte defines the length
					for(int i =1 ; i <= RxLength ; i++){
						PatientData.Patient.MotherName[i-1] = (serial_data[i+3]);
					}
					UART.Transmit.ErrorByte = 0;
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient Name %s",(char *)PatientData.Patient.MotherName);
						serial_printf(); 
					#endif							
					break;
	/*---------------------- Duration ----------------------------*/										
				case DURATION:				// Duration is 1 byte, in number of days, PatientDetail[69]
					PatientData.Patient.Duration = serial_data[4];
					UART.Transmit.ErrorByte = 0;
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Duration = %d days",(uint32_t)PatientData.Patient.Duration);
						serial_printf(); 
					#endif							
					break;
	/*---------------------- Amount of Distraction ----------------*/										
				case AMT_DIST:				// Amount of Distraction is 1 byte, in mm, PatientDetail[70]
					PatientData.Patient.Amt_of_Distraction = serial_data[4];
					UART.Transmit.ErrorByte = 0;					
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Amount of Distraction = %d mm",(uint32_t)PatientData.Patient.Amt_of_Distraction);
						serial_printf(); 
					#endif							
					break;
	/*--------------------- Interval of Distraction ----------------*/										
				case INTERVAL_DIST:				// Interval of Distraction is 1 byte, in min, PatientDetail[71]
					PatientData.Patient.Interval = serial_data[4];		
					UART.Transmit.ErrorByte = 0;					
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Interval of Distraction = %d min",(uint32_t)PatientData.Patient.Interval);
						serial_printf(); 
					#endif							
					break;
	/*--------------------- Next Appointment Date  ----------------*/										
				case NXT_APP_DATE:			// NAD 3 bytes dd mm yy , PatientDetail[72-74]
					for(int i =1 ; i <= RxLength ; i++){
						PatientData.Patient.NextAppDate[i-1] = (serial_data[i+3]); 
					}					
					UART.Transmit.ErrorByte = 0;					
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Next Appointment with Doctor on %d : %d : %d (ddmmyy)",
						(char)PatientData.Patient.NextAppDate[0],(char)PatientData.Patient.NextAppDate[1],
						(char)PatientData.Patient.NextAppDate[2]);
						serial_printf(); 
					#endif							
					break;
	/*--------------------- Save Patient Details in Flash  ----------------*/										
				case SAVE_TO_FLASH:
					UART.Transmit.ErrorByte = flash_write(PATIENT_DETAILS_ADDR, COUNTOF(PatientData.Buffer),(uint64_t *) &PatientData.Buffer,0);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Flash write Successfully");
					else if(UART.Transmit.ErrorByte == 1)
						sprintf(dispvar,"\n Flash write Failed");
						serial_printf(); 
					#endif							
					break;					
			}
			break;
/*---------------------------Change database------------------------------------*/
case CHANGE_DATABASE :
			break;			
/*------------------------- View System Details -----------------*/
		
		case VIEW_SYSTEM_DETAILS:
			UART.Transmit.ErrorByte = 0;
			switch (UART.Transmit.OpCode2){
/*---------------------------------------------------------------*/
				case VIEW_FIRMWARE_VERSION: 
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Firmware version is %s ",FIRMWARE_VERSION);
						serial_printf(); 
					#else
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &FIRMWARE_VERSION,0,COUNTOF(FIRMWARE_VERSION));
						UART.Transmit.DataLength = (COUNTOF(FIRMWARE_VERSION) - 1);					
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case VIEW_DEVICE_ID:
					flash_read(DEVICE_CONFIG_ADDR, 10,(uint64_t *)&DeviceData.Device.ID);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Device ID is %s", (char *)DeviceData.Device.ID);	
					serial_printf(); 
					#else
						UART.Transmit.DataLength = (COUNTOF(DeviceData.Device.ID));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &DeviceData.Device.ID,0,COUNTOF(DeviceData.Device.ID));			
					#endif					
					break;
/*---------------------------------------------------------------*/	
				case VIEW_NO_OF_MOT:
					flash_read(DEVICE_CONFIG_ADDR+10 /*device Id offset*/, 1,(uint64_t *)&DeviceData.Device.No_of_Mot);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Number of motors is %d", (int)DeviceData.Device.No_of_Mot);	
						serial_printf(); 
				#else
						UART.Transmit.DataLength = 1;
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &DeviceData.Device.No_of_Mot,0,1);			
					#endif					
					break;
/*---------------------------------------------------------------*/						
				case READ_RTC:
					RTC_TimeShow(aShowTime,aShowDate);		
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Date is %02d : %02d : %02d \n Time is %02d : %02d : %02d",
							(uint8_t)aShowDate[2],(uint8_t)aShowDate[1],(uint8_t)aShowDate[0],(uint8_t)aShowTime[0],
							(uint8_t) aShowTime[1],(uint8_t)aShowTime[2] );	
					serial_printf(); 
					#else
						UART.Transmit.DataLength = ((COUNTOF(aShowTime))+(COUNTOF(aShowDate)));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowDate,0,COUNTOF(aShowDate));			
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowTime,COUNTOF(aShowDate),COUNTOF(aShowTime));						
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case VIEW_MODE:		
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Currently the device is running in %s", SysMode[mode] );	
						serial_printf(); 
				#else
						UART.Transmit.DataLength = ((COUNTOF(aShowTime))+(COUNTOF(aShowDate)));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowDate,0,COUNTOF(aShowDate));			
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowTime,COUNTOF(aShowDate),COUNTOF(aShowTime));						
					#endif					
					break;					
/*---------------------------------------------------------------*/					
				case VIEW_ALL_DETAILS:
					flash_read(DEVICE_CONFIG_ADDR, 10,(uint64_t *)&DeviceData.Device.ID);
					flash_read(DEVICE_CONFIG_ADDR+10/*device Id offset*/, 1,(uint64_t *)&DeviceData.Device.No_of_Mot);
					RTC_TimeShow(aShowTime,aShowDate);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Firmware version is %s \n Device ID is %s \n Date is %02d : %02d : %02d \n Time is %02d : %02d : %02d",
							FIRMWARE_VERSION,(char *)DeviceData.Device.ID,(uint8_t)aShowDate[2],
							(uint8_t)aShowDate[1],(uint8_t)aShowDate[0],(uint8_t)aShowTime[0],
							(uint8_t) aShowTime[1],(uint8_t)aShowTime[2] );	
						serial_printf(); 
				#else
						UART.Transmit.DataLength = ((COUNTOF(FIRMWARE_VERSION )-1)+(COUNTOF(DeviceData.Device.ID))+
											(COUNTOF(aShowTime))+(COUNTOF(aShowDate)));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &FIRMWARE_VERSION,0,COUNTOF(FIRMWARE_VERSION));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &DeviceData.Device.ID,COUNTOF(FIRMWARE_VERSION),COUNTOF(DeviceData.Device.ID));				
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowDate,COUNTOF(FIRMWARE_VERSION)+COUNTOF(DeviceData.Device.ID),COUNTOF(aShowDate));			
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowTime,COUNTOF(FIRMWARE_VERSION)+COUNTOF(DeviceData.Device.ID)+COUNTOF(aShowDate),COUNTOF(aShowTime));	
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &DeviceData.Device.No_of_Mot,COUNTOF(FIRMWARE_VERSION)+COUNTOF(DeviceData.Device.ID)+COUNTOF(aShowDate)+COUNTOF(aShowTime),1);	
					#endif					
					break;					
			}			
			break;
/*---------------------------------------------------------------*/
		case VIEW_PATIENT_DETAILS :
			UART.Transmit.ErrorByte = 0;
			switch (UART.Transmit.OpCode2){
				
	/*---------------------View Patient ID ----------------------*/
				case PATIENT_ID:				// Patient ID is 3 bytes out of which 2 bytes are reserved, PatientDetail[0-2]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0){
						sprintf(dispvar,"\n Patient ID = %d",(uint32_t)PatientData.Patient.ID);
						serial_printf(); 
				}
					#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] = (uint8_t)PatientData.Patient.ID;
					#endif	
					break;
	/*---------------------- DOB and Gender------------------------*/					
				case DOB_GENDER:				// DoB and Gender is 4 bytes dd mm yy '(M/F/O)' , PatientDetail[3-6]				
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"/nDate of Birth %d : %d : %d (ddmmyy)",
							(uint32_t)PatientData.Patient.DoB[0],(uint32_t)PatientData.Patient.DoB[1],
							(uint32_t)PatientData.Patient.DoB[2]);
					serial_printf();
					if(PatientData.Patient.Gender == 'M')
						sprintf(dispvar,"\nGender Male");
					else if(PatientData.Patient.Gender == 'F')
						sprintf(dispvar,"\nGender Female");
					else if(PatientData.Patient.Gender == 'O')
						sprintf(dispvar,"\nGender Others");
						serial_printf(); 
				#else
						UART.Transmit.DataLength =  (COUNTOF(PatientData.Patient.DoB) - 1)+1;
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.DoB,0,COUNTOF(PatientData.Patient.DoB));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.Gender,COUNTOF(PatientData.Patient.DoB),1);	
					#endif	
					break;
	/*---------------------- Patient Name------------------------*/					
				case PATIENT_NAME:			// Patient Name is 30 bytes, PatientDetail[7-37] first byte defines the length
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient Name %s",(char *)PatientData.Patient.Name);
						serial_printf(); 
				#else
						UART.Transmit.DataLength = (COUNTOF(PatientData.Patient.Name) - 1);
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.Name,0,COUNTOF(PatientData.Patient.Name));				
					#endif							
					break;
	/*---------------------- Mother's Name------------------------*/					
				case MOTHER_NAME:				// Mother's Name is 30 bytes, PatientDetail[38-68] first byte defines the length
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient Name %s",(char *)PatientData.Patient.MotherName);
						serial_printf(); 
				#else
						UART.Transmit.DataLength = (COUNTOF(PatientData.Patient.MotherName) - 1);
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.MotherName,0,COUNTOF(PatientData.Patient.MotherName));
					#endif							
					break;
	/*---------------------- Duration ----------------------------*/										
				case DURATION:				// Duration is 1 byte, in number of days, PatientDetail[69]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Duration = %d days",(uint32_t)PatientData.Patient.Duration);
						serial_printf(); 
				#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] = (uint8_t) PatientData.Patient.Duration;					
					#endif							
					break;
	/*---------------------- Amount of Distraction ----------------*/										
				case AMT_DIST:				// Amount of Distraction is 1 byte, in mm, PatientDetail[70]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Amount of Distraction = %d mm",(uint32_t)PatientData.Patient.Amt_of_Distraction);
						serial_printf(); 
				#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] =(uint8_t) PatientData.Patient.Amt_of_Distraction;					
					#endif							
					break;
	/*--------------------- Interval of Distraction ----------------*/										
				case INTERVAL_DIST:				// Interval of Distraction is 1 byte, in min, PatientDetail[71]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Interval of Distraction = %d min",(uint32_t)PatientData.Patient.Interval);
						serial_printf(); 
				#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] = (uint8_t)PatientData.Patient.Interval;					
					#endif							
					break;
	/*--------------------- Next Appointment Date  ----------------*/										
				case NXT_APP_DATE:			// NAD 3 bytes dd mm yy , PatientDetail[72-74]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Next Appointment with Doctor on %d : %d :%d (ddmmyy)",
							(uint32_t)PatientData.Patient.NextAppDate[0],(uint32_t)PatientData.Patient.NextAppDate[1],
							(uint32_t)PatientData.Patient.NextAppDate[2]);
						serial_printf(); 
				#else
						UART.Transmit.DataLength = (COUNTOF(PatientData.Patient.NextAppDate) - 1);													
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.NextAppDate,0,COUNTOF(PatientData.Patient.NextAppDate));
					#endif							
					break;
	/*--------------------- Read Patient Details from Flash  ----------------*/										
				case READ_FLASH:
					UART.Transmit.ErrorByte = flash_read(PATIENT_DETAILS_ADDR, 100,(uint64_t *) &PatientData.Patient);
					UART.Transmit.DataLength = 0;
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Flash read Successfully");
					else
						sprintf(dispvar,"\n Flash read Failed");
					serial_printf(); 
					#endif							
					break;					
			}
			break;
/*--------------------------VIEW PROGRESS-------------------------------------*/
		case VIEW_PROGRESS:
			break;
					
/*---------------------------MODES OF OPERATION------------------------------------*/	
		case MODE_COMMANDS:
			UART.Transmit.DataLength = 0;
			switch(UART.Transmit.OpCode2){
/*----------------------------------------------------------------------------------*/
				case ENTER_TEST_MODE:
//					mode=TEST;
				break;
/*----------------------------------------------------------------------------------*/				
				case EXIT_TEST_MODE:
//					mode=MANUAL;
//					ExitTestModeFlag=SET;
					break;
/*----------------------------------------------------------------------------------*/				
				case ENTER_MANUAL_MODE:
//					mode = MANUAL;
					break;

/*----------------------------------------------------------------------------------*/				
				case ENTER_AUTOMATIC_MODE:
//					mode = AUTOMATIC;
					break;
/*----------------------------------------------------------------------------------*/
//        case EXIT_AUTOMATIC_MODE:
//           mode=MANUAL;
//           ExitAutomaticModeFlag=SET;				
/*----------------------------------------------------------------------------------*/				
			}
			break;
/*---------------------------CLEAR------------------------------------*/
		case CLEAR_DATA:
			UART.Transmit.DataLength = 0;
			switch (UART.Transmit.OpCode2){
/*---------------------------------------------------------------*/				
				case CLEAR_SYSTEM_DATA: 
					UART.Transmit.ErrorByte = flash_erase(DEVICE_CONFIG_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n System data cleared ");
					else
						sprintf(dispvar,"\n Flash Erase Failed");
						serial_printf(); 
				#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_PATIENT_DETAILS: 
					UART.Transmit.ErrorByte = flash_erase(PATIENT_DETAILS_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient data cleared ");
					else
						sprintf(dispvar,"\n Flash Erase Failed");
						serial_printf(); 
				#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_PATIENT_LOG_DATA: 
					for(int i = 0 ; i < PATIENT_LOG_PAGES ; i++)
						UART.Transmit.ErrorByte = flash_erase((PATIENT_LOG_ADDR + (i*FLASH_PAGE_SIZE)),1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient Log Cleared ");
					else
						sprintf(dispvar,"\n Flash Erase Failed");
						serial_printf(); 
				#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_EMK_DTATBASE: 
					UART.Transmit.ErrorByte = flash_erase(DATA_BASE_START_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n EMK Database cleared ");
					else
						sprintf(dispvar,"\n Flash Erase Failed");
						serial_printf(); 
				#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_ALL_DATA: 
					#ifdef DEBUG_MODE
						sprintf(dispvar,"\n Please wait till all the data is cleared");
					#endif
					UART.Transmit.ErrorByte = flash_erase(DEVICE_CONFIG_ADDR,1);
					if(UART.Transmit.ErrorByte != 0){
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n System data cleared ");
					else
						sprintf(dispvar,"\n System data Erase Failed");
						serial_printf(); 
				#endif
					}
					UART.Transmit.ErrorByte = flash_erase(PATIENT_DETAILS_ADDR,1);
					if(UART.Transmit.ErrorByte != 0){
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient data cleared ");
					else
						sprintf(dispvar,"\n Patient data Erase Failed");
							serial_printf(); 
			#endif
					}
					for(int i = 0 ; i < PATIENT_LOG_PAGES ; i++)
						UART.Transmit.ErrorByte = flash_erase((PATIENT_LOG_ADDR + (i*FLASH_PAGE_SIZE)),1);
					if(UART.Transmit.ErrorByte != 0){
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n Patient Log Cleared ");
					else
						sprintf(dispvar,"\n Patient Log Erase Failed");
						serial_printf(); 
				#endif
					}
					UART.Transmit.ErrorByte = flash_erase(DATA_BASE_START_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						sprintf(dispvar,"\n EMK Database cleared \n All data cleared ");
					else
						sprintf(dispvar,"\n EMK Database Erase Failed");
						serial_printf(); 
				#endif					
					break;					
			}			
			break;
/*---------------------------------EMERGENCY STOP------------------------------------------*/
			case EMERGENCY_STOP:
//for(SelectMotor=0 ; SelectMotor < MotorMax ; SelectMotor++)
//	{
//		Motor[SelectMotor].ReadMem();
//		if(Motor[SelectMotor].MemVar.isAlive)
//		Motor[SelectMotor].StopMotor();
//	}
//			state = IDLE;
			break;		
/*----------------------------------TERMINATE COMMUNICATION---------------------------------*/
			case TERMINATE_COMMUNICATION:
//			state=IDLE;
			break;
/*----------------------------TEST COMMANDS-----------------------------------*/	
			case TEST_MODE_COMMANDS:
			UART.Transmit.DataLength = 0;
			switch (UART.Transmit.OpCode2){
/*-----------------------------------------------------------------------------*/				
				case TEST_RTC_ALARM:
//					RTC_AlarmConfig(1,0,0,10);
//				      if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BCD) != HAL_OK)
							{
    /* Initialization Error */
//								return 1;
//								sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
//    Error_Handler(); 
							}
//				  HAL_Delay(15000);
//				  if(alarm_works==SET)
//							sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
//						Error_Handler(205);
				  
					break;
/*-----------------------------------------------------------------------------*/
				case TEST_MOTOR_ENCODER_1:
//					__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
//				  SelectMotor=0;
//				  Motor[SelectMotor].StartMotor();
//				  HAL_Delay(1000);
//				  Motor[SelectMotor].StopMotor();
//					count = __HAL_TIM_GET_COUNTER(this->TempVar.Enc_Handler);
//				    if (count==0)
//						{
//							printf("error:encoder[%d] not working",SelectMotor);
//						}
					break;
/*-----------------------------------------------------------------------------*/				
				case TEST_MOTOR_ENCODER_2:
//					__HAL_TIM_SET_COUNTER(&TimHandle_Enc2,0);
//				__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
//				  SelectMotor=1;
//				  Motor[SelectMotor].StartMotor();
//				  HAL_Delay(1000);
//				  Motor[SelectMotor].StopMotor();
//					count = __HAL_TIM_GET_COUNTER(this->TempVar.Enc_Handler);
//				    if (count==0)
//						{
//							printf("error:encoder[%d] not working",SelectMotor);
//						}				
					break;
/*-----------------------------------------------------------------------------*/				
				case TEST_MOTOR_ENCODER_3:
//					__HAL_TIM_SET_COUNTER(&TimHandle_Enc3,0);
//				__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
//				  SelectMotor=2;
//				  Motor[SelectMotor].StartMotor();
//				  HAL_Delay(1000);
//				  Motor[SelectMotor].StopMotor();
//					count = __HAL_TIM_GET_COUNTER(this->TempVar.Enc_Handler);
//				    if (count==0)
//						{
//							printf("error:encoder[%d] not working",SelectMotor);
//						}			
					break;
/*-----------------------------------------------------------------------------*/				
				case TEST_MOTOR_ENCODER_4:
//					__HAL_TIM_SET_COUNTER(&TimHandle_Enc4,0);
//				__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
//				  SelectMotor=3;
//				  Motor[SelectMotor].StartMotor();
//				  HAL_Delay(1000);
//				  Motor[SelectMotor].StopMotor();
//					count = __HAL_TIM_GET_COUNTER(this->TempVar.Enc_Handler);
//				    if (count==0)
//						{
//							printf("error:encoder[%d] not working",SelectMotor);
//						}						 
					break;
/*-----------------------------------------------------------------------------*/				
				case TEST_BUZZER:
					break;
				
			}
			break;
/*-----------------------------MANUAL MODE COMMANDS------------------------------------------------*/
	case MANUAL_MODE_COMMANDS:
				UART.Transmit.DataLength = 0;
			switch (UART.Transmit.OpCode2){
/*-----------------------------------------------------------------------------------------------*/				
        case MANUAL_MODE_PARAMETERS:
					
						break;
				case START_MOTOR:
//					for(SelectMotor=0 ; SelectMotor < MotorMax ; SelectMotor++)
//				{
//							Motor[SelectMotor].ReadMem();
//							if(Motor[SelectMotor].MemVar.isAlive)
//							Motor[SelectMotor].StartMotor();
//					
//				}
					 break;
			}
			break;
/*-------------------------------AUTOMATIC MODE COMMANDS----------------------------------------------*/
	case AUTOMATIC_MODE_COMMANDS:
       UART.Transmit.DataLength = 0;
			switch(UART.Transmit.OpCode2){
/*-----------------------------------------------------------------------------------------------------*/			
				case SET_RTC_FIRST_ALARM:
//					RTC_AlarmConfig(1,0,30,0);
					break;
/*-----------------------------------------------------------------------------------------------------*/				
				case START_DISTRACTION_PROC:
//				if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BCD) != HAL_OK)
//				{
//					 /* Initialization Error */
//							return 1;
//				sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
//					//    Error_Handler(); 
//				}
					break;
/*-----------------------------------------------------------------------------------------------------*/				
				case STOP_DISTRACTION_PROC:
//					HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm)
					break;
				
			}
			break;
/*----------------------------------DEFAULT ERROR-------------------------------------------------------------------*/			
	case DEFAULT_ERROR:
		UART.Transmit.DataLength = 0;
		switch(UART.Transmit.OpCode2){
			case ERR_EXIT_TEST_MODE:
//				if (ExitTestModeFlag!=SET)
//			sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
//					Error_Handler(501);
				break;
//			case ERR_EXIT_MANUAL_MODE:
//				break;
//			case ERR_EXIT_AUTOMATIC_MODE:
//				if (ExitAutomaticModeFlag!=SET)
//			sprintf(errvar,"This is line %d of file %s (function %s)\n", __LINE__, __FILE__, __func__);
//					Error_Handler(502);
				
			case ERR_WRONG_INSTRUCTION:
				break;
			}
				break;	
         					
				
	}
	#ifndef DEBUG_MODE
		uart_transmit((char *)&UART.Buffer,UART.Transmit.DataLength+5);
	#endif
	receive_count = 0;
	RxLength = 0;
	SerialReceiveReady = RESET;
	return;
}

void EnterStandByMode(void)
{
	/* The Following Wakeup sequence is highly recommended prior to each Standby mode entry
		mainly  when using more than one wakeup source this is to not miss any wakeup event.
		- Disable all used wakeup sources,
		- Clear all related wakeup flags, 
		- Re-enable all used wakeup sources,
		- Enter the Standby mode.
	*/
	
	for(SelectMotor=0 ; SelectMotor < MotorMax ; SelectMotor++)
		if(Motor[SelectMotor].MemVar.isAlive)
		{
			Motor[SelectMotor].EraseMem();
			Motor[SelectMotor].WriteMem();
		}
		
	/* Disable all used wakeup sources*/
	//if(mode!=TEST)
	//{	
	HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
	
  /* Disable all used wakeup sources: WKUP pin */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

	/* Clear all related wakeup flags */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	
  /* Clear wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
    
  /* Enable wakeup pin WKUP2 */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW);
	
	/* Enter the Standby mode */
	HAL_PWR_EnterSTANDBYMode();
//}
}

void ExitStandByMode(void)
{
	ExitStandBy = RESET;
	HAL_RTC_AlarmAEventCallback(&RtcHandle);
	/* Check and Clear the Wakeup flag */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF2) != RESET)
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
// TODO: Enter Communication state and turn on BT for 5 mins		
//		STATE = COM;
		// turn on bluetooth for 5 mins
  }
}

void UpdateDemand(void)
{
//	pSa = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);  //Present Encoder Count
//	local_demand = absolute_int(demand - pSa);
//	error = RESET;			
			for(SelectMotor=0;SelectMotor < MotorMax ;SelectMotor++)
			{
				if(Motor[SelectMotor].MemVar.isAlive)
					/* Get the present encoder count for the motor */
					Motor[SelectMotor].getFunc(P);
			}
	for(SelectMotor=0;SelectMotor < MotorMax ;SelectMotor++)
	{
		if(Motor[SelectMotor].MemVar.isAlive)
		{
			Motor[SelectMotor].Data.Demand = absolute_int(demand - Motor[SelectMotor].Data.Position + Motor[SelectMotor].MemVar.PreError);
			Motor[SelectMotor].MemVar.PreError = 0;
			/* Set direction*/
			if(Motor[SelectMotor].Data.Demand < 0)
				Motor[SelectMotor].Data.Direction = REV;
			else
				Motor[SelectMotor].Data.Direction = POS;

			/* Decide control algorithm based on demand */
			if(Motor[SelectMotor].Data.Demand <= TBC_THRESHOLD)
				Motor[SelectMotor].Data.Control = TBC;	// Time Based Control (TBC = 0)
			else
				Motor[SelectMotor].Data.Control = PBC;	// Position Based Control (PBC = 1)
		}
	}
	/* For observation only. Not used in algorithm */
	iteration = RESET;
	
	previous_demand = demand;
		

	isDemandChanged = SET;
	
}

void DemandChanged(void)
{
	/* Interpolation */				
	for(SelectMotor=0;SelectMotor < MotorMax ;SelectMotor++)
		if(Motor[SelectMotor].MemVar.isAlive)
			Motor[SelectMotor].Interpolate();
	
	/* Start Motor */					
	for(SelectMotor=0;SelectMotor < MotorMax ;SelectMotor++)
		if(Motor[SelectMotor].MemVar.isAlive)				
			Motor[SelectMotor].StartMotor();
		
		
	/* If Time based control, the pulse will be on untill local_time equals tOn */
	local_time = 0;

	/* Start timer interrupt */				
	HAL_TIM_Base_Start(&TimHandle_int);
	HAL_TIM_Base_Start_IT(&TimHandle_int);
	
	/* For observation only. Not used in algorithm */
	if(iteration == 0)
		pulse = SET;
	iteration++;
	
	isDemandChanged = RESET;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */

void Error_Handler( uint32_t error_code )
{
	//#ifdef DEBUG_MODE
	  
		sprintf(dispvar,"Error %d",error_code);
		serial_printf(); 
		StringCopy((uint64_t*)dispvar,(uint64_t*)errvar,0,COUNTOF(errvar));
		serial_printf();
	//#endif
  while (1)
  {
  }
}

/*  EMERGENCY STOP BUTTON*/

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == USER_BUTTON_PIN)
  {    
    /* Wait that user release the User push-button */
    while(BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_RESET){}

// TODO:		
// stop motors
// if reinit<3			
			// set Alarm B after 10 min
			// else if reinit>3 <5 set alarm after 1 hr
			ReInit++;
			return;
			// else reinit=0
			//long beep
			// disable rtc alarm
			// stop automatic procedure (enter manual mode and sleep)
			//return;
  }
}

void StringCopy(uint64_t * dest, uint64_t * src, uint32_t start, uint32_t count)
{
	for(int i = 0; i< count; i++)
		dest[i+start] = src[i];
}

uint32_t absolute_int(int32_t number)
{
	if(number < 0)
		return -number;
	else 
		return number;
}

