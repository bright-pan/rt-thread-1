/*********************************************************************
 * Filename:			fprint.c
 *
 * Description:
 *
 * Author:				Bright Pan
 * Email:				bright_pan@yuettak.com
 * Date:				2015-04-08
 *
 * Modify:
 *
 * Copyright (C) 2015 Yuettak Co.,Ltd
 ********************************************************************/

#include "stm32f10x.h"
#include <rtdevice.h>
#include <stdlib.h>
#include <fprint.h>
#define RT_USING_UART2

#define RT_DEVICE_CTRL_CLR_FIFO 0x20
#define RT_SERIAL_CLEAR(serial) {((struct rt_serial_rx_fifo*)serial->serial_rx)->get_index = \
                               ((struct rt_serial_rx_fifo*)serial->serial_rx)->put_index = 0;}

#define FPRINT_DEBUG 1 // fprint debug switch
#define DEVICE_NAME_FPRINT "uart2" // fprint device define
#define FPRINT_MODEL_OFFSET 0 // (0 <= offset <= 999) fprint template id offset
#define FPRINT_MODEL_SIZE (1000 - 1) // (1 <= offset <= 2000) fprint template numbers
#define FPRINT_MODEL_ID_START FPRINT_MODEL_OFFSET // start id for template
#define FPRINT_MODEL_ID_END (FPRINT_MODEL_OFFSET + KEY_NUMBERS - 1) // end id for template

#define FRAME_PARAM_DATA_SIZE 2 // fpirnt for comm frame size

static const u16 fprint_param_data_size_map[] = {
    32, 64, 128, 256,
};

//head define
#define FRAME_START 0xef01
//#define FRAME_ADDR 0x06030024
#define FRAME_ADDR 0xffffffff
#define FRAME_PW 0x00000000

#define FRAME_PID_CMD 0x01
#define FRAME_PID_DATA 0x02
#define FRAME_PID_ACK 0x07
#define FRAME_PID_END 0x08

const static struct {
    int32_t req;
    int32_t rep;
} frame_data_size_map[] = {
    {0,0},
    {0,0},
    {sizeof(FRAME_REQ_IMG2TZ_TYPEDEF), 0},
    {0,0},
    {sizeof(FRAME_REQ_SEARCH_TYPEDEF),sizeof(FRAME_REP_SEARCH_TYPEDEF)},
    {0,0},
    {sizeof(FRAME_REQ_STORECHAR_TYPEDEF),0},
    {sizeof(FRAME_REQ_LOADCHAR_TYPEDEF),0},
    {sizeof(FRAME_REQ_UPCHAR_TYPEDEF),0},
    {sizeof(FRAME_REQ_DOWNCHAR_TYPEDEF),0},
    {0,0},
    {0,0},
    {sizeof(FRAME_REQ_DELETECHAR_TYPEDEF),0},
    {0,0},
    {sizeof(FRAME_REQ_SETSYSPARA_TYPEDEF),0},
    {0,0}, //0x0f
    {0,0},
    {0,0},
    {0,0},
    {sizeof(FRAME_REQ_VRYPWD_TYPEDEF),0},
    {0,0},
    {sizeof(FRAME_REQ_SETADDR_TYPEDEF),0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x1f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x2f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x3f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x4f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {sizeof(FRAME_REQ_AUTOLOGIN_TYPEDEF),0},
    {sizeof(FRAME_REQ_AUTOSEARCH_TYPEDEF),sizeof(FRAME_REP_AUTOSEARCH_TYPEDEF)},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x5f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x6f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x7f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x8f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0x9f
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0xaf
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0xbf
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0xcf
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0xdf
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0xef    
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}, //0xff
};
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

static uint16_t
check_sum(void *frame, int32_t len)
{
	int32_t i;
	uint16_t cs = 0;
    uint8_t *frame_bk = frame;
    
	for (i = 0; i < len; i++) {
		cs += *(frame_bk + i);
	}

	return cs;
}

