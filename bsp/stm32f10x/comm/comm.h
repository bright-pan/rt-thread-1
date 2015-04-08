/********************************************************************
 * Filename:			comm.h
 *
 * Description:
 *
 * Author:              Bright Pan
 * Email:				bright_pan@yuettak.com
 * Date:				2014-03-14
 *
 * Modify:
 *
 * Copyright (C) 2014 Yuettak Co.,Ltd
 ********************************************************************/
#ifndef _COMM_H_
#define _COMM_H_

#include <rthw.h>
#include <rtthread.h>
#include <stm32f10x.h>

#define COMM_DEBUG 0

#define DEVICE_NAME_COMM "uart2"

int32_t
send_ctx_mail(int32_t comm_type, int32_t order, int32_t delay, void *buf, int32_t len);

/*
    ����·�㷢�����ݰ���
    buf:���ݵ�ַ
    size�����ݴ�С
    delay��д����ʱ����ʱ���ء�
    ���أ�>= 0(CW_STATUS_OK);���ͳɹ���
          -CW_STATUS_TIME_OUT;��ʱ
          -CW_STATUS_ERROR;ʧ��
*/
__STATIC_INLINE int32_t
comm_write(void *buf, int32_t size, uint16_t delay);
/*
    ����·���ȡ���ݰ���
    buf:���ݴ�ſռ��ַ
    size�����ݴ�ſռ��С
    delay����ȡ��ʱ����ʱ���ء�
    ���أ�>= 0(RT_EOK);���ͳɹ���
          -RT_ETIMEOUT;��ʱ
          -RT_ERROR;ʧ��
*/
rt_err_t 
comm_read(void *buf, int32_t size, int32_t delay);

#endif /* _COMM_H_ */
