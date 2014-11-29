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
#include "untils.h"

/*
    CHRG INPUT FLAG: P3.2
*/
#define CHRG_INPUT_FLAG_PORT_IDX 3
#define CHRG_INPUT_FLAG_PIN 2
#define CHRG_INPUT_FLAG_PORT PORT_MAP[CHRG_INPUT_FLAG_PORT_IDX]
#define CHRG_INPUT_FLAG_BIT BIT_MASK(CHRG_INPUT_FLAG_PIN)

/*
    CHRG OUTPUT ENABLE: P3.0
*/
#define CHRG_OUTPUT_EN_PORT_IDX 3
#define CHRG_OUTPUT_EN_PIN 0
#define CHRG_OUTPUT_EN_PORT PORT_MAP[CHRG_OUTPUT_EN_PORT_IDX]
#define CHRG_OUTPUT_EN_BIT BIT_MASK(CHRG_OUTPUT_EN_PIN)

__INLINE int get_charge_input_flag(void)
{
    return iport(CHRG_INPUT_FLAG_PORT_IDX, CHRG_INPUT_FLAG_PIN);
}

__INLINE int set_charge_output_enable(char dat)
{
    oport(CHRG_OUTPUT_EN_PORT_IDX, CHRG_OUTPUT_EN_PIN, dat);
    return dat;
}

int rt_hw_charge_init(void)
{
#ifdef CHRG_INPUT_FLAG_PORT
    /* Configure the charge input flag pin */
    GPIO_SetMode(CHRG_INPUT_FLAG_PORT, CHRG_INPUT_FLAG_BIT, GPIO_PMD_INPUT);
#endif

#ifdef CHRG_OUTPUT_EN_PORT
    /* Configure the charge output enable pin */
    set_charge_output_enable(CHRG_OUTPUT_EN_OFF);
    GPIO_SetMode(CHRG_OUTPUT_EN_PORT, CHRG_OUTPUT_EN_BIT, GPIO_PMD_OUTPUT);
#endif
    return 0;
}

