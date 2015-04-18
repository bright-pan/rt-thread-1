/**************************** Copyright (c) ****************************************
**                              
**                        SILONE MAGCARD INC.Ltd,                
**					    深圳西龙同辉技术股份有限公司
**                                  
****************************File Information****************************************
**文   件   名: RotateApp.c
**创   建   人: 赖 浩
**创 建 日  期: 2014年4月3日
**最后修改日期:
**描        述: 卡盒旋转处理源文件 							
************************************************************************************/
#define     _NUMERICDISPLAY_APP_C_

#include <rtthread.h>
#include <stm32f10x.h>
//#include "includes.h"
//#include "MotorControlApp.h"
#include "NumericDisplay.h"

//数目管段选数组，用于显示0~F
uint16_t DisplayTable[17]={0x3E7C,0x7FFE,0x3D7D,0x3DFC,0x7CFE,0xBCFC,0xBC7C,0x3FFE,0x3C7C,0x3CFC,0x3C7E,0xFC7C,0xBE7D,0x7D7C,0xBC7D,0xBC7F,0xFFFF};
uint8  table[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//小数点显示段码
uint16 DecimalPoint = 0xFBFF;


//卡盒旋转控制信号量
static OS_EVENT 	*pDisplayEvent;			//事件控制块
static uint8 		nDisplayEventErr;		    //错误标志

uint8 NumberKey  = 0;			//数字键值
uint8 NumberKey0 = 0;	   		//数字键0
uint8 NumberKey1 = 0;	   		//数字键1
uint8 NumberKey2 = 0;	   		//数字键2
uint8 NumberKey3 = 0;	   		//数字键3
uint8 NumberKey4 = 0;	   		//数字键4
uint8 NumberKey5 = 0;	   		//数字键5
uint8 NumberKey6 = 0;	   		//数字键6
uint8 NumberKey7 = 0;	   		//数字键7
uint8 NumberKey8 = 0;	   		//数字键8
uint8 NumberKey9 = 0;	   		//数字键9
uint8 EffectiveNumber = 0;      //数字输入有效变量

uint8 StepSetKey =0; 			//步序设定键
uint8 ProductionKey = 0;		//产量设定键
uint8 Whippletree = 0;  		//横杠键
uint8 EnterKey = 0;				//输入键
uint8 ClearKey = 0;             //清除键
uint8 CopyKey = 0;				//复制件

uint8 AutoKey = 0;   			//自动按键
uint8 BrakeKey = 0;				//刹车键
uint8 ForwardKey = 0;			//跳段键
uint8 RetreatKey = 0;			//返回按键，即退段键
uint8 ReturnKey = 0;			//复归按键
uint8 StopKey = 0;				//停止键
uint8 StartKey = 0;				//启动键

uint8 StartStepKey = 0;			//起始步续键
uint8 FinishStepKey = 0;		//结束步续键
uint8 AutoHomingKey = 0;		//自动归位键
uint8 AutoStartKey  = 0;        //自动启动键

uint8 DataChoiceKey = 0;		//资料选择键
uint8 ArrangeDirectionKey = 0;	//排线方向键
uint8 CoilingDirectionKey = 0;	//绕线方向键
uint8 EndsStopKeyKey = 0;       //两端停车键

uint8 RunSpeedKey = 0;			//转速键
uint8 MakeZeroKey = 0;			//归零键

uint8 MakeForwardKey = 0;		//步进电机前进
uint8 MakeBackKey = 0;			//步进电机后退键

//按键按下标志
uint16 KeyFlag = 0;
uint8 KeyEffectiv = 0;

//步序设定变量
uint16 StartStep = 1;			//设置初始起始步序值
uint16 FinishStep = 4;			//设置初始结束步序值
uint16 TempStartStep = 0;		//起始步序临时变量
uint16 TempFinishStep = 0;		//结束步序临时变量
uint16 TempStep = 0;			//步序设定时用到的临时变量
uint8  StartStepTable[3] ={0,0,1};
uint8  FinishStepTable[3]={0,0,4}; 
//数目管位选变量
uint16 DisplayPosition = 0;

//数码管位选临时变量
uint16 TempPosition = 0;

//按键按下是的当前地址
uint16 PresentPosition = 0;
//临时存储按键扫描值
uint8  Pin13Data= 0;
uint8  Pin14Data= 0;
uint8  Pin15Data= 0;
uint8  Pin16Data= 0;
uint8  Pin17Data= 0;

//扫描按键临时存储值
uint8  TempPin13Data= 0;
uint8  TempPin14Data= 0;
uint8  TempPin15Data= 0;
uint8  TempPin16Data= 0;
uint8  TempPin17Data= 0;

//与绕线电机控制的9个变量
uint16 StartPoint = 0;	  				//起绕点
uint16 Breadth = 0;		  				//幅宽
uint16 WireDiameter = 0;	  			//线径
uint16 RotationTotalNumber = 300;		//绕线总圈数
uint16 RotationSlowlyNumber = 50;		//开始绕线时慢速绕线圈数
uint16 RotationFastlyNumber = 50;		//结束绕线时慢速绕线圈数
uint8  HighSpeed = 50;					//高速绕线的速度
uint8  SlowSpeed = 10;					//低速绕线的速度
uint8  BackUp = 0;						//备用

//
uint16 TempStartPoint = 0;	  			//起绕点
uint16 TempBreadth = 0;		  			//幅宽
uint16 TempWireDiameter = 0;	  		//线径
uint16 TempRotationTotalNumber = 300;	//绕线总圈数
uint16 TempRotationSlowlyNumber = 50;	//开始绕线时慢速绕线圈数
uint16 TempRotationFastlyNumber = 50;	//结束绕线时慢速绕线圈数
uint8  TempHighSpeed = 50;				//高速绕线的速度
uint8  TempSlowSpeed = 10;				//低速绕线的速度
uint8  TempBackUp = 0;					//备用

/*****************************************************************************************
* 名	称：FSMC_PortInit()
* 功	能：FSMC的GPIO配置
* 入口参数：无
* 出口参数：无
* 说	明：
******************************************************************************************/
void Display_PortInit(void)
{
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

	TIM4_Init();
   
}

/*****************************************************************************************
* 名	称：Display_WriteByte()
* 功	能：向数码管某一位写一个数据，用于显示
* 入口参数：PositionAddr 位地址，Byte 写的数据，即显示的数据，FixedPoint 显示的数据是否需要小数点。
* 出口参数：无
* 说	明：往某个扩展串口的某个寄存器写一个字节数据
******************************************************************************************/
void Display_WriteByte(uint16 PositionAddr, uint16 Data, uint8 FixedPoint)
{
	uint16 TempData;

	TempData = GPIO_ReadOutputData(GPIOF);

	TempData = TempData & 0xFFF0;

    //选择位选
	GPIO_Write(GPIOF, (TempData | PositionAddr));

	//显示段选
	TempData = GPIO_ReadOutputData(GPIOD);

	TempData = TempData & 0x3FFC;

	GPIO_Write(GPIOD, (TempData | Data));

	TempData = GPIO_ReadOutputData(GPIOE);

	TempData = TempData & 0xF87F;

	GPIO_Write(GPIOE, (TempData | Data));

}


/****************************************************************************
* 名	称：KeysScan()
* 功	能：按键扫描函数
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/ 
void KeysScan(void)
{
	//读取当前按输入信号
	Pin13Data = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11);
	Pin14Data = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12);
	Pin15Data = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13);
	Pin16Data = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14);
	Pin17Data = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15);

	if(!Pin13Data || !Pin14Data || !Pin15Data || !Pin16Data || !Pin17Data)
	{
		if( KeyFlag == 0)
		{
			TempPin13Data = Pin13Data;
			TempPin14Data = Pin14Data;
			TempPin15Data = Pin15Data;
			TempPin16Data = Pin16Data;
			TempPin17Data = Pin17Data;
		}

		if((TempPin13Data == Pin13Data)&&(TempPin14Data == Pin14Data)&&(TempPin15Data == Pin15Data)&&(TempPin16Data == Pin16Data)&&(TempPin17Data == Pin17Data))
		{
			KeyFlag = KeyFlag + 1;
			PresentPosition = DisplayPosition & 0x0007;
			
			if(KeyFlag == 2)
			{
				KeyEffectiv = 1;			
			}		
		}
	
	}

	TempPosition = DisplayPosition & 0x0007;
	
	if(TempPosition == PresentPosition)
	{	

		if(Pin13Data && Pin14Data && Pin15Data && Pin16Data && Pin17Data)
		{
			KeyFlag = 0;
			KeyEffectiv = 0;		
		}
		
		//如按键标志已经有两次扫描检测到了，则说明按键发生了，注意这里以后需要添加KeyFlag==1的情况，看情况修改
		if((KeyFlag >=2) && (KeyEffectiv == 1))
		{

			switch(TempPosition)
			{
				case 0x0000:
					 if(TempPin15Data == 0)		//按下SW34键,排线杆右移键
					 {

						//按键标识设为1，以便长时间按下键是，让排线杆连续向左移动
						KeyEffectiv = 1;

						if(KeyFlag == 2)
						{
							PostBuzzerOn();						
						}
						KeyFlag = 3;
											 
					 }
					 if(TempPin16Data == 0)		//按下SW26键，转速键
					 {	

						KeyEffectiv = 0;
						
						PostBuzzerOn();					 
					 }
					 break;
				case 0x0001:
					 if(TempPin15Data == 0)		//按下SW35键，排线杆左移
					 {

						//按键标识设为1，以便长时间按下键是，让排线杆连续向左移动
						KeyEffectiv = 1;

						if(KeyFlag == 2)
						{
							PostBuzzerOn();						
						}

						//KeyFlag 设置为3，以确保长时间按下是不超过范围
						KeyFlag = 3;					 
					 }
					 if(TempPin16Data == 0)		//按下SW25键，归零键
					 {
						KeyEffectiv = 0;

						//发送蜂鸣器蜂鸣信号量
					    PostBuzzerOn();					 
					 }
					 if(TempPin17Data == 0)		//按下SW33键，自动键
					 {
						KeyEffectiv = 0;
						
						//发送蜂鸣器蜂鸣信号量
					    PostBuzzerOn();					 
					 }
					 break;				
				case 0x0002:
					 if(TempPin13Data == 0)		//按下SW6键，资料选择键
					 {
																		 
					 }
					 if(TempPin14Data == 0)		//按下SW12键，排线方向键
					 {
													 
					 }
					 if(TempPin15Data == 0)		//按下SW18键，绕线方向键
					 {
					 
					 }
					 if(TempPin16Data == 0)		//按下SW24键，两端停车键
					 {
					 
					 }
					 if(TempPin17Data == 0)		//按下SW32键，刹车键
					 {
					 
					 }

					 //按键设置为无效
					 KeyEffectiv = 0;

					 //发送蜂鸣器蜂鸣信号量
					 PostBuzzerOn();

					 break;	
				case 0x0003:
					 if(TempPin13Data == 0)		//按下SW5键，起始步序
					 {
						StartStepKey = 1;												 
					 }
					 if(TempPin14Data == 0)		//按下SW11键，结束步序
					 {
						FinishStepKey = 1;												 
					 }
					 if(TempPin15Data == 0)		//按下SW17键，自动归位
					 {
					 
					 }
					 if(TempPin16Data == 0)		//按下SW23键，自动启动
					 {
					 
					 }
					 if(TempPin17Data == 0)		//按下SW31键，跳段
					 {
					 
					 }

					 //按键设置为无效，以确保长时间按下不会有问题
					 KeyEffectiv = 0;

					 //发送蜂鸣器蜂鸣信号量
					 PostBuzzerOn();

					 break;
				case 0x0004:
					 if(TempPin13Data == 0)		//按下SW4键，数字“7”键
					 {
						NumberKey = 7;

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;												 
					 }
					 if(TempPin14Data == 0)		//按下SW10键，数字“4”键
					 {
						NumberKey = 4;

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;																		 
					 }
					 if(TempPin15Data == 0)		//按下SW16键，数字“1”键
					 {
						NumberKey = 1;					 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin16Data == 0)		//按下SW22键，数字“0”键
					 {
						NumberKey = 0;					 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin17Data == 0)		//按下SW30键，退段键
					 {
					 
					 }

					 //按键设置为无效，以确保长时间按下不会有问题
					 KeyEffectiv = 0;

					 //发送蜂鸣器蜂鸣信号量
					 PostBuzzerOn();

					 break;
				case 0x0005:
					 if(TempPin13Data == 0)		//按下SW3键，数字“8”键
					 {
						NumberKey = 8;												 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin14Data == 0)		//按下SW9键，数字“5”键
					 {
						NumberKey = 5;													 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin15Data == 0)		//按下SW15键，数字“2”键
					 {
						NumberKey = 2;													 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin16Data == 0)		//按下SW21键，清除键
					 {
					 
					 }
					 if(TempPin17Data == 0)		//按下SW29键，复归键
					 {
					 
					 }
					 
					 //按键设置为无效，以确保长时间按下不会有问题
					 KeyEffectiv = 0;

					 //发送蜂鸣器蜂鸣信号量
					 PostBuzzerOn();

					 break;
				case 0x0006:
					 if(TempPin13Data == 0)		//按下SW2键，数字“9”键
					 {
						NumberKey = 9;												 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin14Data == 0)		//按下SW8键，数字“6”键
					 {
						NumberKey = 6;					 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin15Data == 0)		//按下SW14键，数字“3”键
					 {
						NumberKey = 3;													 

						//表示当期输入的是有效数字数据
						EffectiveNumber = 1;
					 }
					 if(TempPin16Data == 0)		//按下SW20键，“Copy” 键
					 {
					 
					 }
					 if(TempPin17Data == 0)		//按下SW28键，停车键
					 {
					 
					 }

					 //按键设置为无效，以确保长时间按下不会有问题
					 KeyEffectiv = 0;

					 //发送蜂鸣器蜂鸣信号量
					 PostBuzzerOn();

					 break;	
				case 0x0007:
					 if(TempPin13Data == 0)		//按下SW1键，步序设定键
					 {
						StepSetKey = 1;												 
					 }
					 if(TempPin14Data == 0)		//按下SW7键，产量设定键
					 {
											 
					 }
					 if(TempPin15Data == 0)		//按下SW13键，“-”键
					 {
							
					 }
					 if(TempPin16Data == 0)		//按下SW19键，输入键
					 {
						EnterKey = 1;
					 }
					 if(TempPin17Data == 0)		//按下SW27键，启动键
					 {
					   						
					 }
					 
					 //按键设置为无效，以确保长时间按下不会有问题
					 KeyEffectiv = 0;


					 //发送蜂鸣器蜂鸣信号量
					 PostBuzzerOn();

					 break;					
				
			}		
		}

		//}
	}

}

//绕线资料
uint8 RotationData = 0;
uint8 i = 0 ,j = 0;
void GainKeyValue(void)
{
	if(StartStepKey == 1)
	{
		if((StartStep <100) && (EffectiveNumber == 1))
		{		
			TempStartStep = TempStartStep*10 + NumberKey;
			NumberKey = 0;
			EffectiveNumber = 0;		
		}
		else
		{
			NumberKey = 0;
			EffectiveNumber = 0;		
		}

		TempStep = TempStartStep;

		StartStep = TempStartStep;

		/*
		for(i=0;i<3;i++)
		{

			if(((TempStep%10) > 0) || i==0)
			{
				StartStepTable[2-i] = TempStep%10;			
			}
			else
			{
				StartStepTable[2-i] = 16;			
			} 
			
			TempStep = TempStep/10;					
		}
		*/

		for(i=0;i<3;i++)
		{

			if(((TempStep%10) > 0) || i==0)
			{
				table[2-i] = TempStep%10;			
			}
			else
			{
				table[2-i] = 16;			
			} 
			
			TempStep = TempStep/10;					
		}

		if(EnterKey == 1)
		{
			//键值归零
			EnterKey = 0;

			//键值归零
			StartStepKey = 0;
			
			TempStartStep = 0;
			
			NumberKey = 0;		
		}	
	
	}

	if(FinishStepKey == 1)
	{
		if((TempFinishStep < 100) && (EffectiveNumber == 1))
		{
			TempFinishStep = TempFinishStep*10 + NumberKey;

			NumberKey = 0;

			EffectiveNumber = 0;
		}
		else
		{
			NumberKey = 0;
			EffectiveNumber = 0;		
		}

		FinishStep = TempFinishStep;

		TempStep = TempFinishStep;

		for(i=0;i<3;i++)
		{

			if(((TempStep%10) > 0) || i==0)
			{
				table[2-i] = TempStep%10;			
			}
			else
			{
				table[2-i] = 16;			
			} 
			
			TempStep = TempStep/10;					
		}
		
		if(EnterKey == 1)
		{
			//键值归零
			EnterKey = 0;
			
			//键值归零
			FinishStepKey = 0;
			
			TempFinishStep = 0;	
			
			//table[2-i]	
		}	
	
	}

	//如果步续设定键有效
	if(StepSetKey == 1)
	{
		//如果输入键有效
		if(EnterKey == 1)
		{
			if(RotationData == 0)
			{
				//起绕点设置
				TempStartPoint = TempStartPoint*10 + NumberKey;		
			}
			else if(RotationData == 1)
			{
				//幅宽设置
				TempBreadth	= TempBreadth*10 + NumberKey;		
			}
			else if(RotationData == 2)
			{
				//线径
				TempWireDiameter = TempWireDiameter*10 + NumberKey;			
			}
			else if(RotationData == 3)
			{
				//绕线总圈数
				TempRotationTotalNumber = TempRotationTotalNumber*10 + NumberKey;			
			}
			else if(RotationData == 4)
			{
				//开始绕线时慢速绕线圈数
				RotationSlowlyNumber = RotationSlowlyNumber*10 + NumberKey;			
			}
			else if(RotationData == 5)
			{
				//结束绕线时慢速绕线圈数
				TempRotationFastlyNumber = TempRotationFastlyNumber*10 + NumberKey;			
			}
			else if(RotationData == 6)
			{
				//高速绕线的速度
				TempHighSpeed = TempHighSpeed*10 + NumberKey;			
			}
			else if(RotationData == 7)
			{
				//低速绕线的速度
				TempSlowSpeed = TempSlowSpeed*10 + NumberKey;			
			}
			else if(RotationData == 8)
			{
				//备用
				TempBackUp = TempBackUp*10 + NumberKey;			
			}

		}
		else if(EnterKey == 0)
		{
			RotationData = RotationData + 1;
			
			if(RotationData == 9)
			{
				RotationData = 0;			
			}		
		}	
	}
}

