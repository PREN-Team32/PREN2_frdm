/*!
 *  ____  ____  _____ _   _       _____ _____
 * |  _ \|  _ \| ____| \ | |     | ____|_   _|
 * | |_) | |_) |  _| |  \| |_____|  _|   | |
 * |  __/|  _ <| |___| |\  |_____| |___  | |
 * |_|   |_| \_\_____|_| \_|     |_____| |_|
 *
 * \file l6480.c
 * \brief Driver for Stepper driver L6480 from ST Microelectronics
 * \author pren-et
 *
 */

//#include "platform.h"
#include "l6480.h"
#if PL_FRDM
    #include "Stepperspi.h"
    #include "shell.h"
#endif /* PL_FRDM */

#if PL_HAS_SHELL
    #include "UTIL1.h"
    #include "CLS1.h"
#endif /* PL_HAS_SHELL */

/* Definition of necessary functions provided by a SPI module later */
/*! \fn spi_write(uint8_t *data)
 *  \todo Define in SPI Module
 */
void spi_write(uint8_t *data) {
    #if PL_FRDM
        Stepperspi_SendChar(*data);
    #endif /* PL_FRDM */
    #if PL_PC
        printf("write: 0x%X\n", *data);
    #endif /* PL_PC */
    return;
}
/*! \fn spi_read(uint8_t *data)
 *  \todo Define in SPI Module
 */
void spi_read(uint8_t *data) {
    #if PL_FRDM
        /* dummy data*/
        uint8_t zero = 0;
        /* delete previously received data */
        //while (Stepperspi_GetCharsInRxBuf()) {
        //    Stepperspi_RecvChar(NULL);
        //}
        /* send zeros to read data */
        Stepperspi_SendChar((int) &zero);
        /* Wait until data transfer has completed */
        //while (Stepperspi_GetCharsInRxBuf()) {}
        /* read data */
        //Stepperspi_RecvChar(data);
    #endif /* PL_FRDM */
    #if PL_PC
        //unsigned int readdata;
        printf("read:  \n");
        //scanf("%X", &readdata);
        //*data = (uint8_t) *readdata;
        *data = 0x00;
    #endif /* PL_PC */
    return;
}

void l6480_init(void) {
    /* test orientation of bitfields */
    test_bitfield_t test_bitfield;      /* testvariable */
    test_bitfield.byte = 0x00;
    test_bitfield.bitfield.low = 0x01;  /* write testvalue to lower nibble */
    if (test_bitfield.byte != 0x01) {   /* test if lower nibble has been written */
        while (1) {                     /* loop to stop executing if wrong order */
            /* Hey Programmer
                It seems that your compiler uses a
                different order for bitfields than mine.
                If you still want to use this library,
                change ,if possible, the order of bitfields in
                your compiler or in this library! */
        }
    }

    return;
}

void l6480_send_cmd(uint8_t cmd, uint8_t len, uint8_t read, uint8_t *data) {
    /* local variables */
    uint8_t i;          /* variable to count number of sent bits */
    spi_write(&cmd);    /* send command */
    /*! \todo Check if byte order for sending a command is correct */
    if (read) {         /* check if reading data is needed */
        for (i = 0; i < (len - 1); i++) {
            spi_read(data+len-2-i);   /* read data */
        }
    } else {
        for (i = 0; i < (len - 1); i++) {
            spi_write(data+len-2-i);  /* write data */
        }
    }
}

int32_t l6480_get_abs_pos(void) {
    /* local variables */
    l6480_reg_abs_pos_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ABS_POS),
        L6480_CMD_GETPARAM_LEN(ABS_POS),
        L6480_CMD_GETPARAM_READ(ABS_POS),
        reg.array);

    /* return data */
    return reg.raw.data;
}

void l6480_set_abs_pos(int32_t pos) {
    /* local variables */
    l6480_reg_abs_pos_t reg;

    /* input value limitation */
    if (pos >= L6480_REG_ABS_POS_MAX) {
        pos  = L6480_REG_ABS_POS_MAX;
    }
    if (pos <= L6480_REG_ABS_POS_MIN) {
        pos  = L6480_REG_ABS_POS_MIN;
    }

    /* prepare data local */
    reg.raw.data = pos;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ABS_POS),
        L6480_CMD_SETPARAM_LEN(ABS_POS),
        L6480_CMD_SETPARAM_READ(ABS_POS),
        reg.array);

    return;
}

int16_t l6480_get_el_pos(void) {
    /* local variables */
    l6480_reg_el_pos_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(EL_POS),
        L6480_CMD_GETPARAM_LEN(EL_POS),
        L6480_CMD_GETPARAM_READ(EL_POS),
        reg.array);

    /* return data */
    return reg.raw.data;
}

void l6480_set_el_pos(uint8_t fullstep, uint8_t microstep) {
    /* local variables */
    l6480_reg_el_pos_t reg;

    /* input value limitation */
    if (fullstep >= 3) {
        fullstep  = 3;
    }
    if (microstep >= 127) {
        microstep  = 127;
    }

    /* prepare data local */
    reg.reg.step = fullstep;
    reg.reg.microstep = microstep;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(EL_POS),
        L6480_CMD_SETPARAM_LEN(EL_POS),
        L6480_CMD_SETPARAM_READ(EL_POS),
        reg.array);
    return;
}

int32_t l6480_get_mark(void) {
    /* local variables */
    l6480_reg_mark_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(MARK),
        L6480_CMD_GETPARAM_LEN(MARK),
        L6480_CMD_GETPARAM_READ(MARK),
        reg.array);

    /* return data */
    return reg.raw.data;
}

void l6480_set_mark(int32_t mark) {
    /* local variables */
    l6480_reg_mark_t reg;

    /* input value limitation */
    if (mark >= L6480_REG_MARK_MAX) {
        mark  = L6480_REG_MARK_MAX;
    }
    if (mark <= L6480_REG_MARK_MIN) {
        mark  = L6480_REG_MARK_MIN;
    }

    /* prepare data local */
    reg.raw.data = mark;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(MARK),
        L6480_CMD_SETPARAM_LEN(MARK),
        L6480_CMD_SETPARAM_READ(MARK),
        reg.array);

    return;
}

int32_t l6480_get_speed(void) {
    /* local variables */
    l6480_reg_speed_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(SPEED),
        L6480_CMD_GETPARAM_LEN(SPEED),
        L6480_CMD_GETPARAM_READ(SPEED),
        reg.array);

    /* return data */
    return reg.raw.data;
}

int32_t l6480_get_speed_millisteps_s(void) {
    /* local variables */
    int32_t speed;

    /* read current speed from device */
    speed = l6480_get_speed();

    /*! calculate speed in millisteps per second
        \f[
            \text{speed}~[\si{step\per\second}]
            = \frac{\text{SPEED} \cdot 2^{-28}}{250~[\si{\second}]}
        \f]
        \f[
            \text{speed}~[\si{\milli step\per\second}]
            = \frac{\text{SPEED} \cdot 1000 \cdot 2^{-28}}{250~[\si{\second}]}
            = \text{SPEED} \cdot 2^{-26} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 14.90
        \f]
    */
    speed = speed * 149 / 10;

    /* return speed */
    return speed;
}

uint16_t l6480_get_acc(void) {
    /* local variables */
    l6480_reg_acc_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ACC),
        L6480_CMD_GETPARAM_LEN(ACC),
        L6480_CMD_GETPARAM_READ(ACC),
        reg.array);

    /* return data */
    return reg.raw.data;
}

uint16_t l6480_get_acc_steps_ss(void) {
    /* local variables */
    uint32_t acc;

    /* read current acc from device */
    acc = l6480_get_acc();

    /*! calculate acc in steps per second^2
        \f[
            \text{acc}~[\si{step\per\second^2}]
            = \frac{\text{ACC} \cdot 2^{-40}}{\left(250~[\si{\second}] \cdot 10^{-9}\right)^2}
            = \text{ACC} \cdot 2^{-36} \cdot 10^{12}
            \approx \text{MIN\_SPEED} \cdot 14.55
        \f]
    */
    acc = acc * 1455 / 100;

    /* return acc */
    return (uint16_t) acc;
}

void l6480_set_acc(uint16_t acc) {
    /* local variables */
    l6480_reg_acc_t reg;

    /* input value limitation */
    if (acc >= L6480_REG_ACC_MAX) {
        acc  = L6480_REG_ACC_MAX;
    }

    /* prepare data local */
    reg.raw.data = acc;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ACC),
        L6480_CMD_SETPARAM_LEN(ACC),
        L6480_CMD_SETPARAM_READ(ACC),
        reg.array);

    return;
}

void l6480_set_acc_steps_ss(uint16_t acc) {
    /* local variables */

    /*! Calculate acc register value
        \f[
            \text{acc}~[\si{step\per\second^2}]
            = \frac{\text{ACC} \cdot 2^{-40}}{\left(250~[\si{\second}] \cdot 10^{-9}\right)^2}
            = \text{ACC} \cdot 2^{-36} \cdot 10^{12}
            \approx \text{MIN\_SPEED} \cdot 14.55
        \f]
    */
    acc = (uint16_t) ((uint32_t) acc * 100 / 1455);

    /* send data to device */
    l6480_set_acc(acc);

    return;
}

