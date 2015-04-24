/*
 * BLDC.c
 *
 *  Created on: Mar 7, 2015
 *      Author: Ninux
 */

/* Include shared modules, which are used for whole project */

#include "BLDC.h"
#include "BLDCspi.h"
#if CS_HANDELD_BY_CODEEXPERT == 0
	#include "CS_BLDC1.h"
	#include "CS_BLDC2.h"
#endif
#include "BLDC1_IRQ.h"
#if MORE_AS_ONE_SLAVE
	#include "BLDC2_IRQ.h"
	#include "WAIT1.h"
#endif
#include <string.h>
#include <stdio.h>

#define CS_ENABLE FALSE
#define CS_DISABLE TRUE

#define BLDC_RPM_MIN 1
#define BLDC_RPM_MAX 0xFFFF
#define BLDC_PWM_MIN 1
#define BLDC_PWM_MAX 100
#define BLDC_DRV_VOLTAGE_MIN 60
#define BLDC_DRV_VOLTAGE_MAX 2400
#define BLDC_DRV_CURRENT_MIN 25
#define BLDC_DRV_CURRENT_MAX 255
#define BLDC_MOTORS_MIN 0
#define BLDC_MOTORS_MAX 1

#define CMD_DUMMY		   0x00
#define CMD_START          0x10
#define CMD_STOP           0x20
#define CMD_SET_RPM		   0x32
#define CMD_SET_VOLTAGE    0x42
#define CMD_SET_CURRENT    0x51
#define CMD_ARE_YOU_ALIVE  0x71
#define CMD_SET_PWM		   0x81
#define CMD_GET_STATUS     0x64

#define I_AM_ALIVE	       0x55


typedef union
{
    struct
    {
        uint8_t low;    /*!< Low byte */
        uint8_t high;   /*!< High byte */
    } byte;             /*!< Nibbles */
    uint16_t value;     /*!< Byte */
} DobuleByteStruct;

typedef struct{
	DobuleByteStruct rpm;
	uint8_t ErrorCode;
	MotorState State;
	CLS1_StdIOType io;
}BLDC_MotorState;

static uint8_t actualCmd = CMD_DUMMY;
static BLDC_MotorState BLDC1_Status;
static DobuleByteStruct Rpm_to_send;
static uint16_t BLDC_enable = 0;
static uint16_t BLDC_rpm = 0;
static BldcMotors_t Motor = BLDC1;


void BLDC_init(void)
{
	BLDC1_Status.ErrorCode = 0x00;
	BLDC1_Status.State = OFF;
	BLDC1_Status.rpm.value = 0x8000;
	if( BLDC1_Status.rpm.byte.high != 0x80)
	{
		while (1) {
				    /* loop to stop executing if wrong order */
					/* Hey Programmer */
					/* It seems that your compiler uses a */
					/* different order for bitfields than mine. */
					/* If you still want to use this library, */
					/* change ,if possible, the order of bitfields in */
					/* your compiler or in this library! */
				}
	}
	BLDC1_Status.rpm.value = 0x0000;
	Rpm_to_send.value = 0;
	Motor = BLDC1;
}

void handleCS(bool en)
{
#if CS_HANDELD_BY_CODEEXPERT == 0
	if(Motor == BLDC1)
		CS_BLDC1_PutVal(en);
	else if(Motor == BLDC2)
		CS_BLDC2_PutVal(en);
#endif
}

void setMotor(BldcMotors_t m)
{
	Motor = m;
}

uint8_t putBLDC(MotorState s)
{
	if( (CS_BLDC1_GetVal() == CS_ENABLE ) || ( CS_BLDC2_GetVal() == CS_ENABLE) )
		return 1;

	if(s == ON)
		actualCmd = CMD_START;
	else
		actualCmd = CMD_STOP;

	handleCS(CS_ENABLE);
	(void) BLDCspi_SendChar(actualCmd);
	return 0;
}

uint8_t setSpeed(uint16_t val)
{
	if( (CS_BLDC1_GetVal() == CS_ENABLE ) || ( CS_BLDC2_GetVal() == CS_ENABLE) )
		return 1;
	BLDC1_Status.rpm.value = val;
	actualCmd = CMD_SET_RPM;
	handleCS(CS_ENABLE);
	(void) BLDCspi_SendChar(actualCmd);
	BLDCspi_SendChar((uint8_t)BLDC1_Status.rpm.byte.high);
	BLDCspi_SendChar((uint8_t)BLDC1_Status.rpm.byte.low);
	return 0;
}

