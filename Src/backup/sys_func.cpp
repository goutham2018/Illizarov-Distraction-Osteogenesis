#include "serial_receive.h"
#include "sys_func.h"
#include <string.h>
#include <math.h>
extern TIM_HandleTypeDef    TimHandle_int;
extern TIM_HandleTypeDef    TimHandle_Enc1;
extern TIM_HandleTypeDef    TimHandle_Enc2;
extern TIM_HandleTypeDef    TimHandle_Enc3;
extern TIM_HandleTypeDef    TimHandle_Enc4;
extern UART_HandleTypeDef UartHandle;
extern RTC_HandleTypeDef RtcHandle;

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
		uint64_t ID[10];
		uint64_t RTC_Data[7];
		uint64_t BatStat;		
	}Device;
	uint64_t Buffer[10];
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
extern uint32_t data_index2,data_index1;
uint32_t local_demand=0,previous_demand=0, demand = 0,SelectMotor;
extern uint64_t emk[EMK_Length][3];//, local_db[EMK_Length][3];
int32_t error = 0;
double tOn1=0, tOn2=0, pOn1=0, pOn2=0, pS1=0, pS2=0;
float PBC_PCC = 1,TBC_PCC = 1;
uint32_t pOn=0, tOn=0, pS=0, pSa;
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
		case CONFIG_DEVICE_ID :
			UART.Transmit.DataLength = 0;
			for(int i =1 ; i <= RxLength ; i++)
				DeviceData.Device.ID[i-1] = serial_data[i+3];
			UART.Transmit.ErrorByte = flash_write(DEVICE_CONFIG_ADDR, (COUNTOF(DeviceData.Device.ID)), DeviceData.Device.ID,0);
			#ifdef DEBUG_MODE	
				if(UART.Transmit.ErrorByte == 0)
					printf("\n Device ID is %s", (char *)DeviceData.Device.ID);
				else if(UART.Transmit.ErrorByte == 1)
					printf("\n Flash write Failed");				
			#endif							
			break;		
/*-----------------------Config RTC------------------------------*/
		case CONFIG_RTC :
			UART.Transmit.DataLength = 0;
			UART.Transmit.ErrorByte = set_date_time(serial_data[4],serial_data[5],serial_data[6],
									serial_data[7],serial_data[8],serial_data[9],serial_data[10]);
//			temp = RTC_AlarmConfig(RTC_ALARM_A,serial_data[8],serial_data[9]+1,0);
//			EnterStandBy=1;
			#ifdef DEBUG_MODE	
				if(UART.Transmit.ErrorByte == 0)
					printf("\nRTC Configured Successfully");
				else if(UART.Transmit.ErrorByte == 1)
					printf("\nRTC Date Setting Failed");
				else if(UART.Transmit.ErrorByte == 2)
					printf("\nRTC Time Setting Failed");
			#endif							
			break;
/*---------------------------------------------------------------*/	
/*-----------------------Config Patient Details -----------------*/
				
		case CONFIG_DEVICE :
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
						printf("\n Patient ID = %d",(uint32_t)PatientData.Patient.ID);
					}
					#endif	
					break;
	/*---------------------- DOB and Gender------------------------*/					
				case DOB_GENDER:				// DoB and Gender is 4 bytes dd mm yy '(M/F/O)' , PatientDetail[3-6]
					for(int i =1 ; i <= RxLength-1 ; i++){
						PatientData.Patient.DoB[i-1] = (serial_data[i+3]);
					}
					PatientData.Patient.Gender = serial_data[6];
					UART.Transmit.ErrorByte = 0;				
					#ifdef DEBUG_MODE	
 					if(UART.Transmit.ErrorByte == 0)
						printf("/nDate of Birth %d : %d : %d (ddmmyy)",
							(uint32_t)PatientData.Patient.DoB[0],(uint32_t)PatientData.Patient.DoB[1],
							(uint32_t)PatientData.Patient.DoB[2]);
					if(PatientData.Patient.Gender == 'M')
						printf("\nGender Male");
					else if(PatientData.Patient.Gender == 'F')
						printf("\nGender Female");
					else if(PatientData.Patient.Gender == 'O')
						printf("\nGender Others");
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
						printf("\n Patient Name %s",(char *)PatientData.Patient.Name);
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
						printf("\n Patient Name %s",(char *)PatientData.Patient.MotherName);
					#endif							
					break;
	/*---------------------- Duration ----------------------------*/										
				case DURATION:				// Duration is 1 byte, in number of days, PatientDetail[69]
					PatientData.Patient.Duration = serial_data[4];
					UART.Transmit.ErrorByte = 0;
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Duration = %d days",(uint32_t)PatientData.Patient.Duration);
					#endif							
					break;
	/*---------------------- Amount of Distraction ----------------*/										
				case AMT_DIST:				// Amount of Distraction is 1 byte, in mm, PatientDetail[70]
					PatientData.Patient.Amt_of_Distraction = serial_data[4];
					UART.Transmit.ErrorByte = 0;					
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Amount of Distraction = %d mm",(uint32_t)PatientData.Patient.Amt_of_Distraction);
					#endif							
					break;
	/*--------------------- Interval of Distraction ----------------*/										
				case INTERVAL_DIST:				// Interval of Distraction is 1 byte, in min, PatientDetail[71]
					PatientData.Patient.Interval = serial_data[4];		
					UART.Transmit.ErrorByte = 0;					
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Interval of Distraction = %d min",(uint32_t)PatientData.Patient.Interval);
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
						printf("\n Next Appointment with Doctor on %d : %d : %d (ddmmyy)",
						(char)PatientData.Patient.NextAppDate[0],(char)PatientData.Patient.NextAppDate[1],
						(char)PatientData.Patient.NextAppDate[2]);
					#endif							
					break;
	/*--------------------- Save Patient Details in Flash  ----------------*/										
				case SAVE_TO_FLASH:
					UART.Transmit.ErrorByte = flash_write(PATIENT_DETAILS_ADDR, COUNTOF(PatientData.Buffer),(uint64_t *) &PatientData.Buffer,0);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Flash write Successfully");
					else if(UART.Transmit.ErrorByte == 1)
						printf("\n Flash write Failed");
					#endif							
					break;					
			}
			break;
