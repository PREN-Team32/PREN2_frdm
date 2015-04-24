/* ###################################################################
**     Filename    : main.c
**     Project     : uC_Host
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-03-06, 09:12, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "LedGreen.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
#include "LedRed.h"
#include "LEDpin2.h"
#include "BitIoLdd2.h"
#include "CLS1.h"
#include "WAIT1.h"
#include "CS1.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "RxBuf1.h"
#include "PWM_DC.h"
#include "PwmLdd3.h"
#include "TU2.h"
#include "CS_BLDC1.h"
#include "BitIoLdd4.h"
#include "CS_BLDC2.h"
#include "BitIoLdd5.h"
#include "BLDC1_IRQ.h"
#include "ExtIntLdd1.h"
#include "BLDC2_IRQ.h"
#include "ExtIntLdd2.h"
#include "BLDCspi.h"
#include "SMasterLdd1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

#include "uC_Host.h"
#include "Error.h"
#include "BLDC.h"
#include "DC.h"


static void Task1(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		LedGreen_Neg();
		FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
	}

}

static void Task2(void *pvParameters)
{
	(void)pvParameters;
	while (1) {
		LedRed_Neg();
		FRTOS1_vTaskDelay(500/portTICK_RATE_MS);
	}
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
	xTaskHandle A;
	xTaskHandle B;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */


  set_status(STATUS_RESET);
  BLDC_init();
  DC_init();
  SHELL_Init();

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.10]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
