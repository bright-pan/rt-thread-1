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
    向链路层发送数据包。
    buf:数据地址
    size：数据大小
    delay：写入延时，超时返回。
    返回：>= 0(CW_STATUS_OK);发送成功。
          -CW_STATUS_TIME_OUT;超时
          -CW_STATUS_ERROR;失败
*/
__STATIC_INLINE int32_t
comm_write(void *buf, int32_t size, uint16_t delay);
/*
    从链路层读取数据包。
    buf:数据存放空间地址
    size：数据存放空间大小
    delay：读取延时，超时返回。
    返回：>= 0(RT_EOK);发送成功。
          -RT_ETIMEOUT;超时
          -RT_ERROR;失败
*/
rt_err_t 
comm_read(void *buf, int32_t size, int32_t delay);

#endif /* _COMM_H_ */