void DisplayTest(uint16 TestData)
{
	uint8 i;

	for(i=0;i<3;i++)
	{

		if(((TestData%10) > 0) || i==0)
		{
			table[2-i] = TestData%10;			
		}
		else
		{
			table[2-i] = 16;			
		} 
		
		TestData = TestData/10;					
	}
}
/****************************************************************************
* 名	称：NumericDisplayVariInit()
* 功	能：
* 入口参数：
* 出口参数：
* 说	明：数码管显示变量初始化
****************************************************************************/
void DisplayVariInit(void)
{
	pDisplayEvent = NULL;
	
	//创建AD转换完成信号量
	if(pDisplayEvent == NULL) 
	{
		pDisplayEvent = OSSemCreate(0);
	}

	//参数初始化

}

/****************************************************************************
* 名	称：WaitSampleEvent()
* 功	能：等待采样信号
* 入口参数：无
* 出口参数：无
* 说	明：
****************************************************************************/
void WaitNumericDisplayEvent(void)
{
	//等待数据解析完成
	OSSemPend(pDisplayEvent, 0, &nDisplayEventErr);
}

/****************************************************************************
* 名	称：PostSampleSem()
* 功	能：发送电机旋转信号
* 入口参数：无
* 出口参数：
* 说	明：
****************************************************************************/
void PostNumericDisplaySem(void)
{
	OSSemPost(pDisplayEvent);	 
}