/*---------------------------------------------------------------*/	
/*------------------------- View System Details -----------------*/
			
		case VIEW_SYSTEM_DETAILS:
			UART.Transmit.ErrorByte = 0;
			switch (UART.Transmit.OpCode2){
/*---------------------------------------------------------------*/
				case VIEW_FIRMWARE_VERSION: 
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Firmware version is %s ",FIRMWARE_VERSION);
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
						printf("\n Device ID is %s", (char *)DeviceData.Device.ID);	
					#else
						UART.Transmit.DataLength = (COUNTOF(DeviceData.Device.ID));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &DeviceData.Device.ID,0,COUNTOF(DeviceData.Device.ID));			
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case READ_RTC:
					RTC_TimeShow(aShowTime,aShowDate);		
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Date is %02d : %02d : %02d \n Time is %02d : %02d : %02d",
							(uint8_t)aShowDate[2],(uint8_t)aShowDate[1],(uint8_t)aShowDate[0],(uint8_t)aShowTime[0],
							(uint8_t) aShowTime[1],(uint8_t)aShowTime[2] );	
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
						printf("\n Currently the device is running in %s", SysMode[mode] );	
					#else
						UART.Transmit.DataLength = ((COUNTOF(aShowTime))+(COUNTOF(aShowDate)));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowDate,0,COUNTOF(aShowDate));			
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowTime,COUNTOF(aShowDate),COUNTOF(aShowTime));						
					#endif					
					break;					
