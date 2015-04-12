/*
 * DC.h
 *
 *  Created on: 12.04.2015
 *      Author: studer.yves
 */

#ifndef DC_H_
#define DC_H_

#define DC_PWM_MIN 1
#define DC_PWM_MAX 100

#define DC_PARSE_COMMAND_ENABLED 1 /* set to 1 if method ParseCommand()
					is present, 0 otherwise */
byte DC_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
void DC_update_task(void *pvParameters);
void DC_init(void);



#endif /* DC_H_ */
