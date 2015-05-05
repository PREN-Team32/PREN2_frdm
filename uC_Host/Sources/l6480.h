/*!
 *  ____  ____  _____ _   _       _____ _____ 
 * |  _ \|  _ \| ____| \ | |     | ____|_   _|
 * | |_) | |_) |  _| |  \| |_____|  _|   | |  
 * |  __/|  _ <| |___| |\  |_____| |___  | |  
 * |_|   |_| \_\_____|_| \_|     |_____| |_|  
 *                                            
 * \file l6480.h
 * \brief Driver for Stepper driver L6480 from ST Microelectronics
 * \author pren-et
 * 
 */

#ifndef L6480_H
#define L6480_H

//#include "platform.h"

#if PL_PC
    #include <stdio.h>
#endif /* PL_PC */

#if PL_FRDM
    #include "Stepperspi.h"
    #include "shell.h"
#endif /* PL_FRDM */

#if PL_HAS_SHELL
    #include "CLS1.h"
#endif /* PL_HAS_SHELL */

#include <stdint.h>
#if PL_HAS_SHELL
    #include "CLS1.h"
#endif /* PL_HAS_SHELL */

/******************************************************************************
Test compiler dependencies
******************************************************************************/
/*! \union test_bitfield_t
 *  \brief Type for testing the implementation of bitfields in the current compiler
 */
typedef union {
    struct {
        uint8_t low     : 4;    /*!< Low nibble */
        uint8_t high    : 4;    /*!< High nibble */
    } bitfield;             /*!< Nibbles */
    uint8_t     byte;       /*!< Byte */
} test_bitfield_t;

/******************************************************************************
Registers
******************************************************************************/
/*! \enum l6480_reg_rw_t
 *  \brief Possible access levels for registers.
 */
typedef enum {
    L6480_REG_RW_R,  /*!< readable only */
    L6480_REG_RW_WH, /*!< writable when outputs in high impedance */
    L6480_REG_RW_WS, /*!< writable when motor is stopped */
    L6480_REG_RW_WR  /*!< always writable */
} l6480_reg_rw_t;

/*! \name abs_pos
 * @{
 */
#define L6480_REG_ABS_POS_ADDR      0x01
#define L6480_REG_ABS_POS_LEN       3
#define L6480_REG_ABS_POS_RW        L6480_REG_RW_WS
#define L6480_REG_ABS_POS_DEFAULT   0x000000
#define L6480_REG_ABS_POS_MAX       2097151
#define L6480_REG_ABS_POS_MIN       -2097152
/*! @} */
/*! \union l6480_reg_abs_pos_t
 *  \brief Register ABS_POS
 */
