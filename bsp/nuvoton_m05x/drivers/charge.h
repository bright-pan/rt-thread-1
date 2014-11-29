/*
 * File      : charge.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2014, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-11-29     Bright      first implementation
 */

#ifndef __CHARGE_H__
#define __CHARGE_H__

#include <rthw.h>
#include <rtthread.h>
#include "M051Series.h"

#define CHRG_INPUT_FLAG_FULL 1

#define CHRG_OUTPUT_EN_ON 1
#define CHRG_OUTPUT_EN_OFF 0

__INLINE int get_charge_input_flag(void);
__INLINE int set_charge_output_enable(char dat);

#endif
