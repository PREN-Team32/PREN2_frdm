
#include "PWM1.h"
#include "FRTOS1.h"
#include "PWM1.h"
#include "DC.h"
#include <stdio.h>


typedef enum
{
	ON,
	OFF
}MotorState;

typedef struct{
	uint8_t pwm;
	MotorState State;
}DC_MotorState;


static void DC_set_rpm(uint8_t pwm_in_Prozent);

static DC_MotorState DC_Status;

static uint8_t PrintHelp(const CLS1_StdIOType *io)
{
	CLS1_SendHelpStr((unsigned char*)"DC",
			 (unsigned char*)"Group of DC commands\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  help",
			 (unsigned char*)"Print help\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  on|off",
			 (unsigned char*)"Turns it on or off\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  setpwm n ",
			 (unsigned char*)"Sets PWM to n\r\n",
			 io->stdOut);
	return ERR_OK;
}

static uint8_t PrintStatus(const CLS1_StdIOType *io)
{
	unsigned char rpm_message[40] = { '\0' };
	CLS1_SendStatusStr((unsigned char*)"DC Status ist",(unsigned char*)"\r\n", io->stdOut);

	if (DC_Status.State == ON)
	{
		CLS1_SendStatusStr((unsigned char*)"  on", (unsigned char*)"yes\r\n", io->stdOut);
	}
	else
	{
		CLS1_SendStatusStr((unsigned char*)"  on", (unsigned char*)"no\r\n", io->stdOut);
	}
	sprintf(rpm_message, "PWM-Wert ist %i\r\n", DC_Status.pwm);
	CLS1_SendStatusStr((unsigned char*)"  RPM", (unsigned char*)rpm_message, io->stdOut);

	return ERR_OK;
}

byte DC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io)
{
	uint8_t res = ERR_OK;
	unsigned char message[64] = { '\0'};
	int32_t val;
	const unsigned char *p;

	if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0 || UTIL1_strcmp((char*)cmd, "DC help") == 0)
	{
		*handled = TRUE;
		return PrintHelp(io);
	}
	else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS) == 0) || (UTIL1_strcmp((char*)cmd, "DC status") == 0))
	{
		*handled = TRUE;
		PrintStatus(io);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "DC on") == 0)
	{
		PWM1_Enable();
		DC_Status.State = ON;
		*handled = TRUE;
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "DC off") == 0)
	{
		PWM1_Disable();
		DC_Status.State = OFF;
		*handled = TRUE;
		return ERR_OK;
	}
	else if (UTIL1_strncmp((char*)cmd, "DC setpwm ", sizeof("DC setpwm")-1) == 0)
	{
		p = cmd+sizeof("DC setpwm");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= DC_PWM_MIN && val <= DC_PWM_MAX)
		{
			*handled = TRUE;
			DC_Status.pwm =(uint8_t) val;
			DC_set_rpm(val);
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", DC_PWM_MIN, DC_PWM_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}
	return ERR_OK;
}

static void DC_set_rpm(uint8_t pwm_in_Prozent)
{
	uint8_t tmp = 0xFFFF/100*pwm_in_Prozent;
	PWM1_SetRatio16(tmp);
}

void DC_update_task(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
//		PwmLdd2_SetRatio16(PwmLdd2_DeviceData, BLDC_PWM_ratio);
		FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
	}
}

void DC_init(void)
{
	DC_Status.pwm = 75;
	DC_Status.State = OFF;
}