/*
    reverse copy for src data.
    uint8_t *dst: data destination.
    uint8_t *src: data source.
    uint8_t len:  data length.
*/
__STATIC_INLINE void
reverse(void *dst, void *src, int32_t len)
{
    uint8_t *dst_bk = dst;
    uint8_t *src_bk = src;
    if (len > 0)
        src_bk += len - 1;
    else
        return;

    while (len-- > 0)
    {
        *dst_bk++ = *src_bk--;
    }
}
/*
static void
fprint_reset(void)
{
}
*/

/*
    send fprint frame to device.
    FRAME_HEAD_TYPEDEF *head: frame head.
    FRAME_REQ_DATA_TYPEDEF *req_data: frame request data.
*/
__STATIC_INLINE int32_t
send_frame(struct rt_serial_device *fprint_device, FRAME_HEAD_TYPEDEF *head,
                    FRAME_REQ_TYPEDEF *req)
{
	int32_t error = -FRAME_ERR_ERROR;
	uint16_t length;
    
	if (fprint_device == RT_NULL)
		goto process_error;

    reverse(&length, head->len, sizeof(head->len));
    
	fprint_device->parent.control((rt_device_t)fprint_device, RT_DEVICE_CTRL_CLR_FIFO, RT_NULL);

    // send to device
    fprint_device->parent.write((rt_device_t)fprint_device, 0, head, sizeof(*head));
    fprint_device->parent.write((rt_device_t)fprint_device, 0, req, length - sizeof(req->cs));
    fprint_device->parent.write((rt_device_t)fprint_device, 0, req->cs, sizeof(req->cs));
    RT_DEBUG_LOG(FPRINT_DEBUG, ("\nfprint send :---------------------\n");
        print_hex(head, sizeof(*head));
        print_hex(req, length - sizeof(req->cs));
        print_hex(req->cs, sizeof(req->cs));
        rt_kprintf("------------------------------------\n");
    );
	error = FRAME_ERR_OK;
process_error:
	return error;
}

void
delay_us(uint32_t time)
{
	uint8_t nCount;
	while(time--)
	{
		for(nCount = 6 ; nCount != 0; nCount--);
	}
}


int32_t 
fprint_device_read(struct rt_serial_device *fprint_device, void *buf, int32_t length, int32_t counts)
{
    int32_t i, size;
    int32_t error = -1;
    
    if (buf == RT_NULL || length == 0 || counts == 0)
        goto process_error;

    // receive frame head
	for (i = 0, size = 0; i < counts && size < length; i++) {
        //delay_us(300);
        rt_thread_delay(RT_TICK_PER_SECOND/100);//10ms
		size += fprint_device->parent.read((rt_device_t)fprint_device, 0, buf, length - size);
	}
    if (size >= length)
        error = 0;
process_error:
    //RT_DEBUG_LOG(1, ("counts = %d\n", i));
    return error;
}