uint16_t l6480_get_dec(void) {
    /* local variables */
    l6480_reg_dec_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(DEC),
        L6480_CMD_GETPARAM_LEN(DEC),
        L6480_CMD_GETPARAM_READ(DEC),
        reg.array);

    /* return data */
    return reg.raw.data;
}

uint16_t l6480_get_dec_steps_ss(void) {
    /* local variables */
    uint32_t dec;

    /* read current dec from device */
    dec = l6480_get_dec();

    /*! calculate dec in steps per second^2
        \f[
            \text{dec}~[\si{step\per\second^2}]
            = \frac{\text{DEC} \cdot 2^{-40}}{\left(250~[\si{\second}] \cdot 10^{-9}\right)^2}
            = \text{DEC} \cdot 2^{-36} \cdot 10^{12}
            \approx \text{MIN\_SPEED} \cdot 14.55
        \f]
    */
    dec = dec * 1455 / 100;

    /* return dec */
    return (uint16_t) dec;
}

void l6480_set_dec(uint16_t dec) {
    /* local variables */
    l6480_reg_dec_t reg;

    /* input value limitation */
    if (dec >= L6480_REG_DEC_MAX) {
        dec  = L6480_REG_DEC_MAX;
    }

    /* prepare data local */
    reg.raw.data = dec;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(DEC),
        L6480_CMD_SETPARAM_LEN(DEC),
        L6480_CMD_SETPARAM_READ(DEC),
        reg.array);

    return;
}

void l6480_set_dec_steps_ss(uint16_t dec) {
    /* local variables */

    /*! Calculate dec register value
        \f[
            \text{dec}~[\si{step\per\second^2}]
            = \frac{\text{DEC} \cdot 2^{-40}}{\left(250~[\si{\second}] \cdot 10^{-9}\right)^2}
            = \text{DEC} \cdot 2^{-36} \cdot 10^{12}
            \approx \text{MIN\_SPEED} \cdot 14.55
        \f]
    */
    dec = (uint16_t) ((uint32_t) dec * 100 / 1455);

    /* send data to device */
    l6480_set_dec(dec);

    return;
}

uint16_t l6480_get_max_speed(void) {
    /* local variables */
    l6480_reg_max_speed_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(MAX_SPEED),
        L6480_CMD_GETPARAM_LEN(MAX_SPEED),
        L6480_CMD_GETPARAM_READ(MAX_SPEED),
        reg.array);

    /* return data */
    return reg.raw.data;
}

uint16_t l6480_get_max_speed_steps_s(void) {
    /* local variables */
    uint32_t max_speed;

    /* read max_speed from device */
    max_speed = l6480_get_max_speed();

    /*! calculate max_speed in steps per second
        \f[
            \text{max\_speed}~[\si{step\per\second}]
            = \frac{\text{MAX\_SPEED} \cdot 2^{-18}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{MAX\_SPEED} \cdot 2^{-16} \cdot 10^{6}
            \approx \text{MIN\_SPEED} \cdot 15.259
        \f]
    */
    max_speed = max_speed * 15259 / 1000;

    /* return max_speed */
    return (uint16_t) max_speed;
}

void l6480_set_max_speed(uint16_t max_speed) {
    /* local variables */
    l6480_reg_max_speed_t reg;

    /* input value limitation */
    if (max_speed >= L6480_REG_MAX_SPEED_MAX) {
        max_speed  = L6480_REG_MAX_SPEED_MAX;
    }

    /* prepare data local */
    reg.raw.data = max_speed;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(MAX_SPEED),
        L6480_CMD_SETPARAM_LEN(MAX_SPEED),
        L6480_CMD_SETPARAM_READ(MAX_SPEED),
        reg.array);

    return;
}

void l6480_set_max_speed_steps_s(uint16_t max_speed) {
    /* local variables */

    /*! Calculate max_speed register value
        \f[
            \text{max\_speed}~[\si{step\per\second}]
            = \frac{\text{MAX\_SPEED} \cdot 2^{-18}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{MAX\_SPEED} \cdot 2^{-16} \cdot 10^{6}
            \approx \text{MIN\_SPEED} \cdot 15.259
        \f]
    */
    max_speed = (uint16_t) ((uint32_t) max_speed * 1000 / 15259);

    /* send data to device */
    l6480_set_max_speed(max_speed);

    return;
}

uint16_t l6480_get_min_speed(void) {
    /* local variables */
    l6480_reg_min_speed_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(MIN_SPEED),
        L6480_CMD_GETPARAM_LEN(MIN_SPEED),
        L6480_CMD_GETPARAM_READ(MIN_SPEED),
        reg.array);

    /* Analyze data */
    if (reg.reg.lspd_opt == 1) {
        reg.reg.min_speed = 0; /* minimum speed zero when low speed optimization on */
    }

    /* return data */
    return reg.reg.min_speed;
}

uint32_t l6480_get_min_speed_millisteps_s(void) {
    /* local variables */
    uint32_t min_speed;

    /* read min_speed from device */
    min_speed = l6480_get_min_speed();

    /*! calculate min_speed in millisteps per second
        \f[
            \text{min\_speed}~[\si{step\per\second}]
            = \frac{\text{MIN\_SPEED} \cdot 2^{-24}}{250~[\si{\second}] \cdot 10^{-9}}
        \f]
        \f[
            \text{min\_speed}~[\si{\milli step\per\second}]
            = \frac{\text{MIN\_SPEED} \cdot 2^{-24} \cdot 10^{3}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{MIN\_SPEED} \cdot 2^{-22} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 238.42
        \f]
    */
    min_speed = min_speed * 2384 / 10;

    /* return min_speed */
    return min_speed;
}

uint16_t l6480_get_lspd_opt_speed(void) {
    /* local variables */
    l6480_reg_min_speed_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(MIN_SPEED),
        L6480_CMD_GETPARAM_LEN(MIN_SPEED),
        L6480_CMD_GETPARAM_READ(MIN_SPEED),
        reg.array);

    /* Analyze data */
    if (reg.reg.lspd_opt == 0) {
        reg.reg.min_speed = 0; /* low speed optimization threshold zero when low speed optimization off */
    }

    /* return data */
    return reg.reg.min_speed;
}

uint32_t l6480_get_lspd_opt_speed_millisteps_s(void) {
    /* local variables */
    uint32_t min_speed;

    /* read min_speed from device */
    min_speed = l6480_get_min_speed();

    /*! calculate min_speed in millisteps per second
        \f[
            \text{min\_speed}~[\si{step\per\second}]
            = \frac{\text{MIN\_SPEED} \cdot 2^{-24}}{250~[\si{\second}] \cdot 10^{-9}}
        \f]
        \f[
            \text{min\_speed}~[\si{\milli step\per\second}]
            = \frac{\text{MIN\_SPEED} \cdot 2^{-24} \cdot 10^{3}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{MIN\_SPEED} \cdot 2^{-22} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 238.42
        \f]
    */
    min_speed = min_speed * 2384 / 10;

    /* return min_speed */
    return min_speed;
}

void l6480_set_min_speed(uint16_t speed) {
    /* local variables */
    l6480_reg_min_speed_t reg;

    /* input value limitation */
    if (speed >= L6480_REG_MIN_SPEED_MAX) {
        speed  = L6480_REG_MIN_SPEED_MAX;
    }

    /* prepare data local */
    reg.reg.min_speed = speed;
    reg.reg.lspd_opt  = 0;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(MIN_SPEED),
        L6480_CMD_SETPARAM_LEN(MIN_SPEED),
        L6480_CMD_SETPARAM_READ(MIN_SPEED),
        reg.array);

    return;
}

void l6480_set_min_speed_millisteps_s(uint32_t speed) {
    /* local variables */

    /*! Calculate max_speed register value
        \f[
            \text{min\_speed}~[\si{\milli step\per\second}]
            = \frac{\text{MIN\_SPEED} \cdot 2^{-24} \cdot 10^{3}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{MIN\_SPEED} \cdot 2^{-22} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 238.42
        \f]
    */
    speed = (uint16_t) (speed * 10 / 2384);

    /* send data to device */
    l6480_set_min_speed(speed);

    return;
}

void l6480_set_lspd_opt_speed(uint16_t speed) {
    /* local variables */
    l6480_reg_min_speed_t reg;

    /* input value limitation */
    if (speed >= L6480_REG_MIN_SPEED_MAX) {
        speed  = L6480_REG_MIN_SPEED_MAX;
    }

    /* prepare data local */
    reg.reg.min_speed = speed;
    reg.reg.lspd_opt  = 1;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(MIN_SPEED),
        L6480_CMD_SETPARAM_LEN(MIN_SPEED),
        L6480_CMD_SETPARAM_READ(MIN_SPEED),
        reg.array);

    return;
}

