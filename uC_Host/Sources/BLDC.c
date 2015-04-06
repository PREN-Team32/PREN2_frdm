/*
 * BLDC.c
 *
 *  Created on: Mar 7, 2015
 *      Author: Ninux
 */

/* Include shared modules, which are used for whole project */
#include "IO_Map.h"
#include "PWM1.h"
#include "FRTOS1.h"

#include "BLDC.h"
#include "Error.h"
#include "SM1.h"
#include <string.h>
#include <stdio.h>
typedef enum
{
	ON,
	OFF
}MotorState;

typedef struct{
	uint8_t rpmLow    :8;
	uint8_t rpmHigh   :8;
	uint8_t ErrorCode :8;
	MotorState State;
	CLS1_StdIOType io;
}BLDC_MotorState;

static char actualCmd = 0x00;
static BLDC_MotorState BLDC1_Status;

static void BLDC_set_enable(bool status);
static void BLDC_set_rpm(int rpm);
static int get_pwm_ratio(int rpm);
void BLDC_update_FSM(void);

static int BLDC_enable = 0;
static int BLDC_rpm = 0;
static int BLDC_PWM_ratio = 0;

void BLDC_init(void)
{
	BLDC1_Status.ErrorCode = 0x00;
	BLDC1_Status.State = OFF;
	BLDC1_Status.rpmHigh = 0x00;
	BLDC1_Status.rpmLow = 0x00;
}

void spi_onReceived(void)
{
	BLDC_update_FSM();
}

static uint8_t PrintStatus(const CLS1_StdIOType *io)
{
	unsigned char rpm_message[40] = { '\0' };
	unsigned char error_message[10] = { '\0' };
	CLS1_SendStatusStr((unsigned char*)"BLDC_1 Status ist",(unsigned char*)"\r\n", io->stdOut);

	if (BLDC1_Status.State == ON)
	{
		CLS1_SendStatusStr((unsigned char*)"  on", (unsigned char*)"yes\r\n", io->stdOut);
	}
	else
	{
		CLS1_SendStatusStr((unsigned char*)"  on", (unsigned char*)"no\r\n", io->stdOut);
	}
	sprintf(error_message, " %i\r\n", BLDC1_Status.ErrorCode);
	sprintf(rpm_message, "High-Byte = %i, Low_Byte = %i\r\n", BLDC1_Status.rpmHigh, BLDC1_Status.rpmLow);
	CLS1_SendStatusStr((unsigned char*)"  Error code", (unsigned char*)error_message, io->stdOut);
	CLS1_SendStatusStr((unsigned char*)"  RPM", (unsigned char*)rpm_message, io->stdOut);

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
	return ERR_OK;
}

byte BLDC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io)
{
	uint8_t res = ERR_OK;
	unsigned char message[64] = { '\0'};
	int32_t val;
	const unsigned char *p;
	BLDC1_Status.io = *io;

	if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0 || UTIL1_strcmp((char*)cmd, "BLDC help") == 0)
	{
		*handled = TRUE;
		return PrintHelp(io);
	}
	else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS) == 0) || (UTIL1_strcmp((char*)cmd, "BLDC status") == 0))
	{
		*handled = TRUE;
		actualCmd = CMD_GET_STATUS;
		(void) SM1_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "BLDC on") == 0)
	{
		*handled = TRUE;
		actualCmd = CMD_START;
		(void) SM1_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "BLDC off") == 0)
	{
		*handled = TRUE;
		actualCmd = CMD_STOP;
		(void) SM1_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "debug") == 0)
	{
		SM1_TComData tmp;
		*handled = TRUE;
		actualCmd = CMD_ARE_YOU_ALIVE;
		(void) SM1_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "BLDC reset") == 0)
	{
		*handled = TRUE;
		set_status(STATUS_OK);
	}
	else if (UTIL1_strncmp((char*)cmd, "BLDC setrpm ", sizeof("BLDC setrpm")-1) == 0)
	{
		if (!BLDC_enable)
		{
			CLS1_SendStr((unsigned char*)"BLDC is off, cannot set RPM\r\n", io->stdErr);
			res = ERR_FAILED;
		}
		else
		{
			p = cmd+sizeof("BLDC setrpm");
			if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= BLDC_RPM_MIN && val <= BLDC_RPM_MAX)
			{
				BLDC_set_rpm(val);
				*handled = TRUE;
			}
			else
			{
				sprintf(message, "Wrong argument, must be in range %i to %i", BLDC_RPM_MIN, BLDC_RPM_MAX);
				CLS1_SendStr((unsigned char*)message, io->stdErr);
			}
		}
	}
	return ERR_OK;
}

void BLDC_update_FSM(void)
{
	SM1_TComData recv;
	SM1_RecvChar( &recv );
	switch(actualCmd & 0xF0)
	{
	case (CMD_ARE_YOU_ALIVE & 0xF0):
		if(actualCmd == CMD_ARE_YOU_ALIVE - 1)
		{
			/* response to CMD_ARE_YOU_ALIVE */
			if( recv == I_AM_ALIVE )
				CLS1_SendStr("Das Board ist am leben \r\n", *BLDC1_Status.io.stdOut);
		}
		break;
	case (CMD_GET_STATUS & 0xF0):
		if ( actualCmd == CMD_GET_STATUS - 1 )
		{
			/*This is the Motor-state */
			if( recv == 0 || recv == 1 )
				BLDC1_Status.State = OFF;
			if( recv == 2 || recv == 3 )
				BLDC1_Status.State = ON;
		}
		else if ( actualCmd == CMD_GET_STATUS - 2 )
		{
			/*This is the Motor-error code */
			BLDC1_Status.ErrorCode = recv;
		}
		else if ( actualCmd == CMD_GET_STATUS - 3 )
		{
			/*This is the Motor-error code */
			BLDC1_Status.rpmHigh = recv;
		}
		else if ( actualCmd == CMD_GET_STATUS - 4 )
		{
			/*This is the Motor-error code */
			BLDC1_Status.rpmLow = recv;
			PrintStatus(&BLDC1_Status.io);
		}
		break;
	}
	if( (actualCmd & 0x0F) != 0)
	{
		SM1_SendChar(CMD_DUMMY);
		actualCmd--;
	}
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


void DC_update_task(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		PwmLdd2_SetRatio16(PwmLdd2_DeviceData, BLDC_PWM_ratio);
		FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
	}
}

void BLDC_FSM_update_task(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		FRTOS1_vTaskDelay(10/portTICK_RATE_MS);
	}
}
