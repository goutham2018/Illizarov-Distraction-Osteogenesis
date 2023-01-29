#ifndef __SYS_INIT__
#define __SYS_INIT__

#include "main.h"
#include "flash.h"
#include "rtc.h"

#define Weight1 1
#define Weight2 (10-Weight1)
#define TBC_THRESHOLD 200


void System_Initialise(void);
void sys_config(void);
void SystemPower_Config(void);
void GPIO_Init(GPIO_TypeDef* temp_port, uint32_t temp_pin, uint32_t temp_mode, uint32_t temp_pull, uint32_t temp_speed, uint32_t temp_af);

#endif