/****************************************************************************
* 名	称：TIM4Start()
* 功	能：
* 入口参数：
* 出口参数：无
* 说	明：定时器4启动
****************************************************************************/
void TIM4Start(void)
{
	//清除第一次假中断
	TIM4->CR1 = (1 << 2) |	(1 << 0);			//允许计数器和中断
	TIM4->EGR = 1 << 0;							//产生更新事件
	TIM4->CR1 &= ~(1 << 0);						//禁止计数器
	TIM4->CR1 &= ~(1 << 2);						//禁止中断
	TIM4->SR &= ~(1<<0);        				//清除中断标志		

	//开启定时器
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	 //允许产生更新中断	
	TIM_Cmd(TIM4, ENABLE);	
}

/****************************************************************************
* 名	称：TIM4Stop()
* 功	能：
* 入口参数：
* 出口参数：无
* 说	明：定时器4停止
****************************************************************************/
void TIM4Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);		//禁止计数器
	TIM_SetCounter(TIM4, 0);	//清零计数器
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);	 //禁止中断
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //清除中断标志
}

/****************************************************************************
* 名	称：Timer3ResetRun()
* 功	能：定时器3复位并运行
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void TIM4ResetRun(void)
{
	TIM_SetCounter(TIM4, 0);
   	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	 //允许产生更新中断	
	TIM_Cmd(TIM4, ENABLE);					 	 //允许计数器
}

/****************************************************************************
* 名	称：Timer3Init()
* 功	能：
* 入口参数：
* 出口参数：无
* 说	明：无
****************************************************************************/
void TIM4_Init(void)
{
		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//始终配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_DeInit(TIM4);

	TIM_TimeBaseStructure.TIM_Period = 5000;  			//7200			// TIM_Period
	TIM_TimeBaseStructure.TIM_Prescaler = 1;		//TIM_Prescaler
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_SetCounter(TIM4, 0);
	TIM_ARRPreloadConfig(TIM4, DISABLE);	  //自动重装载预装载允许	

	TIM_Cmd(TIM4, DISABLE);

	TIM_ITConfig(TIM4,TIM_IT_Update, DISABLE);
}

