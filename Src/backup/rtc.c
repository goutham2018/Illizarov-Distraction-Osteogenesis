#include "rtc.h"

	/* RTC handler declaration */
	RTC_HandleTypeDef RtcHandle;

/* Configure the RTC peripheral #######################################*/

  RTC_DateTypeDef  sdatestructure;
  RTC_TimeTypeDef  stimestructure;
  RTC_AlarmTypeDef salarmstructure;



void rtc_init(void)
{
	 RtcHandle.Instance = RTC;
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
   /* Initialization Error */
		Error_Handler(201);
  }
}

int RTC_AlarmConfig(uint32_t alarm_name, uint32_t hour, uint32_t min, uint32_t sec)
{
 
  /* Configure the RTC Alarm peripheral #################################*/
  /* RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
  salarmstructure.Alarm = alarm_name;
  salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
  salarmstructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  salarmstructure.AlarmTime.Hours = hour;
  salarmstructure.AlarmTime.Minutes = min;
  salarmstructure.AlarmTime.Seconds = sec;
  salarmstructure.AlarmTime.SubSeconds = 0x00;
  
  if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
		return 1;
//    Error_Handler(); 
  }
	return 0;
}

int set_date_time(uint8_t year, uint8_t month, uint8_t date, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
  /*##-1- Configure the Date #################################################*/
  sdatestructure.Year    = year  ;
  sdatestructure.Month   = month ;
  sdatestructure.Date    = date  ;
  sdatestructure.WeekDay = day   ;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
		return 1;
//    Error_Handler(); 
  } 
  
  /*##-2- Configure the Time #################################################*/
  stimestructure.Hours          = hour                     ;
  stimestructure.Minutes        = min                      ;
  stimestructure.Seconds        = sec                      ;
  stimestructure.TimeFormat     = RTC_HOURFORMAT12_AM      ;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE  ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET ;
  
  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
   /* Initialization Error */
		return 2;
//    Error_Handler(); 
  }  
	return 0;
}
/**
  * @brief  Display the current time.
  * @param  showtime : pointer to buffer
  * @retval None
  */
void RTC_TimeShow(uint8_t* showtime,uint8_t* showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	showdate[0]=sdatestructureget.Year;
	showdate[1]=sdatestructureget.Month;
	showdate[2]=sdatestructureget.Date;
	showdate[3]=sdatestructureget.WeekDay;

	showtime[0]=stimestructureget.Hours;
	showtime[1]=stimestructureget.Minutes;
	showtime[2]=stimestructureget.Seconds;
//	/* Display date Format : yy:mm:dd */
//	sprintf((char*)showdate,"%d%d%d%d",sdatestructureget.Year,sdatestructureget.Month,sdatestructureget.Date,sdatestructureget.WeekDay);
//  /* Display time Format : hh:mm:ss */
//  sprintf((char*)showtime,"%d%d%d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
} 

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{

}

