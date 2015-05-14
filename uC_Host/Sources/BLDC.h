/*
 * BLDC.h
 *
 *  Created on: Mar 7, 2015
 *      Author: Ninux
 */

#ifndef BLDC_H_
#define BLDC_H_

#include "FRTOS1.h"

#define BLDC_PARSE_COMMAND_ENABLED 1 /* set to 1 if method ParseCommand()
					is present, 0 otherwise */

#define CS_HANDELD_BY_CODEEXPERT 0

#if CS_HANDELD_BY_CODEEXPERT == 0
#define MORE_AS_ONE_SLAVE 1
#endif


void BLDC_init(void);


void BLDC_Receive_from_spi(void);
byte BLDC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

int BLDC_get_enable(void);

int BLDC_get_rpm(void);

void bldc1_irq_occurred(void);
#if MORE_AS_ONE_SLAVE
void bldc2_irq_occurred(void);
#endif

#endif /* BLDC_H_ */
