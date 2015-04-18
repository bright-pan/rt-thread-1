/**************************** Copyright (c) ****************************************
**                              
**                        SILONE MAGCARD INC.Ltd,                
**					    ��������ͬ�Լ����ɷ����޹�˾
**                                  
****************************File Information****************************************
**��   ��   ��: RotateApp.c
**��   ��   ��: �� ��
**�� �� ��  ��: 2014��4��3��
**����޸�����:
**��        ��: ������ת����Դ�ļ� 							
************************************************************************************/
#define     _NUMERICDISPLAY_APP_C_

#include <rtthread.h>
#include <stm32f10x.h>
//#include "includes.h"
//#include "MotorControlApp.h"
#include "NumericDisplay.h"

//��Ŀ�ܶ�ѡ���飬������ʾ0~F
uint16_t DisplayTable[17]={0x3E7C,0x7FFE,0x3D7D,0x3DFC,0x7CFE,0xBCFC,0xBC7C,0x3FFE,0x3C7C,0x3CFC,0x3C7E,0xFC7C,0xBE7D,0x7D7C,0xBC7D,0xBC7F,0xFFFF};
uint8  table[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//С������ʾ����
uint16 DecimalPoint = 0xFBFF;


//������ת�����ź���
static OS_EVENT 	*pDisplayEvent;			//�¼����ƿ�
static uint8 		nDisplayEventErr;		    //�����־

uint8 NumberKey  = 0;			//���ּ�ֵ
uint8 NumberKey0 = 0;	   		//���ּ�0
uint8 NumberKey1 = 0;	   		//���ּ�1
uint8 NumberKey2 = 0;	   		//���ּ�2
uint8 NumberKey3 = 0;	   		//���ּ�3
uint8 NumberKey4 = 0;	   		//���ּ�4
uint8 NumberKey5 = 0;	   		//���ּ�5
uint8 NumberKey6 = 0;	   		//���ּ�6
uint8 NumberKey7 = 0;	   		//���ּ�7
uint8 NumberKey8 = 0;	   		//���ּ�8
uint8 NumberKey9 = 0;	   		//���ּ�9
uint8 EffectiveNumber = 0;      //����������Ч����

uint8 StepSetKey =0; 			//�����趨��
uint8 ProductionKey = 0;		//�����趨��
uint8 Whippletree = 0;  		//��ܼ�
uint8 EnterKey = 0;				//�����
uint8 ClearKey = 0;             //�����
uint8 CopyKey = 0;				//���Ƽ�

uint8 AutoKey = 0;   			//�Զ�����
uint8 BrakeKey = 0;				//ɲ����
uint8 ForwardKey = 0;			//���μ�
uint8 RetreatKey = 0;			//���ذ��������˶μ�
uint8 ReturnKey = 0;			//���鰴��
uint8 StopKey = 0;				//ֹͣ��
uint8 StartKey = 0;				//������

uint8 StartStepKey = 0;			//��ʼ������
uint8 FinishStepKey = 0;		//����������
uint8 AutoHomingKey = 0;		//�Զ���λ��
uint8 AutoStartKey  = 0;        //�Զ�������

uint8 DataChoiceKey = 0;		//����ѡ���
uint8 ArrangeDirectionKey = 0;	//���߷����
uint8 CoilingDirectionKey = 0;	//���߷����
uint8 EndsStopKeyKey = 0;       //����ͣ����

uint8 RunSpeedKey = 0;			//ת�ټ�
uint8 MakeZeroKey = 0;			//�����

uint8 MakeForwardKey = 0;		//�������ǰ��
uint8 MakeBackKey = 0;			//����������˼�

//�������±�־
uint16 KeyFlag = 0;
uint8 KeyEffectiv = 0;

//�����趨����
uint16 StartStep = 1;			//���ó�ʼ��ʼ����ֵ
uint16 FinishStep = 4;			//���ó�ʼ��������ֵ
uint16 TempStartStep = 0;		//��ʼ������ʱ����
uint16 TempFinishStep = 0;		//����������ʱ����
uint16 TempStep = 0;			//�����趨ʱ�õ�����ʱ����
uint8  StartStepTable[3] ={0,0,1};
uint8  FinishStepTable[3]={0,0,4}; 
//��Ŀ��λѡ����
uint16 DisplayPosition = 0;

//�����λѡ��ʱ����
uint16 TempPosition = 0;

//���������ǵĵ�ǰ��ַ
uint16 PresentPosition = 0;
//��ʱ�洢����ɨ��ֵ
uint8  Pin13Data= 0;
uint8  Pin14Data= 0;
uint8  Pin15Data= 0;
uint8  Pin16Data= 0;
uint8  Pin17Data= 0;

//ɨ�谴����ʱ�洢ֵ
uint8  TempPin13Data= 0;
uint8  TempPin14Data= 0;
uint8  TempPin15Data= 0;
uint8  TempPin16Data= 0;
uint8  TempPin17Data= 0;

//�����ߵ�����Ƶ�9������
uint16 StartPoint = 0;	  				//���Ƶ�
uint16 Breadth = 0;		  				//����
uint16 WireDiameter = 0;	  			//�߾�
uint16 RotationTotalNumber = 300;		//������Ȧ��
uint16 RotationSlowlyNumber = 50;		//��ʼ����ʱ��������Ȧ��
uint16 RotationFastlyNumber = 50;		//��������ʱ��������Ȧ��
uint8  HighSpeed = 50;					//�������ߵ��ٶ�
uint8  SlowSpeed = 10;					//�������ߵ��ٶ�
uint8  BackUp = 0;						//����

//
uint16 TempStartPoint = 0;	  			//���Ƶ�
uint16 TempBreadth = 0;		  			//����
uint16 TempWireDiameter = 0;	  		//�߾�
uint16 TempRotationTotalNumber = 300;	//������Ȧ��
uint16 TempRotationSlowlyNumber = 50;	//��ʼ����ʱ��������Ȧ��
uint16 TempRotationFastlyNumber = 50;	//��������ʱ��������Ȧ��
uint8  TempHighSpeed = 50;				//�������ߵ��ٶ�
uint8  TempSlowSpeed = 10;				//�������ߵ��ٶ�
uint8  TempBackUp = 0;					//����

/*****************************************************************************************
* ��	�ƣ�FSMC_PortInit()
* ��	�ܣ�FSMC��GPIO����
* ��ڲ�������
* ���ڲ�������
* ˵	����
******************************************************************************************/
void Display_PortInit(void)
{
	//����ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//����GPIO��ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | 
							RCC_APB2Periph_GPIOF , ENABLE);
	
	//���Ƶ�ַ�ߵ�GPIO����			GPIO_Mode_Out_PP
	//A0->A4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;	                              
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	//GPIO_SetBits(GPIOF,GPIO_Pin_0);

	//GPIO����
	//������D0->D3�ܽ�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_0 | GPIO_Pin_1 ;                                  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	//GPIO_SetBits(GPIOD,GPIO_Pin_15);
	//GPIO_ResetBits(GPIOD,GPIO_Pin_14);


	//������D4->D7�ܽ�����  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//�����ź�����������  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	TIM4_Init();
   
}