void l6480_set_lspd_opt_speed_millisteps_s(uint32_t speed) {
    /* local variables */

    /*! Calculate max_speed register value
        \f[
            \text{min\_speed}~[\si{\milli step\per\second}]
            = \frac{\text{MIN\_SPEED} \cdot 2^{-24} \cdot 10^{3}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{MIN\_SPEED} \cdot 2^{-22} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 238.42
        \f]
    */
    speed = (uint16_t) (speed * 10 / 2384);

    /* send data to device */
    l6480_set_lspd_opt_speed(speed);

    return;
}

uint8_t l6480_get_boost_mode(void) {
    /* local variables */
    l6480_reg_fs_spd_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FS_SPD),
        L6480_CMD_GETPARAM_LEN(FS_SPD),
        L6480_CMD_GETPARAM_READ(FS_SPD),
        reg.array);

    /* return data */
    return reg.reg.boost_mode;
}

void l6480_set_boost_mode_on(void) {
    /* local variables */
    l6480_reg_fs_spd_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FS_SPD),
        L6480_CMD_GETPARAM_LEN(FS_SPD),
        L6480_CMD_GETPARAM_READ(FS_SPD),
        reg.array);

    /* Set boost mode */
    reg.reg.boost_mode = 1;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(MIN_SPEED),
        L6480_CMD_SETPARAM_LEN(MIN_SPEED),
        L6480_CMD_SETPARAM_READ(MIN_SPEED),
        reg.array);
}

void l6480_set_boost_mode_off(void) {
    /* local variables */
    l6480_reg_fs_spd_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FS_SPD),
        L6480_CMD_GETPARAM_LEN(FS_SPD),
        L6480_CMD_GETPARAM_READ(FS_SPD),
        reg.array);

    /* Set boost mode */
    reg.reg.boost_mode = 0;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(MIN_SPEED),
        L6480_CMD_SETPARAM_LEN(MIN_SPEED),
        L6480_CMD_SETPARAM_READ(MIN_SPEED),
        reg.array);
}

uint16_t l6480_get_fs_spd(void) {
    /* local variables */
    l6480_reg_fs_spd_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FS_SPD),
        L6480_CMD_GETPARAM_LEN(FS_SPD),
        L6480_CMD_GETPARAM_READ(FS_SPD),
        reg.array);

    /* return fs_spd */
    return reg.reg.fs_spd;
}

uint16_t l6480_get_fs_spd_steps_s(void) {
    /* local variables */
    uint32_t fs_spd;

    /* read min_speed from device */
    fs_spd = l6480_get_fs_spd();

    /*! calculate fs_spd in steps per second
        \f[
            \text{fs\_speed}~[\si{step\per\second}]
            = (\frac{\text{FS\_SPEED} + 0.5) \cdot 2^{-18}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{FS\_SPEED} \cdot 2^{-16} \cdot 10^{6} + 2^{-17} \cdot 10^{6}
            \approx \text{FS\_SPEED} \cdot 15.259 + 7.629
        \f]
    */
    fs_spd = ((fs_spd * 15259) + 7629) / 1000 ;

    /* return fs_spd */
    return fs_spd;
}

void l6480_set_fs_spd(uint16_t speed) {
    /* local variables */
    l6480_reg_fs_spd_t reg;

    /* input value limitation */
    if (speed >= L6480_REG_FS_SPD_MAX) {
        speed  = L6480_REG_FS_SPD_MAX;
    }

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FS_SPD),
        L6480_CMD_GETPARAM_LEN(FS_SPD),
        L6480_CMD_GETPARAM_READ(FS_SPD),
        reg.array);

    /* prepare data local */
    reg.reg.fs_spd = speed;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(FS_SPD),
        L6480_CMD_SETPARAM_LEN(FS_SPD),
        L6480_CMD_SETPARAM_READ(FS_SPD),
        reg.array);

    return;
}

void l6480_set_fs_spd_steps_s(uint16_t speed) {
    /* local variables */

    /*! calculate fs_spd in steps per second
        \f[
            \text{fs\_speed}~[\si{step\per\second}]
            = (\frac{\text{FS\_SPEED} + 0.5) \cdot 2^{-18}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{FS\_SPEED} \cdot 2^{-16} \cdot 10^{6} + 2^{-17} \cdot 10^{6}
            \approx \text{FS\_SPEED} \cdot 15.259 + 7.629
        \f]
    */
    speed = (uint16_t) ((((uint32_t) speed * 1000) - 7629) / 15259);

    /* send data to device */
    l6480_set_fs_spd(speed);

    return;
}

uint8_t l6480_get_kval_hold(void) {
    /* local variables */
    l6480_reg_kval_hold_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(KVAL_HOLD),
        L6480_CMD_GETPARAM_LEN(KVAL_HOLD),
        L6480_CMD_GETPARAM_READ(KVAL_HOLD),
        reg.array);

    /* return kval_hold */
    return reg.raw.data;
}

void l6480_set_kval_hold(uint8_t value) {
    /* local variables */
    l6480_reg_kval_hold_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(KVAL_HOLD),
        L6480_CMD_SETPARAM_LEN(KVAL_HOLD),
        L6480_CMD_SETPARAM_READ(KVAL_HOLD),
        reg.array);

    return;
}

uint8_t l6480_get_kval_run(void) {
    /* local variables */
    l6480_reg_kval_run_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(KVAL_RUN),
        L6480_CMD_GETPARAM_LEN(KVAL_RUN),
        L6480_CMD_GETPARAM_READ(KVAL_RUN),
        reg.array);

    /* return kval_run */
    return reg.raw.data;
}

void l6480_set_kval_run(uint8_t value) {
    /* local variables */
    l6480_reg_kval_run_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(KVAL_RUN),
        L6480_CMD_SETPARAM_LEN(KVAL_RUN),
        L6480_CMD_SETPARAM_READ(KVAL_RUN),
        reg.array);

    return;
}

uint8_t l6480_get_kval_acc(void) {
    /* local variables */
    l6480_reg_kval_acc_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(KVAL_ACC),
        L6480_CMD_GETPARAM_LEN(KVAL_ACC),
        L6480_CMD_GETPARAM_READ(KVAL_ACC),
        reg.array);

    /* return kval_acc */
    return reg.raw.data;
}

void l6480_set_kval_acc(uint8_t value) {
    /* local variables */
    l6480_reg_kval_acc_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(KVAL_ACC),
        L6480_CMD_SETPARAM_LEN(KVAL_ACC),
        L6480_CMD_SETPARAM_READ(KVAL_ACC),
        reg.array);

    return;
}

uint8_t l6480_get_kval_dec(void) {
    /* local variables */
    l6480_reg_kval_dec_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(KVAL_DEC),
        L6480_CMD_GETPARAM_LEN(KVAL_DEC),
        L6480_CMD_GETPARAM_READ(KVAL_DEC),
        reg.array);

    /* return kval_dec */
    return reg.raw.data;
}

void l6480_set_kval_dec(uint8_t value) {
    /* local variables */
    l6480_reg_kval_dec_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(KVAL_DEC),
        L6480_CMD_SETPARAM_LEN(KVAL_DEC),
        L6480_CMD_SETPARAM_READ(KVAL_DEC),
        reg.array);

    return;
}

uint16_t l6480_get_int_speed(void) {
    /* local variables */
    l6480_reg_int_speed_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(INT_SPEED),
        L6480_CMD_GETPARAM_LEN(INT_SPEED),
        L6480_CMD_GETPARAM_READ(INT_SPEED),
        reg.array);

    /* return int_speed */
    return reg.raw.data;
}

uint32_t l6480_get_int_speed_millisteps_s(void) {
    /* local variables */
    uint32_t int_speed;

    /* read min_speed from device */
    int_speed = l6480_get_int_speed();

    /*! calculate fs_spd in steps per second
        \f[
            \text{int\_speed}~[\si{step\per\second}]
            = \frac{\text{INT\_SPEED} \cdot 2^{-26}}{250~[\si{\second}] \cdot 10^{-9}}
            \qquad \to \text{ Probably error in datasheet}
        \f]
        \f[
            \text{int\_speed}~[\si{step\per\second}]
            = \frac{\text{INT\_SPEED} \cdot 10^{3} \cdot 2^{-26}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{INT\_SPEED} \cdot 2^{-24} \cdot 10^{9}
            \approx \text{INT\_SPEED} \cdot 59.6
        \f]
    */
    int_speed = int_speed * 596 / 10 ;

    /* return int_speed */
    return int_speed;
}

void l6480_set_int_speed(uint16_t speed) {
    /* local variables */
    l6480_reg_int_speed_t reg;

    /* input value limitation */
    if (speed >= L6480_REG_INT_SPEED_MAX) {
        speed  = L6480_REG_INT_SPEED_MAX;
    }

    /* prepare data local */
    reg.raw.data = speed;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(INT_SPEED),
        L6480_CMD_SETPARAM_LEN(INT_SPEED),
        L6480_CMD_SETPARAM_READ(INT_SPEED),
        reg.array);

    return;
}