/*
    receive frame
    uint8_t cmd: frame command
    FRAME_HEAD_TYPEDEF *head: frame head
    FRAME_REP_DATA_TYPEDEF *rep_data: frame response data
*/
__STATIC_INLINE int32_t
recv_frame(struct rt_serial_device *fprint_device, FRAME_HEAD_TYPEDEF *head, FRAME_REP_TYPEDEF *rep)
{
	int32_t error = -FRAME_ERR_ERROR;
    int32_t result;
	uint16_t cs, length;

	if (fprint_device == RT_NULL)
		goto error_process;
    // receive frame head

    result = fprint_device_read(fprint_device, head, sizeof(*head), 5000);
	if (result < 0 || head->pid != FRAME_PID_ACK) {
        goto error_process;
    }

    reverse(&length, head->len, sizeof(head->len));
    // receice from device
    result = fprint_device_read(fprint_device, rep, length - sizeof(rep->cs), 5000);
	if (result < 0)
		goto error_process;
    result = fprint_device_read(fprint_device, rep->cs, sizeof(rep->cs), 5000);
	if (result < 0)
		goto error_process;
    // verify check sum
    reverse(&cs, rep->cs, sizeof(rep->cs));
    if (cs == (uint16_t)(check_sum((uint8_t *)head+6, sizeof(*head)-6) + check_sum(rep, length - sizeof(rep->cs))))
        error = FRAME_ERR_OK;

    RT_DEBUG_LOG(FPRINT_DEBUG, ("\nfprint recv :---------------------\n");
        print_hex(head, sizeof(*head));
        print_hex(rep, length - sizeof(rep->cs));
        print_hex(rep->cs, sizeof(rep->cs));
        rt_kprintf("------------------------------------\n"));

error_process:

	return error;
}
// struct member offset
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/*
    receive frame data.
    uint8_t *buf: receive frame data.
    uint16_t size: receive frame length
*/
__STATIC_INLINE uint16_t
recv_frame_data(struct rt_serial_device *fprint_device, uint8_t *buf, uint16_t size)
{
	int32_t error = -FRAME_ERR_ERROR;
    int32_t result;
	uint16_t cs, check;
	uint16_t length, offset;
    FRAME_HEAD_TYPEDEF head;

    cs = check = 0;
    length = offset = 0;
    
	if (fprint_device == RT_NULL)
		goto error_process;
    
    // frame head process
    while (size > 0)
    {
        result = fprint_device_read(fprint_device, &head, sizeof(head), 5000);
        if (result < 0)
            goto error_process;
        reverse(&length, head.len, sizeof(head.len));
        
        length -= 2;

        if (length > size)
            goto error_process;

        switch (head.pid)
        {
            case FRAME_PID_DATA:
                {
                    size -= length;
                    break;
                }
            case FRAME_PID_END:
                {
                    size = 0;
                    break;
                }

            default :
                {
                    size = 0;
                    break;
                }
        }
        // receice from device
        result = fprint_device_read(fprint_device, buf + offset, length, 5000);
        if (result < 0)
            goto error_process;
        result = fprint_device_read(fprint_device, &check, 2, 5000);
        if (result < 0)
            goto error_process;
        // verify check sum
        cs = 0;
        cs = check_sum((uint8_t *)&head + 6, sizeof(head) - 6) + check_sum(buf + offset, length);

        RT_DEBUG_LOG(FPRINT_DEBUG, ("\nfprint recv :---------------------\n");
            print_hex((uint8_t *)&head, sizeof(head));
            print_hex((uint8_t *)buf+offset, length);
            print_hex((uint8_t *)&check, sizeof(check));
            rt_kprintf("------------------------------------\n"));
        //reverse((uint8_t *)&check, (uint8_t *)&check, sizeof(check));
        __REV16(check);
        if (cs == check)
            error = FRAME_ERR_OK;

        offset += length;
    }
error_process:

	return error;

}