/*---------------------------------------------------------------*/					
				case VIEW_ALL_DETAILS:
					flash_read(DEVICE_CONFIG_ADDR, 10,(uint64_t *)&DeviceData.Device.ID);
					RTC_TimeShow(aShowTime,aShowDate);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Firmware version is %s \n Device ID is %s \n Date is %02d : %02d : %02d \n Time is %02d : %02d : %02d",
							FIRMWARE_VERSION,(char *)DeviceData.Device.ID,(uint8_t)aShowDate[2],
							(uint8_t)aShowDate[1],(uint8_t)aShowDate[0],(uint8_t)aShowTime[0],
							(uint8_t) aShowTime[1],(uint8_t)aShowTime[2] );	
					#else
						UART.Transmit.DataLength = ((COUNTOF(FIRMWARE_VERSION )-1)+(COUNTOF(DeviceData.Device.ID))+
											(COUNTOF(aShowTime))+(COUNTOF(aShowDate)));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &FIRMWARE_VERSION,0,COUNTOF(FIRMWARE_VERSION));
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &DeviceData.Device.ID,COUNTOF(FIRMWARE_VERSION),COUNTOF(DeviceData.Device.ID));				
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowDate,COUNTOF(DeviceData.Device.ID),COUNTOF(aShowDate));			
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &aShowTime,COUNTOF(aShowDate),COUNTOF(aShowTime));	
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
						printf("\n Patient ID = %d",(uint32_t)PatientData.Patient.ID);
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
						printf("/nDate of Birth %d : %d : %d (ddmmyy)",
							(uint32_t)PatientData.Patient.DoB[0],(uint32_t)PatientData.Patient.DoB[1],
							(uint32_t)PatientData.Patient.DoB[2]);
					if(PatientData.Patient.Gender == 'M')
						printf("\nGender Male");
					else if(PatientData.Patient.Gender == 'F')
						printf("\nGender Female");
					else if(PatientData.Patient.Gender == 'O')
						printf("\nGender Others");
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
						printf("\n Patient Name %s",(char *)PatientData.Patient.Name);
					#else
						UART.Transmit.DataLength = (COUNTOF(PatientData.Patient.Name) - 1);
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.Name,0,COUNTOF(PatientData.Patient.Name));				
					#endif							
					break;
	/*---------------------- Mother's Name------------------------*/					
				case MOTHER_NAME:				// Mother's Name is 30 bytes, PatientDetail[38-68] first byte defines the length
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Patient Name %s",(char *)PatientData.Patient.MotherName);
					#else
						UART.Transmit.DataLength = (COUNTOF(PatientData.Patient.MotherName) - 1);
						StringCopy((uint64_t *) &UART.Transmit.Data,(uint64_t *) &PatientData.Patient.MotherName,0,COUNTOF(PatientData.Patient.MotherName));
					#endif							
					break;
	/*---------------------- Duration ----------------------------*/										
				case DURATION:				// Duration is 1 byte, in number of days, PatientDetail[69]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Duration = %d days",(uint32_t)PatientData.Patient.Duration);
					#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] = (uint8_t) PatientData.Patient.Duration;					
					#endif							
					break;
	/*---------------------- Amount of Distraction ----------------*/										
				case AMT_DIST:				// Amount of Distraction is 1 byte, in mm, PatientDetail[70]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Amount of Distraction = %d mm",(uint32_t)PatientData.Patient.Amt_of_Distraction);
					#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] =(uint8_t) PatientData.Patient.Amt_of_Distraction;					
					#endif							
					break;
	/*--------------------- Interval of Distraction ----------------*/										
				case INTERVAL_DIST:				// Interval of Distraction is 1 byte, in min, PatientDetail[71]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Interval of Distraction = %d min",(uint32_t)PatientData.Patient.Interval);
					#else
						UART.Transmit.DataLength = 1;
						UART.Transmit.Data[0] = (uint8_t)PatientData.Patient.Interval;					
					#endif							
					break;
	/*--------------------- Next Appointment Date  ----------------*/										
				case NXT_APP_DATE:			// NAD 3 bytes dd mm yy , PatientDetail[72-74]
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Next Appointment with Doctor on %d : %d :%d (ddmmyy)",
							(uint32_t)PatientData.Patient.NextAppDate[0],(uint32_t)PatientData.Patient.NextAppDate[1],
							(uint32_t)PatientData.Patient.NextAppDate[2]);
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
						printf("\n Flash read Successfully");
					else
						printf("\n Flash read Failed");
					#endif							
					break;					
			}
			break;
/*---------------------------------------------------------------*/
		case CLEAR_DATA:
			UART.Transmit.DataLength = 0;
			switch (UART.Transmit.OpCode2){
/*---------------------------------------------------------------*/				
				case CLEAR_SYSTEM_DATA: 
					UART.Transmit.ErrorByte = flash_erase(DEVICE_CONFIG_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n System data cleared ");
					else
						printf("\n Flash Erase Failed");
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_PATIENT_DETAILS: 
					UART.Transmit.ErrorByte = flash_erase(PATIENT_DETAILS_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Patient data cleared ");
					else
						printf("\n Flash Erase Failed");
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_PATIENT_LOG_DATA: 
					for(int i = 0 ; i < PATIENT_LOG_PAGES ; i++)
						UART.Transmit.ErrorByte = flash_erase((PATIENT_LOG_ADDR + (i*FLASH_PAGE_SIZE)),1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Patient Log Cleared ");
					else
						printf("\n Flash Erase Failed");
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_EMK_DTATBASE: 
					UART.Transmit.ErrorByte = flash_erase(DATA_BASE_START_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n EMK Database cleared ");
					else
						printf("\n Flash Erase Failed");
					#endif					
					break;
/*---------------------------------------------------------------*/					
				case CLEAR_ALL_DATA: 
					#ifdef DEBUG_MODE
						printf("\n Please wait till all the data is cleared");
					#endif
					UART.Transmit.ErrorByte = flash_erase(DEVICE_CONFIG_ADDR,1);
					if(UART.Transmit.ErrorByte != 0){
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n System data cleared ");
					else
						printf("\n System data Erase Failed");
					#endif
					}
					UART.Transmit.ErrorByte = flash_erase(PATIENT_DETAILS_ADDR,1);
					if(UART.Transmit.ErrorByte != 0){
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Patient data cleared ");
					else
						printf("\n Patient data Erase Failed");
					#endif
					}
					for(int i = 0 ; i < PATIENT_LOG_PAGES ; i++)
						UART.Transmit.ErrorByte = flash_erase((PATIENT_LOG_ADDR + (i*FLASH_PAGE_SIZE)),1);
					if(UART.Transmit.ErrorByte != 0){
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n Patient Log Cleared ");
					else
						printf("\n Patient Log Erase Failed");
					#endif
					}
					UART.Transmit.ErrorByte = flash_erase(DATA_BASE_START_ADDR,1);
					#ifdef DEBUG_MODE	
					if(UART.Transmit.ErrorByte == 0)
						printf("\n EMK Database cleared \n All data cleared ");
					else
						printf("\n EMK Database Erase Failed");
					#endif					
					break;					
			}			
			break;			
/*---------------------------------------------------------------*/			
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
	/* Disable all used wakeup sources*/
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
}

void ExitStandByMode(void)
{
	ExitStandBy = RESET;
	/* Check and Clear the Wakeup flag */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF2) != RESET)
  {
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
//		STATE = COM;
		// turn on bluetooth for 5 mins
  }
}

void UpdateDemand(void)
{
	pSa = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);  //Present Encoder Count
	local_demand = absolute1(demand - pSa);
	error = RESET;			
	/* Set direction*/
	if(demand < pSa)
		dir = REV;
	else
		dir = POS;

	/* Decide control algorithm based on demand */
	if(local_demand <= TBC_THRESHOLD)
		control = TBC;	// Time Based Control (TBC = 0)
	else
		control = PBC;	// Position Based Control (PBC = 1)
	
	/* For observation only. Not used in algorithm */
	iteration = RESET;
	
	previous_demand = demand;
	isDemandChanged = SET;
	
}

