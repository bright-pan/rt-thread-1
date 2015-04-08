/*********************************************************************
 * Filename:			comm_window.c
 *
 * Description:
 *
 * Author:              Bright Pan
 * Email:				bright_pan@yuettak.com
 * Date:				2014-03-17
 *
 * Modify:
 *
 * Copyright (C) 2014 Yuettak Co.,Ltd
 ********************************************************************/

#include "comm_window.h"
#include "comm.h"

#define COMM_TWINDOW_SIZE 5
#define CTW_TIMER_TIMEOUT_TICKS 10
#define CTW_TIMER_SEND_DELAY 5
#define CTW_TIMER_RESEND_COUNTS 5

#define COMM_RWINDOW_SIZE 5
#define CRW_TIMER_TIMEOUT_TICKS 10

struct comm_twindow_list comm_twindow_list;
struct comm_rwindow_list comm_rwindow_list;

void
send_frame(rt_device_t device, struct comm_tmail *mail, uint8_t order);

__STATIC_INLINE void *
print_hex(void *buf, uint16_t length)
{
	uint32_t i;
    uint8_t *buf_bk = buf;
	if (buf != RT_NULL) {
        for (i = 0; i < length; i++) {
            rt_kprintf("%02X ", buf_bk[i]);
        }
        rt_kprintf("\n");
    }
    return buf;
}

static void
ctw_timer_out(void *parameters)
{
	struct comm_twindow_node *tmp;
	struct comm_twindow_list *ctw_list = (struct comm_twindow_list *)parameters;
	struct list_head *pos, *q;

	rt_mutex_take(ctw_list->mutex, RT_WAITING_FOREVER);
	list_for_each_safe(pos, q, &ctw_list->list)
	{
		tmp= list_entry(pos, struct comm_twindow_node, list);
        if (tmp->data.cnts++ > tmp->data.delay)
        {
            RT_DEBUG_LOG(COMM_DEBUG,("resend request frame %d \ncmd: 0x%02X, order: 0x%02X, length: %d\n",
                       tmp->data.r_cnts + 1,(tmp->data.mail).comm_type, tmp->data.order, (tmp->data.mail).len);
            print_hex((tmp->data.mail).buf, (tmp->data.mail).len));
            send_frame(ctw_list->device, &((tmp->data).mail), tmp->data.order);

            tmp->data.cnts = 0;
            tmp->data.r_cnts++;
        }
        if (tmp->data.r_cnts > CTW_TIMER_RESEND_COUNTS)
        {
            RT_DEBUG_LOG(COMM_DEBUG,("send failure and delete cw node\ncomm_type: 0x%02X, order: 0x%02X, length: %d\n",
                       (tmp->data.mail).comm_type, tmp->data.order, (tmp->data.mail).len);
            print_hex((tmp->data.mail).buf, (tmp->data.mail).len));
            *((tmp->data.mail).result) = -CW_STATUS_TIME_OUT;
            rt_sem_release((tmp->data.mail).result_sem);
            ///*
            rt_free((tmp->data.mail).buf);
            (tmp->data.mail).buf = RT_NULL;
            //*/
            list_del(pos);
            ctw_list->size--;
            rt_free(tmp);
        }
    }
	rt_mutex_release(ctw_list->mutex);
}

