/*
 * BLDC.h
 *
 *  Created on: Mar 7, 2015
 *      Author: Ninux
 */

#ifndef BLDC_H_
#define BLDC_H_

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "LEDpin2.h"
#include "CLS1.h"
#include "UTIL1.h"

#include "Cpu.h"

#define BLDC_RPM_MIN 0
#define BLDC_RPM_MAX 20000

#define PWM_RATIO_MIN 0
#define PWM_RATIO_MAX 0xFFFF

#define BLDC_PWM_MIN 3276
#define BLDC_PWM_MAX 6554

#define BLDC_PARSE_COMMAND_ENABLED 1 /* set to 1 if method ParseCommand()
					is present, 0 otherwise */

byte BLDC_ParseCommand(const unsigned char *cmd,
		       bool *handled,
		       const CLS1_StdIOType *io);

int BLDC_get_enable(void);

int BLDC_get_rpm(void);

void BLDC_update_task(void *pvParameters);

#endif /* BLDC_H_ */
