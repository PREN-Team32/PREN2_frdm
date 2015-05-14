/*
 * stepper.c
 *
 *  Created on: 01.05.2015
 *      Author: studer.yves
 */
#include "FRTOS1.h"
#include "stepper.h"
#include "l6480.h"
#include "Error.h"

void StepperInitialisation(void *pvParameters);
void init_Stepper(void)
{
	if (FRTOS1_xTaskCreate(
			StepperInitialisation,
			"Stepper initialisation",
			configMINIMAL_STACK_SIZE,
			NULL,
			tskIDLE_PRIORITY,
			NULL ) != pdPASS) {
		while (1) {
			set_status(STATUS_ERROR);
		}
	}
}

void StepperInitialisation(void *pvParameters)
{
	static uint8_t done = 0;
	(void)pvParameters;
    done = 1;
    l6480_init();
    /*******************************************************
    * Einstellungen f�r den Motor und die Treiberstufen   *
    ******************************************************/
    l6480_cmd_resetdevice();

	l6480_reg_step_mode_t stepmode_config;
	stepmode_config.reg.step_sel = L6480_STEP_SEL_MICRO_128;                //128 steps
	stepmode_config.reg.sync_sel = 0;               //sync unimportant due to BUSY - mode
	stepmode_config.reg.sync_en = 0;                //Busy mode
	l6480_set_step_mode(stepmode_config.raw.data);

	l6480_reg_config_t config;
	config.reg.osc_sel = 0;  //unused
	config.reg.ext_clk = 0;  //unused
	config.reg.sw_mode = 1;  //user disposal
	config.reg.en_vscomp  = 0; //Bridge shut down
	config.reg.oc_sd   = 1;     //Bridge shutdown
	config.reg.uvloval = 0;     //6.9V,6.3V etc
	config.reg.vccval  = 0;     //7.5V
	config.reg.f_pwm_dec = 4;
	config.reg.f_pwm_int = 1;
	l6480_set_config(config.raw.data);

    l6480_set_ocd_th_millivolt(1000); 			// Overcurrentdetection Treshold
    l6480_set_stall_th_millivolt(1000); 			// Stalldetection Tresold
    l6480_set_gatecfg1_igate_milliampere(96);		// Gatstrom
    l6480_set_gatecfg1_tcc_nanosecond(250);		// Bestromungszeiten
    l6480_set_gatecfg1_tboost_nanosecond(125);
    l6480_set_gatecfg2_tdt_nanosecond(250);
    l6480_set_gatecfg2_tblank_nanosecond(250);	// Pausenzeit Messung
    l6480_set_kval_hold(20);						// KVAL Motor Stillstand
    l6480_set_kval_run(32);						// kVAL Motor Run
    l6480_set_kval_acc(50);
    l6480_set_kval_dec(50);
    l6480_cmd_hardstop();
	while (1) {
    	FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
	}
}