int32_t
ctw_list_init(struct comm_twindow_list *ctw_list)
{
    rt_device_t device_comm = rt_device_find(DEVICE_NAME_COMM);
    if (device_comm != RT_NULL) {
        if (device_comm->open_flag == RT_DEVICE_OFLAG_CLOSE)
            rt_device_open(device_comm, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    else {
        RT_DEBUG_LOG(COMM_DEBUG,("the device %s is not found!\n", DEVICE_NAME_COMM));
    }
    
	INIT_LIST_HEAD(&ctw_list->list);
	ctw_list->size = 0;
	ctw_list->timer = rt_timer_create("t_ctw", ctw_timer_out, ctw_list, CTW_TIMER_TIMEOUT_TICKS, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_PERIODIC);
	ctw_list->device = device_comm;
	ctw_list->mutex = rt_mutex_create("m_ctw", RT_IPC_FLAG_FIFO);
	if ((ctw_list->timer == RT_NULL) ||
		(ctw_list->device == RT_NULL) ||
		(ctw_list->mutex ==RT_NULL))
	{
		if (ctw_list->timer != RT_NULL)
		{
			rt_timer_delete(ctw_list->timer);
		}
		if (ctw_list->mutex != RT_NULL)
		{
			rt_mutex_delete(ctw_list->mutex);
		}

		return -CW_STATUS_INIT_ERROR;
	}
	rt_timer_start(ctw_list->timer);
	return CW_STATUS_OK;
}

int32_t
ctw_list_new(struct comm_twindow_node **node, struct comm_twindow_list *ctw_list, struct comm_twindow_node_data *data)
{
	if (ctw_list->size <= COMM_TWINDOW_SIZE)
	{
		*node = rt_malloc(sizeof(**node));
		if (*node != RT_NULL)
		{
            if (!data->delay)
                data->delay = CTW_TIMER_SEND_DELAY;
            (*node)->data = *data;
            RT_DEBUG_LOG(COMM_DEBUG,("add cmd: 0x%02X, order: 0x%02X, length: %d\n",
                       ((*node)->data.mail).comm_type, (*node)->data.order, ((*node)->data.mail).len));

			list_add(&(*node)->list, &ctw_list->list);
			ctw_list->size++;
			rt_mutex_release(ctw_list->mutex);

		}
		else
			return -CW_STATUS_NEW_ERROR;
	}
	else
		return -CW_STATUS_FULL;

	return CW_STATUS_OK;
}

static void
crw_timer_out(void *parameters)
{
    rt_err_t error;
	struct comm_rwindow_node *tmp;
	struct comm_rwindow_list *crw_list = (struct comm_rwindow_list *)parameters;
	struct list_head *pos, *q;

	rt_mutex_take(crw_list->mutex, RT_WAITING_FOREVER);
	list_for_each_safe(pos, q, &crw_list->list)
	{
		tmp= list_entry(pos, struct comm_rwindow_node, list);
		if (tmp->data.sem != RT_NULL)
		{
			error = rt_sem_take(tmp->data.sem, RT_WAITING_NO);
			if (error == RT_EOK) {
				send_ctx_mail(tmp->data.comm_type, tmp->data.order, 0, (uint8_t *)(tmp->data.result), 1);
                
                if (tmp->data.buf != RT_NULL) {
                    rt_free(tmp->data.buf);
                    tmp->data.buf = RT_NULL;
                }
                if (tmp->data.result != RT_NULL) {
                    rt_free(tmp->data.result);
                    tmp->data.result = RT_NULL;
                }
                if (tmp->data.sem != RT_NULL)
                    rt_sem_delete(tmp->data.sem);
                
				list_del(pos);
				crw_list->size--;
				rt_free(tmp);
			}
		}
		else
		{
			list_del(pos);
			crw_list->size--;
			rt_free(tmp);
		}
    }
	rt_mutex_release(crw_list->mutex);
}

int32_t
crw_list_init(struct comm_rwindow_list *crw_list)
{
	INIT_LIST_HEAD(&crw_list->list);
	crw_list->size = 0;
	crw_list->timer = rt_timer_create("t_crw", crw_timer_out, crw_list, CRW_TIMER_TIMEOUT_TICKS, RT_TIMER_FLAG_SOFT_TIMER|RT_TIMER_FLAG_PERIODIC);
	crw_list->mutex = rt_mutex_create("m_crw", RT_IPC_FLAG_FIFO);
	if ((crw_list->timer == RT_NULL) ||
		(crw_list->mutex ==RT_NULL))
	{
		if (crw_list->timer != RT_NULL)
		{
			rt_timer_delete(crw_list->timer);
		}
		if (crw_list->mutex != RT_NULL)
		{
			rt_mutex_delete(crw_list->mutex);
		}

		return -CW_STATUS_INIT_ERROR;
	}
	rt_timer_start(crw_list->timer);
	return CW_STATUS_OK;
}

int32_t
crw_list_new(struct comm_rwindow_node **node, struct comm_rwindow_list *crw_list, struct comm_rwindow_node_data *data)
{
	if (crw_list->size <= COMM_RWINDOW_SIZE)
	{
		*node = rt_malloc(sizeof(**node));
		if (*node != RT_NULL)
		{
            (*node)->data = *data;
			rt_mutex_take(crw_list->mutex, RT_WAITING_FOREVER);
			list_add(&(*node)->list, &crw_list->list);
			crw_list->size++;
			rt_mutex_release(crw_list->mutex);

		}
		else
			return -CW_STATUS_NEW_ERROR;
	}
	else
		return -CW_STATUS_FULL;

	return CW_STATUS_OK;
}