/*
    send frame extra data.
    void *buffer: data adress.
    uint16_t size: data length.
*/
__STATIC_INLINE uint16_t
send_frame_data(struct rt_serial_device *fprint_device, void *buffer, uint16_t size)
{
	uint16_t cs;
    u8 *buf;
	uint16_t length, offset, frame_data_size;
    FRAME_HEAD_TYPEDEF head;
    uint16_t start = FRAME_START;
    uint32_t addr = FRAME_ADDR;
    
    length = 0;
    offset = 0;
    frame_data_size = fprint_param_data_size_map[FRAME_PARAM_DATA_SIZE];
    buf = buffer;
    
	if (fprint_device == RT_NULL)
		goto error_process;
    while (size > 0)
    {
        rt_memset(&head, 0, sizeof(head));

        reverse(head.start, (uint8_t *)&start, sizeof(start));
        reverse(head.addr, (uint8_t *)&addr, sizeof(addr));

        if (size > frame_data_size) {
            length = frame_data_size;
            head.pid = FRAME_PID_DATA;
        } else {
            length = size;
            head.pid = FRAME_PID_END;
        }
        size -= length;

        //reverse((uint8_t *)&length, head.len, sizeof(length));


        length += 2;
        reverse(head.len, &length, sizeof(length));
        length -= 2;
        cs = check_sum((uint8_t *)&head+6, sizeof(head)-6) + check_sum(buf + offset, length);
        fprint_device->parent.write((rt_device_t)fprint_device, 0, &head, sizeof(head));
        fprint_device->parent.write((rt_device_t)fprint_device, 0, buf + offset, length);
        cs = __REV16(cs);
        fprint_device->parent.write((rt_device_t)fprint_device, 0, &cs, 2);
        RT_DEBUG_LOG(FPRINT_DEBUG, ("\nfprint send :---------------------\n");
            print_hex(&head, sizeof(head));
            print_hex(buf + offset, length);
            print_hex(&cs, sizeof(cs));
            rt_kprintf("------------------------------------\n"));
        offset += length;
    }
error_process:

	return offset;

}
/*
    frame process for send request frame and receive response frame.
    uint8_t cmd: frame cmd 
    FRAME_REQ_DATA_TYPEDEF *req_data: frame request data 
    FRAME_REP_DATA_TYPEDEF *rep_data: frame response data
*/
__STATIC_INLINE int32_t
frame_cmd_process(struct rt_serial_device *fprint_device, FRAME_REQ_TYPEDEF *req, FRAME_REP_TYPEDEF *rep, void *buf, uint16_t *size)
{
	int32_t error = -FRAME_ERR_ERROR;
	FRAME_HEAD_TYPEDEF headreq, headrep;
    
    uint16_t start = FRAME_START;
    uint32_t addr = FRAME_ADDR;
    uint16_t length = 0;
    uint16_t cs = 0;

	//rt_memset(&head, 0, sizeof(head));

	reverse(headreq.start, &start, sizeof(start));
    reverse(headreq.addr, &addr, sizeof(addr));

    headreq.pid = FRAME_PID_CMD;
    // frame length
    length = sizeof(req->cmd) + frame_data_size_map[req->cmd].req + sizeof(req->cs);
    // frame head process
    reverse(headreq.len, &length, sizeof(headreq.len));
    
    // check sum process
	cs = check_sum((uint8_t *)&headreq+6, sizeof(headreq)-6) + check_sum(req, length - sizeof(req->cs));
    reverse(req->cs, &cs, sizeof(req->cs));
    
    // send frame data
	error = send_frame(fprint_device, &headreq, req);
	if (error < 0)
		goto process_error;
    // receive frame data
	error = recv_frame(fprint_device, &headrep, rep);
	if (error < 0)
		goto process_error;
    if (FRAME_CMD_AUTOLOGIN == req->cmd && rep->result == 0x56) {
        
        error = recv_frame(fprint_device, &headrep, rep);
        if (error < 0)
            goto process_error;        
    }
    if (FRAME_CMD_DOWNCHAR == req->cmd && buf != RT_NULL && size != RT_NULL) {
        send_frame_data(fprint_device, buf, *size);
        RT_DEBUG_LOG(FPRINT_DEBUG, ("DATA:-------------");
            print_hex(buf, *size);
            rt_kprintf("--------------------");
        );
    }
    if (FRAME_CMD_UPCHAR == req->cmd && buf != RT_NULL && size != RT_NULL) {
        recv_frame_data(fprint_device, buf, *size);
        RT_DEBUG_LOG(FPRINT_DEBUG, ("DATA:-------------");
            print_hex(buf, *size);
            rt_kprintf("--------------------");
        );
    }
    // process result
    error = -rep->result;
process_error:
	return error;
}

/* USART2 */
#define UART2_GPIO_TX        GPIO_Pin_2
#define UART2_GPIO_RX        GPIO_Pin_3
#define UART2_GPIO           GPIOA

#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

/* STM32 uart driver */
struct stm32_uart
{
    USART_TypeDef* uart_device;
    IRQn_Type irq;
};

static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart* uart;
    USART_InitTypeDef USART_InitStructure;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct stm32_uart *)serial->parent.user_data;

    USART_InitStructure.USART_BaudRate = cfg->baud_rate;

    if (cfg->data_bits == DATA_BITS_8){
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    } else if (cfg->data_bits == DATA_BITS_9) {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    }

    if (cfg->stop_bits == STOP_BITS_1){
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    } else if (cfg->stop_bits == STOP_BITS_2){
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
    }

    if (cfg->parity == PARITY_NONE){
        USART_InitStructure.USART_Parity = USART_Parity_No;
    } else if (cfg->parity == PARITY_ODD) {
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
    } else if (cfg->parity == PARITY_EVEN) {
        USART_InitStructure.USART_Parity = USART_Parity_Even;
    }

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart->uart_device, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(uart->uart_device, ENABLE);

    return RT_EOK;
}