/****************************************************************************
* 名	称：NumericDisplayProcess()
* 功	能：卡盒旋转处理线程
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/ 
void DisplayProcess(void) reentrant
{
	//等待旋转信号
	OSSemPend(pDisplayEvent, 0, &nDisplayEventErr);

	Display_WriteByte(DisplayPosition, DisplayTable[table[DisplayPosition]],0);
	
	KeysScan();

	DisplayPosition++;

	if(DisplayPosition == 16) DisplayPosition = 0;



}


/****************************************************************************
* 名	称：TIM4_IRQHandler()
* 功	能：
* 入口参数：
* 出口参数：无
* 说	明：此函数产生PWM波来控制电机速度，通过改变PwmPulse的门限值来设置PWM波的占空比
****************************************************************************/

void TIM4_IRQHandler (void) 
{
#if OS_CRITICAL_METHOD == 3  
	OS_CPU_SR  cpu_sr = 0;
#endif

	//进入中断
	OSIntEnter();
	 
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)	
	{
	
		OS_ENTER_CRITICAL();	

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //清除中断标志	  

		//发送数目管显示信号量
		PostNumericDisplaySem();

		OS_EXIT_CRITICAL();
	}

	//退出中断
	OSIntExit(); 
}


/******************************** END OF FILE *******************************/