static uint8_t PrintStatus(const CLS1_StdIOType *io)
{
	unsigned char rpm_message[40] = { '\0' };
	unsigned char error_message[10] = { '\0' };
	CLS1_SendStatusStr((unsigned char*)"BLDC Status ist",(unsigned char*)"\r\n", io->stdOut);

	if (BLDC1_Status.State == ON)
	{
		CLS1_SendStatusStr((unsigned char*)"  on", (unsigned char*)"yes\r\n", io->stdOut);
	}
	else
	{
		CLS1_SendStatusStr((unsigned char*)"  on", (unsigned char*)"no\r\n", io->stdOut);
	}
	sprintf(error_message, "%i\r\n", BLDC1_Status.ErrorCode);
	sprintf(rpm_message, "High-Byte = %i, Low-Byte = %i, RPM-Wert = %i\r\n", BLDC1_Status.rpm.byte.high, BLDC1_Status.rpm.byte.low, BLDC1_Status.rpm.value);
	CLS1_SendStatusStr((unsigned char*)"  Error code", (unsigned char*)error_message, io->stdOut);
	CLS1_SendStatusStr((unsigned char*)"  RPM", (unsigned char*)rpm_message, io->stdOut);

	return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io)
{
	CLS1_SendHelpStr((unsigned char*)"BLDC",
			 (unsigned char*)"Group of BLDC commands\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  help",
			 (unsigned char*)"Print help\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  on|off",
			 (unsigned char*)"Turns it on or off\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  setrpm n",
			 (unsigned char*)"Sets RPM to n\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  setpwm n ",
			 (unsigned char*)"Sets PWM to n\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  setvoltage n ",
			 (unsigned char*)"Sets the voltage on the DRV to n mV\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  setcurrent n ",
			 (unsigned char*)"Sets the current on the DRV to n mA\r\n",
			 io->stdOut);
#if MORE_AS_ONE_SLAVE
	CLS1_SendHelpStr((unsigned char*)"  use n ",
			 (unsigned char*)"select the configurable motor\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  all on ",
			 (unsigned char*)"starts all BLDC motors\r\n",
			 io->stdOut);
	CLS1_SendHelpStr((unsigned char*)"  all off ",
			 (unsigned char*)"stops all BLDC motors\r\n",
			 io->stdOut);
#endif
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
		handleCS(CS_ENABLE);
		(void) BLDCspi_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "BLDC on") == 0)
	{
		*handled = TRUE;
		actualCmd = CMD_START;
		handleCS(CS_ENABLE);
		(void) BLDCspi_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "BLDC off") == 0)
	{
		*handled = TRUE;
		actualCmd = CMD_STOP;
		handleCS(CS_ENABLE);
		(void) BLDCspi_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strcmp((char*)cmd, "debug") == 0)
	{
		BLDCspi_TComData tmp;
		*handled = TRUE;
		actualCmd = CMD_ARE_YOU_ALIVE;
		handleCS(CS_ENABLE);
		(void) BLDCspi_SendChar(actualCmd);
		return ERR_OK;
	}
	else if (UTIL1_strncmp((char*)cmd, "BLDC setrpm ", sizeof("BLDC setrpm")-1) == 0)
	{
		p = cmd+sizeof("BLDC setrpm");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= BLDC_RPM_MIN && val <= BLDC_RPM_MAX)
		{
			BLDC1_Status.rpm.value = val;
			actualCmd = CMD_SET_RPM;
			handleCS(CS_ENABLE);
			(void) BLDCspi_SendChar(actualCmd);
			BLDCspi_SendChar((uint8_t)BLDC1_Status.rpm.byte.high);
			BLDCspi_SendChar((uint8_t)BLDC1_Status.rpm.byte.low);
			*handled = TRUE;
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", BLDC_RPM_MIN, BLDC_RPM_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}else if (UTIL1_strncmp((char*)cmd, "BLDC setpwm ", sizeof("BLDC setpwm")-1) == 0)
	{
		p = cmd+sizeof("BLDC setpwm");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= BLDC_PWM_MIN && val <= BLDC_PWM_MAX)
		{
			actualCmd = CMD_SET_PWM;
			handleCS(CS_ENABLE);
			(void) BLDCspi_SendChar(actualCmd);
			(void) BLDCspi_SendChar((char)val);
			*handled = TRUE;
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", BLDC_PWM_MIN, BLDC_PWM_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}else if (UTIL1_strncmp((char*)cmd, "BLDC setvoltage ", sizeof("BLDC setvoltage")-1) == 0)
	{
		p = cmd+sizeof("BLDC setvoltage");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= BLDC_DRV_VOLTAGE_MIN && val <= BLDC_DRV_VOLTAGE_MAX)
		{
			DobuleByteStruct tmp;
			tmp.value = val;
			actualCmd = CMD_SET_VOLTAGE;
			handleCS(CS_ENABLE);
			(void) BLDCspi_SendChar(actualCmd);
			(void) BLDCspi_SendChar((uint8_t)tmp.byte.high);
			(void) BLDCspi_SendChar((uint8_t)tmp.byte.low);
			*handled = TRUE;
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", BLDC_DRV_VOLTAGE_MIN, BLDC_DRV_VOLTAGE_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}else if (UTIL1_strncmp((char*)cmd, "BLDC setcurrent ", sizeof("BLDC setcurrent")-1) == 0)
	{
		p = cmd+sizeof("BLDC setcurrent");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= BLDC_DRV_CURRENT_MIN && val <= BLDC_DRV_CURRENT_MAX)
		{
			actualCmd = CMD_SET_CURRENT;
			handleCS(CS_ENABLE);
			(void) BLDCspi_SendChar(actualCmd);
			(void) BLDCspi_SendChar((uint8_t)val);
			*handled = TRUE;
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", BLDC_DRV_CURRENT_MIN, BLDC_DRV_CURRENT_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}
#if MORE_AS_ONE_SLAVE
	else if (UTIL1_strncmp((char*)cmd, "BLDC use ", sizeof("BLDC use")-1) == 0)
	{
		p = cmd+sizeof("BLDC use");
		if (UTIL1_xatoi(&p, &val) == ERR_OK && val >= BLDC_MOTORS_MIN && val <= BLDC_MOTORS_MAX)
		{
			*handled = TRUE;
			Motor = val & 0x000F;
		}
		else
		{
			sprintf(message, "Wrong argument, must be in range %i to %i", BLDC_MOTORS_MIN, BLDC_MOTORS_MAX);
			CLS1_SendStr((unsigned char*)message, io->stdErr);
		}
	}else if (UTIL1_strcmp((char*)cmd, "BLDC all on") == 0)
	{
		*handled = TRUE;
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
		return ERR_OK;
	}else if (UTIL1_strcmp((char*)cmd, "BLDC all off") == 0)
	{
		*handled = TRUE;
		setMotor(BLDC1);
		putBLDC(OFF);
		WAIT1_Waitus(5);
		setMotor(BLDC2);
		putBLDC(OFF);
		return ERR_OK;
	}
#endif
	return ERR_OK;
}

void BLDC_Receive_from_spi(void)
{
	BLDCspi_TComData recv;
	BLDCspi_RecvChar( &recv );
	switch(actualCmd & 0xF0)
	{
	case (CMD_ARE_YOU_ALIVE & 0xF0):
		if(actualCmd == CMD_ARE_YOU_ALIVE - 1)
		{
			/* response to CMD_ARE_YOU_ALIVE */
			handleCS(CS_DISABLE);
			if( recv == I_AM_ALIVE )
				CLS1_SendStatusStr((unsigned char*)" BLDC Status", (unsigned char*)"OK\r\n", *BLDC1_Status.io.stdOut);
			else
				CLS1_SendStatusStr((unsigned char*)" BLDC Status", (unsigned char*)"NOK\r\n", *BLDC1_Status.io.stdOut);
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
			BLDC1_Status.rpm.byte.high = recv;
		}
		else if ( actualCmd == CMD_GET_STATUS - 4 )
		{
			/*This is the Motor-error code */
			BLDC1_Status.rpm.byte.low = recv;
			handleCS(CS_DISABLE);
			PrintStatus(&BLDC1_Status.io);
		}
		break;
	}
	if( (actualCmd & 0x0F) != 0)
	{
		BLDCspi_SendChar(CMD_DUMMY);
		actualCmd--;
	}
	else
		handleCS(CS_DISABLE);
}

void bldc1_irq_occurred(void)
{
	Motor = BLDC1;
	if( BLDC1_IRQ_GetVal() )
	{
		CLS1_SendStatusStr((unsigned char*)" BLDC1 error", (unsigned char*)"an error occurred\r\n", *BLDC1_Status.io.stdOut);
		actualCmd = CMD_STOP;
		handleCS(CS_ENABLE);
		(void) BLDCspi_SendChar(actualCmd);
	}
}

#if MORE_AS_ONE_SLAVE
void bldc2_irq_occurred(void)
{
	Motor = BLDC2;
	if( BLDC2_IRQ_GetVal() )
	{
		CLS1_SendStatusStr((unsigned char*)" BLDC2 error", (unsigned char*)"an error occurred\r\n", *BLDC1_Status.io.stdOut);
		actualCmd = CMD_STOP;
		handleCS(CS_ENABLE);
		(void) BLDCspi_SendChar(actualCmd);
	}
}
#endif
