﻿/**************************************************************************************
* 因为emWin显示只支持UTF-8编码格式的中文，如果希望直接显示在Keil直接输入的中文，      *
*            比如使用：GUI_DispStringHCenterAt("流水灯",110,120);                     *
* 该文件必须以UTF-8编码格式，不然中文无法正常显示。                                   *
*                                                                                     *
* 如果只是个别例程出现中文显示乱码（如果所有例程都无法显示中文可能是字库问题），      *
* 把对应的例程文件(比如LEDapp.c)用电脑的记事本软件打开，然后选择另存为，在弹出对      *
* 话框中“保存(S)"按钮的左边有个"编码(E)"选项，选择"UTF-8",然后同样保存为同名称的      *
* C语言文件(覆盖原来文件)，再编译。                                                   *
*                                                                                     *
* 如果编译工程时出现下面类似错误也是该文件编码格式问题,必须把文件保存为UTF-8格式      *
* 再编译                                                                              *
* ..\..\User\app\LEDapp.c(275): error:  #8: missing closing quote                     *
*        GUI_DispStringHCenterAt("娴?姘?鐏?",110,120);                                *
* ..\..\User\app\LEDapp.c(276): error:  #165: too few arguments in function call      *
*        GUI_DispStringHCenterAt("瑙?鎽?鍋?宸?澶?鎵?闇€瑕?瑙?鎽?鏍?鍑?",110,215);     *
* ..\..\User\app\LEDapp.c(276): error:  #18: expected a ")"                           *
*        GUI_DispStringHCenterAt("瑙?鎽?鍋?宸?澶?鎵?闇€瑕?瑙?鎽?鏍?鍑?",110,215);     *
*                                                                                     *
* 修改文件后编译就出错这是Keil5软件问题(Keil4没这问题)，推荐使用其他程序编辑工具，    *
* 只用Keil5完成编译和下载工作。                                                       *
***************************************************************************************
*                      实验平台: 秉火STM32 ISO 开发板                                 *
*                      论    坛: http://www.chuxue123.com                             *
*                      淘    宝: http://firestm32.taobao.com                          *
*                      邮    箱: wildfireteam@163.com                                 *
***************************************************************************************
*/
/**************************************************************************************
*                                                                                     *
*                SEGGER Microcontroller GmbH & Co. KG                                 *
*        Solutions for real time microcontroller applications                         *
*                                                                                     *
***************************************************************************************
*                                                                                     *
* C-file generated by:                                                                *
*                                                                                     *
*        GUI_Builder for emWin version 5.22                                           *
*        Compiled Jul  4 2013, 15:16:01                                               *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG                                *
*                                                                                     *
***************************************************************************************
*                                                                                     *
*        Internet: www.segger.com  Support: support@segger.com                        *
*                                                                                     *
***************************************************************************************
*/
// USER START (Optionally insert additional includes)
#include "includes.h"
#include  "app.h"
// USER END
/**************************************************************************************
*
*       Defines
*
***************************************************************************************
*/
// USER START (Optionally insert additional defines)
#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
static uint16_t ADC_ConvertedValue;
static float data;
static GRAPH_DATA_Handle  	hData;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateADC[] = {
  { FRAMEWIN_CreateIndirect, "ADC Converte", 0, 0, 0, 240, 320, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "ADC DATA", GUI_ID_TEXT0, 70, 30, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "DATA", GUI_ID_TEXT1, 30, 50, 180, 20, 0, 0x64, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
// USER START (Optionally insert additional static code)
/**
  * @brief  使能ADC1和DMA1的时钟，初始化PC.01
  * @param  无
  * @retval 无
  */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
	
	/* Configure PC.01  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC1,输入时不用设置速率
}

/**
  * @brief  配置ADC1的工作模式为MDA模式
  * @param  无
  * @retval 无
  */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  				//内存地址固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				//禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 								//要转换的通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*配置ADC1的通道11为55.	5个采样周期，序列为1 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  ADC1初始化
  * @param  无
  * @retval 无
  */
static void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}
/*
*禁用ADC
*/
static void stopADC(void)
{
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);
	
	/* Disable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, DISABLE);
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, DISABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, DISABLE);
	
	/* Disable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
	
	/* Disable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
}
// USER END

/*********************************************************************
*
*       _UserDraw
*
* Function description
*   This routine is called by the GRAPH object before anything is drawn
*   and after the last drawing operation.
*/
static void _UserDraw(WM_HWIN hWin, int Stage) {
  if (Stage == GRAPH_DRAW_LAST) 
  {
    char acText[] = "ADCConvertedValue (V)";
    GUI_RECT Rect;
    GUI_RECT RectInvalid;
    int FontSizeY;

    GUI_SetFont(&GUI_Font13_ASCII);
    FontSizeY = GUI_GetFontSizeY();
    WM_GetInsideRect(&Rect);
    WM_GetInvalidRect(hWin, &RectInvalid);
    Rect.x1 = Rect.x0 + FontSizeY;
    GUI_SetColor(GUI_YELLOW);
    GUI_DispStringInRectEx(acText, &Rect, GUI_TA_HCENTER, strlen(acText), GUI_ROTATE_CCW);
  }
}

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogADC(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  // USER START (Optionally insert additional variables)
  // USER END
  switch (pMsg->MsgId) {
  case WM_DELETE:
		OS_INFO("ADCapp delete\n");
		Flag_ICON102 = 0;
		stopADC();
		UserApp_Flag = 0;
		WM_DeleteWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_GRAPH0));
		tpad_flag=0;
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'ADC TEST'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
    FRAMEWIN_SetTitleHeight(hItem, 20);
		FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
		FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'ADC DATA'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'DATA'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetText(hItem, "*********");
		TEXT_SetBkColor(hItem,GUI_WHITE);
    TEXT_SetTextColor(hItem, GUI_BLUE);
    // USER START (Optionally insert additional code for further widget initialization)
		/* enable adc1 and config adc1 to dma mode */
		ADC1_Init();
    // USER END
    break;
		// USER START (Optionally insert additional message handling)
  case WM_PAINT:	
		GUI_SetBkColor(APPBKCOLOR);
		GUI_SetColor(APPTEXTCOLOR);
		GUI_Clear();
		GUI_DispStringHCenterAt("电位器电压读取",115,10);
		break;
		// USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*Program Size: Code=134032 RO-data=67916 RW-data=2608 ZI-data=45824  
*       CreateADC TEST
*/
void FUN_ICON102Clicked(void)
{
	WM_HWIN 			hWin;
	WM_HWIN         	hGraph;
	GRAPH_SCALE_Handle 	hScaleVert,hScaleH;
	uint8_t i=0;  
	char pValue[25]={0};
	OS_INFO("ADCapp create\n");
	hWin=GUI_CreateDialogBox(_aDialogCreateADC, GUI_COUNTOF(_aDialogCreateADC), _cbDialogADC, WM_HBKWIN, 0, 0);
	hGraph      = GRAPH_CreateEx (5, 80, 220, 200, WM_GetClientWindow(hWin), WM_CF_SHOW , 0, GUI_ID_GRAPH0);
	hData       = GRAPH_DATA_YT_Create(GUI_RED,  /* 绘制数据要使用的颜色 */
										   250,  /* 数据项的最大数 */
											 0,  /* 指向要添加到对象的数据的指针。该指针应指向一组I16值。*/
											 0); /* 要添加的数据项数。*/
	
	hScaleVert  = GRAPH_SCALE_Create(25,                      /* 相对于图形小工具的左边/顶边的位置。 */        
	                                 GUI_TA_RIGHT,            /* 用于绘制编号的文本对齐方式 */
									 GRAPH_SCALE_CF_VERTICAL, /* 创建垂直刻度对象 */ 
									 50);

	hScaleH = GRAPH_SCALE_Create(190, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
    GRAPH_SCALE_SetTextColor(hScaleH, GUI_YELLOW);

	WIDGET_SetEffect(hGraph, &WIDGET_Effect_Simple);
						   
	/* 设置边框的尺寸 （上、下、左、右） */
	GRAPH_SetBorder       (hGraph,   /* 句柄 */
	                            0,   /* 左边框的尺寸 */
							    0,   /* 上边框的尺寸 */
							    0,   /* 右边框的尺寸 */
								0); /* 下边框的尺寸 */
								
	/* 设置用户绘制函数。此函数由小工具在绘制过程中调用，使应用程序能够绘制用户定义的数据 */							
	GRAPH_SetUserDraw     (hGraph, _UserDraw);

	 GRAPH_SetGridDistY(hGraph, 50);
	 GRAPH_SetGridVis(hGraph, 1);
	 GRAPH_SetGridFixedX(hGraph, 1);
	/* 设置数据的对齐方式 左对齐或者右对齐 */
	GRAPH_DATA_YT_SetAlign(hData,  GRAPH_ALIGN_RIGHT);

	/* 
		置用于计算要绘制的编号的因子
	    1. 不使用因子时，刻度对象的单位为 “像素”。因此给定因子应将像素值转换为所需的单位
	*/
	GRAPH_SCALE_SetFactor   (hScaleVert, 0.02f);
	
	/*
		设置用于在正或负方向上 “移动”刻度对象的偏移。
		1. 水平刻度对象从数据区的底边开始向顶部进行标记，垂直刻度对象从左边 （水平刻度）开始向右边
		   进行标记，其中第一个位置是零点。在许多情况下，不希望第一个位置是零点。如果刻度要向正方
		   向 “移动”，应添加正偏移，要向负方向 “移动”，应添加负值。
	*/
	/* 设置用于绘制编号的文本颜色。*/
	GRAPH_SCALE_SetTextColor(hScaleVert, GUI_YELLOW);
	
	/* 将数据对象附加到现有图形小工具。 */
	GRAPH_AttachData (hGraph, hData);
	/* 将刻度对象附加到现有图形小工具。 */
	GRAPH_AttachScale(hGraph, hScaleVert);
	GRAPH_AttachScale(hGraph, hScaleH);
	while(Flag_ICON102)
	{
		i++;
		if(i>=50)
		{
			data=(float)ADC_ConvertedValue;
			//printf("\r\n1 The current AD value = %f  \r\n", data);
			data=data*3.3/4096;		
			//printf("\r\nThe current AD value = 0x%04X -> %f\r\n", ADC_ConvertedValue,data); 
			sprintf(pValue,"0x%04X --> %fV",ADC_ConvertedValue,data);
			pValue[24]='\0';
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1), pValue);
			GRAPH_DATA_YT_AddValue(hData,data*50);
			i=0;
		}
		if(tpad_flag)WM_DeleteWindow(hWin);
		GUI_Delay(10); 
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
