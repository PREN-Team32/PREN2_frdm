/*
 * BLDC.h
 *
 *  Created on: Mar 7, 2015
 *      Author: Ninux
 */

#ifndef BLDC_H_
#define BLDC_H_


#define BLDC_RPM_MIN 1
#define BLDC_RPM_MAX 0xFFFF
#define BLDC_PWM_MIN 1
#define BLDC_PWM_MAX 100

#define CMD_DUMMY		   0x00
#define CMD_START          0x10
#define CMD_STOP           0x20
#define CMD_SET_RPM		   0x32
#define CMD_ARE_YOU_ALIVE  0x71
#define CMD_SET_PWM		   0x81
#define CMD_GET_STATUS     0x64

#define I_AM_ALIVE	       0x55

#define BLDC_PARSE_COMMAND_ENABLED 1 /* set to 1 if method ParseCommand()
					is present, 0 otherwise */

void BLDC_init(void);

void BLDC_Receive_from_spi(void);
byte BLDC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

int BLDC_get_enable(void);

int BLDC_get_rpm(void);

void DC_update_task(void *pvParameters);

//void BLDC_FSM_update_task(void *pvParameters);

#endif /* BLDC_H_ */
