/*
 * Error.c
 *
 *  Created on: Mar 7, 2015
 *      Author: Nino
 */

#include "Error.h"
#include "LED_red.h"
#include "LED_green.h"
#include "LED_blue.h"


void set_status(status s)
{
	switch (s) {
	case STATUS_OK:
		LED_red_Off();
		LED_green_On();
		LED_blue_Off();
		break;
	case STATUS_BUSY:
		LED_red_Off();
		LED_green_Off();
		LED_blue_On();
	case STATUS_ERROR:
		LED_red_On();
		LED_green_Off();
		LED_blue_Off();
	case STATUS_RESET:
		LED_red_On();
		LED_green_On();
		LED_blue_Off();
		break;
	default:
		LED_red_Off();
		LED_green_Off();
		LED_blue_Off();
		break;
	}
}
