/*********************************************************************
 * Filename:            comm.c
 *
 * Description:
 *
 * Author:                Bright Pan
 * Email:                bright_pan@yuettak.com
 * Date:                2014-03-14
 *
 * Modify:
 *
 * Copyright (C) 2014 Yuettak Co.,Ltd
 ********************************************************************/

#include <rtthread.h>
#include "comm.h"
#include "comm_window.h"

#define COMM_MAIL_MAX_MSGS 5

#define COMM_TYPE_CONNECT   0
#define COMM_TYPE_DATA      1

#define BUF_SIZE 768

rt_mq_t comm_tx_mq = RT_NULL;
rt_mq_t comm_rx_mq = RT_NULL;
//rt_mutex_t comm_tx_mutex = RT_NULL;
//rt_device_t *comm_pipe = RT_NULL;

#define FRAME_STATUS_INVALID    0
#define FRAME_STATUS_VALID      1
#define FRAME_STATUS_OK         2

void
send_frame(rt_device_t device, struct comm_tmail *mail, int32_t order);

rt_err_t
send_crx_mail(void *buf, int32_t size);

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

int32_t
check_frame(uint8_t *str)
{
    static int32_t flag;

    if (*str == '\r')
        flag = FRAME_STATUS_VALID;
	else if (*str == '\n') {
        if (flag == FRAME_STATUS_VALID)
            flag = FRAME_STATUS_OK;
		else
            flag = FRAME_STATUS_INVALID;
    }
    else
        flag = FRAME_STATUS_INVALID;

    return flag;
}

__STATIC_INLINE int32_t
process_response(int32_t cmd, void *frame_data, int32_t length)
{
    int32_t result = -CW_STATUS_ERROR;
    uint8_t *frame_data_bk = frame_data;
    switch (cmd)// process response
    {
        case COMM_TYPE_CONNECT:
        case COMM_TYPE_DATA:
            {
                result = *frame_data_bk;
                break;
            }
        default :
            {
                RT_DEBUG_LOG(COMM_DEBUG,("the response cmd %02x is invalid!\n", cmd));
                break;
            }
    }

    return result;
}

__STATIC_INLINE int32_t
process_request(int32_t cmd, int32_t order, void *frame_data, int32_t length)
{
    int32_t result = -CW_STATUS_ERROR;
    uint8_t rep_cmd = cmd | 0x80;

    switch (cmd)// process request
    {
        case COMM_TYPE_CONNECT:
            {
                result = CW_STATUS_OK;
                //send_crx_mail(frame_data, length);
                send_ctx_mail(rep_cmd, order, 0, &result, 1);
                break;
            }
        case COMM_TYPE_DATA:
            {
                result = CW_STATUS_OK;
                send_crx_mail(frame_data, length);
                send_ctx_mail(rep_cmd, order, 0, &result, 1);
                break;
            }
        default :
            {
                RT_DEBUG_LOG(COMM_DEBUG,("the request cmd %02x is invalid!\n", cmd));
                break;
            }
    }

    return result;
}

static int32_t
process_frame(void *frame, int32_t frame_size)
{
    uint8_t *frame_bk = frame;
    int32_t cmd, order;
    int32_t length;
    int32_t result = 0;
    struct comm_twindow_node *tmp;
    struct comm_twindow_list *ctw_list_bk = &comm_twindow_list;
    struct list_head *pos, *q;
    RT_ASSERT(frame_bk!=RT_NULL);
    RT_ASSERT(frame_size<=BUF_SIZE);
    RT_ASSERT(ctw_list_bk!=RT_NULL);

    cmd = *(frame_bk + 2);
    order = *(frame_bk + 3);
    length = frame_size - 6;

    frame_bk += 4;

    if (cmd & 0x80)
    {
        RT_DEBUG_LOG(COMM_DEBUG,("recv response frame \ncmd: 0x%02X, order: 0x%02X, length: %d\n", cmd, order, length);
        print_hex(frame_bk, length));
        cmd &= 0x7f;
        rt_mutex_take(ctw_list_bk->mutex, RT_WAITING_FOREVER);
        list_for_each_safe(pos, q, &ctw_list_bk->list)
        {
            tmp= list_entry(pos, struct comm_twindow_node, list);
            if ((tmp->data.mail).comm_type == cmd &&
                tmp->data.order == order)
            {
                RT_DEBUG_LOG(COMM_DEBUG,("recv response and delete cw node\n"));
                *((tmp->data.mail).result) = process_response(cmd, frame_bk, length);
                rt_sem_release((tmp->data.mail).result_sem);
                /*
                rt_free((tmp->data.mail).buf);
                (tmp->data.mail).buf = RT_NULL;
                */
                list_del(pos);
                ctw_list_bk->size--;
                rt_free(tmp);
            }
        }
        rt_mutex_release(ctw_list_bk->mutex);
    }
    else
    {
        RT_DEBUG_LOG(COMM_DEBUG,("recv request frame \ncmd: 0x%02X, order: 0x%02X, length: %d\n", cmd, order, length);
        print_hex(frame_bk, length));
        process_request(cmd, order, frame_bk, length);
    }
    return result;
}