void l6480_set_int_speed_millisteps_s(uint32_t speed) {
    /* local variables */

    /*! Calculate int_speed register value
        \f[
            \text{int\_speed}~[\si{step\per\second}]
            = \frac{\text{INT\_SPEED} \cdot 2^{-26}}{250~[\si{\second}] \cdot 10^{-9}}
            \qquad \to \text{ Probably error in datasheet}
        \f]
        \f[
            \text{int\_speed}~[\si{step\per\second}]
            = \frac{\text{INT\_SPEED} \cdot 10^{3} \cdot 2^{-26}}{250~[\si{\second}] \cdot 10^{-9}}
            = \text{INT\_SPEED} \cdot 2^{-24} \cdot 10^{9}
            \approx \text{INT\_SPEED} \cdot 59.6
        \f]
    */
    speed = (uint16_t) (speed * 10 / 596);

    /* send data to device */
    l6480_set_int_speed(speed);

    return;
}

uint8_t l6480_get_st_slp(void) {
    /* local variables */
    l6480_reg_st_slp_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ST_SLP),
        L6480_CMD_GETPARAM_LEN(ST_SLP),
        L6480_CMD_GETPARAM_READ(ST_SLP),
        reg.array);

    /* return st_slp */
    return reg.raw.data;
}

void l6480_set_st_slp(uint8_t slope) {
    /* local variables */
    l6480_reg_st_slp_t reg;

    /* prepare data local */
    reg.raw.data = slope;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ST_SLP),
        L6480_CMD_SETPARAM_LEN(ST_SLP),
        L6480_CMD_SETPARAM_READ(ST_SLP),
        reg.array);

    return;
}

uint8_t l6480_get_fn_slp_acc(void) {
    /* local variables */
    l6480_reg_fn_slp_acc_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FN_SLP_ACC),
        L6480_CMD_GETPARAM_LEN(FN_SLP_ACC),
        L6480_CMD_GETPARAM_READ(FN_SLP_ACC),
        reg.array);

    /* return fn_slp_acc */
    return reg.raw.data;
}

void l6480_set_fn_slp_acc(uint8_t slope) {
    /* local variables */
    l6480_reg_fn_slp_acc_t reg;

    /* prepare data local */
    reg.raw.data = slope;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(FN_SLP_ACC),
        L6480_CMD_SETPARAM_LEN(FN_SLP_ACC),
        L6480_CMD_SETPARAM_READ(FN_SLP_ACC),
        reg.array);

    return;
}

uint8_t l6480_get_fn_slp_dec(void) {
    /* local variables */
    l6480_reg_fn_slp_dec_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(FN_SLP_DEC),
        L6480_CMD_GETPARAM_LEN(FN_SLP_DEC),
        L6480_CMD_GETPARAM_READ(FN_SLP_DEC),
        reg.array);

    /* return fn_slp_dec */
    return reg.raw.data;
}

void l6480_set_fn_slp_dec(uint8_t slope) {
    /* local variables */
    l6480_reg_fn_slp_dec_t reg;

    /* prepare data local */
    reg.raw.data = slope;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(FN_SLP_DEC),
        L6480_CMD_SETPARAM_LEN(FN_SLP_DEC),
        L6480_CMD_SETPARAM_READ(FN_SLP_DEC),
        reg.array);

    return;
}

uint8_t l6480_get_k_therm(void) {
    /* local variables */
    l6480_reg_k_therm_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(K_THERM),
        L6480_CMD_GETPARAM_LEN(K_THERM),
        L6480_CMD_GETPARAM_READ(K_THERM),
        reg.array);

    /* return k_therm */
    return reg.raw.data;
}

uint16_t l6480_get_k_therm_milli(void) {
    /* local variables */
    uint32_t k_therm;

    /* read k_therm from device */
    k_therm = l6480_get_k_therm();

    /* Calculate k_therm coefficient */
    k_therm = (uint16_t) (k_therm * 3125 / 100);

    /* return k_therm */
    return k_therm;
}

void l6480_set_k_therm(uint8_t value) {
    /* local variables */
    l6480_reg_k_therm_t reg;

    /* input value limitation */
    if (value >= L6480_REG_K_THERM_MAX) {
        value  = L6480_REG_K_THERM_MAX;
    }

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(K_THERM),
        L6480_CMD_SETPARAM_LEN(K_THERM),
        L6480_CMD_SETPARAM_READ(K_THERM),
        reg.array);

    return;
}

void l6480_set_k_therm_milli(uint16_t value) {
    /* local variables */

    /* input value limitation */
    if (value >= L6480_REG_K_THERM_MAX_MILLI) {
        value  = L6480_REG_K_THERM_MAX_MILLI;
    }
    if (value <= L6480_REG_K_THERM_MIN_MILLI) {
        value  = L6480_REG_K_THERM_MIN_MILLI;
    }

    /* Calculate k_therm register value */
    value = (uint16_t) ((uint32_t) (value - 1000) * 100 / 3125);

    /* send data to device */
    l6480_set_k_therm(value);

    return;
}

uint8_t l6480_get_adc_out(void) {
    /* local variables */
    l6480_reg_adc_out_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ADC_OUT),
        L6480_CMD_GETPARAM_LEN(ADC_OUT),
        L6480_CMD_GETPARAM_READ(ADC_OUT),
        reg.array);

    /* return adc_out */
    return reg.raw.data;
}

uint8_t l6480_get_ocd_th(void) {
    /* local variables */
    l6480_reg_ocd_th_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(OCD_TH),
        L6480_CMD_GETPARAM_LEN(OCD_TH),
        L6480_CMD_GETPARAM_READ(OCD_TH),
        reg.array);

    /* return ocd_th */
    return reg.raw.data;
}

uint16_t l6480_get_ocd_th_millivolt(void) {
    /* local variables */
    uint32_t ocd_th;

    /* read ocd_th from device */
    ocd_th = l6480_get_ocd_th();

    /* Calculate ocd_th in millivolts */
    ocd_th = (uint16_t) ((ocd_th + 1) * 3125 / 100);

    /* return ocd_th */
    return ocd_th;
}

void l6480_set_ocd_th(uint8_t threshold) {
    /* local variables */
    l6480_reg_ocd_th_t reg;

    /* input value limitation */
    if (threshold >= L6480_REG_OCD_TH_MAX) {
        threshold  = L6480_REG_OCD_TH_MAX;
    }

    /* prepare data local */
    reg.raw.data = threshold;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(OCD_TH),
        L6480_CMD_SETPARAM_LEN(OCD_TH),
        L6480_CMD_SETPARAM_READ(OCD_TH),
        reg.array);

    return;
}

void l6480_set_ocd_th_millivolt(uint16_t threshold) {
    /* local variables */

    /* input value limitation */
    if (threshold >= L6480_REG_OCD_TH_MAX_MILLI) {
        threshold  = L6480_REG_OCD_TH_MAX_MILLI;
    }
    if (threshold <= L6480_REG_OCD_TH_MIN_MILLI) {
        threshold  = L6480_REG_OCD_TH_MIN_MILLI;
    }

    /* Calculate ocd_th register value */
    threshold = (uint8_t) ((uint32_t) (threshold) * 100 / 3125) - 1;

    /* send data to device */
    l6480_set_ocd_th(threshold);

    return;
}

uint8_t l6480_get_stall_th(void) {
    /* local variables */
    l6480_reg_stall_th_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STALL_TH),
        L6480_CMD_GETPARAM_LEN(STALL_TH),
        L6480_CMD_GETPARAM_READ(STALL_TH),
        reg.array);

    /* return stall_th */
    return reg.raw.data;
}

uint16_t l6480_get_stall_th_millivolt(void) {
    /* local variables */
    uint32_t stall_th;

    /* read stall_th from device */
    stall_th = l6480_get_stall_th();

    /* Calculate stall_th in millivolts */
    stall_th = (uint16_t) ((stall_th + 1) * 3125 / 100);

    /* return stall_th */
    return stall_th;
}

void l6480_set_stall_th(uint8_t threshold) {
    /* local variables */
    l6480_reg_stall_th_t reg;

    /* input value limitation */
    if (threshold >= L6480_REG_STALL_TH_MAX) {
        threshold  = L6480_REG_STALL_TH_MAX;
    }

    /* prepare data local */
    reg.raw.data = threshold;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STALL_TH),
        L6480_CMD_SETPARAM_LEN(STALL_TH),
        L6480_CMD_SETPARAM_READ(STALL_TH),
        reg.array);

    return;
}

