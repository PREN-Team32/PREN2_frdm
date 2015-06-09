
#include "DC.h"
#include "PWM_DC.h"
#include "FRTOS1.h"
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


static void DC_set_pwm(uint8_t pwm_in_Prozent);
static void HandleDcMotor(void *pvParameters);

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
		DC_set_pwm(DC_Status.pwm);
		DC_Status.State = ON;
		*handled = TRUE;
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "DC off") == 0)
	{
		DC_set_pwm(1);
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
			DC_Status.State = ON;
			DC_Status.pwm = (uint8_t) val;
			DC_set_pwm(DC_Status.pwm);
//			if (FRTOS1_xTaskCreate(HandleDcMotor, "DC-Motor handler", configMINIMAL_STACK_SIZE, NULL,
//		            tskIDLE_PRIORITY, NULL) != pdPASS) {
//		    for (;;) {
//		    }
//		    }
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", DC_PWM_MIN, DC_PWM_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}
	return ERR_OK;
}

static void DC_set_pwm(uint8_t pwm_in_Prozent)
{
	uint16_t tmp = (uint16_t)(0xFFFF/100)*(uint16_t)pwm_in_Prozent;
	PWM_DC_SetRatio16(100-tmp);
}

static void HandleDcMotor(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		if(DC_Status.State == ON)
		{
			DC_Status.pwm -= 15;
//			FRTOS1_vTaskDelay(100/portTICK_RATE_MS);// portTICK_RATE_MS = 1000/100
			if(DC_Status.pwm <= 85)
			{
				DC_Status.pwm -= 10;
//				FRTOS1_vTaskDelay(200/portTICK_RATE_MS);// portTICK_RATE_MS = 1000/100
			}

			if(DC_Status.pwm <= 35 )
			{
				DC_Status.pwm = 35;
//				FRTOS1_vTaskDelay(400/portTICK_RATE_MS);// portTICK_RATE_MS = 1000/100
			}
			DC_set_pwm(DC_Status.pwm);
		}
//		if(DC_Status.State == OFF)
//		{
////			FRTOS1_vTaskDelay(400/portTICK_RATE_MS);// portTICK_RATE_MS = 1000/100
//			DC_set_pwm(1);
//		}
		FRTOS1_vTaskDelay(200/portTICK_RATE_MS);// portTICK_RATE_MS = 1000/100
	}
}


void DC_init(void)
{
	DC_Status.pwm = 1;
	DC_set_pwm(DC_Status.pwm);
	DC_Status.State = OFF;
}
