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
#include "key.h"
#include "untils.h"

#define KEY_VALID_TIMEOUT 50000
#define KEY_VALID_TIME 500
#define KEY_VALID_VALUE 0

/* Initial gpio key pin  */
int rt_hw_key_init(void)
{
    /* Configure the key sw pin */
    set_key_sw(1);
    GPIO_SetMode(KEY_SW_PORT, KEY_SW_BIT, GPIO_PMD_QUASI);
    GPIO_EnableEINT1(KEY_SW_PORT, KEY_SW_PIN, GPIO_INT_FALLING);

    /* Configure the key power pin */
    set_key_pwr(1);
    GPIO_SetMode(KEY_PWR_PORT, KEY_PWR_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_PWR_PORT, KEY_PWR_PIN, GPIO_INT_FALLING);
    
    /* Configure the key down pin */
    set_key_down(1);
    GPIO_SetMode(KEY_DOWN_PORT, KEY_DOWN_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_DOWN_PORT, KEY_DOWN_PIN, GPIO_INT_FALLING);
    
    /* Configure the key up pin */
    set_key_up(1);
    GPIO_SetMode(KEY_UP_PORT, KEY_UP_BIT, GPIO_PMD_QUASI);
    GPIO_EnableInt(KEY_UP_PORT, KEY_UP_PIN, GPIO_INT_FALLING);
    
    /* Enable interrupt*/
    NVIC_EnableIRQ(EINT1_IRQn); // key switch
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn); // key power, key down, key up

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 * 10 KHz clock */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(KEY_SW_PORT, KEY_SW_BIT);
    GPIO_ENABLE_DEBOUNCE(KEY_PWR_PORT, KEY_PWR_BIT);
    GPIO_ENABLE_DEBOUNCE(KEY_DOWN_PORT, KEY_DOWN_BIT);
    GPIO_ENABLE_DEBOUNCE(KEY_UP_PORT, KEY_UP_BIT);
    
    return 0;
}

/**
 * @brief       Port0/Port1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Port0/Port1 default IRQ, declared in startup_M051Series.s.
 */
void GPIOP0P1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* To check if P1.3 interrupt occurred */
    if(GPIO_GET_INT_FLAG(P1, BIT3))
    {
        GPIO_CLR_INT_FLAG(P1, BIT3);
        //printf("P1.3 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT0, PORT1 interrupts */
        P0->ISRC = P0->ISRC;
        P1->ISRC = P1->ISRC;
        RT_DEBUG_LOG(DEBUG_KEY, ("Un-expected PORT0, PORT1 interrupts.\n"));
    }
    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * @brief       Port2/Port3/Port4 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Port2/Port3/Port4 default IRQ, declared in startup_M051Series.s.
 */
void GPIOP2P3P4_IRQHandler(void)
{
    register int cnts, i;
    /* enter interrupt */
    rt_interrupt_enter();


    if(GPIO_GET_INT_FLAG(KEY_PWR_PORT, KEY_PWR_BIT))
    {
        cnts = 0;
        for (i = 0; i < KEY_VALID_TIMEOUT; i++) {
            if (get_key_pwr() == KEY_VALID_VALUE) {
                ++cnts;
            } else {
                --cnts;
            }
            if (cnts > KEY_VALID_TIME)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key power...\n"));
                break;
            }
            if (cnts < -KEY_VALID_TIME)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_PWR_PORT, KEY_PWR_BIT);
    }
    else if(GPIO_GET_INT_FLAG(KEY_DOWN_PORT, KEY_DOWN_BIT))
    {
        cnts = 0;
        for (i = 0; i < KEY_VALID_TIMEOUT; i++) {
            if (get_key_down() == KEY_VALID_VALUE) {
                ++cnts;
            } else {
                --cnts;
            }
            if (cnts > KEY_VALID_TIME)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key down...\n"));
                break;
            }
            if (cnts < -KEY_VALID_TIME)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_DOWN_PORT, KEY_DOWN_BIT);
    }
    else if(GPIO_GET_INT_FLAG(KEY_UP_PORT, KEY_UP_BIT))
    {
        cnts = 0;
        for (i = 0; i < KEY_VALID_TIMEOUT; i++) {
            if (get_key_up() == KEY_VALID_VALUE) {
                ++cnts;
            } else {
                --cnts;
            }
            if (cnts > KEY_VALID_TIME)
            {
                RT_DEBUG_LOG(DEBUG_KEY, ("it is key up...\n"));
                break;
            }
            if (cnts < -KEY_VALID_TIME)
            {
                break;
            }
        }
        GPIO_CLR_INT_FLAG(KEY_UP_PORT, KEY_UP_BIT);
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT2, PORT3 and PORT4 interrupts */
        P2->ISRC = P2->ISRC;
        P3->ISRC = P3->ISRC;
        P4->ISRC = P4->ISRC;
        RT_DEBUG_LOG(DEBUG_KEY, ("Un-expected PORT2, PORT3 and PORT4 interrupts.\n"));
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

void EINT1_IRQHandler(void)
{
    register int cnts, i;
    /* enter interrupt */
    rt_interrupt_enter();

    cnts = 0;
    for (i = 0; i < KEY_VALID_TIMEOUT; i++) {
        if (get_key_sw() == KEY_VALID_VALUE) {
            ++cnts;
        } else {
            --cnts;
        }
        if (cnts > KEY_VALID_TIME)
        {
            RT_DEBUG_LOG(DEBUG_KEY, ("it is key switch...\n"));
            break;
        }
        if (cnts < -KEY_VALID_TIME)
        {
            break;
        }
    }
    GPIO_CLR_INT_FLAG(KEY_SW_PORT, KEY_SW_BIT);

    /* leave interrupt */
    rt_interrupt_leave();
}


/* Initial components for device */
INIT_DEVICE_EXPORT(rt_hw_key_init);