/*****************************************************************************************
* ��	�ƣ�Display_WriteByte()
* ��	�ܣ��������ĳһλдһ�����ݣ�������ʾ
* ��ڲ�����PositionAddr λ��ַ��Byte д�����ݣ�����ʾ�����ݣ�FixedPoint ��ʾ�������Ƿ���ҪС���㡣
* ���ڲ�������
* ˵	������ĳ����չ���ڵ�ĳ���Ĵ���дһ���ֽ�����
******************************************************************************************/
void Display_WriteByte(uint16 PositionAddr, uint16 Data, uint8 FixedPoint)
{
	uint16 TempData;

	TempData = GPIO_ReadOutputData(GPIOF);

	TempData = TempData & 0xFFF0;

    //ѡ��λѡ
	GPIO_Write(GPIOF, (TempData | PositionAddr));

	//��ʾ��ѡ
	TempData = GPIO_ReadOutputData(GPIOD);

	TempData = TempData & 0x3FFC;

	GPIO_Write(GPIOD, (TempData | Data));

	TempData = GPIO_ReadOutputData(GPIOE);

	TempData = TempData & 0xF87F;

	GPIO_Write(GPIOE, (TempData | Data));

}


/****************************************************************************
* ��	�ƣ�KeysScan()
* ��	�ܣ�����ɨ�躯��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/ 
void KeysScan(void)
{
	//��ȡ��ǰ�������ź�
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
		
		//�簴����־�Ѿ�������ɨ���⵽�ˣ���˵�����������ˣ�ע�������Ժ���Ҫ���KeyFlag==1�������������޸�
		if((KeyFlag >=2) && (KeyEffectiv == 1))
		{

			switch(TempPosition)
			{
				case 0x0000:
					 if(TempPin15Data == 0)		//����SW34��,���߸����Ƽ�
					 {

						//������ʶ��Ϊ1���Ա㳤ʱ�䰴�¼��ǣ������߸����������ƶ�
						KeyEffectiv = 1;

						if(KeyFlag == 2)
						{
							PostBuzzerOn();						
						}
						KeyFlag = 3;
											 
					 }
					 if(TempPin16Data == 0)		//����SW26����ת�ټ�
					 {	

						KeyEffectiv = 0;
						
						PostBuzzerOn();					 
					 }
					 break;
				case 0x0001:
					 if(TempPin15Data == 0)		//����SW35�������߸�����
					 {

						//������ʶ��Ϊ1���Ա㳤ʱ�䰴�¼��ǣ������߸����������ƶ�
						KeyEffectiv = 1;

						if(KeyFlag == 2)
						{
							PostBuzzerOn();						
						}

						//KeyFlag ����Ϊ3����ȷ����ʱ�䰴���ǲ�������Χ
						KeyFlag = 3;					 
					 }
					 if(TempPin16Data == 0)		//����SW25���������
					 {
						KeyEffectiv = 0;

						//���ͷ����������ź���
					    PostBuzzerOn();					 
					 }
					 if(TempPin17Data == 0)		//����SW33�����Զ���
					 {
						KeyEffectiv = 0;
						
						//���ͷ����������ź���
					    PostBuzzerOn();					 
					 }
					 break;				
				case 0x0002:
					 if(TempPin13Data == 0)		//����SW6��������ѡ���
					 {
																		 
					 }
					 if(TempPin14Data == 0)		//����SW12�������߷����
					 {
													 
					 }
					 if(TempPin15Data == 0)		//����SW18�������߷����
					 {
					 
					 }
					 if(TempPin16Data == 0)		//����SW24��������ͣ����
					 {
					 
					 }
					 if(TempPin17Data == 0)		//����SW32����ɲ����
					 {
					 
					 }

					 //��������Ϊ��Ч
					 KeyEffectiv = 0;

					 //���ͷ����������ź���
					 PostBuzzerOn();

					 break;	
				case 0x0003:
					 if(TempPin13Data == 0)		//����SW5������ʼ����
					 {
						StartStepKey = 1;												 
					 }
					 if(TempPin14Data == 0)		//����SW11������������
					 {
						FinishStepKey = 1;												 
					 }
					 if(TempPin15Data == 0)		//����SW17�����Զ���λ
					 {
					 
					 }
					 if(TempPin16Data == 0)		//����SW23�����Զ�����
					 {
					 
					 }
					 if(TempPin17Data == 0)		//����SW31��������
					 {
					 
					 }

					 //��������Ϊ��Ч����ȷ����ʱ�䰴�²���������
					 KeyEffectiv = 0;

					 //���ͷ����������ź���
					 PostBuzzerOn();

					 break;
				case 0x0004:
					 if(TempPin13Data == 0)		//����SW4�������֡�7����
					 {
						NumberKey = 7;

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;												 
					 }
					 if(TempPin14Data == 0)		//����SW10�������֡�4����
					 {
						NumberKey = 4;

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;																		 
					 }
					 if(TempPin15Data == 0)		//����SW16�������֡�1����
					 {
						NumberKey = 1;					 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin16Data == 0)		//����SW22�������֡�0����
					 {
						NumberKey = 0;					 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin17Data == 0)		//����SW30�����˶μ�
					 {
					 
					 }

					 //��������Ϊ��Ч����ȷ����ʱ�䰴�²���������
					 KeyEffectiv = 0;

					 //���ͷ����������ź���
					 PostBuzzerOn();

					 break;
				case 0x0005:
					 if(TempPin13Data == 0)		//����SW3�������֡�8����
					 {
						NumberKey = 8;												 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin14Data == 0)		//����SW9�������֡�5����
					 {
						NumberKey = 5;													 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin15Data == 0)		//����SW15�������֡�2����
					 {
						NumberKey = 2;													 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin16Data == 0)		//����SW21���������
					 {
					 
					 }
					 if(TempPin17Data == 0)		//����SW29���������
					 {
					 
					 }
					 
					 //��������Ϊ��Ч����ȷ����ʱ�䰴�²���������
					 KeyEffectiv = 0;

					 //���ͷ����������ź���
					 PostBuzzerOn();

					 break;
				case 0x0006:
					 if(TempPin13Data == 0)		//����SW2�������֡�9����
					 {
						NumberKey = 9;												 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin14Data == 0)		//����SW8�������֡�6����
					 {
						NumberKey = 6;					 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin15Data == 0)		//����SW14�������֡�3����
					 {
						NumberKey = 3;													 

						//��ʾ�������������Ч��������
						EffectiveNumber = 1;
					 }
					 if(TempPin16Data == 0)		//����SW20������Copy�� ��
					 {
					 
					 }
					 if(TempPin17Data == 0)		//����SW28����ͣ����
					 {
					 
					 }

					 //��������Ϊ��Ч����ȷ����ʱ�䰴�²���������
					 KeyEffectiv = 0;

					 //���ͷ����������ź���
					 PostBuzzerOn();

					 break;	
				case 0x0007:
					 if(TempPin13Data == 0)		//����SW1���������趨��
					 {
						StepSetKey = 1;												 
					 }
					 if(TempPin14Data == 0)		//����SW7���������趨��
					 {
											 
					 }
					 if(TempPin15Data == 0)		//����SW13������-����
					 {
							
					 }
					 if(TempPin16Data == 0)		//����SW19���������
					 {
						EnterKey = 1;
					 }
					 if(TempPin17Data == 0)		//����SW27����������
					 {
					   						
					 }
					 
					 //��������Ϊ��Ч����ȷ����ʱ�䰴�²���������
					 KeyEffectiv = 0;


					 //���ͷ����������ź���
					 PostBuzzerOn();

					 break;					
				
			}		
		}

		//}
	}

}

//��������
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
			//��ֵ����
			EnterKey = 0;

			//��ֵ����
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
			//��ֵ����
			EnterKey = 0;
			
			//��ֵ����
			FinishStepKey = 0;
			
			TempFinishStep = 0;	
			
			//table[2-i]	
		}	
	
	}

	//��������趨����Ч
	if(StepSetKey == 1)
	{
		//����������Ч
		if(EnterKey == 1)
		{
			if(RotationData == 0)
			{
				//���Ƶ�����
				TempStartPoint = TempStartPoint*10 + NumberKey;		
			}
			else if(RotationData == 1)
			{
				//��������
				TempBreadth	= TempBreadth*10 + NumberKey;		
			}
			else if(RotationData == 2)
			{
				//�߾�
				TempWireDiameter = TempWireDiameter*10 + NumberKey;			
			}
			else if(RotationData == 3)
			{
				//������Ȧ��
				TempRotationTotalNumber = TempRotationTotalNumber*10 + NumberKey;			
			}
			else if(RotationData == 4)
			{
				//��ʼ����ʱ��������Ȧ��
				RotationSlowlyNumber = RotationSlowlyNumber*10 + NumberKey;			
			}
			else if(RotationData == 5)
			{
				//��������ʱ��������Ȧ��
				TempRotationFastlyNumber = TempRotationFastlyNumber*10 + NumberKey;			
			}
			else if(RotationData == 6)
			{
				//�������ߵ��ٶ�
				TempHighSpeed = TempHighSpeed*10 + NumberKey;			
			}
			else if(RotationData == 7)
			{
				//�������ߵ��ٶ�
				TempSlowSpeed = TempSlowSpeed*10 + NumberKey;			
			}
			else if(RotationData == 8)
			{
				//����
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
* ��	�ƣ�NumericDisplayVariInit()
* ��	�ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵	�����������ʾ������ʼ��
****************************************************************************/
void DisplayVariInit(void)
{
	pDisplayEvent = NULL;
	
	//����ADת������ź���
	if(pDisplayEvent == NULL) 
	{
		pDisplayEvent = OSSemCreate(0);
	}

	//������ʼ��

}

