/*********************************************************************
 * Filename:			comm_window.h
 *
 * Description:
 *
 * Author:				Bright Pan
 * Email:				bright_pan@yuettak.com
 * Date:				2014-03-17
 *
 * Modify:
 *
 * Copyright (C) 2014 Yuettak Co.,Ltd
 ********************************************************************/
#ifndef _COMM_WINDOW_H_
#define _COMM_WINDOW_H_

#include <rthw.h>
#include <rtthread.h>
#include <stm32f10x.h>

//#include "board.h"
#include "list.h"

#define CTW_FLAG_REQUEST 0
#define CTW_FLAG_RESPONSE 1

#define	CW_STATUS_OK            0
#define	CW_STATUS_ERROR         1
#define	CW_STATUS_FULL          2
#define	CW_STATUS_INIT_ERROR    3
#define	CW_STATUS_NEW_ERROR     4
#define	CW_STATUS_TIME_OUT      5

struct comm_tmail {

	rt_sem_t result_sem;
	int32_t *result;
	int32_t comm_type;
	int32_t order;
	int32_t delay;
	uint8_t *buf;
	int32_t len;

};

struct comm_rmail {

	uint8_t *buf;
	int32_t len;

};

struct comm_twindow_node_data{

	struct comm_tmail mail;
	int32_t order;
	int32_t r_cnts;// resend counts
	int32_t cnts;// counts for timer
	int32_t delay;
    
};

struct comm_twindow_node{

	struct list_head list;
    struct comm_twindow_node_data data;
    
};

struct comm_twindow_list{

	struct list_head list;
	rt_mutex_t mutex;
	rt_timer_t timer;
	rt_device_t device;
	int32_t size;

};

struct comm_rwindow_node_data{

    rt_sem_t sem;
    int32_t *result;
    int32_t comm_type;
	int32_t order;
    uint8_t *buf;

};

struct comm_rwindow_node{

	struct list_head list;
    struct comm_rwindow_node_data data;

};

struct comm_rwindow_list{

	struct list_head list;
	rt_mutex_t mutex;
	rt_timer_t timer;
	int32_t size;

};

extern struct comm_twindow_list comm_twindow_list;
extern struct comm_rwindow_list comm_rwindow_list;

int32_t
ctw_list_init(struct comm_twindow_list *ctw_list);

int32_t
ctw_list_new(struct comm_twindow_node **node, struct comm_twindow_list *ctw_list, struct comm_twindow_node_data *data);

int32_t
crw_list_init(struct comm_rwindow_list *crw_list);

int32_t
crw_list_new(struct comm_rwindow_node **node, struct comm_rwindow_list *crw_list, struct comm_rwindow_node_data *data);

#endif /* _COMM_WINDOW_H_ */
