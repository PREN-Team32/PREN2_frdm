/*
 * BLDC.c
 *
 *  Created on: Mar 7, 2015
 *      Author: Ninux
 */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PWM1.h"
#include "PwmLdd2.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "WAIT1.h"
/* Include inherited beans */
#include "LEDpin2.h"
#include "CLS1.h"
#include "UTIL1.h"

#include "BLDC.h"
#include "LED_red.h"
#include "LED_green.h"
#include "LED_blue.h"
#include "Error.h"
#include "SM1.h"
#include <string.h>
#include <stdio.h>

#define BLDC_ENABLE_DEFAULT 0
#define BLDC_RPM_DEFAULT 0

static void BLDC_set_enable(bool status);
static void BLDC_set_rpm(int rpm);
static int get_pwm_ratio(int rpm);
static void BLDC_reset(void);

static int BLDC_enable = 0;
static int BLDC_rpm = 0;
static int BLDC_PWM_ratio = 0;

static uint8_t PrintStatus(const CLS1_StdIOType *io)
{
	unsigned char rpm_message[12] = { '\0' };
	CLS1_SendStatusStr((unsigned char*)"BLDC",
			   (unsigned char*)"\r\n", io->stdOut);

	if (BLDC_get_enable()!=0) {
		CLS1_SendStatusStr((unsigned char*)"  on",
				   (unsigned char*)"yes\r\n",
				   io->stdOut);
	} else {
		CLS1_SendStatusStr((unsigned char*)"  on",
				   (unsigned char*)"no\r\n",
				   io->stdOut);
	}

	sprintf(rpm_message, "%i\r\n", BLDC_rpm);
	CLS1_SendStatusStr((unsigned char*)"  RPM",
			   (unsigned char*)rpm_message,
			   io->stdOut);

	return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io)
{
	CLS1_SendHelpStr((unsigned char*)"BLDC",
			 (unsigned char*)"Group of BLDC commands\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  help|status",
			 (unsigned char*)"Print help or status information\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  on|off",
			 (unsigned char*)"Turns it on or off\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  setrpm n",
			 (unsigned char*)"Sets RPM to n\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  reset",
			 (unsigned char*)"Reset to initial setup\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  init",
			 (unsigned char*)"Initialize motor-control\r\n",
			 io->stdOut);
	return ERR_OK;
}

byte BLDC_ParseCommand(const unsigned char *cmd,
		       bool *handled,
		       const CLS1_StdIOType *io)
{
	uint8_t res = ERR_OK;
	unsigned char message[64] = { '\0'};
	int32_t val;
	const unsigned char *p;

	if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0
	    || UTIL1_strcmp((char*)cmd, "BLDC help") == 0) {
		*handled = TRUE;
		return PrintHelp(io);
	} else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS) == 0)
		   || (UTIL1_strcmp((char*)cmd, "BLDC status") == 0)) {
		*handled = TRUE;
		return PrintStatus(io);
	} else if (UTIL1_strcmp((char*)cmd, "BLDC on") == 0) {
		*handled = TRUE;
		(void) SM1_SendChar(0x10);
		return ERR_OK;
	} else if (UTIL1_strcmp((char*)cmd, "BLDC off") == 0) {
		*handled = TRUE;
		(void) SM1_SendChar(0x20);
		return ERR_OK;
	} else if (UTIL1_strcmp((char*)cmd, "debug") == 0) {
		SM1_TComData tmp;
		*handled = TRUE;
		(void) SM1_SendChar(0x71);
		(void) SM1_SendChar(0x00);
		(void) SM1_RecvChar(&tmp);
		if( tmp == 0x55)
			set_status(STATUS_ERROR);

		return ERR_OK;
	}else if (UTIL1_strcmp((char*)cmd, "BLDC reset") == 0) {
		*handled = TRUE;
		set_status(STATUS_OK);
	} else if (UTIL1_strncmp((char*)cmd, "BLDC setrpm ",
				 sizeof("BLDC setrpm")-1) == 0) {
		if (!BLDC_enable) {
			CLS1_SendStr((unsigned char*)"BLDC is off, cannot set RPM\r\n",
				     io->stdErr);
			res = ERR_FAILED;
		} else {
			p = cmd+sizeof("BLDC setrpm");
			if (UTIL1_xatoi(&p, &val) == ERR_OK
			    && val >= BLDC_RPM_MIN && val <= BLDC_RPM_MAX) {
				BLDC_set_rpm(val);
				*handled = TRUE;
			} else {
				sprintf(message,
					"Wrong argument, must be in range %i to %i",
					BLDC_RPM_MIN, BLDC_RPM_MAX);
				CLS1_SendStr((unsigned char*)message,
					     io->stdErr);
			}
		}
	} else if (UTIL1_strcmp((char*)cmd, "BLDC init") == 0) {
		*handled = TRUE;
	}
	return ERR_OK;
}

int BLDC_get_enable(void)
{
	return BLDC_enable;
}

static void BLDC_set_enable(bool status)
{
	if (status) {
		BLDC_enable = status;
		BLDC_PWM_ratio = get_pwm_ratio(BLDC_rpm);
	} else {
		BLDC_enable = status;
		BLDC_PWM_ratio = get_pwm_ratio(0);
	}
}

int BLDC_get_rpm(void)
{
	return BLDC_rpm;
}

static void BLDC_set_rpm(int rpm)
{
	BLDC_rpm = rpm;
	BLDC_PWM_ratio = get_pwm_ratio(rpm);
}


static int get_pwm_ratio(int rpm)
{
	return ((BLDC_PWM_MAX - BLDC_PWM_MIN) / (float)BLDC_RPM_MAX)*rpm
		+ BLDC_PWM_MIN;
}

void BLDC_reset(void)
{
	BLDC_set_enable(BLDC_ENABLE_DEFAULT);
	BLDC_set_rpm(BLDC_RPM_DEFAULT);
}

void BLDC_update_task(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		PwmLdd2_SetRatio16(PwmLdd2_DeviceData, BLDC_PWM_ratio);
		FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
	}
}
