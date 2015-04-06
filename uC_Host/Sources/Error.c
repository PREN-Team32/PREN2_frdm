/*
 * Error.c
 *
 *  Created on: Mar 7, 2015
 *      Author: Nino
 */

#include "Error.h"
#include "LedRed.h"
#include "LedGreen.h"
//#include "LED_blue.h"


void set_status(status s)
{
	switch (s) {
	case STATUS_OK:
		LedRed_Off();
		LedGreen_On();
		//LED_blue_Off();
		break;
	case STATUS_BUSY:
		LedRed_Off();
		LedGreen_Off();
		//LED_blue_On();
	case STATUS_ERROR:
		LedRed_On();
		LedGreen_Off();
		//LED_blue_Off();
	case STATUS_RESET:
		LedRed_On();
		LedGreen_On();
		//LED_blue_Off();
		break;
	default:
		LedRed_Off();
		LedGreen_Off();
		//LED_blue_Off();
		break;
	}
}