void l6480_set_stall_th_millivolt(uint16_t threshold) {
    /* local variables */

    /* input value limitation */
    if (threshold >= L6480_REG_STALL_TH_MAX_MILLI) {
        threshold  = L6480_REG_STALL_TH_MAX_MILLI;
    }
    if (threshold <= L6480_REG_STALL_TH_MIN_MILLI) {
        threshold  = L6480_REG_STALL_TH_MIN_MILLI;
    }

    /* Calculate stall_th register value */
    threshold = (uint8_t) ((uint32_t) (threshold) * 100 / 3125) - 1;

    /* send data to device */
    l6480_set_stall_th(threshold);

    return;
}

uint8_t l6480_get_step_mode(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* return step_mode */
    return reg.raw.data;
}

uint8_t l6480_get_step_mode_sync_en(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* return sync_en */
    return reg.reg.sync_en;
}

uint8_t l6480_get_step_mode_sync_sel(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* return sync_sel */
    return reg.reg.sync_sel;
}

uint8_t l6480_get_step_mode_step_sel(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* return step_sel */
    return reg.reg.step_sel;
}

uint8_t l6480_get_step_mode_step_sel_steps(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* return step_sel */
    return (1 << reg.reg.step_sel);
}

void l6480_set_step_mode(uint8_t value) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STEP_MODE),
        L6480_CMD_SETPARAM_LEN(STEP_MODE),
        L6480_CMD_SETPARAM_READ(STEP_MODE),
        reg.array);

    return;
}

void l6480_set_step_mode_sync_en_on(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* prepare data local */
    reg.reg.sync_en = 1;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STEP_MODE),
        L6480_CMD_SETPARAM_LEN(STEP_MODE),
        L6480_CMD_SETPARAM_READ(STEP_MODE),
        reg.array);

    return;
}

void l6480_set_step_mode_sync_en_off(void) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* prepare data local */
    reg.reg.sync_en = 0;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STEP_MODE),
        L6480_CMD_SETPARAM_LEN(STEP_MODE),
        L6480_CMD_SETPARAM_READ(STEP_MODE),
        reg.array);

    return;
}

void l6480_set_step_mode_sync_sel(uint8_t sync_sel) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* prepare data local */
    reg.reg.sync_sel = sync_sel;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STEP_MODE),
        L6480_CMD_SETPARAM_LEN(STEP_MODE),
        L6480_CMD_SETPARAM_READ(STEP_MODE),
        reg.array);

    return;
}

void l6480_set_step_mode_step_sel(uint8_t sel) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* prepare data local */
    reg.reg.step_sel = sel;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STEP_MODE),
        L6480_CMD_SETPARAM_LEN(STEP_MODE),
        L6480_CMD_SETPARAM_READ(STEP_MODE),
        reg.array);

    return;
}

void l6480_set_step_mode_step_sel_steps(uint8_t steps) {
    /* local variables */
    l6480_reg_step_mode_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STEP_MODE),
        L6480_CMD_GETPARAM_LEN(STEP_MODE),
        L6480_CMD_GETPARAM_READ(STEP_MODE),
        reg.array);

    /* prepare data local */
    if (steps <=      (1 << L6480_STEP_SEL_FULL)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_FULL;
    }
    else if (steps <= (1 << L6480_STEP_SEL_HALF)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_HALF;
    }
    else if (steps <= (1 << L6480_STEP_SEL_MICRO_4)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_4;
    }
    else if (steps <= (1 << L6480_STEP_SEL_MICRO_8)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_8;
    }
    else if (steps <= (1 << L6480_STEP_SEL_MICRO_16)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_16;
    }
    else if (steps <= (1 << L6480_STEP_SEL_MICRO_32)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_32;
    }
    else if (steps <= (1 << L6480_STEP_SEL_MICRO_64)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_64;
    }
    else if (steps <= (1 << L6480_STEP_SEL_MICRO_128)) {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_128;
    }
    else {
        reg.reg.sync_sel  = L6480_STEP_SEL_MICRO_128;
    }

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(STEP_MODE),
        L6480_CMD_SETPARAM_LEN(STEP_MODE),
        L6480_CMD_SETPARAM_READ(STEP_MODE),
        reg.array);

    return;
}

uint8_t l6480_get_alarm_en(void) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* return step_sel */
    return reg.raw.data;
}

void l6480_set_alarm_en(uint8_t alarm_en) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* prepare data local */
    reg.raw.data = alarm_en;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_overcurrent(uint8_t overcurrent) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.overcurrent = overcurrent;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_th_shutdown(uint8_t th_shutdown) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.th_shutdown = th_shutdown;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_th_warning(uint8_t th_warning) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.th_warning = th_warning;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_uvlo(uint8_t uvlo) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.uvlo = uvlo;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_adc_uvlo(uint8_t adc_uvlo) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.adc_uvlo = adc_uvlo;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_stall_detect(uint8_t stall_det) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.stall_det = stall_det;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_switch_on(uint8_t switch_on) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.switch_on = switch_on;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

void l6480_set_alarm_en_command_err(uint8_t cmd_err) {
    /* local variables */
    l6480_reg_alarm_en_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(ALARM_EN),
        L6480_CMD_GETPARAM_LEN(ALARM_EN),
        L6480_CMD_GETPARAM_READ(ALARM_EN),
        reg.array);

    /* prepare data local */
    reg.reg.cmd_error = cmd_err;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(ALARM_EN),
        L6480_CMD_SETPARAM_LEN(ALARM_EN),
        L6480_CMD_SETPARAM_READ(ALARM_EN),
        reg.array);

    return;
}

uint16_t l6480_get_gatecfg1(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* return gatecfg1 */
    return reg.raw.data;
}

uint8_t l6480_get_gatecfg1_wd_en(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* return step_sel */
    return reg.reg.wd_en;
}

uint8_t l6480_get_gatecfg1_tboost(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* return step_sel */
    return reg.reg.tboost;
}

uint16_t l6480_get_gatecfg1_tboost_nanosecond(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;
    uint16_t tboost;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* Calculate tboost in nanoseconds */
    switch (reg.reg.tboost) {
        case L6480_GATECFG1_TBOOST_0:
            tboost =    0;
            break;
        case L6480_GATECFG1_TBOOST_62:
            tboost =   62;
            break;
        case L6480_GATECFG1_TBOOST_125:
            tboost =  125;
            break;
        case L6480_GATECFG1_TBOOST_250:
            tboost =  250;
            break;
        case L6480_GATECFG1_TBOOST_375:
            tboost =  375;
            break;
        case L6480_GATECFG1_TBOOST_500:
            tboost =  500;
            break;
        case L6480_GATECFG1_TBOOST_750:
            tboost =  750;
            break;
        case L6480_GATECFG1_TBOOST_1000:
            tboost = 1000;
            break;
        default:
            tboost = 0;
            break;
    }

    /* return step_sel */
    return tboost;
}

uint8_t l6480_get_gatecfg1_igate(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* return step_sel */
    return reg.reg.igate;
}

uint8_t l6480_get_gatecfg1_igate_milliampere(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;
    uint8_t igate;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* Calculate igate in milliamperes */
    switch (reg.reg.igate) {
        case L6480_GATECFG1_IGATE_4_A:
            igate =  4;
            break;
        case L6480_GATECFG1_IGATE_4_B:
            igate =  4;
            break;
        case L6480_GATECFG1_IGATE_8:
            igate =  8;
            break;
        case L6480_GATECFG1_IGATE_16:
            igate = 16;
            break;
        case L6480_GATECFG1_IGATE_24:
            igate = 24;
            break;
        case L6480_GATECFG1_IGATE_32:
            igate = 32;
            break;
        case L6480_GATECFG1_IGATE_64:
            igate = 64;
            break;
        case L6480_GATECFG1_IGATE_96:
            igate = 96;
            break;
        default:
            igate = 0;
            break;
    }

    /* return step_sel */
    return igate;
}

uint8_t l6480_get_gatecfg1_tcc(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* return tcc */
    return reg.reg.tcc;
}

uint16_t l6480_get_gatecfg1_tcc_nanosecond(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;
    uint16_t tcc;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* Calculate tcc in nanoseconds */
    if (reg.reg.tcc <= 29) {
        tcc = (uint16_t) reg.reg.tcc * 125;
    }
    else {
        tcc = 3750;
    }

    /* return tcc */
    return tcc;
}