static void
comm_rx_thread_entry(void *parameters)
{
    int32_t length;
    int32_t recv_counts;
    int32_t flag = 0;
    uint8_t *process_buf_bk;
    uint8_t process_buf[BUF_SIZE];

    rt_device_t device_comm = rt_device_find(DEVICE_NAME_COMM);
    if (device_comm != RT_NULL) {
        if (device_comm->open_flag == RT_DEVICE_OFLAG_CLOSE)
            rt_device_open(device_comm, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    else {
        RT_DEBUG_LOG(COMM_DEBUG,("the device %s is not found!\n", DEVICE_NAME_COMM));
    }

    RT_ASSERT(crw_list_init(&comm_rwindow_list) == CW_STATUS_OK);
    while (1) {
        process_buf_bk = process_buf;
        recv_counts = 0;
        while (1)
        {
            if (rt_device_read(device_comm, 0, process_buf_bk, 1))
            {
                flag = 0;
                recv_counts++;
                if (check_frame(process_buf_bk) == FRAME_STATUS_OK)
                {
                    length = *((uint16_t *)process_buf);
                    if (length > BUF_SIZE -4 || length + 4 < recv_counts) {
                        RT_DEBUG_LOG(COMM_DEBUG,("\ncomm recv error frame length: %d\n", recv_counts);
                        print_hex(process_buf, recv_counts));
                        break;
                    }
                    if (length + 4 > recv_counts)
                        goto continue_check;
                    if (length + 4 == recv_counts)
                    {
                        RT_DEBUG_LOG(COMM_DEBUG,("\ncomm recv frame length: %d\n", recv_counts);
                        print_hex(process_buf, recv_counts));
                        process_frame(process_buf, recv_counts);
                        break;
                    }
                }
          continue_check:
                if (recv_counts >= BUF_SIZE) {
                    RT_DEBUG_LOG(COMM_DEBUG,("\ncomm recv error frame length: %d\n", recv_counts);
                    print_hex(process_buf, recv_counts));
                    break;
                }
                process_buf_bk++;
            }
            else
            {
                if (flag++ < 10)
                {
                    rt_thread_delay(1);
                }
                else
                {
                    flag = 0;
                    break;
                }
            }
        }
    }
}

static void
comm_tx_thread_entry(void *parameters)
{
    rt_err_t result;
    struct comm_tmail comm_tmail_buf;
    struct comm_twindow_node_data data;
    int32_t cw_status;
    struct comm_twindow_node *ctw_node;
    int32_t order = 0;
    rt_device_t device_comm = rt_device_find(DEVICE_NAME_COMM);
    if (device_comm != RT_NULL) {
        if (device_comm->open_flag == RT_DEVICE_OFLAG_CLOSE)
            rt_device_open(device_comm, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
    else {
        RT_DEBUG_LOG(COMM_DEBUG,("the device %s is not found!\n", DEVICE_NAME_COMM));
    }

    RT_ASSERT(device_comm != RT_NULL);
    RT_ASSERT(ctw_list_init(&comm_twindow_list) == CW_STATUS_OK);

    while (1)
    {
        // receive mail
        rt_memset(&comm_tmail_buf, 0, sizeof(comm_tmail_buf));
        result = rt_mq_recv(comm_tx_mq, &comm_tmail_buf,
                            sizeof(comm_tmail_buf),
                            100);
        if (result == RT_EOK)
        {
            // process mail
            //RT_ASSERT(comm_tmail_buf.result_sem != RT_NULL);
            RT_ASSERT(comm_tmail_buf.result != RT_NULL);
            //RT_ASSERT(comm_tmail_buf.buf != RT_NULL);
            if (comm_tmail_buf.comm_type & 0x80) {
                if (comm_tmail_buf.buf != RT_NULL) {
                    send_frame(device_comm, &comm_tmail_buf, comm_tmail_buf.order);
                    RT_DEBUG_LOG(COMM_DEBUG,("send response frame and delete cw node\n cmd: 0x%02X, order: 0x%02X, length: %d\n",
                               comm_tmail_buf.comm_type, comm_tmail_buf.order, comm_tmail_buf.len);
                    print_hex(comm_tmail_buf.buf, comm_tmail_buf.len));
                    /*
                    if (comm_tmail_buf.buf != RT_NULL) {
                        rt_free(comm_tmail_buf.buf);
                        comm_tmail_buf.buf = RT_NULL;
                    }
                    */
                }


            } else {
                rt_memset(&data, 0, sizeof(data));
                data.mail = comm_tmail_buf;
                if (comm_tmail_buf.order)
                {
                    data.order = comm_tmail_buf.order;
                }
                else
                {
                    if (order++)
                        data.order = order++;
                    else
                        data.order = 1;
                }
                data.delay = comm_tmail_buf.delay;

                cw_status = ctw_list_new(&ctw_node, &comm_twindow_list, &data);
                if (cw_status == CW_STATUS_OK)
                {
                    send_frame(device_comm, &comm_tmail_buf, data.order);
                    RT_DEBUG_LOG(COMM_DEBUG,("send request frame\n cmd: 0x%02X, order: 0x%02X, length: %d\n",
                               comm_tmail_buf.comm_type, data.order, comm_tmail_buf.len);
                    print_hex(comm_tmail_buf.buf, comm_tmail_buf.len));
                }
                else
                {
                    /* tell error for mail sender */
                    if (!(comm_tmail_buf.comm_type & 0x80)) {
                        *comm_tmail_buf.result = cw_status;
                        rt_sem_release(comm_tmail_buf.result_sem);
                    }
                    /*
                    if (comm_tmail_buf.buf != RT_NULL) {
                        rt_free(comm_tmail_buf.buf);
                        comm_tmail_buf.buf = RT_NULL;
                    }
                    */

                }
            }

        }
        else // time out
        {

        }
    }
}

void
send_frame(rt_device_t device, struct comm_tmail *mail, int32_t order)
{
    uint16_t length;
    //struct rt_serial_device *dev = (struct rt_serial_device *)device;
    //struct stm32_uart *uart = dev->parent.user_data;
    //rt_mutex_take(uart->lock, RT_WAITING_FOREVER);
    // send length data
    length = mail->len + 2;
    rt_device_write(device, 0, (uint8_t *)&length, 2);
    // send comm_type data
    rt_device_write(device, 0, (uint8_t *)&mail->comm_type, 1);// comm_type
    rt_device_write(device, 0, &order, 1);// order
    // send buf data
    rt_device_write(device, 0, mail->buf, mail->len);
    // send "\r\n"
    rt_device_write(device, 0, "\r\n", 2);
    // free mail buf memory

    //rt_mutex_release(uart->lock);
}

int32_t
send_ctx_mail(int32_t comm_type, int32_t order, int32_t delay, void *buf, int32_t len)
{
    rt_err_t result = -RT_EFULL;
    uint8_t *buf_bk = RT_NULL;
    struct comm_tmail comm_tmail_buf;
    int32_t cw_status = -CW_STATUS_ERROR;

    rt_memset(&comm_tmail_buf, 0, sizeof(comm_tmail_buf));
    comm_tmail_buf.result_sem = RT_NULL;
    if (comm_tx_mq != RT_NULL)
    {
        if (len) {
            buf_bk = (uint8_t *)rt_malloc(len);
            if (buf_bk == RT_NULL)
                goto __free_process;
            rt_memcpy(buf_bk, buf, len);
        }
        else
            buf_bk = RT_NULL;

        if (comm_type & 0x80)
            comm_tmail_buf.result_sem = RT_NULL;
        else {
            comm_tmail_buf.result_sem = rt_sem_create("s_ctx", 0, RT_IPC_FLAG_FIFO);
            if (comm_tmail_buf.result_sem == RT_NULL)
                goto __free_process;
        }
        comm_tmail_buf.result = &cw_status;
        comm_tmail_buf.comm_type = comm_type;
        comm_tmail_buf.order = order;
        comm_tmail_buf.buf = buf_bk;
        comm_tmail_buf.len = len;
        comm_tmail_buf.delay = delay;

        if (!(comm_type & 0x80))
            result = rt_mq_send(comm_tx_mq, &comm_tmail_buf, sizeof(comm_tmail_buf));
        else
            result = rt_mq_urgent(comm_tx_mq, &comm_tmail_buf, sizeof(comm_tmail_buf));

        if (result == -RT_EFULL)
        {
            RT_DEBUG_LOG(COMM_DEBUG,("comm_mq is full!!!\n"));
            goto __free_process;
        }
        else
        {
            if (!(comm_type & 0x80)) {
                rt_sem_take(comm_tmail_buf.result_sem, RT_WAITING_FOREVER);
                RT_DEBUG_LOG(COMM_DEBUG,("send result is %d\n", *comm_tmail_buf.result));
            }
        }
    }
    else
    {
        RT_DEBUG_LOG(COMM_DEBUG,("comm_mq is RT_NULL!!!!\n"));
    }

__free_process:
    if (buf_bk != RT_NULL)
        rt_free(buf_bk);
    if (comm_tmail_buf.result_sem != RT_NULL)
        rt_sem_delete(comm_tmail_buf.result_sem);
    return cw_status;
}

__STATIC_INLINE int32_t
comm_write(void *buf, int32_t size, uint16_t delay)
{
    return send_ctx_mail(COMM_TYPE_DATA, 0, delay, buf, size);
}

rt_err_t
send_crx_mail(void *buf, int32_t size)
{
    rt_err_t result = -RT_ENOMEM;
    struct comm_rmail comm_rmail_buf;

    if (size <= BUF_SIZE) {
        comm_rmail_buf.buf = rt_malloc(size);
        rt_memcpy(comm_rmail_buf.buf, buf, size);
        comm_rmail_buf.len = size;
        result = rt_mq_send(comm_rx_mq, &comm_rmail_buf, sizeof(comm_rmail_buf));
        if (result == -RT_EFULL) {
            RT_DEBUG_LOG(COMM_DEBUG,("comm_rx_mq is full, -RT_EFULL\n"));
        }
    } else {
        RT_DEBUG_LOG(COMM_DEBUG,("buf size too big, -RT_ENOMEM!!!\n"));
    }
    return result;
}

rt_err_t
comm_read(void *buf, int32_t size, int32_t delay)
{
    rt_err_t result = -RT_ERROR;
    struct comm_rmail comm_rmail_buf;

    rt_memset(&comm_rmail_buf, 0, sizeof(comm_rmail_buf));
    result = rt_mq_recv(comm_rx_mq, &comm_rmail_buf,
                        sizeof(comm_rmail_buf),
                        delay);
    if (result == RT_EOK)
    {
        if (size > comm_rmail_buf.len)
            size = comm_rmail_buf.len;
        rt_memcpy(buf, comm_rmail_buf.buf, size);
        rt_free(comm_rmail_buf.buf);
    }
    return result;
}
int rt_comm_init(void)
{
    rt_thread_t comm_tx_thread;
    rt_thread_t comm_rx_thread;
    // initial comm tx msg queue
    comm_tx_mq = rt_mq_create("comm_tx", sizeof(struct comm_tmail),
                           COMM_MAIL_MAX_MSGS, RT_IPC_FLAG_FIFO);
    // initial comm rx msg queue
    comm_rx_mq = rt_mq_create("comm_rx", sizeof(struct comm_rmail),
                           COMM_MAIL_MAX_MSGS, RT_IPC_FLAG_FIFO);
    // initial comm tx thread
    comm_tx_thread = rt_thread_create("comm_tx",
                                   comm_tx_thread_entry, RT_NULL,
                                   512, 10, 5);
    if (comm_tx_thread != RT_NULL)
    {
        rt_thread_startup(comm_tx_thread);
    }
    // initial comm rx thread
    comm_rx_thread = rt_thread_create("comm_rx",
                                      comm_rx_thread_entry, RT_NULL,
                                      1536, 9, 5);
    if (comm_rx_thread != RT_NULL)
    {
        rt_thread_startup(comm_rx_thread);
    }
    return 0;
}
INIT_APP_EXPORT(rt_comm_init);


#ifdef RT_USING_FINSH
#include <finsh.h>
void comm_read_test(void)
{
    uint8_t buf[10];
    comm_read(buf, 10, 50);
    print_hex(buf, 10);
}


FINSH_FUNCTION_EXPORT(comm_write, comm_write[buf size delay]);
FINSH_FUNCTION_EXPORT(comm_read, comm_read[buf size delay]);
FINSH_FUNCTION_EXPORT(comm_read_test, comm_read_test[void]);

/*
rt_pipe_create("p_comm",RT_PIPE_FLAG_BLOCK_WR|RT_PIPE_FLAG_BLOCK_RD, 200);
void pipe_operate(char *name, char *obuf, int size, char op)
{
    uint8_t buf[10];

    rt_device_t dev = rt_device_find(name);
    if (dev != RT_NULL)
    {
        if (dev->open_flag == RT_DEVICE_OFLAG_CLOSE)
        {
            rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
        }
        if (op)
        {
            rt_device_write(dev, 0, obuf, size);
            print_hex(obuf, size);
        }
        else
        {
            if (size > 10)
                size = 10;
            rt_device_read(dev, 0, buf, size);
            print_hex(buf, size);
        }
    }
}

FINSH_FUNCTION_EXPORT(pipe_operate, pipe_operate[name size op]);
*/
#endif // RT_USING_FINSH