/****************************************************************************
* ��	�ƣ�WaitSampleEvent()
* ��	�ܣ��ȴ������ź�
* ��ڲ�������
* ���ڲ�������
* ˵	����
****************************************************************************/
void WaitNumericDisplayEvent(void)
{
	//�ȴ����ݽ������
	OSSemPend(pDisplayEvent, 0, &nDisplayEventErr);
}

/****************************************************************************
* ��	�ƣ�PostSampleSem()
* ��	�ܣ����͵����ת�ź�
* ��ڲ�������
* ���ڲ�����
* ˵	����
****************************************************************************/
void PostNumericDisplaySem(void)
{
	OSSemPost(pDisplayEvent);	 
}


/****************************************************************************
* ��	�ƣ�TIM4Start()
* ��	�ܣ�
* ��ڲ�����
* ���ڲ�������
* ˵	������ʱ��4����
****************************************************************************/
void TIM4Start(void)
{
	//�����һ�μ��ж�
	TIM4->CR1 = (1 << 2) |	(1 << 0);			//������������ж�
	TIM4->EGR = 1 << 0;							//���������¼�
	TIM4->CR1 &= ~(1 << 0);						//��ֹ������
	TIM4->CR1 &= ~(1 << 2);						//��ֹ�ж�
	TIM4->SR &= ~(1<<0);        				//����жϱ�־		

	//������ʱ��
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	 //������������ж�	
	TIM_Cmd(TIM4, ENABLE);	
}

