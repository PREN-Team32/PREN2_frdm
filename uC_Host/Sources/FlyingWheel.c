/*
 * FlyingWheel.c
 *
 *  Created on: 12.04.2015
 *      Author: studer.yves
 */

#include "FlyingWheel.h"
#include "FRTOS1.h"
#include "BLDC.h"
#include "WAIT1.h"
#include <stdio.h>

static uint8_t PrintHelp(const CLS1_StdIOType *io)
{
	CLS1_SendHelpStr((unsigned char*)"FLYING",
			 (unsigned char*)"Group of FlyingWheel commands\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  FLYING on n",
			 (unsigned char*)"Start the competition with n degree (Basket position)\r\n",
			 io->stdOut);
	return ERR_OK;
}

byte FlyingWheel_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io)
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
	else if (UTIL1_strncmp((char*)cmd, "FLYING on ", sizeof("FLYING on")-1) == 0)
	{
		p = cmd+sizeof("FLYING on");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= FLYING_ANGKE_MIN && val <= FLYING_ANGKE_MAX)
		{
			*handled = TRUE;
			/* Do Something */
			setMotor(BLDC1);
			setSpeed(60000);
			WAIT1_Waitus(5);
			putBLDC(ON);
			WAIT1_Waitus(5);
			setMotor(BLDC2);
			setSpeed(60000);
			WAIT1_Waitus(5);
			putBLDC(ON);
			WAIT1_Waitus(5);
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", FLYING_ANGKE_MIN, FLYING_ANGKE_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}
	return ERR_OK;
}
void FlyingWheel(void)
{

}

void FlyingWheel_init(void)
{
	  if (FRTOS1_xTaskCreate(
			  FlyingWheel_update_task,
			  (signed portCHAR *)"FlyingWheel update",
			  configMINIMAL_STACK_SIZE,
			  (void*)NULL,
			  tskIDLE_PRIORITY,
			  (xTaskHandle*)NULL
			  ) != pdPASS) {
		  while (1) {
			  // out of heap?
		  }
	  }
}


void FlyingWheel_update_task(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		FlyingWheel();
		FRTOS1_vTaskDelay(100/portTICK_RATE_MS);
	}
}