static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    switch (cmd)
    {
        /* disable interrupt */
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        UART_DISABLE_IRQ(uart->irq);
        /* disable interrupt */
        USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
        break;
        /* enable interrupt */
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        UART_ENABLE_IRQ(uart->irq);
        /* enable interrupt */
        USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
        break;
    case RT_DEVICE_CTRL_CLR_FIFO:
        RT_SERIAL_CLEAR(serial);
        break;
    case RT_DEVICE_CTRL_SEND_CMD: {
        int32_t error;
        struct fprint_args *fargs = arg;
        error = frame_cmd_process(serial, fargs->req, fargs->rep, fargs->buf, fargs->size);
        if (error < 0)
            RT_DEBUG_LOG(FPRINT_DEBUG, ("frame cmd process result is 0X%X\n", abs(error)));
        return error;
        }
    }
    return RT_EOK;
}

static int stm32_putc(struct rt_serial_device *serial, char c)
{
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    uart->uart_device->DR = c;
    while (!(uart->uart_device->SR & USART_FLAG_TC));

    return 1;
}

static int stm32_getc(struct rt_serial_device *serial)
{
    int ch;
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    ch = -1;
    if (uart->uart_device->SR & USART_FLAG_RXNE)
    {
        ch = uart->uart_device->DR & 0xff;
    }

    return ch;
}

static const struct rt_uart_ops stm32_uart_ops =
{
    stm32_configure,
    stm32_control,
    stm32_putc,
    stm32_getc,
};

/* UART1 device driver structure */
struct stm32_uart fprint_uart =
{
    USART2,
    USART2_IRQn,
};
struct rt_serial_device fprint_device;