void DemandChanged(void)
{
				/* Search the data base and get previous and immediate next data points */
				for(data_index1=0;data_index1<EMK_Length;data_index1++)
					if(emk[data_index1][2] > local_demand )
						break;
					
		/*
			 /\		|	
			/	 \	|
			 ||		|
			 ||		|
		position|
						|
				 pS	|_________________________________________._._._._.
						|  				        											 '
						|				       												'
						|				    												'
						|				 													 '
						|				 													'---------> dynamic positon of the motor 
						|         	 									 	'
						|			       									 '
						|			     									 '
						|		   	    								'
						|_______________________ _'_________\ pOn
						|	            					|						/
						|		      						' |
						|             	    ' 	|
						|          		    '    	|-----> pulse to turn on the motor 	
						|	    				'       	|
						|      		 '           	|
						|   	 '              	|
						|		'                  	|
						|_______________________|_____________________________
						|<---------tOn--------->|											 t->
					
					pOn -> motor position at the moment when pulse is turned off
					tOn -> duration for which motor is turned on 
					pS  -> motor settling position
		*/
				/* Get previous values for interpolation */
				tOn1 = (double)(emk[data_index1-1][0]) ; 	// Previous tOn
				tOn2 = (double)(emk[data_index1][0])   ;	 	// Next tOn
				pOn1 = (double)(emk[data_index1-1][1]) ;		// Previous pOn
				pOn2 = (double)(emk[data_index1][1])   ;		// Next pOn	
				pS1  = (double)(emk[data_index1-1][2]) ;		// Previous pS
				pS2  = (double)(emk[data_index1][2])   ;		// Next pS
				
				/* Interpolation */
				pOn  = (uint32_t)(pOn1 + (pOn2 - pOn1)*((((double)(local_demand)) - pS1)/(pS2 - pS1))); // Predict pOn
				tOn  = (uint32_t)(tOn1 + (tOn2 - tOn1)*((((double)(local_demand)) - pS1)/(pS2 - pS1)));	// Predict tOn
				
				/* Adaptation */
				pOn = (uint32_t)(((float)pOn)*PBC_PCC);
				tOn = (uint32_t)(((float)tOn)*TBC_PCC);			
				
				/* Define pOn depending on direction */
				if(dir == POS)
					pOn = pSa + pOn;
				else
					pOn = pSa - pOn;
				
				/* If Time based control, the pulse will be on untill local_time equals tOn */
				local_time = 0;
				
				SelectMotor =A;
				Motor[SelectMotor].StartMotor();

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
	#ifdef DEBUG_MODE
		printf("Error %d",error_code);
	#endif
  while (1)
  {
  }
}

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

// stop motors
// if reinit<3			
			// set alarm after 10 min
			// else if reinit>3 <5 set alarm after 1 hr
			ReInit++;
			return;
			// else reinit=0
			//long beep
			// disable rtc alarm
			// stop procedure
			//return;
  }
}

void StringCopy(uint64_t * dest, uint64_t * src, uint32_t start, uint32_t count)
{
	for(int i = 0; i< count; i++)
		dest[i+start] = src[i];
}

uint32_t absolute1(int32_t number)
{
	if(number < 0)
		return -number;
	else 
		return number;
}

