#include <rtthread.h>
#include <stm32f10x.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define KD_DEBUG 0

#define DISP_POINT_MASK 0xFBFF
#define DISP_FLOAT0_2(f) disp_float(0,2,(f))
#define DISP_FLOAT3_7(f) disp_float(3,7,(f))
#define DISP_FLOAT8_12(f) disp_float(8,12,(f))

#define DISP_INT0_2(i, mask) disp_int(0,2,(i), (mask))
#define DISP_INT3_7(i, mask) disp_int(3,7,(i), (mask))
#define DISP_INT8_12(i, mask) disp_int(8,12,(i), (mask))

const static uint16_t 
disp_map[]={0x3E7C,0x7FFE,0x3D7D,0x3DFC,
            0x7CFE,0xBCFC,0xBC7C,0x3FFE,
            0x3C7C,0x3CFC,0x3C7E,0xFC7C,
            0xBE7D,0x7D7C,0xBC7D,0xBC7F,
            0xFFFF};

            
union disp_led1{
    struct {
        unsigned short running:1;//运转
        unsigned short standby:1;//待机
        unsigned short reserve11_13:3;
        unsigned short width:1;//幅宽
        unsigned short point:1;//起绕点
        unsigned short breaken:1;//断线
        unsigned short overspeed:1;//超速
        unsigned short reserve2_6:5;
        unsigned short position:1;//定位
        unsigned short slow:1;//起绕点
    }bits;
    uint16_t data;
};
union disp_led2{
    struct {
        unsigned short cycles:1;//总圈数
        unsigned short diameter:1;//线径
        unsigned short reserve10_13:4;
        unsigned short back:1;//后退
        unsigned short slowspeed:1;//低速
        unsigned short highspeed:1;//超速
        unsigned short reserve2_6:5;
        unsigned short stopslow:1;//停止慢车
        unsigned short startslow:1;//起绕慢车
    }bits;
    uint16_t data;
};
union disp_led3{
    struct {
        unsigned short pxdirection:1;//排线方向
        unsigned short reserve11_14:4;
        unsigned short autosetup:1;//自动启动
        unsigned short autoreturn:1;//自动归位
        unsigned short stopstep:1;//结束步续
        unsigned short startstep:1;//开始步续
        unsigned short reserve2_6:5;
        unsigned short ldstop:1;//两端停车
        unsigned short rxdirection:1;//绕线方向
    }bits;
    uint16_t data;
};

struct disp_cache {
    uint16_t data[16];
};

static struct disp_cache cache;
static rt_mutex_t disp_mutex;

static void 
delay_us(uint32_t time)
{
    uint8_t nCount;
    
    while(time--) {
        for(nCount = 6 ; nCount != 0; nCount--);
    }
}
__STATIC_INLINE
void disp_addr(uint16_t addr)
{
	uint16_t temp;
	temp = GPIO_ReadOutputData(GPIOF);

	temp &= 0xFFF0;
    addr &= 0x000f;

    //选择位选
	GPIO_Write(GPIOF, (temp | addr));

}
__STATIC_INLINE
void disp_write(uint16_t data)
{
	uint16_t temp;
    
    rt_mutex_take(disp_mutex, RT_WAITING_FOREVER);
	
	//显示段选
	temp = GPIO_ReadOutputData(GPIOD);

	temp = temp & 0x3FFC;

	GPIO_Write(GPIOD, (temp | data));

	temp = GPIO_ReadOutputData(GPIOE);

	temp = temp & 0xF87F;

	GPIO_Write(GPIOE, (temp | data));
    rt_mutex_release(disp_mutex);
	
}

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

__STATIC_INLINE
void disp_cache_update(void)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(cache.data); i++) {
        disp_addr(i);
        disp_write(cache.data[i]);
        /* FIXME */
        rt_thread_delay(1);
    }
}

__STATIC_INLINE
void disp_number(uint8_t addr, uint8_t number, uint8_t point)
{
    uint16_t data;
    
    RT_ASSERT(number < 0x0f);
    data = disp_map[number];
    
    if (point)
        data &= DISP_POINT_MASK;
    rt_mutex_take(disp_mutex, RT_WAITING_FOREVER);
    cache.data[addr] = data;
    rt_mutex_release(disp_mutex);
}

__STATIC_INLINE
void disp_float(uint16_t addr_s, uint16_t addr_d, float f)
{
    char buf[20];
    int i, addr;
    
    snprintf(buf, sizeof(buf), "%5.4f", f);
    for (i = 0, addr = addr_s; buf[i] != '\0' && addr <= addr_d; i++, addr++) {
        if (buf[i+1] == '.') {
            disp_number(addr, buf[i] - '0', 1);
            i++; 
        } else {
            disp_number(addr, buf[i] - '0', 0);
        }
    }
}

#define BIT_MASK(b) (0x00000001 << (b))

const char *int_fmt_map[] = {
    "",
    "",
    "",
    "%03d",
    "",
    "%05d",
};

__STATIC_INLINE
void disp_int(uint16_t addr_s, uint16_t addr_d, int num, uint16_t dot_mask)
{
    char buf[20];
    int i, addr;
    
    snprintf(buf, sizeof(buf), int_fmt_map[addr_d - addr_s + 1], num);
    for (i = 0, addr = addr_s; buf[i] != '\0' && addr <= addr_d; i++, addr++) {
        if(dot_mask & BIT_MASK(i))
            disp_number(addr, buf[i] - '0', 1);
        else
            disp_number(addr, buf[i] - '0', 0);
    }
}
void
kd_thread_entry(void *parameter)
{
    while(1) {
        disp_cache_update();
    }
}

int
kd_init(void)
{
    rt_thread_t kd_thread;
	//定义结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启GPIO的时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | 
							RCC_APB2Periph_GPIOF , ENABLE);
	
	//控制地址线的GPIO配置			GPIO_Mode_Out_PP
	//A0->A4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;	                              
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	//GPIO_SetBits(GPIOF,GPIO_Pin_0);

	//GPIO配置
	//数据线D0->D3管脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1 ;                                  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	//GPIO_SetBits(GPIOD,GPIO_Pin_15);
	//GPIO_ResetBits(GPIOD,GPIO_Pin_14);


	//数据线D4->D7管脚配置  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//按键信号输入线配置  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	GPIO_Init(GPIOE, &GPIO_InitStructure);

    // create timer
    /*
    kd_timer = rt_timer_create("kd", kd_timeout, RT_NULL, 5, 
                               RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
    return rt_timer_start(kd_timer);
    */
    disp_mutex = rt_mutex_create("kd", RT_IPC_FLAG_FIFO);
    
    rt_memset(cache.data, 0xff, sizeof(cache.data));
    
    kd_thread = rt_thread_create("kd", kd_thread_entry, RT_NULL, 512, 10, 20);
    if (kd_thread != RT_NULL)
        return rt_thread_startup(kd_thread);
    return -1;
}

INIT_APP_EXPORT(kd_init);

#ifdef RT_USING_FINSH
#include <finsh.h>

int disp_float_test(int s, int d, int i, int f)
{
    float temp;
    if (f)
        temp = f * 0.01 * i;
    else
        temp = 1;
    disp_float(s, d, temp);
    return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(disp_number, d_num, [addr number point]);
FINSH_FUNCTION_EXPORT_ALIAS(disp_float_test, d_ft, [addr_s addr_d  x f]);
FINSH_FUNCTION_EXPORT_ALIAS(disp_int, d_int, [addr_s addr_d i x]);

FINSH_FUNCTION_EXPORT_ALIAS(disp_write, d_w, [data]);
#endif
