/*
 * Error.h
 *
 *  Created on: Mar 7, 2015
 *      Author: Nino
 */

#ifndef ERROR_H_
#define ERROR_H_

typedef enum {
	STATUS_BUSY,
	STATUS_OK,
	STATUS_ERROR,
	STATUS_RESET
} status;

void set_status(status s);
// void print_error(status s, const char *message);

#endif /* ERROR_H_ */
