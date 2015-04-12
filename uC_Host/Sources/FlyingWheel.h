/*
 * FlyingWheel.h
 *
 *  Created on: 12.04.2015
 *      Author: studer.yves
 */

#ifndef FLYINGWHEEL_H_
#define FLYINGWHEEL_H_

#include "CLS1.h"

#define FLYING_ANGKE_MIN -20
#define FLYING_ANGKE_MAX 20
#define FLYING_PARSE_COMMAND_ENABLED 1 /* set to 1 if method ParseCommand()
					is present, 0 otherwise */

void FlyingWheel_init(void);
void FlyingWheel_update_task(void *pvParameters);
byte FlyingWheel_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);



#endif /* FLYINGWHEEL_H_ */
