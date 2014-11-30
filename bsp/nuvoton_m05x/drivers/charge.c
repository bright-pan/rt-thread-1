/*
 * File      : key.c
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

#include <stdlib.h>
#include "charge.h"

int rt_hw_charge_init(void)
{
    /* Configure the charge input flag pin */
    GPIO_SetMode(CHRG_INPUT_FLAG_PORT, CHRG_INPUT_FLAG_BIT, GPIO_PMD_INPUT);

    /* Configure the charge output enable pin */
    set_charge_output_enable(CHRG_OUTPUT_EN_OFF);
    GPIO_SetMode(CHRG_OUTPUT_EN_PORT, CHRG_OUTPUT_EN_BIT, GPIO_PMD_OUTPUT);
    
    return 0;
}