void l6480_set_gatecfg1(uint16_t value) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_wd_en(uint8_t wd_en) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.wd_en = wd_en;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_wd_en_on(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.wd_en = 1;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_wd_en_off(void) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.wd_en = 0;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_tboost(uint8_t time) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.tboost = time;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_tboost_nanosecond(uint16_t time) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    if (time == 0) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_0;
    }
    else if (time <= 62) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_62;
    }
    else if (time <= 125) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_125;
    }
    else if (time <= 250) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_250;
    }
    else if (time <= 375) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_375;
    }
    else if (time <= 500) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_500;
    }
    else if (time <= 750) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_750;
    }
    else if (time <= 1000) {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_1000;
    }
    else {
        reg.reg.tboost = L6480_GATECFG1_TBOOST_1000;
    }

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_igate(uint8_t current) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.igate = current;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_igate_milliampere(uint8_t current) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    if (current <= 4) {
        reg.reg.igate = L6480_GATECFG1_IGATE_4_A;
    }
    else if (current <= 8) {
        reg.reg.igate = L6480_GATECFG1_IGATE_8;
    }
    else if (current <= 16) {
        reg.reg.igate = L6480_GATECFG1_IGATE_16;
    }
    else if (current <= 24) {
        reg.reg.igate = L6480_GATECFG1_IGATE_24;
    }
    else if (current <= 32) {
        reg.reg.igate = L6480_GATECFG1_IGATE_32;
    }
    else if (current <= 64) {
        reg.reg.igate = L6480_GATECFG1_IGATE_64;
    }
    else if (current <= 96) {
        reg.reg.igate = L6480_GATECFG1_IGATE_96;
    }
    else {
        reg.reg.igate = L6480_GATECFG1_IGATE_96;
    }

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_tcc(uint8_t time) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.tcc = time;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

void l6480_set_gatecfg1_tcc_nanosecond(uint16_t time) {
    /* local variables */
    l6480_reg_gatecfg1_t reg;

    /* input value limitation */
    if (time >= L6480_REG_GATECFG1_TCC_NANO_MAX) {
        time  = L6480_REG_GATECFG1_TCC_NANO_MAX;
    }

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG1),
        L6480_CMD_GETPARAM_LEN(GATECFG1),
        L6480_CMD_GETPARAM_READ(GATECFG1),
        reg.array);

    /* prepare data local */
    reg.reg.tcc = time / 125;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG1),
        L6480_CMD_SETPARAM_LEN(GATECFG1),
        L6480_CMD_SETPARAM_READ(GATECFG1),
        reg.array);

    return;
}

uint8_t l6480_get_gatecfg2(void) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* return gatecfg2 */
    return reg.raw.data;
}

uint8_t l6480_get_gatecfg2_tblank(void) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* return tblank */
    return reg.reg.tblank;
}

uint16_t l6480_get_gatecfg2_tblank_nanosecond(void) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* return tblank in nanoseconds */
    return ((uint16_t) reg.reg.tblank + 1) * 125;
}

uint8_t l6480_get_gatecfg2_tdt(void) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* return tdt */
    return reg.reg.tdt;
}

uint16_t l6480_get_gatecfg2_tdt_nanosecond(void) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* return tdt in nanoseconds */
    return ((uint16_t) reg.reg.tdt + 1) * 125;
}

void l6480_set_gatecfg2(uint8_t value) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG2),
        L6480_CMD_SETPARAM_LEN(GATECFG2),
        L6480_CMD_SETPARAM_READ(GATECFG2),
        reg.array);

    return;
}

void l6480_set_gatecfg2_tblank(uint8_t time) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* prepare data local */
    reg.reg.tblank = time;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG2),
        L6480_CMD_SETPARAM_LEN(GATECFG2),
        L6480_CMD_SETPARAM_READ(GATECFG2),
        reg.array);

    return;
}

void l6480_set_gatecfg2_tblank_nanosecond(uint16_t time) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* input value limitation */
    if (time <= L6480_REG_GATECFG2_TBLANK_NANO_MIN) {
        time  = L6480_REG_GATECFG2_TBLANK_NANO_MIN;
    }
    if (time >= L6480_REG_GATECFG2_TBLANK_NANO_MAX) {
        time  = L6480_REG_GATECFG2_TBLANK_NANO_MAX;
    }

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* prepare data local */
    reg.reg.tblank = (time / 125) - 1;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG2),
        L6480_CMD_SETPARAM_LEN(GATECFG2),
        L6480_CMD_SETPARAM_READ(GATECFG2),
        reg.array);

    return;
}

void l6480_set_gatecfg2_tdt(uint8_t time) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* prepare data local */
    reg.reg.tdt = time;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG2),
        L6480_CMD_SETPARAM_LEN(GATECFG2),
        L6480_CMD_SETPARAM_READ(GATECFG2),
        reg.array);

    return;
}

void l6480_set_gatecfg2_tdt_nanosecond(uint16_t time) {
    /* local variables */
    l6480_reg_gatecfg2_t reg;

    /* input value limitation */
    if (time <= L6480_REG_GATECFG2_TDT_NANO_MIN) {
        time  = L6480_REG_GATECFG2_TDT_NANO_MIN;
    }
    if (time >= L6480_REG_GATECFG2_TDT_NANO_MAX) {
        time  = L6480_REG_GATECFG2_TDT_NANO_MAX;
    }

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(GATECFG2),
        L6480_CMD_GETPARAM_LEN(GATECFG2),
        L6480_CMD_GETPARAM_READ(GATECFG2),
        reg.array);

    /* prepare data local */
    reg.reg.tdt = (time / 125) - 1;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(GATECFG2),
        L6480_CMD_SETPARAM_LEN(GATECFG2),
        L6480_CMD_SETPARAM_READ(GATECFG2),
        reg.array);

    return;
}

uint16_t l6480_get_config(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.raw.data;
}

uint8_t l6480_get_config_f_pwm_int(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.f_pwm_int;
}

uint8_t l6480_get_config_f_pwm_dec(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.f_pwm_dec;
}

uint8_t l6480_get_config_vccval(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.vccval;
}

uint8_t l6480_get_config_uvloval(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.uvloval;
}

uint8_t l6480_get_config_oc_sd(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.oc_sd;
}

uint8_t l6480_get_config_en_vscomp(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.en_vscomp;
}

uint8_t l6480_get_config_sw_mode(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.sw_mode;
}

uint8_t l6480_get_config_ext_clk(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.ext_clk;
}

uint8_t l6480_get_config_osc_sel(void) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* return config */
    return reg.reg.osc_sel;
}

void l6480_set_config(uint16_t value) {
    /* local variables */
    l6480_reg_config_t reg;

    /* prepare data local */
    reg.raw.data = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_f_pwm_int(uint8_t factor) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.f_pwm_int = factor;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_f_pwm_dec(uint8_t factor) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.f_pwm_dec = factor;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_vccval(uint8_t value) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.vccval = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_uvloval(uint8_t value) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.uvloval = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_oc_sd(uint8_t value) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.oc_sd = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_en_vscomp(uint8_t value) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.en_vscomp = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_sw_mode(uint8_t mode) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.sw_mode = mode;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_ext_clk(uint8_t value) {
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.ext_clk = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

void l6480_set_config_osc_sel(uint8_t value){
    /* local variables */
    l6480_reg_config_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(CONFIG),
        L6480_CMD_GETPARAM_LEN(CONFIG),
        L6480_CMD_GETPARAM_READ(CONFIG),
        reg.array);

    /* prepare data local */
    reg.reg.osc_sel = value;

    /* send data to device */
    l6480_send_cmd( L6480_CMD_SETPARAM(CONFIG),
        L6480_CMD_SETPARAM_LEN(CONFIG),
        L6480_CMD_SETPARAM_READ(CONFIG),
        reg.array);

    return;
}

uint16_t l6480_get_status(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.raw.data;
}

uint8_t l6480_get_status_step_loss_b(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.step_loss_b;
}

uint8_t l6480_get_status_step_loss_a(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.step_loss_a;
}

uint8_t l6480_get_status_ocd(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.ocd;
}

uint8_t l6480_get_status_th_status(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.th_status;
}

uint8_t l6480_get_status_uvlo_adc(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.uvlo_adc;
}

uint8_t l6480_get_status_uvlo(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.uvlo;
}

uint8_t l6480_get_status_stck_mod(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.stck_mod;
}

uint8_t l6480_get_status_cmd_error(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.cmd_error;
}

uint8_t l6480_get_status_mot_status(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.mot_status;
}

uint8_t l6480_get_status_dir(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.dir;
}

uint8_t l6480_get_status_sw_evn(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.sw_evn;
}

uint8_t l6480_get_status_sw_f(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.sw_f;
}

uint8_t l6480_get_status_busy(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.busy;
}

uint8_t l6480_get_status_hiz(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETPARAM(STATUS),
        L6480_CMD_GETPARAM_LEN(STATUS),
        L6480_CMD_GETPARAM_READ(STATUS),
        reg.array);

    /* return status */
    return reg.reg.hiz;
}

void l6480_cmd_nop(void) {
    l6480_send_cmd( L6480_CMD_NOP,
        L6480_CMD_NOP_LEN,
        L6480_CMD_NOP_READ,
        0);
}

void l6480_cmd_run(l6480_dir_t dir, uint32_t speed){
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (speed >= L6480_CMD_RUN_SPEED_MAX) {
        speed  = L6480_CMD_RUN_SPEED_MAX;
    }

    /* Prepare data local */
    value.speed.data = speed;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_RUN(dir),
        L6480_CMD_RUN_LEN(dir),
        L6480_CMD_RUN_READ(dir),
        value.array);

    return;
}