/****************************************************************************
* ��	�ƣ�TIM4Stop()
* ��	�ܣ�
* ��ڲ�����
* ���ڲ�������
* ˵	������ʱ��4ֹͣ
****************************************************************************/
void TIM4Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);		//��ֹ������
	TIM_SetCounter(TIM4, 0);	//���������
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);	 //��ֹ�ж�
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //����жϱ�־
}

/****************************************************************************
* ��	�ƣ�Timer3ResetRun()
* ��	�ܣ���ʱ��3��λ������
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM4ResetRun(void)
{
	TIM_SetCounter(TIM4, 0);
   	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);	 //������������ж�	
	TIM_Cmd(TIM4, ENABLE);					 	 //���������
}

/****************************************************************************
* ��	�ƣ�Timer3Init()
* ��	�ܣ�
* ��ڲ�����
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM4_Init(void)
{
		
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	//ʼ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_DeInit(TIM4);

	TIM_TimeBaseStructure.TIM_Period = 5000;  			//7200			// TIM_Period
	TIM_TimeBaseStructure.TIM_Prescaler = 1;		//TIM_Prescaler
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_SetCounter(TIM4, 0);
	TIM_ARRPreloadConfig(TIM4, DISABLE);	  //�Զ���װ��Ԥװ������	

	TIM_Cmd(TIM4, DISABLE);

	TIM_ITConfig(TIM4,TIM_IT_Update, DISABLE);
}

/****************************************************************************
* ��	�ƣ�NumericDisplayProcess()
* ��	�ܣ�������ת�����߳�
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/ 
void DisplayProcess(void) reentrant
{
	//�ȴ���ת�ź�
	OSSemPend(pDisplayEvent, 0, &nDisplayEventErr);

	Display_WriteByte(DisplayPosition, DisplayTable[table[DisplayPosition]],0);
	
	KeysScan();

	DisplayPosition++;

	if(DisplayPosition == 16) DisplayPosition = 0;



}


/****************************************************************************
* ��	�ƣ�TIM4_IRQHandler()
* ��	�ܣ�
* ��ڲ�����
* ���ڲ�������
* ˵	�����˺�������PWM�������Ƶ���ٶȣ�ͨ���ı�PwmPulse������ֵ������PWM����ռ�ձ�
****************************************************************************/

void TIM4_IRQHandler (void) 
{
#if OS_CRITICAL_METHOD == 3  
	OS_CPU_SR  cpu_sr = 0;
#endif

	//�����ж�
	OSIntEnter();
	 
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)	
	{
	
		OS_ENTER_CRITICAL();	

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //����жϱ�־	  

		//������Ŀ����ʾ�ź���
		PostNumericDisplaySem();

		OS_EXIT_CRITICAL();
	}

	//�˳��ж�
	OSIntExit(); 
}


/******************************** END OF FILE *******************************/