void USART2_IRQHandler(void)
{
    struct stm32_uart* uart;

    uart = &fprint_uart;

    /* enter interrupt */
    rt_interrupt_enter();
    if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        rt_hw_serial_isr(&fprint_device, RT_SERIAL_EVENT_RX_IND);
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
    if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET)
    {
        stm32_getc(&fprint_device);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

static void RCC_Configuration(void)
{
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

}

static void NVIC_Configuration(struct stm32_uart* uart)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = uart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int rt_hw_fprint_device_init(void)
{
    struct stm32_uart* uart;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    RCC_Configuration();
    GPIO_Configuration();

    uart = &fprint_uart;

    config.baud_rate = 57600;
    config.bufsz = 1024;
    fprint_device.ops    = &stm32_uart_ops;
    fprint_device.config = config;

    NVIC_Configuration(&fprint_uart);

    /* register UART1 device */
    return rt_hw_serial_register(&fprint_device, "fprint",
                                 RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                                 uart);
}
INIT_BOARD_EXPORT(rt_hw_fprint_device_init);

int32_t 
fprint_init(void)
{
    int32_t result = -FRAME_ERR_ERROR;
    FRAME_REQ_TYPEDEF req;
    FRAME_REP_TYPEDEF rep;
    struct fprint_args fargs;
    
    uint32_t pwd = FRAME_PW;
    uint32_t addr = FRAME_ADDR;
    uint8_t buf[512];
    uint16_t size = sizeof(buf);
    fargs.buf = buf;
    fargs.size = &size;
    fargs.req = &req;
    fargs.rep = &rep;

    rt_device_open(&(fprint_device.parent), RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    rt_thread_delay(RT_TICK_PER_SECOND/100);
    //rt_device_read(&(fprint_device.parent), 0, buf, 512);
    rt_device_write(&(fprint_device.parent), 0, "asdfasdf", 8);
    rt_device_read(&(fprint_device.parent), 0, buf, 8);
    //print_hex(buf, 8);
    // echo test
    req.cmd = FRAME_CMD_GETECHO;
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    if (result < 0)
    // verify password
    req.cmd = FRAME_CMD_VRYPWD;
    reverse(req.data.vrypwd.pwd, &pwd, sizeof(req.data.vrypwd.pwd));
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    // delete all model
    req.cmd = FRAME_CMD_EMPTY;
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
	// set ADDR
    req.cmd = FRAME_CMD_SETADDR;
    reverse(req.data.setaddr.addr, &addr, sizeof(req.data.setaddr.addr));
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
	// set package size
    req.cmd = FRAME_CMD_SETSYSPARA;
	req.data.setsyspara.type = 6;
	req.data.setsyspara.data = FRAME_PARAM_DATA_SIZE;
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    
    return result;
}

int32_t 
fprint_login(uint8_t timeout, uint16_t id)
{
    int32_t result = -FRAME_ERR_ERROR;
    FRAME_REQ_TYPEDEF req;
    FRAME_REP_TYPEDEF rep;
    struct fprint_args fargs;
    
    fargs.buf = RT_NULL;
    fargs.size = RT_NULL;
    fargs.req = &req;
    fargs.rep = &rep;
    req.cmd = FRAME_CMD_AUTOLOGIN;
    req.data.autologin.push_timeout = timeout;
    req.data.autologin.push_times = 2;
    reverse(req.data.autologin.model_id, &id, sizeof(req.data.autologin.model_id));
    req.data.autologin.rep_flag = 0;
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    return result;
}
int32_t 
fprint_search(uint8_t timeout, uint16_t start_id, uint16_t number)
{
    int32_t result = -FRAME_ERR_ERROR;
    FRAME_REQ_TYPEDEF req;
    FRAME_REP_TYPEDEF rep;
    struct fprint_args fargs;
    
    fargs.buf = RT_NULL;
    fargs.size = RT_NULL;
    fargs.req = &req;
    fargs.rep = &rep;
    req.cmd = FRAME_CMD_AUTOSEARCH;
    req.data.autosearch.push_timeout = timeout;
    reverse(req.data.autosearch.start_model_id, &start_id, sizeof(req.data.autosearch.start_model_id));
    reverse(req.data.autosearch.model_number, &number, sizeof(req.data.autosearch.model_number));
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    if (result >= 0) {
        result = 0;
        reverse(&result, rep.data.autosearch.model_id, sizeof(rep.data.autosearch.model_id));
    }
    return result;
}

uint32_t 
fprint_read_model(uint16_t id)
{
    int32_t result = -FRAME_ERR_ERROR;
    FRAME_REQ_TYPEDEF req;
    FRAME_REP_TYPEDEF rep;
    struct fprint_args fargs;
    uint8_t buf[512];
    uint16_t size = sizeof(buf);
    rt_memset(buf, 0, size);
    fargs.buf = buf;
    fargs.size = &size;
    fargs.req = &req;
    fargs.rep = &rep;
    req.cmd = FRAME_CMD_LOADCHAR;
    req.data.loadchar.buf_id = 1;
    reverse(req.data.loadchar.model_id, &id, sizeof(req.data.loadchar.model_id));
    result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    if (result >= 0) {
        req.cmd = FRAME_CMD_UPCHAR;
        req.data.upchar.buf_id = 1;
        reverse(req.data.loadchar.model_id, &id, sizeof(req.data.loadchar.model_id));
        result = rt_device_control(&(fprint_device.parent), RT_DEVICE_CTRL_SEND_CMD, &fargs);
    }
    return result;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif
FINSH_FUNCTION_EXPORT_ALIAS(fprint_init, fp_init, [void]);
FINSH_FUNCTION_EXPORT_ALIAS(fprint_login, fp_lgn, [uint8_t timeout uint16_t id]);
FINSH_FUNCTION_EXPORT_ALIAS(fprint_search, fp_sch, [uint8_t timeout uint16_t start_id uint16_t number]);
FINSH_FUNCTION_EXPORT_ALIAS(fprint_read_model, fp_rml, [uint16_t id]);