void l6480_cmd_run_millisteps_s(l6480_dir_t dir, uint32_t speed){
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (speed >= L6480_CMD_RUN_SPEED_MILLI_MAX) {
        speed  = L6480_CMD_RUN_SPEED_MILLI_MAX;
    }

    /*! calculate speed in millisteps per second
        \f[
            \text{speed}~[\si{step\per\second}]
            = \frac{\text{SPEED} \cdot 2^{-28}}{250~[\si{\second}]}
        \f]
        \f[
            \text{speed}~[\si{\milli step\per\second}]
            = \frac{\text{SPEED} \cdot 1000 \cdot 2^{-28}}{250~[\si{\second}]}
            = \text{SPEED} \cdot 2^{-26} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 14.90
        \f]
    */
    speed = speed * 10 / 149;

    /* Prepare data local */
    value.speed.data = speed;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_RUN(dir),
        L6480_CMD_RUN_LEN(dir),
        L6480_CMD_RUN_READ(dir),
        value.array);

    return;
}

void l6480_cmd_stepclock(l6480_dir_t dir) {
    l6480_send_cmd( L6480_CMD_STEPCLOCK(dir),
        L6480_CMD_STEPCLOCK_LEN(dir),
        L6480_CMD_STEPCLOCK_READ(dir),
        0);
}

void l6480_cmd_move(l6480_dir_t dir, uint32_t n_step) {
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (n_step >= L6480_CMD_MOVE_STEP_MAX) {
        n_step  = L6480_CMD_MOVE_STEP_MAX;
    }

    /* Prepare data local */
    value.step.data = n_step;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_MOVE(dir),
        L6480_CMD_MOVE_LEN(dir),
        L6480_CMD_MOVE_READ(dir),
        value.array);

    return;
}

void l6480_cmd_goto(int32_t abs_pos) {
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (abs_pos <= L6480_CMD_GOTO_STEP_MIN) {
        abs_pos  = L6480_CMD_GOTO_STEP_MIN;
    }
    if (abs_pos >= L6480_CMD_GOTO_STEP_MAX) {
        abs_pos  = L6480_CMD_GOTO_STEP_MAX;
    }

    /* Prepare data local */
    value.position.data = abs_pos;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_GOTO,
        L6480_CMD_GOTO_LEN,
        L6480_CMD_GOTO_READ,
        value.array);

    return;
}

void l6480_cmd_goto_dir(l6480_dir_t dir, int32_t abs_pos) {
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (abs_pos <= L6480_CMD_GOTO_STEP_MIN) {
        abs_pos  = L6480_CMD_GOTO_STEP_MIN;
    }
    if (abs_pos >= L6480_CMD_GOTO_STEP_MAX) {
        abs_pos  = L6480_CMD_GOTO_STEP_MAX;
    }

    /* Prepare data local */
    value.position.data = abs_pos;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_GOTO_DIR(dir),
        L6480_CMD_GOTO_DIR_LEN(dir),
        L6480_CMD_GOTO_DIR_READ(dir),
        value.array);

    return;
}

void l6480_cmd_gountil(l6480_act_t act, l6480_dir_t dir, uint32_t speed) {
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (speed >= L6480_CMD_GOUNTIL_SPEED_MAX) {
        speed  = L6480_CMD_GOUNTIL_SPEED_MAX;
    }

    /* Prepare data local */
    value.speed.data = speed;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_GOUNTIL(act, dir),
        L6480_CMD_GOUNTIL_LEN(act, dir),
        L6480_CMD_GOUNTIL_READ(act, dir),
        value.array);

    return;
}

void l6480_cmd_gountil_millisteps_s(l6480_act_t act, l6480_dir_t dir, uint32_t speed) {
    /* local variables */
    l6480_cmd_val_t value;

    /* input value limitation */
    if (speed >= L6480_CMD_GOUNTIL_SPEED_MILLI_MAX) {
        speed  = L6480_CMD_GOUNTIL_SPEED_MILLI_MAX;
    }

    /*! calculate speed in millisteps per second
        \f[
            \text{speed}~[\si{step\per\second}]
            = \frac{\text{SPEED} \cdot 2^{-28}}{250~[\si{\second}]}
        \f]
        \f[
            \text{speed}~[\si{\milli step\per\second}]
            = \frac{\text{SPEED} \cdot 1000 \cdot 2^{-28}}{250~[\si{\second}]}
            = \text{SPEED} \cdot 2^{-26} \cdot 10^{9}
            \approx \text{MIN\_SPEED} \cdot 14.90
        \f]
    */
    speed = speed * 10 / 149;

    /* Prepare data local */
    value.speed.data = speed;

    /* Send command to device */
    l6480_send_cmd( L6480_CMD_GOUNTIL(act, dir),
        L6480_CMD_GOUNTIL_LEN(act, dir),
        L6480_CMD_GOUNTIL_READ(act, dir),
        value.array);

    return;
}

void l6480_cmd_releasesw(l6480_act_t act, l6480_dir_t dir) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_RELEASESW(act, dir),
        L6480_CMD_RELEASESW_LEN(act, dir),
        L6480_CMD_RELEASESW_READ(act, dir),
        0);

    return;
}
void l6480_cmd_gohome(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_GOHOME,
        L6480_CMD_GOHOME_LEN,
        L6480_CMD_GOHOME_READ,
        0);

    return;
}

void l6480_cmd_gomark(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_GOMARK,
        L6480_CMD_GOMARK_LEN,
        L6480_CMD_GOMARK_READ,
        0);

    return;
}

void l6480_cmd_resetpos(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_RESETPOS,
        L6480_CMD_RESETPOS_LEN,
        L6480_CMD_RESETPOS_READ,
        0);

    return;
}

void l6480_cmd_resetdevice(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_RESETDEVICE,
        L6480_CMD_RESETDEVICE_LEN,
        L6480_CMD_RESETDEVICE_READ,
        0);

    return;
}

void l6480_cmd_softstop(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_SOFTSTOP,
        L6480_CMD_SOFTSTOP_LEN,
        L6480_CMD_SOFTSTOP_READ,
        0);

    return;
}

void l6480_cmd_hardstop(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_HARDSTOP,
        L6480_CMD_HARDSTOP_LEN,
        L6480_CMD_HARDSTOP_READ,
        0);

    return;
}

void l6480_cmd_softhiz(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_SOFTHIZ,
        L6480_CMD_SOFTHIZ_LEN,
        L6480_CMD_SOFTHIZ_READ,
        0);

    return;
}

void l6480_cmd_hardhiz(void) {
    /* Send command to device */
    l6480_send_cmd( L6480_CMD_HARDHIZ,
        L6480_CMD_HARDHIZ_LEN,
        L6480_CMD_HARDHIZ_READ,
        0);

    return;
}

uint16_t l6480_cmd_getstatus(void) {
    /* local variables */
    l6480_reg_status_t reg;

    /* read data from device */
    l6480_send_cmd( L6480_CMD_GETSTATUS,
        L6480_CMD_GETSTATUS_LEN,
        L6480_CMD_GETSTATUS_READ,
        reg.array);

    /* return status */
    return reg.raw.data;
}

