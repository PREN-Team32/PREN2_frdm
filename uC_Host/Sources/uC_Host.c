/*
 * Shell.c
 *
 *  Created on: Mar 7, 2015
 *      Author: Nino
 */

#include "FRTOS1.h"
#include "uC_Host.h"
#include "CLS1.h"
#include "LedRed.h"
#include "LedGreen.h"
#include "Error.h"
#include "BLDC.h"
#include "DC.h"
#include "WAIT1.h"
#include "FlyingWheel.h"

static const CLS1_ParseCommandCallback CmdParserTable[] =
{
		CLS1_ParseCommand,
#if LedGreen_PARSE_COMMAND_ENABLED
		LedGreen_ParseCommand,
#endif

#if LedRed_PARSE_COMMAND_ENABLED
		LedRed_ParseCommand,
#endif

#if LedBlue_PARSE_COMMAND_ENABLED
		LedBlue_ParseCommand,
#endif

#if BLDC_PARSE_COMMAND_ENABLED
		BLDC_ParseCommand,
#endif

#if DC_PARSE_COMMAND_ENABLED
		DC_ParseCommand,
#endif

#if FLYING_PARSE_COMMAND_ENABLED
		FlyingWheel_ParseCommand,
#endif
		NULL
};

static void ShellTask(void *pvParameters)
{
	unsigned char buf[48];

	(void)pvParameters;
	buf[0] = '\0';
	(void)CLS1_ParseWithCommandTable((unsigned char*)CLS1_CMD_HELP, CLS1_GetStdio(), CmdParserTable);

	while (1) {
		(void)CLS1_ReadAndParseWithCommandTable(buf, sizeof(buf), CLS1_GetStdio(), CmdParserTable);
		// FRTOS1_vTaskDelay(50/portTICK_RATE_MS);
	}
}

void SHELL_Init(void)
{
	set_status(STATUS_BUSY);
	WAIT1_Waitms(1000);
	CLS1_Init();
	if (FRTOS1_xTaskCreate(
			ShellTask,
			"Shell",
			configMINIMAL_STACK_SIZE+200,
			NULL,
			tskIDLE_PRIORITY,
			NULL ) != pdPASS) {
		while (1) {
			set_status(STATUS_ERROR);
		}
	}
	set_status(STATUS_OK);
}