typedef union {
    uint8_t array[L6480_REG_ABS_POS_LEN];   /*!< array access */
    struct {
        uint32_t data   : 22;                   /*!< data */
        uint32_t unused :  2;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_abs_pos_t;

/*! \name el_pos
 * @{
 */
#define L6480_REG_EL_POS_ADDR       0x02
#define L6480_REG_EL_POS_LEN        2
#define L6480_REG_EL_POS_RW         L6480_REG_RW_WS
#define L6480_REG_EL_POS_DEFAULT    0x0000
/*! @} */
/*! \union l6480_reg_el_pos_t
 *  \brief Register EL_POS
 */
typedef union {
    uint8_t array[L6480_REG_EL_POS_LEN];    /*!< array access */
    struct {
        uint16_t data   : 9;                    /*!< raw data */
        uint16_t unused : 7;                    /*!< unused bits */
    } raw;                                  /*!< raw data access */
    struct {
        uint8_t microstep   : 7;                /*!< microsteps */
        uint8_t step        : 2;                /*!< full steps */
        uint8_t unused      : 7;                /*!< unused bits */
    } reg;                                  /*!< register access */
} l6480_reg_el_pos_t;

/*! \name mark
 * @{
 */
#define L6480_REG_MARK_ADDR     0x03
#define L6480_REG_MARK_LEN      3
#define L6480_REG_MARK_RW       L6480_REG_RW_WR
#define L6480_REG_MARK_DEFAULT  0x000000
#define L6480_REG_MARK_MAX       2097151
#define L6480_REG_MARK_MIN       -2097152
/*! @} */
/*! \union l6480_reg_mark_t
 *  \brief Register MARK
 */
typedef union {
    uint8_t array[L6480_REG_MARK_LEN];      /*!< array access */
    struct {
        uint32_t data   : 22;                   /*!< raw data */
        uint32_t unused :  2;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_mark_t;

/*! \name speed
 * @{
 */
#define L6480_REG_SPEED_ADDR    0x04
#define L6480_REG_SPEED_LEN     3
#define L6480_REG_SPEED_RW      L6480_REG_RW_R
#define L6480_REG_SEED_DEFAULT  0x000000
/*! @} */
/*! \union l6480_reg_speed_t
 *  \brief Register SPEED
 */
typedef union {
    uint8_t array[L6480_REG_SPEED_LEN];     /*!< array access */
    struct {
        uint32_t data   : 20;                   /*!< raw data */
        uint32_t unused :  4;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_speed_t;

/*! \name acc
 * @{
 */
#define L6480_REG_ACC_ADDR      0x05
#define L6480_REG_ACC_LEN       2
#define L6480_REG_ACC_RW        L6480_REG_RW_WS
#define L6480_REG_ACC_DEFAULT   0x008A
#define L6480_REG_ACC_MAX       4094
/*! @} */
/*! \union l6480_reg_acc_t
 *  \brief Register ACC
 */
typedef union {
    uint8_t array[L6480_REG_ACC_LEN];       /*!< array access */
    struct {
        uint16_t data   : 12;                   /*!< raw data */
        uint16_t unused :  4;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_acc_t;

/*! \name dec
 * @{
 */
#define L6480_REG_DEC_ADDR      0x06
#define L6480_REG_DEC_LEN       2
#define L6480_REG_DEC_RW        L6480_REG_RW_WS
#define L6480_REG_DEC_DEFAULT   0x008A
#define L6480_REG_DEC_MAX       4094
/*! @} */
/*! \union l6480_reg_dec_t
 *  \brief Register DEC
 */
typedef union {
    uint8_t array[L6480_REG_DEC_LEN];       /*!< array access */
    struct {
        uint16_t data   : 12;                   /*!< raw data */
        uint16_t unused :  4;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_dec_t;

/*! \name max_speed
 * @{
 */
#define L6480_REG_MAX_SPEED_ADDR    0x07
#define L6480_REG_MAX_SPEED_LEN     2
#define L6480_REG_MAX_SPEED_RW      L6480_REG_RW_WR
#define L6480_REG_MAX_SPEED_DEFAULT 0x0041
#define L6480_REG_MAX_SPEED_MAX     1023
/*! @} */
/*! \union l6480_reg_max_speed_t
 *  \brief Register MAX_SPEED
 */
typedef union {
    uint8_t array[L6480_REG_MAX_SPEED_LEN]; /*!< array access */
    struct {
        uint16_t data   : 10;                   /*!< raw data */
        uint16_t unused :  6;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_max_speed_t;

/*! \name min_speed
 * @{
 */
#define L6480_REG_MIN_SPEED_ADDR    0x08
#define L6480_REG_MIN_SPEED_LEN     2
#define L6480_REG_MIN_SPEED_RW      L6480_REG_RW_WS
#define L6480_REG_MIN_SPEED_DEFAULT 0x0000
#define L6480_REG_MIN_SPEED_MAX     4095
/*! @} */
/*! \union l6480_reg_min_speed_t
 *  \brief Register MIN_SPEED
 */
typedef union {
    uint8_t array[L6480_REG_MIN_SPEED_LEN]; /*!< array access */
    struct {
        uint16_t data   : 13;                   /*!< raw data */
        uint16_t unused :  3;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
    struct {
        uint16_t min_speed  : 12;               /*!< minimum speed */
        uint16_t lspd_opt   :  1;               /*!< low speed optimization */
        uint16_t unused     :  3;               /*!< unused bits */
    } reg;                                  /*!< register access */
} l6480_reg_min_speed_t;

/*! \name fs_spd
 * @{
 */
#define L6480_REG_FS_SPD_ADDR       0x15
#define L6480_REG_FS_SPD_LEN        2
#define L6480_REG_FS_SPD_RW         L6480_REG_RW_WR
#define L6480_REG_FS_SPD_DEFAULT    0x0027
#define L6480_REG_FS_SPD_MAX        2047
/*! @} */
/*! \union l6480_reg_fs_spd_t
 *  \brief Register FS_SPD
 */
typedef union {
    uint8_t array[L6480_REG_FS_SPD_LEN];    /*!< array access */
    struct {
        uint16_t data   : 12;                   /*!< raw data */
        uint16_t unused :  4;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
    struct {
        uint16_t fs_spd     : 11;               /*!< full step speed */
        uint16_t boost_mode :  1;               /*!< boost mode */
        uint16_t unused     :  4;               /*!< unused bits */
    } reg;                                  /*!< register access */
} l6480_reg_fs_spd_t;

/*! \name kval_hold
 * @{
 */
#define L6480_REG_KVAL_HOLD_ADDR    0x09
#define L6480_REG_KVAL_HOLD_LEN     1
#define L6480_REG_KVAL_HOLD_RW      L6480_REG_RW_WR
#define L6480_REG_KVAL_HOLD_DEFAULT 0x29
/*! @} */
/*! \union l6480_reg_kval_hold_t
 *  \brief Register KVAL_HOLD
 */
typedef union {
    uint8_t array[L6480_REG_KVAL_HOLD_LEN]; /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_kval_hold_t;

/*! \name kval_run
 * @{
 */
#define L6480_REG_KVAL_RUN_ADDR     0x0A
#define L6480_REG_KVAL_RUN_LEN      1
#define L6480_REG_KVAL_RUN_RW       L6480_REG_RW_WR
#define L6480_REG_KVAL_RUN_DEFAULT  0x29
/*! @} */
/*! \union l6480_reg_kval_run_t
 *  \brief Register KVAL_RUN
 */
typedef union {
    uint8_t array[L6480_REG_KVAL_RUN_LEN];  /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_kval_run_t;

/*! \name kval_acc
 * @{
 */
#define L6480_REG_KVAL_ACC_ADDR     0x0B
#define L6480_REG_KVAL_ACC_LEN      1
#define L6480_REG_KVAL_ACC_RW       L6480_REG_RW_WR
#define L6480_REG_KVAL_ACC_DEFAULT  0x29
/*! @} */
/*! \union l6480_reg_kval_acc_t
 *  \brief Register KVAL_ACC
 */
typedef union {
    uint8_t array[L6480_REG_KVAL_ACC_LEN];  /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_kval_acc_t;

/*! \name kval_dec
 * @{
 */
#define L6480_REG_KVAL_DEC_ADDR     0x0C
#define L6480_REG_KVAL_DEC_LEN      1
#define L6480_REG_KVAL_DEC_RW       L6480_REG_RW_WR
#define L6480_REG_KVAL_DEC_DEFAULT  0x29
/*! @} */
/*! \union l6480_reg_kval_dec_t
 *  \brief Register KVAL_DEC
 */
typedef union {
    uint8_t array[L6480_REG_KVAL_DEC_LEN];  /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_kval_dec_t;

/*! \name int_speed
 * @{
 */
#define L6480_REG_INT_SPEED_ADDR    0x0D
#define L6480_REG_INT_SPEED_LEN     2
#define L6480_REG_INT_SPEED_RW      L6480_REG_RW_WH
#define L6480_REG_INT_SPEED_DEFAULT 0x0408
#define L6480_REG_INT_SPEED_MAX     16383
/*! @} */
/*! \union l6480_reg_int_speed_t
 *  \brief Register INT_SPEED
 */
typedef union {
    uint8_t array[L6480_REG_INT_SPEED_LEN]; /*!< array access */
    struct {
        uint16_t data   : 14;                   /*!< raw data */
        uint16_t unused :  2;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_int_speed_t;

/*! \name st_slp
 * @{
 */
#define L6480_REG_ST_SLP_ADDR       0x0E
#define L6480_REG_ST_SLP_LEN        1
#define L6480_REG_ST_SLP_RW         L6480_REG_RW_WH
#define L6480_REG_ST_SLP_DEFAULT    0x19
/*! @} */
/*! \union l6480_reg_st_slp_t
 *  \brief Register ST_SLP
 */
typedef union {
    uint8_t array[L6480_REG_ST_SLP_LEN];    /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_st_slp_t;

/*! \name fn_slp_acc
 * @{
 */
#define L6480_REG_FN_SLP_ACC_ADDR       0x0F
#define L6480_REG_FN_SLP_ACC_LEN        1
#define L6480_REG_FN_SLP_ACC_RW         L6480_REG_RW_WH
#define L6480_REG_FN_SLP_ACC_DEFAULT    0x29
/*! @} */
/*! \union l6480_reg_fn_slp_acc_t
 *  \brief Register FN_SLP_ACC
 */
typedef union {
    uint8_t array[L6480_REG_FN_SLP_ACC_LEN];/*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_fn_slp_acc_t;

/*! \name fn_slp_dec
 * @{
 */
#define L6480_REG_FN_SLP_DEC_ADDR       0x10
#define L6480_REG_FN_SLP_DEC_LEN        1
#define L6480_REG_FN_SLP_DEC_RW         L6480_REG_RW_WH
#define L6480_REG_FN_SLP_DEC_DEFAULT    0x29
/*! @} */
/*! \union l6480_reg_fn_slp_dec_t
 *  \brief Register FN_SLP_DEC
 */
typedef union {
    uint8_t array[L6480_REG_FN_SLP_DEC_LEN];/*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
} l6480_reg_fn_slp_dec_t;

/*! \name k_therm
 * @{
 */
#define L6480_REG_K_THERM_ADDR      0x11
#define L6480_REG_K_THERM_LEN       1
#define L6480_REG_K_THERM_RW        L6480_REG_RW_WR
#define L6480_REG_K_THERM_DEFAULT   0x00
#define L6480_REG_K_THERM_MAX       0x0F
#define L6480_REG_K_THERM_MIN_MILLI 1000
#define L6480_REG_K_THERM_MAX_MILLI 1469
/*! @} */
/*! \union l6480_reg_k_therm_t
 *  \brief Register K_THERM
 */
typedef union {
    uint8_t array[L6480_REG_K_THERM_LEN];   /*!< array access */
    struct {
        uint8_t data   :  4;                    /*!< raw data */
        uint8_t unused :  4;                    /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_k_therm_t;

/*! \name adc_out
 * @{
 */
#define L6480_REG_ADC_OUT_ADDR      0x12
#define L6480_REG_ADC_OUT_LEN       1
#define L6480_REG_ADC_OUT_RW        L6480_REG_RW_R
#define L6480_REG_ADC_OUT_DEFAULT   0x00
/*! @} */
/*! \union l6480_reg_adc_out_t
 *  \brief Register ADC_OUT
 */
typedef union {
    uint8_t array[L6480_REG_ADC_OUT_LEN];   /*!< array access */
    struct {
        uint8_t data   :  5;                    /*!< raw data */
        uint8_t unused :  3;                    /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_adc_out_t;

/*! \name ocd_th
 * @{
 */
#define L6480_REG_OCD_TH_ADDR       0x13
#define L6480_REG_OCD_TH_LEN        1
#define L6480_REG_OCD_TH_RW         L6480_REG_RW_WR
#define L6480_REG_OCD_TH_DEFAULT    0x08
#define L6480_REG_OCD_TH_MAX        0x1F
#define L6480_REG_OCD_TH_MIN_MILLI  32
#define L6480_REG_OCD_TH_MAX_MILLI  1000
/*! @} */
/*! \union l6480_reg_ocd_th_t
 *  \brief Register OCD_TH
 */
typedef union {
    uint8_t array[L6480_REG_OCD_TH_LEN];    /*!< array access */
    struct {
        uint8_t data   :  5;                    /*!< raw data */
        uint8_t unused :  3;                    /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_ocd_th_t;

/*! \name stall_th
 * @{
 */
#define L6480_REG_STALL_TH_ADDR         0x14
#define L6480_REG_STALL_TH_LEN          1
#define L6480_REG_STALL_TH_RW           L6480_REG_RW_WR
#define L6480_REG_STALL_TH_DEFAULT      0x10
#define L6480_REG_STALL_TH_MAX          0x1F
#define L6480_REG_STALL_TH_MIN_MILLI    32
#define L6480_REG_STALL_TH_MAX_MILLI    1000
/*! @} */
/*! \union l6480_reg_stall_th_t
 *  \brief Register STALL_TH
 */
typedef union {
    uint8_t array[L6480_REG_STALL_TH_LEN];  /*!< array access */
    struct {
        uint16_t data   :  5;                   /*!< raw data */
        uint16_t unused :  3;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
} l6480_reg_stall_th_t;

/*! \name step_mode
 * @{
 */
#define L6480_REG_STEP_MODE_ADDR    0x16
#define L6480_REG_STEP_MODE_LEN     1
#define L6480_REG_STEP_MODE_RW      L6480_REG_RW_WH
#define L6480_REG_STEP_MODE_DEFAULT 0x07
/*! @} */
/*! \union l6480_reg_step_mode_t
 *  \brief Register STEP_MODE
 */
typedef union {
    uint8_t array[L6480_REG_STEP_MODE_LEN]; /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
    struct {
        uint8_t step_sel    :  3;               /*!< select step mode */
        uint8_t null        :  1;               /*!< unused bit, must always be zero! */
        uint8_t sync_sel    :  3;               /*!< select synchronization signal source */
        uint8_t sync_en     :  1;               /*!< enable synchronization */
    } reg;                                  /*!< register access */
} l6480_reg_step_mode_t;
/*! \enum l6480_step_sel_t
 * \brief enum for Step mode selection
 */
typedef enum {
    L6480_STEP_SEL_FULL         = 0x00, /*!< Step mode Full-step */
    L6480_STEP_SEL_HALF         = 0x01, /*!< Step mode Half-step */
    L6480_STEP_SEL_MICRO_4      = 0x02, /*!< Step mode 1/4   Microstep */
    L6480_STEP_SEL_MICRO_8      = 0x03, /*!< Step mode 1/8   Microstep */
    L6480_STEP_SEL_MICRO_16     = 0x04, /*!< Step mode 1/16  Microstep */
    L6480_STEP_SEL_MICRO_32     = 0x05, /*!< Step mode 1/32  Microstep */
    L6480_STEP_SEL_MICRO_64     = 0x06, /*!< Step mode 1/64  Microstep */
    L6480_STEP_SEL_MICRO_128    = 0x07, /*!< Step mode 1/128 Microstep */
} l6480_step_sel_t;
/*! \enum l6480_sync_sig_src_t
 * \brief enum for SYNC signal source
 */
typedef enum {
    L6480_SYNC_SEL_EL_POS_7     = 0X00, /*!< Electrical position [7] */
    L6480_SYNC_SEL_EL_POS_6     = 0X01, /*!< Electrical position [6] */
    L6480_SYNC_SEL_EL_POS_5     = 0X02, /*!< Electrical position [5] */
    L6480_SYNC_SEL_EL_POS_4     = 0X03, /*!< Electrical position [4] */
    L6480_SYNC_SEL_EL_POS_3     = 0X04, /*!< Electrical position [3] */
    L6480_SYNC_SEL_EL_POS_2     = 0X05, /*!< Electrical position [2] */
    L6480_SYNC_SEL_EL_POS_1     = 0X06, /*!< Electrical position [1] */
    L6480_SYNC_SEL_EL_POS_0     = 0X07, /*!< Electrical position [0] */
} l6480_sync_sig_src_t;

/*! \name alarm_en
 * @{
 */
#define L6480_REG_ALARM_EN_ADDR     0x17
#define L6480_REG_ALARM_EN_LEN      1
#define L6480_REG_ALARM_EN_RW       L6480_REG_RW_WS
#define L6480_REG_ALARM_EN_DEFAULT  0xFF
/*! @} */
/*! \union l6480_reg_alarm_en_t
 *  \brief Register ALARM_EN
 */
typedef union {
    uint8_t array[L6480_REG_ALARM_EN_LEN];  /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
    struct {
        uint8_t overcurrent     :  1;           /*!< enable Overcurrent alarm */
        uint8_t th_shutdown     :  1;           /*!< enable Thermal shutdown alarm */
        uint8_t th_warning      :  1;           /*!< enable Thermal warning alarm */
        uint8_t uvlo            :  1;           /*!< enable UVLO alarm */
        uint8_t adc_uvlo        :  1;           /*!< enable ADC UVLO alarm */
        uint8_t stall_det       :  1;           /*!< enable Stall detection alarm */
        uint8_t switch_on       :  1;           /*!< enable Switch turn-on event alarm */
        uint8_t cmd_error       :  1;           /*!< enable Command error alarm */
    } reg;                                  /*!< register access */
} l6480_reg_alarm_en_t;
/*! \enum l6480_alarm_en_t
 *  \brief enum for ALARM_EN bits
 */
typedef enum {
    L6480_ALARM_EN_OVERCURRENT      = 0x00, /*!< Overcurrent alarm enabled */
    L6480_ALARM_EN_TH_SHUTDOWN      = 0x01, /*!< Thermal shutdown alarm enabled */
    L6480_ALARM_EN_TH_WARNING       = 0x02, /*!< Thermal warning alarm enabled */
    L6480_ALARM_EN_UVLO             = 0x03, /*!< UVLO alarm enabled */
    L6480_ALARM_EN_ADC_UVLO         = 0x04, /*!< ADC UVLO alarm enabled */
    L6480_ALARM_EN_STALL_DET        = 0x05, /*!< Stall detection alarm enabled */
    L6480_ALARM_EN_SWITCH_ON        = 0x06, /*!< Switch turn-on event alarm enabled */
    L6480_ALARM_EN_CMD_ERROR        = 0x07, /*!< Command error alarm enabled */
} l6480_alarm_en_t;
#define L6480_ALARM_EN_OVERCURRENT_MASK      = (1 << L6480_ALARM_EN_OVERCURRENT   ) /*!< Overcurrent alarm enabled */
#define L6480_ALARM_EN_TH_SHUTDOWN_MASK      = (1 << L6480_ALARM_EN_TH_SHUTDOWN   ) /*!< Thermal shutdown alarm enabled */
#define L6480_ALARM_EN_TH_WARNING_MASK       = (1 << L6480_ALARM_EN_TH_WARNING    ) /*!< Thermal warning alarm enabled */
#define L6480_ALARM_EN_UVLO_MASK             = (1 << L6480_ALARM_EN_UVLO          ) /*!< UVLO alarm enabled */
#define L6480_ALARM_EN_ADC_UVLO_MASK         = (1 << L6480_ALARM_EN_ADC_UVLO      ) /*!< ADC UVLO alarm enabled */
#define L6480_ALARM_EN_STALL_DET_MASK        = (1 << L6480_ALARM_EN_STALL_DET     ) /*!< Stall detection alarm enabled */
#define L6480_ALARM_EN_SWITCH_ON_MASK        = (1 << L6480_ALARM_EN_SWITCH_ON     ) /*!< Switch turn-on event alarm enabled */
#define L6480_ALARM_EN_CMD_ERROR_MASK        = (1 << L6480_ALARM_EN_CMD_ERROR     ) /*!< Command error alarm enabled */

/*! \name gatecfg1
 * @{
 */
#define L6480_REG_GATECFG1_ADDR         0x18
#define L6480_REG_GATECFG1_LEN          2
#define L6480_REG_GATECFG1_RW           L6480_REG_RW_WH
#define L6480_REG_GATECFG1_DEFAULT      0x0000
#define L6480_REG_GATECFG1_TCC_NANO_MAX 3750
/*! @} */
/*! \union l6480_reg_gatecfg1_t
 *  \brief Register GATECFG1
 */
typedef union {
    uint8_t array[L6480_REG_GATECFG1_LEN];  /*!< array access */
    struct {
        uint16_t data   : 12;                   /*!< raw data */
        uint16_t unused :  4;                   /*!< unused bits */
    } raw;                                  /*!< raw data access */
    struct {
        uint16_t tcc    :  5;                   /*!< duration of constant current phase */
        uint16_t igate  :  5;                   /*!< gate current */
        uint16_t tboost :  5;                   /*!< duration of overboost phase */
        uint16_t wd_en  :  5;                   /*!< enable clock source monitoring */
        uint16_t unused :  4;                   /*!< unused bits */
    } reg;                                  /*!< register access */
} l6480_reg_gatecfg1_t;
/*! \enum l6480_gatecfg1_igate_t
 *  \brief enum for IGATE
 */
typedef enum {
    L6480_GATECFG1_IGATE_4_A    = 0x00, /*!< Gate current  4 mA */
    L6480_GATECFG1_IGATE_4_B    = 0x01, /*!< Gate current  4 mA */
    L6480_GATECFG1_IGATE_8      = 0x02, /*!< Gate current  8 mA */
    L6480_GATECFG1_IGATE_16     = 0x03, /*!< Gate current 16 mA */
    L6480_GATECFG1_IGATE_24     = 0x04, /*!< Gate current 24 mA */
    L6480_GATECFG1_IGATE_32     = 0x05, /*!< Gate current 32 mA */
    L6480_GATECFG1_IGATE_64     = 0x06, /*!< Gate current 64 mA */
    L6480_GATECFG1_IGATE_96     = 0x07, /*!< Gate current 96 mA */
} l6480_gatecfg1_igate_t;
/*! \enum l6480_gatecfg1_tboost_t
 *  \brief enum for TBOOST
 */
typedef enum {
    L6480_GATECFG1_TBOOST_0     = 0x00, /*!< Turn-off boost time    0 ns */
    L6480_GATECFG1_TBOOST_62    = 0x01, /*!< Turn-off boost time   62 ns */
    L6480_GATECFG1_TBOOST_125   = 0x02, /*!< Turn-off boost time  125 ns */
    L6480_GATECFG1_TBOOST_250   = 0x03, /*!< Turn-off boost time  250 ns */
    L6480_GATECFG1_TBOOST_375   = 0x04, /*!< Turn-off boost time  375 ns */
    L6480_GATECFG1_TBOOST_500   = 0x05, /*!< Turn-off boost time  500 ns */
    L6480_GATECFG1_TBOOST_750   = 0x06, /*!< Turn-off boost time  750 ns */
    L6480_GATECFG1_TBOOST_1000  = 0x07, /*!< Turn-off boost time 1000 ns */
} l6480_gatecfg1_tboost_t;

/*! \name gatecfg2
 * @{
 */
#define L6480_REG_GATECFG2_ADDR             0x19
#define L6480_REG_GATECFG2_LEN              1
#define L6480_REG_GATECFG2_RW               L6480_REG_RW_WH
#define L6480_REG_GATECFG2_DEFAULT          0x00
#define L6480_REG_GATECFG2_TDT_NANO_MIN     125
#define L6480_REG_GATECFG2_TDT_NANO_MAX     4000
#define L6480_REG_GATECFG2_TBLANK_NANO_MIN  125
#define L6480_REG_GATECFG2_TBLANK_NANO_MAX  1000
/*! @} */
/*! \union l6480_reg_gatecfg2_t
 *  \brief Register GATECFG2
 */
typedef union {
    uint8_t array[L6480_REG_GATECFG2_LEN];  /*!< array access */
    struct {
        uint8_t data;                           /*!< raw data */
    } raw;                                  /*!< raw data access */
    struct {
        uint8_t tdt     :  5;                   /*!< deadtime between gate turn-off and gate turn-on */
        uint8_t tblank  :  3;                   /*!< blanking of current sensing comparators */
    } reg;                                  /*!< register access */
} l6480_reg_gatecfg2_t;

/*! \name config
 * @{
 */
#define L6480_REG_CONFIG_ADDR       0x1A
#define L6480_REG_CONFIG_LEN        2
#define L6480_REG_CONFIG_RW         L6480_REG_RW_WH
#define L6480_REG_CONFIG_DEFAULT    0x2C88
/*! @} */
/*! \union l6480_reg_config_t
 *  \brief Register CONFIG
 */
typedef union {
    uint8_t array[L6480_REG_CONFIG_LEN];    /*!< array access */
    struct {
        uint16_t data;                          /*!< raw data */
    } raw;                                  /*!< raw data access */
    struct {
        uint16_t osc_sel    :  3;               /*!< select system clock source */
        uint16_t ext_clk    :  1;               /*!< enable external clock */
        uint16_t sw_mode    :  1;               /*!< behaviour of external switch */
        uint16_t en_vscomp  :  1;               /*!< motor supply voltage compensation */
        uint16_t reserved   :  1;               /*!< bit not used */
        uint16_t oc_sd      :  1;               /*!< behaviour in overcurrent event */
        uint16_t uvloval    :  1;               /*!< undervoltage lockout protection threshold */
        uint16_t vccval     :  1;               /*!< output voltage of internal Vcc regulator */
        uint16_t f_pwm_dec  :  3;               /*!< pwm frequency division factor */
        uint16_t f_pwm_int  :  3;               /*!< pwm frequency multiplication factor */
    } reg;                                  /*!< register access */
} l6480_reg_config_t;
/*! \enum l6480_osc_sel_t
 *  \brief enum for system clock source selection
 */
typedef enum {
    L6480_CONFIG_OSC_SEL_INT_16_NO_OUT_A    = 0x00, /*!< Internal oscillator 16 MHz, no output */
    L6480_CONFIG_OSC_SEL_INT_16_NO_OUT_B    = 0x01, /*!< Internal oscillator 16 MHz, no output */
    L6480_CONFIG_OSC_SEL_INT_16_NO_OUT_C    = 0x02, /*!< Internal oscillator 16 MHz, no output */
    L6480_CONFIG_OSC_SEL_INT_16_NO_OUT_D    = 0x03, /*!< Internal oscillator 16 MHz, no output */
    L6480_CONFIG_OSC_SEL_INT_16_OUT_2M      = 0x08, /*!< Internal oscillator 16 MHz, output frequency  2 MHz */
    L6480_CONFIG_OSC_SEL_INT_16_OUT_4M      = 0x09, /*!< Internal oscillator 16 MHz, output frequency  4 MHz */
    L6480_CONFIG_OSC_SEL_INT_16_OUT_8M      = 0x0A, /*!< Internal oscillator 16 MHz, output frequency  8 MHz */
    L6480_CONFIG_OSC_SEL_INT_16_OUT_16M     = 0x0B, /*!< Internal oscillator 16 MHz, output frequency 16 MHz */
    L6480_CONFIG_OSC_SEL_EXT_XTAL_8         = 0x04, /*!< External crystal or resonator  8 MHz */
    L6480_CONFIG_OSC_SEL_EXT_XTAL_16        = 0x05, /*!< External crystal or resonator 16 MHz */
    L6480_CONFIG_OSC_SEL_EXT_XTAL_24        = 0x06, /*!< External crystal or resonator 24 MHz */
    L6480_CONFIG_OSC_SEL_EXT_XTAL_32        = 0x07, /*!< External crystal or resonator 32 MHz */
    L6480_CONFIG_OSC_SEL_EXT_CLK_8          = 0x0C, /*!< External clock source  8 MHz */
    L6480_CONFIG_OSC_SEL_EXT_CLK_16         = 0x0D, /*!< External clock source 16 MHz */
    L6480_CONFIG_OSC_SEL_EXT_CLK_24         = 0x0E, /*!< External clock source 24 MHz */
    L6480_CONFIG_OSC_SEL_EXT_CLK_32         = 0x0F, /*!< External clock source 32 MHz */
} l6480_osc_sel_t;
/*! \enum l6480_sw_mode_t
 *  \brief enum for external switch interrupt mode
 */
typedef enum {
    L6480_CONFIG_SW_MODE_HARD  = 0x00, /*!< Hard stop interrupt */ 
    L6480_CONFIG_SW_MODE_SOFT  = 0x01, /*!< User disposal */
} l6480_sw_mode_t;
/*! \enum l6480_oc_sd_t
 *  \brief enum for overcurrent event
 */
typedef enum {
    L6480_CONFIG_OC_SD_SHUTDOWN     = 0x01, /*!< Bridges shut down */
    L6480_CONFIG_OC_SD_NO_SHUTDOWN  = 0x00, /*!< Bridges do not shut down */
} l6480_oc_sd_t;
/*! \enum l6480_vccval_t
 *  \brief enum for Programmable Vcc regulator output voltage
 */
typedef enum {
    L6480_CONFIG_VCCVAL_75  = 0x00, /*!< Vcc voltage 7.5 V */
    L6480_CONFIG_VCCVAL_15  = 0x01, /*!< Vcc voltage 15 V */
} l6480_vccval_t;
/*! \enum l6480_uvloval_t
 *  \brief enum for UVLO protection thresholds
 */
typedef enum {
    L6480_CONFIG_UVLOVAL_LOW    = 0x00, /*!< Vcc_th_on =  6.9 V Vcc_th_off = 6.3 V Vboot_th_on =   6 V Vboot_th_off = 5.5 V */
    L6480_CONFIG_UVLOVAL_HIGH   = 0x01, /*!< Vcc_th_on = 10.4 V Vcc_th_off =  10 V Vboot_th_on = 9.2 V Vboot_th_off = 8.8 V */
} l6480_uvloval_t;
/*! \enum l6480_en_vscomp_t
 *  \brief enum Motor supply voltage compensation
 */ 
typedef enum {
    L6480_CONFIG_EN_VSCOMP_DIS  = 0x00, /*!< Motor supply voltage compensation disabled */
    L6480_CONFIG_EN_VSCOMP_EN   = 0x01, /*!< Motor supply voltage compensation enabled */
} l6480_en_vscomp_t;
/*! \enum l6480_f_pwm_int_t
 *  \brief enum for pwm frequency division factor
 */
typedef enum {
    L6480_CONFIG_F_PWM_INT_1    = 0x00, /*!< Integer division factor 1 */
    L6480_CONFIG_F_PWM_INT_2    = 0x01, /*!< Integer division factor 2 */
    L6480_CONFIG_F_PWM_INT_3    = 0x02, /*!< Integer division factor 3 */
    L6480_CONFIG_F_PWM_INT_4    = 0x03, /*!< Integer division factor 4 */
    L6480_CONFIG_F_PWM_INT_5    = 0x04, /*!< Integer division factor 5 */
    L6480_CONFIG_F_PWM_INT_6    = 0x05, /*!< Integer division factor 6 */
    L6480_CONFIG_F_PWM_INT_7_A  = 0x06, /*!< Integer division factor 7 */
    L6480_CONFIG_F_PWM_INT_7_B  = 0x07, /*!< Integer division factor 7 */
} l6480_f_pwm_int_t;
/*! \enum l6480_f_pwm_dec_t
 *  \brief enum for pwm frequency multiplication factor
 */
typedef enum {
    L6480_CONFIG_F_PWM_DEC_0_625    = 0x00, /*!< Multiplication factor 0.625 */
    L6480_CONFIG_F_PWM_DEC_0_75     = 0x01, /*!< Multiplication factor 0.75  */
    L6480_CONFIG_F_PWM_DEC_0_875    = 0x02, /*!< Multiplication factor 0.875 */
    L6480_CONFIG_F_PWM_DEC_1        = 0x03, /*!< Multiplication factor 1     */
    L6480_CONFIG_F_PWM_DEC_1_25     = 0x04, /*!< Multiplication factor 1.25  */
    L6480_CONFIG_F_PWM_DEC_1_5      = 0x05, /*!< Multiplication factor 1.5   */
    L6480_CONFIG_F_PWM_DEC_1_75     = 0x06, /*!< Multiplication factor 1.75  */
    L6480_CONFIG_F_PWM_DEC_2        = 0x07, /*!< Multiplication factor 2     */
} l6480_f_pwm_dec_t;

/*! \name config
 * @{
 */
#define L6480_REG_STATUS_ADDR       0x1B
#define L6480_REG_STATUS_LEN        2
#define L6480_REG_STATUS_RW         L6480_REG_RW_R
#define L6480_REG_STATUS_DEFAULT    0x0000
/*! @} */
/*! \union l6480_reg_status_t
 *  \brief Register STATUS
 */
typedef union {
    uint8_t array[L6480_REG_STATUS_LEN];    /*!< array access */
    struct {
        uint16_t data;                          /*!< raw data */
    } raw;                                  /*!< raw data access */
    struct {
        uint16_t hiz            :  1;           /*!< bridges in high impedance state */
        uint16_t busy           :  1;           /*!< command executed */
        uint16_t sw_f           :  1;           /*!< switch input status */
        uint16_t sw_evn         :  1;           /*!< switch turn-on event */
        uint16_t dir            :  1;           /*!< direction */
        uint16_t mot_status     :  2;           /*!< current motor status */
        uint16_t cmd_error      :  1;           /*!< spi command error */
        uint16_t stck_mod       :  1;           /*!< device in step-clock mode */
        uint16_t uvlo           :  1;           /*!< undervoltage lockout */
        uint16_t uvlo_adc       :  1;           /*!< adc undervoltage lockout */
        uint16_t th_status      :  2;           /*!< thermal status */
        uint16_t ocd            :  1;           /*!< overcurrent detection */
        uint16_t step_loss_a    :  1;           /*!< stall on bridge A */
        uint16_t step_loss_b    :  1;           /*!< stall on bridge B */
    } reg;                                  /*!< register access */
} l6480_reg_status_t;
/*! \enum l6480_th_status_t
 *  \brief enum for device thermal status interpretation
 */
typedef enum {
    L6480_STATUS_TH_STATUS_NORMAL       = 0x00, /*!< Normal */
    L6480_STATUS_TH_STATUS_WARNING      = 0x01, /*!< Warning */
    L6480_STATUS_TH_STATUS_BRI_SHTDWN   = 0x02, /*!< Bridge shutdown */
    L6480_STATUS_TH_STATUS_DEV_SHTDWN   = 0x03, /*!< Device shutdown */
} l6480_th_status_t;
/*! \enum l6480_dir_status_t
 *  \brief enum for direction status
 */
typedef enum {
    L6480_STATUS_DIR_FWD    = 0x00, /*!< Forward */
    L6480_STATUS_DIR_REV    = 0x01, /*!< Reverse */
} l6480_dir_status_t;
/*! \enum l6480_mot_status_t
 *  \brief enum for motor status interpretation
 */
typedef enum {
    L6480_STATUS_MOT_STATUS_STOP    = 0x00, /*!< Stopped */
    L6480_STATUS_MOT_STATUS_ACC     = 0x01, /*!< Acceleration */
    L6480_STATUS_MOT_STATUS_DEC     = 0x02, /*!< Deceleration */
    L6480_STATUS_MOT_STATUS_CONST   = 0x03, /*!< Constant speed */
} l6480_mot_status_t;

/*! \struct l6480_reg_t
 *  \brief L6480 Registers
 */
typedef struct {
    l6480_reg_abs_pos_t     abs_pos;        /*!< Register ABS_POS     */
    l6480_reg_el_pos_t      el_pos;         /*!< Register EL_POS      */
    l6480_reg_mark_t        mark;           /*!< Register MARK        */
    l6480_reg_speed_t       speed;          /*!< Register SPEED       */
    l6480_reg_acc_t         acc;            /*!< Register ACC         */
    l6480_reg_dec_t         dec;            /*!< Register DEC         */
    l6480_reg_max_speed_t   max_speed;      /*!< Register MAX_SPEED   */
    l6480_reg_min_speed_t   min_speed;      /*!< Register MIN_SPEED   */
    l6480_reg_fs_spd_t      fs_spd;         /*!< Register FS_SPD      */
    l6480_reg_kval_hold_t   kval_hold;      /*!< Register KVAL_HOLD   */
    l6480_reg_kval_run_t    kval_run;       /*!< Register KVAL_RUN    */
    l6480_reg_kval_acc_t    kval_acc;       /*!< Register KVAL_ACC    */
    l6480_reg_kval_dec_t    kval_dec;       /*!< Register KVAL_DEC    */
    l6480_reg_int_speed_t   int_speed;      /*!< Register INT_SPEED   */
    l6480_reg_st_slp_t      st_slp;         /*!< Register ST_SLP      */
    l6480_reg_fn_slp_acc_t  fn_slp_acc;     /*!< Register FN_SLP_ACC  */
    l6480_reg_fn_slp_dec_t  fn_slp_dec;     /*!< Register FN_SLP_DEC  */
    l6480_reg_k_therm_t     k_therm;        /*!< Register K_THERM     */
    l6480_reg_adc_out_t     adc_out;        /*!< Register ADC_OUT     */
    l6480_reg_ocd_th_t      ocd_th;         /*!< Register OCD_TH      */
    l6480_reg_stall_th_t    stall_th;       /*!< Register STALL_TH    */
    l6480_reg_step_mode_t   step_mode;      /*!< Register STEP_MODE   */
    l6480_reg_alarm_en_t    alarm_en;       /*!< Register ALARM_EN    */
    l6480_reg_gatecfg1_t    gatecfg1;       /*!< Register GATECFG1    */
    l6480_reg_gatecfg2_t    gatecfg2;       /*!< Register GATECFG2    */
    l6480_reg_config_t      config;         /*!< Register CONFIG      */
    l6480_reg_status_t      status;         /*!< Register STATUS      */
} l6480_reg_t;

/*! \enum l6480_reg_addr_t
 *  \brief Address for L6480 Registers
 */
typedef enum {
    L6480_REG_ABS_POS_ADDR_ENUM     = L6480_REG_ABS_POS_ADDR,   /*!< Address for ABS_POS */
    L6480_REG_EL_POS_ADDR_ENUM      = L6480_REG_EL_POS_ADDR,    /*!< Address for EL_POS */
    L6480_REG_MARK_ADDR_ENUM        = L6480_REG_MARK_ADDR,      /*!< Address for MARK */
    L6480_REG_SPEED_ADDR_ENUM       = L6480_REG_SPEED_ADDR,     /*!< Address for SPEED */
    L6480_REG_ACC_ADDR_ENUM         = L6480_REG_ACC_ADDR,       /*!< Address for ACC */
    L6480_REG_DEC_ADDR_ENUM         = L6480_REG_DEC_ADDR,       /*!< Address for DEC */
    L6480_REG_MAX_SPEED_ADDR_ENUM   = L6480_REG_MAX_SPEED_ADDR, /*!< Address for MAX_SPEED */
    L6480_REG_MIN_SPEED_ADDR_ENUM   = L6480_REG_MIN_SPEED_ADDR, /*!< Address for MIN_SPEED */
    L6480_REG_FS_SPD_ADDR_ENUM      = L6480_REG_FS_SPD_ADDR,    /*!< Address for FS_SPD */
    L6480_REG_KVAL_HOLD_ADDR_ENUM   = L6480_REG_KVAL_HOLD_ADDR, /*!< Address for KVAL_HOLD */
    L6480_REG_KVAL_RUN_ADDR_ENUM    = L6480_REG_KVAL_RUN_ADDR,  /*!< Address for KVAL_RUN */
    L6480_REG_KVAL_ACC_ADDR_ENUM    = L6480_REG_KVAL_ACC_ADDR,  /*!< Address for KVAL_ACC */
    L6480_REG_KVAL_DEC_ADDR_ENUM    = L6480_REG_KVAL_DEC_ADDR,  /*!< Address for KVAL_DEC */
    L6480_REG_INT_SPEED_ADDR_ENUM   = L6480_REG_INT_SPEED_ADDR, /*!< Address for INT_SPEED */
    L6480_REG_ST_SLP_ADDR_ENUM      = L6480_REG_ST_SLP_ADDR,    /*!< Address for ST_SLP */
    L6480_REG_FN_SLP_ACC_ADDR_ENUM  = L6480_REG_FN_SLP_ACC_ADDR,/*!< Address for FN_SLP_ACC */
    L6480_REG_FN_SLP_DEC_ADDR_ENUM  = L6480_REG_FN_SLP_DEC_ADDR,/*!< Address for FN_SLP_DEC */
    L6480_REG_K_THERM_ADDR_ENUM     = L6480_REG_K_THERM_ADDR,   /*!< Address for K_THERM */
    L6480_REG_ADC_OUT_ADDR_ENUM     = L6480_REG_ADC_OUT_ADDR,   /*!< Address for ADC_OUT */
    L6480_REG_OCD_TH_ADDR_ENUM      = L6480_REG_OCD_TH_ADDR,    /*!< Address for OCD_TH */
    L6480_REG_STALL_TH_ADDR_ENUM    = L6480_REG_STALL_TH_ADDR,  /*!< Address for STALL_TH */
    L6480_REG_STEP_MODE_ADDR_ENUM   = L6480_REG_STEP_MODE_ADDR, /*!< Address for STEP_MODE */
    L6480_REG_ALARM_EN_ADDR_ENUM    = L6480_REG_ALARM_EN_ADDR,  /*!< Address for ALARM_EN */
    L6480_REG_GATECFG1_ADDR_ENUM    = L6480_REG_GATECFG1_ADDR,  /*!< Address for GATECFG1 */
    L6480_REG_GATECFG2_ADDR_ENUM    = L6480_REG_GATECFG2_ADDR,  /*!< Address for GATECFG2 */
    L6480_REG_CONFIG_ADDR_ENUM      = L6480_REG_CONFIG_ADDR,    /*!< Address for CONFIG */
    L6480_REG_STATUS_ADDR_ENUM      = L6480_REG_STATUS_ADDR,    /*!< Address for STATUS */
} l6480_reg_addr_t;

/*******************************************************************************
Commands
*******************************************************************************/

/*! \name Command NOP
 *  \brief Command NOP
 */
#define L6480_CMD_NOP                       0x00
#define L6480_CMD_NOP_LEN                   1
#define L6480_CMD_NOP_READ                  0
/*! @} */

/*! \name Command SetParam(PARAM, VALUE)
 *  \brief Command SetParam(PARAM, VALUE)
 */
#define L6480_CMD_SETPARAM(reg)             (0x00 | L6480_REG_##reg##_ADDR)
#define L6480_CMD_SETPARAM_LEN(reg)         (1 + L6480_REG_##reg##_LEN)
#define L6480_CMD_SETPARAM_READ(reg)        0
/*! @} */

/*! \name Command
 *  \brief Command GetParam(PARAM, VALUE)
 */
#define L6480_CMD_GETPARAM(reg)             (0x20 | L6480_REG_##reg##_ADDR)
#define L6480_CMD_GETPARAM_LEN(reg)         (1 + L6480_REG_##reg##_LEN)
#define L6480_CMD_GETPARAM_READ(reg)        1
/*! @} */

/*! \name Command
 *  \brief Command Run(DIR, SPD)
 */
#define L6480_CMD_RUN(dir)                  (0x50 | (dir?0x01:0x00))
#define L6480_CMD_RUN_LEN(dir)              4
#define L6480_CMD_RUN_READ(dir)             0
#define L6480_CMD_RUN_SPEED_MAX             1048575
#define L6480_CMD_RUN_SPEED_MILLI_MAX       15624985
/*! @} */

/*! \name Command
 *  \brief Command StepClock(DIR)
 */
#define L6480_CMD_STEPCLOCK(dir)            (0x58 | (dir?0x01:0x00))
#define L6480_CMD_STEPCLOCK_LEN(dir)        1
#define L6480_CMD_STEPCLOCK_READ(dir)       0
/*! @} */

/*! \name Command
 *  \brief Command Move(DIR, N_STEP)
 */
#define L6480_CMD_MOVE(dir)                 (0x40 | (dir?0x01:0x00))
#define L6480_CMD_MOVE_LEN(dir)             4
#define L6480_CMD_MOVE_READ(dir)            0
#define L6480_CMD_MOVE_STEP_MAX             4194303
/*! @} */

/*! \name Command
 *  \brief Command GoTo(ABS_POS)
 */
#define L6480_CMD_GOTO                      0x60
#define L6480_CMD_GOTO_LEN                  4
#define L6480_CMD_GOTO_READ                 0
#define L6480_CMD_GOTO_STEP_MIN             -2097152
#define L6480_CMD_GOTO_STEP_MAX             2097151
/*! @} */

/*! \name Command
 *  \brief Command GoTo_DIR(DIR, ABS_POS)
 */
#define L6480_CMD_GOTO_DIR(dir)             (0x68 | (dir?0x01:0x00))
#define L6480_CMD_GOTO_DIR_LEN(dir)         4
#define L6480_CMD_GOTO_DIR_READ(dir)        0
/*! @} */

/*! \name Command
 *  \brief Command GoUntil(ACT, DIR, SPD)
 */
#define L6480_CMD_GOUNTIL(act, dir)         (0x82 | (act?0x08:0x00) | (dir?0x01:0x00))
#define L6480_CMD_GOUNTIL_LEN(act, dir)     4
#define L6480_CMD_GOUNTIL_READ(act, dir)    0
#define L6480_CMD_GOUNTIL_SPEED_MAX         1048575
#define L6480_CMD_GOUNTIL_SPEED_MILLI_MAX   15624985
/*! @} */

/*! \name Command
 *  \brief Command ReleaseSW(act, dir)
 */
#define L6480_CMD_RELEASESW(act, dir)       (0x92 | (act?0x08:0x00) | (dir?0x01:0x00))
#define L6480_CMD_RELEASESW_LEN(act, dir)   1
#define L6480_CMD_RELEASESW_READ(act, dir)  0
/*! @} */

/*! \name Command GoHome
 *  \brief Command GoHome
 */
#define L6480_CMD_GOHOME                    0x70
#define L6480_CMD_GOHOME_LEN                1
#define L6480_CMD_GOHOME_READ               0
/*! @} */

/*! \name Command GoMark
 *  \brief Command GoMark
 */
#define L6480_CMD_GOMARK                    0x74
#define L6480_CMD_GOMARK_LEN                1
#define L6480_CMD_GOMARK_READ               0
/*! @} */

/*! \name Command ResetPos
 *  \brief Command ResetPos
 */
#define L6480_CMD_RESETPOS                  0xD8
#define L6480_CMD_RESETPOS_LEN              1
#define L6480_CMD_RESETPOS_READ             0
/*! @} */

/*! \name Command ResetDevice
 *  \brief Command ResetDevice
 */
#define L6480_CMD_RESETDEVICE               0xC0
#define L6480_CMD_RESETDEVICE_LEN           1
#define L6480_CMD_RESETDEVICE_READ          0
/*! @} */

/*! \name Command SoftStop
 *  \brief Command SoftStop
 */
#define L6480_CMD_SOFTSTOP                  0xB0
#define L6480_CMD_SOFTSTOP_LEN              1
#define L6480_CMD_SOFTSTOP_READ             0
/*! @} */

/*! \name Command HardStop
 *  \brief Command HardStop
 */
#define L6480_CMD_HARDSTOP                  0xB8
#define L6480_CMD_HARDSTOP_LEN              1
#define L6480_CMD_HARDSTOP_READ             0
/*! @} */

/*! \name Command SoftHiZ
 *  \brief Command SoftHiZ
 */
#define L6480_CMD_SOFTHIZ                   0xA0
#define L6480_CMD_SOFTHIZ_LEN               1
#define L6480_CMD_SOFTHIZ_READ              0
/*! @} */

/*! \name Command HardHiZ
 *  \brief Command HardHiZ
 */
#define L6480_CMD_HARDHIZ                   0xA8
#define L6480_CMD_HARDHIZ_LEN               1
#define L6480_CMD_HARDHIZ_READ              0
/*! @} */

/*! \name Command GetStatus
 *  \brief Command GetStatus
 */
#define L6480_CMD_GETSTATUS                 0xD0
#define L6480_CMD_GETSTATUS_LEN             3
#define L6480_CMD_GETSTATUS_READ            1
/*! @} */

/*! \enum l6480_dir_t
 *  \brief Type to determine the direction of movement
 */
typedef enum {
    L6480_DIR_REV   = 0x00,     /*!< Reverse */
    L6480_DIR_FWD   = 0x01,     /*!< Forward */
} l6480_dir_t;

/*! \enum l6480_act_t
 *  \brief Type to determine the action after reaching or releasing switch
 */
typedef enum {
    L6480_ACT_RESET = 0x00,     /*!< Reset ABS_POS_register */
    L6480_ACT_COPY  = 0x01,     /*!< Copy ABS_POS register to MARK register */
} l6480_act_t;

/*! \union l6480_cmd_val_t
 *  \brief Type for values sent by commands. 
 */
typedef union {
    uint8_t array[4];               /*!< array access */
    struct {
        uint32_t data;                  /*!< raw data */
    } raw;                          /*!< raw data access */
    struct {
        uint32_t data       : 20;       /*!< raw data */
        uint32_t unused     : 12;       /*!< unused */
    } speed;                        /*!< speed data access */
    struct {
        uint32_t data       : 22;       /*!< step data */
        uint32_t unused     : 10;       /*!< unused */
    } step;                         /*!< step data access */
    struct {
        int32_t data        : 22;       /*!< position data */
        int32_t unused      : 10;       /*!< unused */
    } position;                     /*!< position data access */
} l6480_cmd_val_t;

/******************************************************************************
Functions
******************************************************************************/
/*! \fn void l6480_init(void)
 *  \brief Initialisation function for L6480
 *  
 *  \return void
 */
void l6480_init(void);

/*! \fn void l6480_send_cmd(uint8_t cmd, uint8_t len, uint8_t read, uint8_t *data)
 *  \brief Send a command to the L6480
 *  
 *  \param  cmd   command to be sent
 *  \param  len   length of command to be sent
 *  \param  read  flag to determine if data has to be read
 *  \param  *data pointer to data to be read or sent
 *  \return void
 */
void l6480_send_cmd(uint8_t cmd, uint8_t len, uint8_t read, uint8_t *data);

/*! \fn int32_t l6480_get_abs_pos(void)
 *  \brief Get absolute position
 *
 *  \return absolute Position
 */
int32_t l6480_get_abs_pos(void);

/*! \fn void l6480_set_abs_pos(int32_t pos)
 *  \brief Set absolute position
 *
 *  \param  pos Position
 *  \return void
 */
void l6480_set_abs_pos(int32_t pos);

/*! \fn int16_t l6480_get_el_pos(void)
 *  \brief Get electrical position
 *
 *  \return electrical Position
 */
int16_t l6480_get_el_pos(void);

/*! \fn void l6480_set_el_pos(uint8_t fullstep, uint8_t microstep)
 *  \brief Set electrical position
 *
 *  \param  fullstep    full steps
 *  \param  microstep   micro steps
 *  \return void
 */
void l6480_set_el_pos(uint8_t fullstep, uint8_t microstep);

/*! \fn int32_t l6480_get_mark(void)
 *  \brief Get mark position
 *
 *  \return mark Position
 */
int32_t l6480_get_mark(void);

/*! \fn void l6480_set_mark(int32_t mark)
 *  \brief Set mark position
 *
 *  \param  mark    mark position
 *  \return void
 */
void l6480_set_mark(int32_t mark);

/*! \fn int32_t l6480_get_speed(void)
 *  \brief Get current motor speed
 *
 *  \return current speed
 */
int32_t l6480_get_speed(void);

/*! \fn int32_t l6480_get_speed_millisteps_s(void)
 *  \brief Get current motor speed in millisteps per second
 *
 *  \return current speed in millisteps per second
 */
int32_t l6480_get_speed_millisteps_s(void);

/*! \fn uint16_t l6480_get_acc(void)
 *  \brief Get current motor acceleration
 *
 *  \return current acceleration
 */
uint16_t l6480_get_acc(void);

/*! \fn uint16_t l6480_get_acc_steps_ss(void)
 *  \brief Get current motor acceleration in steps per second^2
 *
 *  \return current acceleration in steps per second^2
 */
uint16_t l6480_get_acc_steps_ss(void);

/*! \fn void l6480_set_acc(uint16_t acc)
 *  \brief Set acceleration
 *
 *  \param  acc     acceleration
 *  \return void
 */
void l6480_set_acc(uint16_t acc);

/*! \fn void l6480_set_acc_steps_ss(uint16_t acc)
 *  \brief Set acceleration in steps per second^2
 *
 *  \param  acc     acceleration in steps per second^2
 *  \return void
 */
void l6480_set_acc_steps_ss(uint16_t acc);

/*! \fn uint16_t l6480_get_dec(void)
 *  \brief Get current motor deceleration
 *
 *  \return current deceleration
 */
uint16_t l6480_get_dec(void);

/*! \fn uint16_t l6480_get_dec_steps_ss(void)
 *  \brief Get current motor deceleration in steps per second^2
 *
 *  \return current deceleration in steps per second^2
 */
uint16_t l6480_get_dec_steps_ss(void);

/*! \fn void l6480_set_dec(uint16_t dec)
 *  \brief Set deceleration
 *
 *  \param  dec     deceleration
 *  \return void
 */
void l6480_set_dec(uint16_t dec);

/*! \fn void l6480_set_dec_steps_ss(uint16_t dec)
 *  \brief Set deceleration in steps per second^2
 *
 *  \param  dec     deceleration in steps per second^2
 *  \return void
 */
void l6480_set_dec_steps_ss(uint16_t dec);

/*! \fn uint16_t l6480_get_max_speed(void)
 *  \brief Get maximum speed
 *
 *  \return maximum speed
 */
uint16_t l6480_get_max_speed(void);

/*! \fn uint16_t l6480_get_max_speed_steps_ss(void)
 *  \brief Get maximum speed in steps per second
 *
 *  \return maximum speed in steps per second
 */
uint16_t l6480_get_max_speed_steps_ss(void);

/*! \fn void l6480_set_max_speed(uint16_t max_speed)
 *  \brief Set maximum speed
 *
 *  \param  max_speed   maximum speed
 *  \return void
 */
void l6480_set_max_speed(uint16_t max_speed);

/*! \fn void l6480_set_max_speed_steps_ss(uint16_t max_speed)
 *  \brief Set maximum speed in steps per second
 *
 *  \param  max_speed   maximum speed in steps per second
 *  \return void
 */
void l6480_set_max_speed_steps_ss(uint16_t max_speed);

/*! \fn uint16_t l6480_get_min_speed(void)
 *  \brief Get minimum speed
 *
 *  \return minimum speed
 */
uint16_t l6480_get_min_speed(void);

/*! \fn uint32_t l6480_get_min_speed_millisteps_s(void)
 *  \brief Get minimum speed in milli steps per second
 *
 *  \return minimum speed in milli steps per second
 */
uint32_t l6480_get_min_speed_millisteps_s(void);

/*! \fn uint16_t l6480_get_lspd_opt_speed(void)
 *  \brief Get low speed optimization threshold
 *
 *  \return low speed optimization threshold
 */
uint16_t l6480_get_lspd_opt_speed(void);

/*! \fn uint32_t l6480_get_lspd_opt_speed_millisteps_s(void)
 *  \brief Get low speed optimization threshold in milli steps per second
 *
 *  \return low speed optimization threshold in milli steps per second
 */
uint32_t l6480_get_lspd_opt_speed_millisteps_s(void);

/*! \fn void l6480_set_min_speed(uint16_t speed)
 *  \brief Set minimum speed
 *
 *  \param  speed   minimum speed
 *  \return void
 */
void l6480_set_min_speed(uint16_t speed);

/*! \fn void l6480_set_min_speed_millisteps_s(uint32_t speed)
 *  \brief Set minimum speed in milli steps per second
 *
 *  \param  speed   minimum speed in milli steps per second
 *  \return void
 */
void l6480_set_min_speed_millisteps_s(uint32_t speed);

/*! \fn void l6480_set_lspd_opt_speed(uint16_t speed)
 *  \brief Set low speed optimization threshold
 *
 *  \param  speed   low speed optimization threshold
 *  \return void
 */
void l6480_set_lspd_opt_speed(uint16_t speed);

/*! \fn void l6480_set_lspd_opt_speed_millisteps_s(uint32_t speed)
 *  \brief Set low speed optimization threshold in milli steps per second
 *
 *  \param  speed   low speed optimization threshold in milli steps per second
 *  \return void
 */
void l6480_set_lspd_opt_speed_millisteps_s(uint32_t speed);

/*! \fn uint8_t l6480_get_boost_mode(void)
 *  \brief Get boost mode
 *
 *  \return boost mode
 */
uint8_t l6480_get_boost_mode(void);

/*! \fn void l6480_set_boost_mode_on(void)
 *  \brief Turn boost mode on
 *
 *  \return void
 */
void l6480_set_boost_mode_on(void);

/*! \fn void l6480_set_boost_mode_off(void)
 *  \brief Turn boost mode off
 *
 *  \return void
 */
void l6480_set_boost_mode_off(void);

/*! \fn uint16_t l6480_get_fs_spd(void)
 *  \brief Get full-step speed threshold
 *
 *  \return full-step speed threshold
 */
uint16_t l6480_get_fs_spd(void);

/*! \fn uint16_t l6480_get_fs_spd_steps_s(void)
 *  \brief Get full-step speed threshold in steps per second
 *
 *  \return full-step speed threshold in steps per second
 */
uint16_t l6480_get_fs_spd_steps_s(void);

/*! \fn void l6480_set_fs_spd(uint16_t speed)
 *  \brief Set full-step speed threshold
 *
 *  \param  speed    full-step speed threshold
 *  \return void
 */
void l6480_set_fs_spd(uint16_t speed);

/*! \fn void l6480_set_fs_spd_steps_s(uint16_t speed)
 *  \brief Set full-step speed threshold in steps per second
 *
 *  \param  speed    full-step speed threshold in steps per second
 *  \return void
 */
void l6480_set_fs_spd_steps_s(uint16_t speed);

/*! \fn uint8_t l6480_get_kval_hold(void)
 *  \brief Get kval_hold
 *
 *  return kval_hold
 */
uint8_t l6480_get_kval_hold(void);

/*! \fn void l6480_set_kval_hold(uint8_t value)
 *  \brief Ser kval_hold
 *
 *  \param  value kval_hold
 *  \return void
 */
void l6480_set_kval_hold(uint8_t value);

/*! \fn uint8_t l6480_get_kval_run(void)
 *  \brief Get kval_run
 *
 *  return kval_run
 */
uint8_t l6480_get_kval_run(void);

/*! \fn void l6480_set_kval_run(uint8_t value)
 *  \brief Ser kval_run
 *
 *  \param  value kval_run
 *  \return void
 */
void l6480_set_kval_run(uint8_t value);

/*! \fn uint8_t l6480_get_kval_acc(void)
 *  \brief Get kval_acc
 *
 *  return kval_acc
 */
uint8_t l6480_get_kval_acc(void);

/*! \fn void l6480_set_kval_acc(uint8_t value)
 *  \brief Ser kval_acc
 *
 *  \param  value kval_acc
 *  \return void
 */
void l6480_set_kval_acc(uint8_t value);

/*! \fn uint8_t l6480_get_kval_dec(void)
 *  \brief Get kval_dec
 *
 *  return kval_dec
 */
uint8_t l6480_get_kval_dec(void);

/*! \fn void l6480_set_kval_dec(uint8_t value)
 *  \brief Ser kval_dec
 *
 *  \param  value kval_dec
 *  \return void
 */
void l6480_set_kval_dec(uint8_t value);

/*! \fn uint16_t l6480_get_int_speed(void)
 *  \brief Get int_speed
 *
 *  \return int_speed
 */
uint16_t l6480_get_int_speed(void);

/*! \fn uint32_t l6480_get_int_speed_millisteps_s(void)
 *  \brief Get int_speed in millisteps per second
 *
 *  \return int_speed in millisteps per second
 */
uint32_t l6480_get_int_speed_millisteps_s(void);

/*! \fn void l6480_set_int_speed(uint16_t speed)
 *  \brief Set int_speed
 *
 *  \param  speed int_speed
 *  \return void
 */
void l6480_set_int_speed(uint16_t speed);

/*! \fn void l6480_set_int_speed_millisteps_s(uint32_t speed)
 *  \brief Set int_speed in millisteps per second
 *
 *  \param  speed int_speed in millisteps per second
 *  \return void
 */
void l6480_set_int_speed_millisteps_s(uint32_t speed);

/*! \fn uint8_t l6480_get_st_slp(void)
 *  \brief Get st_slp
 *
 *  \return st_slp
 */
uint8_t l6480_get_st_slp(void);

/*! \fn void l6480_set_st_slp(uint8_t slope)
 *  \brief Set st_slp
 *
 *  \param  slope st_slp
 *  \return void
 */
void l6480_set_st_slp(uint8_t slope);

/*! \fn uint8_t l6480_get_fn_slp_acc(void)
 *  \brief Get fn_slp_acc
 *
 *  \return fn_slp_acc
 */
uint8_t l6480_get_fn_slp_acc(void);

/*! \fn void l6480_set_fn_slp_acc(uint8_t slope)
 *  \brief Set fn_slp_dec
 *
 *  \param  slope fn_slp_acc
 *  \return void
 */
void l6480_set_fn_slp_acc(uint8_t slope);

/*! \fn uint8_t l6480_get_fn_slp_dec(void)
 *  \brief Get fn_slp_dec
 *
 *  \return fn_slp_dec
 */
uint8_t l6480_get_fn_slp_dec(void);

/*! \fn void l6480_set_fn_slp_dec(uint8_t slope)
 *  \brief Set fn_slp_dec
 *
 *  \param  slope fn_slp_dec
 *  \return void
 */
void l6480_set_fn_slp_dec(uint8_t slope);

/*! \fn uint8_t l6480_get_k_therm(void)
 *  \brief Get k_therm
 *
 *  \return k_therm
 */
uint8_t l6480_get_k_therm(void);

/*! \fn uint16_t l6480_get_k_therm_milli(void)
 *  \brief Get k_therm coefficient
 *
 *  \return k_therm coefficient
 */
uint16_t l6480_get_k_therm_milli(void);

/*! \fn void l6480_set_k_therm(uint8_t value)
 *  \brief Set k_therm
 *
 *  \param  value k_therm
 *  \return void
 */
void l6480_set_k_therm(uint8_t value);

/*! \fn void l6480_set_k_therm_milli(uint16_t value)
 *  \brief Set k_therm coefficient
 *
 *  \param  value k_therm coefficient
 *  \return void
 */
void l6480_set_k_therm_milli(uint16_t value);

/*! \fn uint8_t l6480_get_adc_out(void)
 *  \brief Get adc_out
 *
 *  \return adc_out
 */
uint8_t l6480_get_adc_out(void);

/*! \fn uint8_t l6480_get_ocd_th(void)
 *  \brief Get ocd_th
 *
 *  \return ocd_th
 */
uint8_t l6480_get_ocd_th(void);

/*! \fn uint16_t l6480_get_ocd_th_millivolt(void)
 *  \brief Get ocd_th in millivolts
 *
 *  \return ocd_th in millivolts
 */
uint16_t l6480_get_ocd_th_millivolt(void);

/*! \fn void l6480_set_ocd_th(uint8_t threshold)
 *  \brief Set ocd_th
 *
 *  \param  threshold ocd_th
 *  \return void
 */
void l6480_set_ocd_th(uint8_t threshold);

/*! \fn void l6480_set_ocd_th_millivolt(uint16_t threshold)
 *  \brief Set ocd_th in millivolts
 *
 *  \param  threshold ocd_th in millivolts
 *  \return void
 */
void l6480_set_ocd_th_millivolt(uint16_t threshold);

/*! \fn uint8_t l6480_get_stall_th(void)
 *  \brief Get stall_th
 *
 *  \return stall_th
 */
uint8_t l6480_get_stall_th(void);

/*! \fn uint16_t l6480_get_stall_th_millivolt(void)
 *  \brief Get stall_th in millivolts
 *
 *  \return stall_th in millivolts
 */
uint16_t l6480_get_stall_th_millivolt(void);

/*! \fn void l6480_set_stall_th(uint8_t threshold)
 *  \brief Set stall_th
 *
 *  \param  threshold stall_th
 *  \return void
 */
void l6480_set_stall_th(uint8_t threshold);

/*! \fn void l6480_set_stall_th_millivolt(uint16_t threshold)
 *  \brief Set stall_th in millivolts
 *
 *  \param  threshold stall_th in millivolts
 *  \return void
 */
void l6480_set_stall_th_millivolt(uint16_t threshold);

/*! \fn uint8_t l6480_get_step_mode(void)
 *  \brief Get step_mode
 *
 *  \return step_mode
 */
uint8_t l6480_get_step_mode(void);

/*! \fn uint8_t l6480_get_step_mode_sync_en(void)
 *  \brief Get sync_en from step_mode
 *
 *  \return sync_en from step_mode
 */
uint8_t l6480_get_step_mode_sync_en(void);

/*! \fn uint8_t l6480_get_step_mode_sync_sel(void)
 *  \brief Get sync_sel from step_mode
 *
 *  \return sync_sel from step_mode
 */
uint8_t l6480_get_step_mode_sync_sel(void);

/*! \fn uint8_t l6480_get_step_mode_step_sel(void)
 *  \brief Get step_sel from step_mode
 *
 *  \return step_sel from step_mode
 */
uint8_t l6480_get_step_mode_step_sel(void);

/*! \fn uint8_t l6480_get_step_mode_step_sel_steps(void)
 *  \brief Get step_sel from step_mode in steps
 *
 *  \return step_sel from step_mode in steps
 */
uint8_t l6480_get_step_mode_step_sel_steps(void);

/*! \fn void l6480_set_step_mode(uint8_t value)
 *  \brief Set step_mode
 *
 *  \param  value step_mode
 *  \return void
 */
void l6480_set_step_mode(uint8_t value);

/*! \fn void l6480_set_step_mode_sync_en_on(void)
 *  \brief Set sync_en from step_mode on
 *
 *  \return void
 */
void l6480_set_step_mode_sync_en_on(void);

/*! \fn void l6480_set_step_mode_sync_en_off(void)
 *  \brief Set sync_en from step_mode off
 *
 *  \return void
 */
void l6480_set_step_mode_sync_en_off(void);

/*! \fn void l6480_set_step_mode_sync_sel(uint8_t sync_sel)
 *  \brief Set sync_sel from step_mode
 *
 *  \param  sync_sel sync_sel
 *  \return void
 */
void l6480_set_step_mode_sync_sel(uint8_t sync_sel);

/*! \fn void l6480_set_step_mode_step_sel(uint8_t sel)
 *  \brief Set step_sel from step_mode
 *
 *  \param  sel step_sel
 *  \return void
 */
void l6480_set_step_mode_step_sel(uint8_t sel);

/*! \fn void l6480_set_step_mode_step_sel_steps(uint8_t steps)
 *  \brief Set step_sel from step_mode in steps
 *
 *  \param  steps step_sel in steps
 *  \return void
 */
void l6480_set_step_mode_step_sel_steps(uint8_t steps);

/*! \fn uint8_t l6480_get_alarm_en(void)
 *  \brief Get alarm_en
 *
 *  \return alarm_en
 */
uint8_t l6480_get_alarm_en(void);

/*! \fn void l6480_set_alarm_en(uint8_t alarm_en)
 *  \brief Set alarm_en
 *
 *  \param  alarm_en alarm_en
 *  \return void
 */
void l6480_set_alarm_en(uint8_t alarm_en);

/*! \fn void l6480_set_alarm_en_overcurrent(uint8_t overcurrent)
 *  \brief Set overcurrent alarm_en
 *
 *  \param  overcurrent overcurrent alarm_en
 *  \return void
 */
void l6480_set_alarm_en_overcurrent(uint8_t overcurrent);

/*! \fn void l6480_set_alarm_en_th_shutdown(uint8_t th_shutdown)
 *  \brief Set th_shutdown alarm_en
 *
 *  \param  th_shutdown th_shutdown alarm_en
 *  \return void
 */
void l6480_set_alarm_en_th_shutdown(uint8_t th_shutdown);

/*! \fn void l6480_set_alarm_en_th_warning(uint8_t th_warning)
 *  \brief Set th_warning alarm_en
 *
 *  \param  th_warning th_warning alarm_en
 *  \return void
 */
void l6480_set_alarm_en_th_warning(uint8_t th_warning);

/*! \fn void l6480_set_alarm_en_uvlo(uint8_t uvlo)
 *  \brief Set uvlo alarm_en
 *
 *  \param  uvlo uvlo alarm_en
 *  \return void
 */
void l6480_set_alarm_en_uvlo(uint8_t uvlo);

/*! \fn void l6480_set_alarm_en_adc_uvlo(uint8_t adc_uvlo)
 *  \brief Set adc_uvlo alarm_en
 *
 *  \param  adc_uvlo adc_uvlo alarm_en
 *  \return void
 */
void l6480_set_alarm_en_adc_uvlo(uint8_t adc_uvlo);

/*! \fn void l6480_set_alarm_en_stall_detect(uint8_t stall_det)
 *  \brief Set stall_detect alarm_en
 *
 *  \param  stall_det stall_detect alarm_en
 *  \return void
 */
void l6480_set_alarm_en_stall_detect(uint8_t stall_det);

/*! \fn void l6480_set_alarm_en_switch_on(uint8_t switch_on)
 *  \brief Set switch_on alarm_en
 *
 *  \param  switch_on switch_on alarm_en
 *  \return void
 */
void l6480_set_alarm_en_switch_on(uint8_t switch_on);

/*! \fn void l6480_set_alarm_en_command_err(uint8_t cmd_err)
 *  \brief Set command_err alarm_en
 *
 *  \param  cmd_err command_err alarm_en
 *  \return void
 */
void l6480_set_alarm_en_command_err(uint8_t cmd_err);

/*! \fn uint16_t l6480_get_gatecfg1(void)
 *  \brief Get gatecfg1
 *
 *  \return gatecfg1
 */
uint16_t l6480_get_gatecfg1(void);

/*! \fn uint8_t l6480_get_gatecfg1_wd_en(void)
 *  \brief Get wd_en from gatecfg1
 *
 *  \return wd_en
 */
uint8_t l6480_get_gatecfg1_wd_en(void);

/*! \fn uint8_t l6480_get_gatecfg1_tboost(void)
 *  \brief Get tboost from gatecfg1
 *
 *  \return tboost
 */
uint8_t l6480_get_gatecfg1_tboost(void);

/*! \fn uint16_t l6480_get_gatecfg1_tboost_nanosecond(void)
 *  \brief Get tboost from gatecfg1 in nanoseconds
 *
 *  \return tboost in nanoseconds
 */
uint16_t l6480_get_gatecfg1_tboost_nanosecond(void);

/*! \fn uint8_t l6480_get_gatecfg1_igate(void)
 *  \brief Get igate from gatecfg1
 *
 *  \return igate
 */
uint8_t l6480_get_gatecfg1_igate(void);

/*! \fn uint8_t l6480_get_gatecfg1_igate_milliampere(void)
 *  \brief Get igate from gatecfg1 in milliamperes
 *
 *  \return igate in milliamperes
 */
uint8_t l6480_get_gatecfg1_igate_milliampere(void);

/*! \fn uint8_t l6480_get_gatecfg1_tcc(void)
 *  \brief Get tcc from gatecfg1
 *
 *  \return tcc
 */
uint8_t l6480_get_gatecfg1_tcc(void);

/*! \fn uint16_t l6480_get_gatecfg1_tcc_nanosecond(void)
 *  \brief Get tcc from gatecfg1 in nanoseconds
 *
 *  \return tcc in nanoseconds
 */
uint16_t l6480_get_gatecfg1_tcc_nanosecond(void);

/*! \fn void l6480_set_gatecfg1(uint16_t value)
 *  \brief Set gatecfg1
 *
 *  \param  value gatecfg1
 *  \return void
 */
void l6480_set_gatecfg1(uint16_t value);

/*! \fn void l6480_set_gatecfg1_wd_en(uint8_t wd_en)
 *  \brief Set wd_en from gatecfg1
 *
 *  \param  wd_en wd_en
 *  \return vaoid
 */
void l6480_set_gatecfg1_wd_en(uint8_t wd_en);

/*! \fn void l6480_set_gatecfg1_wd_en_on(void)
 *  \brief Set wd_en from gatecfg1 on
 *
 *  \return void
 */
void l6480_set_gatecfg1_wd_en_on(void);

/*! \fn void l6480_set_gatecfg1_wd_en_off(void)
 *  \brief Set wd_en from gatecfg1 off
 *
 *  \return void
 */
void l6480_set_gatecfg1_wd_en_off(void);

/*! \fn void l6480_set_gatecfg1_tboost(uint8_t time)
 *  \brief Set tboost from gatecfg1
 *
 *  \param  time tboost
 *  \return void
 */
void l6480_set_gatecfg1_tboost(uint8_t time);

/*! \fn void l6480_set_gatecfg1_tboost_nanosecond(uint16_t time)
 *  \brief Set tboost from gatecfg1 in nanoseconds
 *
 *  \param  time tboost in nanoseconds
 *  \return void
 */
void l6480_set_gatecfg1_tboost_nanosecond(uint16_t time);

/*! \fn void l6480_set_gatecfg1_igate(uint8_t current)
 *  \brief Set igate from gatecfg1
 *
 *  \param  current igate
 *  \return void
 */
void l6480_set_gatecfg1_igate(uint8_t current);

/*! \fn void l6480_set_gatecfg1_igate_milliampere(uint8_t current)
 *  \brief Set igate from gatecfg1 in milliamperes
 *
 *  \param  current igate in milliamperes
 *  \return void
 */
void l6480_set_gatecfg1_igate_milliampere(uint8_t current);

/*! \fn void l6480_set_gatecfg1_tcc(uint8_t time)
 *  \brief Set tcc from gatecfg1
 *
 *  \param  time tcc
 *  \return void
 */
void l6480_set_gatecfg1_tcc(uint8_t time);

/*! \fn void l6480_set_gatecfg1_tcc_nanosecond(uint16_t time)
 *  \brief Set tcc from gatecfg1 in nanoseconds
 *
 *  \param  time tcc in nanoseconds
 *  \return void
 */
void l6480_set_gatecfg1_tcc_nanosecond(uint16_t time);

/*! \fn uint8_t l6480_get_gatecfg2(void)
 *  \brief Get gatecfg2
 *
 *  \return gatecfg2
 */
uint8_t l6480_get_gatecfg2(void);

/*! \fn uint8_t l6480_get_gatecfg2_tblank(void)
 *  \brief Get tblank from gatecfg2
 *
 *  \return tblank
 */
uint8_t l6480_get_gatecfg2_tblank(void);

/*! \fn uint16_t l6480_get_gatecfg2_tblank_nanosecond(void)
 *  \brief Get tblank from gatecfg2 in nanoseconds
 *
 *  \return tblank in nanoseconds
 */
uint16_t l6480_get_gatecfg2_tblank_nanosecond(void);

/*! \fn uint8_t l6480_get_gatecfg2_tdt(void)
 *  \brief Get tdt from gatecfg2
 *
 *  \return tdt
 */
uint8_t l6480_get_gatecfg2_tdt(void);

/*! \fn uint16_t l6480_get_gatecfg2_tdt_nanosecond(void)
 *  \brief Get tdt from gatecfg2 in nanoseconds
 *
 *  \return tdt in nanoseconds
 */
uint16_t l6480_get_gatecfg2_tdt_nanosecond(void);

/*! \fn void l6480_set_gatecfg2(uint8_t value)
 *  \brief Set gatecfg2
 *
 *  \param  value gatecfg2
 *  \return void
 */
void l6480_set_gatecfg2(uint8_t value);

/*! \fn void l6480_set_gatecfg2_tblank(uint8_t time)
 *  \brief Set tblank from gatecfg2
 *
 *  \param  time tblank
 *  \return void
 */
void l6480_set_gatecfg2_tblank(uint8_t time);

/*! \fn void l6480_set_gatecfg2_tblank_nanosecond(uint16_t time)
 *  \brief Set tblank from gatecfg2 in nanoseconds
 *
 *  \param  time tblank in nanoseconds
 *  \return void
 */
void l6480_set_gatecfg2_tblank_nanosecond(uint16_t time);

/*! \fn void l6480_set_gatecfg2_tdt(uint8_t time)
 *  \brief Set tdt from gatecfg2
 *
 *  \param  time tdt
 *  \return void
 */
void l6480_set_gatecfg2_tdt(uint8_t time);

/*! \fn void l6480_set_gatecfg2_tdt_nanosecond(uint16_t time)
 *  \brief Set tdt from gatecfg2 in nanoseconds
 *
 *  \param  time tdt in nanoseconds
 *  \return void
 */
void l6480_set_gatecfg2_tdt_nanosecond(uint16_t time);

/*! \fn uint16_t l6480_get_config(void)
 *  \brief Get config
 *
 *  \return config
 */
uint16_t l6480_get_config(void);

/*! \fn uint8_t l6480_get_config_f_pwm_int(void)
 *  \brief Get f_pwm_int from config
 *
 *  \return f_pwm_int
 */
uint8_t l6480_get_config_f_pwm_int(void);

/*! \fn uint8_t l6480_get_config_f_pwm_dec(void)
 *  \brief Get f_pwm_dec from config
 *
 *  \return f_pwm_dec
 */
uint8_t l6480_get_config_f_pwm_dec(void);

/*! \fn uint8_t l6480_get_config_vccval(void)
 *  \brief Get vccval from config
 *
 *  \return vccval
 */
uint8_t l6480_get_config_vccval(void);

/*! \fn uint8_t l6480_get_config_uvloval(void)
 *  \brief Get uvloval from config
 *
 *  \return uvloval
 */
uint8_t l6480_get_config_uvloval(void);

/*! \fn uint8_t l6480_get_config_oc_sd(void)
 *  \brief Get oc_sd from config
 *
 *  \return oc_sd
 */
uint8_t l6480_get_config_oc_sd(void);

/*! \fn uint8_t l6480_get_config_en_vscomp(void)
 *  \brief Get en_vscomp from config
 *
 *  \return en_vscomp
 */
uint8_t l6480_get_config_en_vscomp(void);

/*! \fn uint8_t l6480_get_config_sw_mode(void)
 *  \brief Get sw_mode from config
 *
 *  \return sw_mode
 */
uint8_t l6480_get_config_sw_mode(void);

/*! \fn uint8_t l6480_get_config_ext_clk(void)
 *  \brief Get ext_clk from config
 *
 *  \return ext_clk
 */
uint8_t l6480_get_config_ext_clk(void);

/*! \fn uint8_t l6480_get_config_osc_sel(void)
 *  \brief Get osc_sel from config
 *
 *  \return osc_sel
 */
uint8_t l6480_get_config_osc_sel(void);

/*! \fn uint8_t l6480_get_config_f_pwm_int(void)
 *  \brief Get f_pwm_int from config
 *
 *  \return f_pwm_int
 */
uint8_t l6480_get_config_f_pwm_int(void);

/*! \fn void l6480_set_config(uint16_t value)
 *  \brief Set config
 *
 *  \param  value config
 *  \return void
 */
void l6480_set_config(uint16_t value);

/*! \fn void l6480_set_config_f_pwm_int(uint8_t factor)
 *  \brief Set f_pwm from config
 *
 *  \param  factor f_pwm_int
 *  \return void
 */
void l6480_set_config_f_pwm_int(uint8_t factor);

/*! \fn void l6480_set_config_f_pwm_dec(uint8_t factor)
 *  \brief Set f_pwm from config
 *
 *  \param  factor f_pwm_dec
 *  \return void
 */
void l6480_set_config_f_pwm_dec(uint8_t factor);

/*! \fn void l6480_set_config_vccval(uint8_t value)
 *  \brief Set f_pwm from config
 *
 *  \param  value vccval
 *  \return void
 */
void l6480_set_config_vccval(uint8_t value);

/*! \fn void l6480_set_config_uvloval(uint8_t value)
 *  \brief Set f_pwm from config
 *
 *  \param  value uvloval
 *  \return void
 */
void l6480_set_config_uvloval(uint8_t value);

/*! \fn void l6480_set_config_oc_sd(uint8_t value)
 *  \brief Set f_pwm from config
 *
 *  \param  value oc_sd
 *  \return void
 */
void l6480_set_config_oc_sd(uint8_t value);

/*! \fn void l6480_set_config_en_vscomp(uint8_t value)
 *  \brief Set f_pwm from config
 *
 *  \param  value en_vscomp
 *  \return void
 */
void l6480_set_config_en_vscomp(uint8_t value);

/*! \fn void l6480_set_config_sw_mode(uint8_t mode)
 *  \brief Set f_pwm from config
 *
 *  \param  mode sw_mode
 *  \return void
 */
void l6480_set_config_sw_mode(uint8_t mode);

/*! \fn void l6480_set_config_ext_clk(uint8_t value)
 *  \brief Set f_pwm from config
 *
 *  \param  value ext_clk
 *  \return void
 */
void l6480_set_config_ext_clk(uint8_t value);

/*! \fn void l6480_set_config_osc_sel(uint8_t value)
 *  \brief Set f_pwm from config
 *
 *  \param  value osc_sel
 *  \return void
 */
void l6480_set_config_osc_sel(uint8_t value);

/*! \fn uint16_t l6480_get_status(void)
 *  \brief Get status
 *
 *  \return status
 */
uint16_t l6480_get_status(void);

/*! \fn uint8_t l6480_get_status_step_loss_b(void)
 *  \brief Get step_loss_b from status
 *
 *  \return step_loss_b
 */
uint8_t l6480_get_status_step_loss_b(void);

/*! \fn uint8_t l6480_get_status_step_loss_a(void)
 *  \brief Get step_loss_a from status
 *
 *  \return step_loss_a
 */
uint8_t l6480_get_status_step_loss_a(void);

/*! \fn uint8_t l6480_get_status_ocd(void)
 *  \brief Get ocd from status
 *
 *  \return ocd
 */
uint8_t l6480_get_status_ocd(void);

/*! \fn uint8_t l6480_get_status_th_status(void)
 *  \brief Get th_status from status
 *
 *  \return th_status
 */
uint8_t l6480_get_status_th_status(void);

/*! \fn uint8_t l6480_get_status_uvlo_adc(void)
 *  \brief Get uvlo_adc from status
 *
 *  \return uvlo_adc
 */
uint8_t l6480_get_status_uvlo_adc(void);

/*! \fn uint8_t l6480_get_status_uvlo(void)
 *  \brief Get uvlo from status
 *
 *  \return uvlo
 */
uint8_t l6480_get_status_uvlo(void);

/*! \fn uint8_t l6480_get_status_stck_mod(void)
 *  \brief Get stck_mod from status
 *
 *  \return stck_mod
 */
uint8_t l6480_get_status_stck_mod(void);

/*! \fn uint8_t l6480_get_status_cmd_error(void)
 *  \brief Get cmd_error from status
 *
 *  \return cmd_error
 */
uint8_t l6480_get_status_cmd_error(void);

/*! \fn uint8_t l6480_get_status_mot_status(void)
 *  \brief Get mot_status from status
 *
 *  \return mot_status
 */
uint8_t l6480_get_status_mot_status(void);

/*! \fn uint8_t l6480_get_status_dir(void)
 *  \brief Get dir from status
 *
 *  \return dir
 */
uint8_t l6480_get_status_dir(void);

/*! \fn uint8_t l6480_get_status_sw_evn(void)
 *  \brief Get sw_evn from status
 *
 *  \return sw_evn
 */
uint8_t l6480_get_status_sw_evn(void);

/*! \fn uint8_t l6480_get_status_sw_f(void)
 *  \brief Get sw_f from status
 *
 *  \return sw_f
 */
uint8_t l6480_get_status_sw_f(void);

/*! \fn uint8_t l6480_get_status_busy(void)
 *  \brief Get busy from status
 *
 *  \return busy
 */
uint8_t l6480_get_status_busy(void);

/*! \fn uint8_t l6480_get_status_hiz(void)
 *  \brief Get hiz from status
 *
 *  \return hiz
 */
uint8_t l6480_get_status_hiz(void);

/*! \fn void l6480_cmd_nop(void)
 *  \brief Send command nop
 *
 *  \return void
 */
void l6480_cmd_nop(void);

/*! \fn void l6480_cmd_run(l6480_dir_t dir, uint32_t speed)
 *  \brief Send command run
 *
 *  \param  dir     Direction of movement
 *  \param  speed   Speed of movement
 *  \return void
 */
void l6480_cmd_run(l6480_dir_t dir, uint32_t speed);

/*! \fn void l6480_cmd_run_millisteps_s(l6480_dir_t dir, uint32_t speed)
 *  \brief Send command run with speed in millisteps per second
 *
 *  \param  dir     Direction of movement
 *  \param  speed   Speed of movement in millisteps per second
 *  \return void
 */
void l6480_cmd_run_millisteps_s(l6480_dir_t dir, uint32_t speed);

/*! \fn void l6480_cmd_stepclock(l6480_dir_t dir)
 *  \brief Send command stepclock
 *
 *  \param  dir     Direction of movement
 *  \return void
 */
void l6480_cmd_stepclock(l6480_dir_t dir);

/*! \fn void l6480_cmd_move(l6480_dir_t dir, uint32_t n_step)
 *  \brief Send command run
 *
 *  \param  dir     Direction of movement
 *  \param  n_step  Number of steps
 *  \return void
 */
void l6480_cmd_move(l6480_dir_t dir, uint32_t n_step);

/*! \fn void l6480_cmd_goto(int32_t abs_pos)
 *  \brief Send command goto
 *
 *  \param  abs_pos End position of movement
 *  \return void
 */
void l6480_cmd_goto(int32_t abs_pos);

/*! \fn void l6480_cmd_goto_dir(l6480_dir_t dir, int32_t abs_pos)
 *  \brief Send command goto with a given direction
 *
 *  \param  dir     Direction of movement
 *  \param  abs_pos End position of movement
 *  \return void
 */
void l6480_cmd_goto_dir(l6480_dir_t dir, int32_t abs_pos);

/*! \fn void l6480_cmd_gountil(l6480_act_t act, l6480_dir_t dir, uint32_t speed)
 *  \brief Send command gountil
 *
 *  \param  act     Action after reaching switch
 *  \param  dir     Direction of movement
 *  \param  speed   Speed of movement
 *  \return void
 */
void l6480_cmd_gountil(l6480_act_t act, l6480_dir_t dir, uint32_t speed);

/*! \fn void l6480_cmd_gountil_millisteps_s(l6480_act_t act, l6480_dir_t dir, uint32_t speed)
 *  \brief Send command gountil with speed in millisteps per second
 *
 *  \param  act     Action after reaching switch
 *  \param  dir     Direction of movement
 *  \param  speed   Speed of movement in millisteps per second
 *  \return void
 */
void l6480_cmd_gountil_millisteps_s(l6480_act_t act, l6480_dir_t dir, uint32_t speed);

/*! \fn void l6480_cmd_releasesw(l6480_act_t act, l6480_dir_t dir)
 *  \brief Send command releasesw
 *
 *  \param  act     Action after reaching switch
 *  \param  dir     Direction of movement
 *  \return void
 */
void l6480_cmd_releasesw(l6480_act_t act, l6480_dir_t dir);

/*! \fn void l6480_cmd_gohome(void)
 *  \brief Send command gohome
 *
 *  \return void
 */
void l6480_cmd_gohome(void);

/*! \fn void l6480_cmd_gomark(void)
 *  \brief Send command gomark
 *
 *  \return void
 */
void l6480_cmd_gomark(void);

/*! \fn void l6480_cmd_resetpos(void)
 *  \brief Send command resetpos
 *
 *  \return void
 */
void l6480_cmd_resetpos(void);

/*! \fn void l6480_cmd_resetdevice(void)
 *  \brief Send command resetdevice
 *
 *  \return void
 */
void l6480_cmd_resetdevice(void);

/*! \fn void l6480_cmd_softstop(void)
 *  \brief Send command softstop
 *
 *  \return void
 */
void l6480_cmd_softstop(void);

/*! \fn void l6480_cmd_hardstop(void)
 *  \brief Send command hardstop
 *
 *  \return void
 */
void l6480_cmd_hardstop(void);

/*! \fn void l6480_cmd_softhiz(void)
 *  \brief Send command softhiz
 *
 *  \return void
 */
void l6480_cmd_softhiz(void);

/*! \fn void l6480_cmd_hardhiz(void)
 *  \brief Send command hardhiz
 *
 *  \return void
 */
void l6480_cmd_hardhiz(void);

/*! \fn uint16_t l6480_cmd_getstatus(void)
 *  \brief Send command getstatus to read device status
 *
 *  \return status
 */
uint16_t l6480_cmd_getstatus(void);

/*! \fn uint8_t l6480_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdioType *io)
 *  \brief Command line parser for l6480
 *
 *  \param  *cmd        pointer to command to parse
 *  \param  *handled    pointer to handled flag, set true if command was parsed
 *  \param  *io         pointer to stdio for returning answer
 *  \return Error flag
 */
#if PL_HAS_SHELL
    uint8_t l6480_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
#endif /* PL_HAS_SHELL */

#endif /* L6480_H */