#if PL_HAS_SHELL
static uint8_t PrintStatus(const CLS1_StdIOType *io) {
    l6480_reg_status_t status;
    CLS1_SendStatusStr((unsigned char*)"l6480", (unsigned char*)"\r\n", io->stdOut);
    status.raw.data = l6480_cmd_getstatus();
    CLS1_SendStatusStr((unsigned char*)"  High Z",                  (status.reg.hiz         ?(unsigned char*)"TRUE\r\n"     :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  BUSY",                    (status.reg.busy        ?(unsigned char*)"Ready\r\n"    :(unsigned char*)"Executing\r\n"),  io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Switch",                  (status.reg.sw_f        ?(unsigned char*)"Closed\r\n"   :(unsigned char*)"Open\r\n"),       io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Switch turn on event",    (status.reg.sw_evn      ?(unsigned char*)"TRUE\r\n"     :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Direction",               (status.reg.dir         ?(unsigned char*)"Forward\r\n"  :(unsigned char*)"Reverse\r\n"),    io-> stdOut);
    switch (status.reg.mot_status) {
        case L6480_STATUS_MOT_STATUS_STOP:
            CLS1_SendStatusStr((unsigned char*)"  Motor status",    (unsigned char*)"Stopped\r\n",          io-> stdOut);
            break;
        case L6480_STATUS_MOT_STATUS_ACC:
            CLS1_SendStatusStr((unsigned char*)"  Motor status",    (unsigned char*)"Acceleration\r\n",     io-> stdOut);
            break;
        case L6480_STATUS_MOT_STATUS_DEC:
            CLS1_SendStatusStr((unsigned char*)"  Motor status",    (unsigned char*)"Deceleration\r\n",     io-> stdOut);
            break;
        case L6480_STATUS_MOT_STATUS_CONST:
            CLS1_SendStatusStr((unsigned char*)"  Motor status",    (unsigned char*)"Constant speed\r\n",   io-> stdOut);
            break;
        default:
            CLS1_SendStatusStr((unsigned char*)"  Motor status",    (unsigned char*)"Unknown status\r\n",   io-> stdOut);
            break;
    }
    CLS1_SendStatusStr((unsigned char*)"  Command error",           (status.reg.cmd_error   ?(unsigned char*)"Unknown Command\r\n"  :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Step-clock mode",         (status.reg.stck_mod    ?(unsigned char*)"TRUE\r\n"             :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Undervoltage lockout",    (status.reg.uvlo        ?(unsigned char*)"TRUE\r\n"             :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  ADC Undervoltage event",  (status.reg.uvlo_adc    ?(unsigned char*)"TRUE\r\n"             :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    switch (status.reg.th_status) {
        case L6480_STATUS_TH_STATUS_NORMAL:
            CLS1_SendStatusStr((unsigned char*)"  Thermal status",  (unsigned char*)"Normal\r\n",           io-> stdOut);
            break;
        case L6480_STATUS_TH_STATUS_WARNING:
            CLS1_SendStatusStr((unsigned char*)"  Thermal status",  (unsigned char*)"Warning\r\n",          io-> stdOut);
            break;
        case L6480_STATUS_TH_STATUS_BRI_SHTDWN:
            CLS1_SendStatusStr((unsigned char*)"  Thermal status",  (unsigned char*)"Bridge shutdown\r\n",  io-> stdOut);
            break;
        case L6480_STATUS_TH_STATUS_DEV_SHTDWN:
            CLS1_SendStatusStr((unsigned char*)"  Thermal status",  (unsigned char*)"Device shutdown\r\n",  io-> stdOut);
            break;
        default:
            CLS1_SendStatusStr((unsigned char*)"  Thermal status",  (unsigned char*)"Unknown status\r\n",   io-> stdOut);
            break;
    }
    CLS1_SendStatusStr((unsigned char*)"  Overcurrent",             (status.reg.ocd         ?(unsigned char*)"TRUE\r\n"             :(unsigned char*)"FALSE\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Stall on bridge A",       (status.reg.step_loss_a ?(unsigned char*)"None\r\n"             :(unsigned char*)"Stall\r\n"),      io-> stdOut);
    CLS1_SendStatusStr((unsigned char*)"  Stall on bridge B",       (status.reg.step_loss_b ?(unsigned char*)"None\r\n"             :(unsigned char*)"Stall\r\n"),      io-> stdOut);
    return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
    CLS1_SendHelpStr((unsigned char*)"l6480",                   (unsigned char*)"Group of l6480 commands\r\n", io->stdOut);
    CLS1_SendHelpStr((unsigned char*)"  help|status",           (unsigned char*)"Print help or status information\r\n", io->stdOut);
    CLS1_SendHelpStr((unsigned char*)"  run (f|r) <speed>",     (unsigned char*)"Run stepper with given direction and speed in millisteps per second\r\n", io->stdOut);
    CLS1_SendHelpStr((unsigned char*)"  goto (f|r) <position>", (unsigned char*)"Go to given position with optional direction\r\n", io->stdOut);
    CLS1_SendHelpStr((unsigned char*)"  move (f|r) <steps>",    (unsigned char*)"Move given number of steps with given direction\r\n", io->stdOut);
    return ERR_OK;
}

static uint8_t ParseCmdRunParameter(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
    const unsigned char *p;
    uint32_t val32u;
    uint8_t res = ERR_OK;
    l6480_dir_t dir;

    if (UTIL1_strncmp((char*)cmd, (char*)"f", sizeof("f")-1)==0 ||
        UTIL1_strncmp((char*)cmd, (char*)"F", sizeof("F")-1)==0) {
        dir = L6480_DIR_FWD;
        p = cmd+sizeof("f");
    }
    else if (UTIL1_strncmp((char*)cmd, (char*)"r", sizeof("r")-1)==0 ||
            UTIL1_strncmp((char*)cmd, (char*)"R", sizeof("R")-1)==0) {
        dir = L6480_DIR_REV;
        p = cmd+sizeof("r");
    }
    else { /* No direction given is threated as forward */
        dir = L6480_DIR_FWD;
        p = cmd;
    }
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
        l6480_cmd_run_millisteps_s(dir, val32u);
        *handled = TRUE;
    }
    else {
        CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
        res = ERR_FAILED;
    }
    return res;
}

static uint8_t ParseCmdGotoParameter(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
    const unsigned char *p;
    uint32_t val32u;
    uint8_t res = ERR_OK;
    l6480_dir_t dir;
    uint8_t dir_given;

    if (UTIL1_strncmp((char*)cmd, (char*)"f", sizeof("f")-1)==0 ||
        UTIL1_strncmp((char*)cmd, (char*)"F", sizeof("F")-1)==0) {
        dir = L6480_DIR_FWD;
        dir_given = TRUE;
        p = cmd+sizeof("f");
    }
    else if (UTIL1_strncmp((char*)cmd, (char*)"r", sizeof("r")-1)==0 ||
            UTIL1_strncmp((char*)cmd, (char*)"R", sizeof("R")-1)==0) {
        dir = L6480_DIR_REV;
        dir_given = TRUE;
        p = cmd+sizeof("r");
    }
    else { /* No direction given -> go to position through shortest path */
        dir_given = FALSE;
        p = cmd;
    }
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
        if (dir_given) {
            l6480_cmd_goto_dir(dir, val32u);
        }
        else {
            l6480_cmd_goto(val32u);
        }
        *handled = TRUE;
    }
    else {
        CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
        res = ERR_FAILED;
    }
    return res;
}

static uint8_t ParseCmdMoveParameter(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
    const unsigned char *p;
    uint32_t val32u;
    uint8_t res = ERR_OK;
    l6480_dir_t dir;

    if (UTIL1_strncmp((char*)cmd, (char*)"f", sizeof("f")-1)==0 ||
        UTIL1_strncmp((char*)cmd, (char*)"F", sizeof("F")-1)==0) {
        dir = L6480_DIR_FWD;
        p = cmd+sizeof("f");
    }
    else if (UTIL1_strncmp((char*)cmd, (char*)"r", sizeof("r")-1)==0 ||
            UTIL1_strncmp((char*)cmd, (char*)"R", sizeof("R")-1)==0) {
        dir = L6480_DIR_REV;
        p = cmd+sizeof("r");
    }
    else { /* No direction given is threated as forward */
        dir = L6480_DIR_FWD;
        p = cmd;
        /* Alternative implementation: No direction threated as error */
        // res = ERR_FAILED;
        // return res;
    }
    if (UTIL1_ScanDecimal32uNumber(&p, &val32u)==ERR_OK) {
        l6480_cmd_move(dir, val32u);
        *handled = TRUE;
    }
    else {
        CLS1_SendStr((unsigned char*)"Wrong argument\r\n", io->stdErr);
        res = ERR_FAILED;
    }
    return res;
}

uint8_t l6480_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
    if      (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP)     == 0 ||
             UTIL1_strcmp((char*)cmd, "l6480 help")      == 0 ||
             UTIL1_strcmp((char*)cmd, "stepper help")    == 0) {
        *handled = TRUE;
        return PrintHelp(io);
    }
    else if (UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS)     == 0 ||
             UTIL1_strcmp((char*)cmd, "l6480 status")      == 0 ||
             UTIL1_strcmp((char*)cmd, "stepper status")    == 0) {
        *handled = TRUE;
        return PrintStatus(io);
    }
    else if (UTIL1_strncmp((char*)cmd, "l6480 run ",    sizeof("l6480 run ")-1)     ==0) {
        return ParseCmdRunParameter(cmd+sizeof("l6480 run ")-1, handled, io);
    }
    else if (UTIL1_strncmp((char*)cmd, "stepper run ",  sizeof("stepper run ")-1)   ==0) {
        return ParseCmdRunParameter(cmd+sizeof("stepper run ")-1, handled, io);
    }
    else if (UTIL1_strncmp((char*)cmd, "l6480 goto ",    sizeof("l6480 goto ")-1)     ==0) {
        return ParseCmdGotoParameter(cmd+sizeof("l6480 goto ")-1, handled, io);
    }
    else if (UTIL1_strncmp((char*)cmd, "stepper goto ",  sizeof("stepper goto ")-1)   ==0) {
        return ParseCmdGotoParameter(cmd+sizeof("stepper goto ")-1, handled, io);
    }
    else if (UTIL1_strncmp((char*)cmd, "l6480 move ",    sizeof("l6480 move ")-1)     ==0) {
        return ParseCmdMoveParameter(cmd+sizeof("l6480 move ")-1, handled, io);
    }
    else if (UTIL1_strncmp((char*)cmd, "stepper move ",  sizeof("stepper move ")-1)   ==0) {
        return ParseCmdMoveParameter(cmd+sizeof("stepper move ")-1, handled, io);
    }
    return ERR_OK;
}
#endif /* PL_HAS_SHELL */
