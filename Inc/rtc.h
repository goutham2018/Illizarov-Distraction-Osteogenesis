#ifndef __RTC__
#define __RTC__

#include "main.h"


//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV    0x7F
#define RTC_SYNCH_PREDIV     0xF9
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif

void rtc_init(void);
int RTC_AlarmConfig(uint32_t alarm_name, uint32_t hour, uint32_t min, uint32_t sec);
int set_date_time(uint8_t year, uint8_t month, uint8_t date, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
void RTC_TimeShow(uint8_t* showtime,uint8_t* showdate); //uint8_t* showtime
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

#endif
