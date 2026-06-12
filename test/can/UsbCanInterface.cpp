// UsbCanInterface.cpp: implementation of the UsbCanInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "../comon.h"
#include "UsbCanInterface.h"

#pragma comment(lib,"ECanVci.lib")


#define DEV_TYPE	USBCAN1	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UsbCanInterface::UsbCanInterface()
{

}

UsbCanInterface::~UsbCanInterface()
{

}
BOOL UsbCanInterface::ReceiveCan(CAN_OBJ frameinfo[50],int &Count)
{
	Count=1;
	Count=Receive(DEV_TYPE,0,0,frameinfo,1,10);
	if(Count<=0)
	{
		return FALSE;
	}                                                                              
	else
		return TRUE;
}
BOOL UsbCanInterface::SendCan(CAN_OBJ frameinfo[50],int Count)
{
	if(Transmit(DEV_TYPE,0,0,frameinfo,Count)!=1)
		return FALSE;
	return TRUE;
}
/*
	M1=0,
	K800,
	K500,
	K250,
	K125,
	K100,
	k50,
	K20,
	K10
*/
const UCHAR g_tim0[10]=
{
	0X00,		//1000Kbps
	0X00,		// 800Kbps
	0X00,		// 500Kbps
	0X01,		// 250Kbps
	0X03,		// 125Kbps
	0X04,		// 100Kbps
	0X09,		// 50Kbps
	0X18,		// 20Kbps
	0X31		// 10Kbps
};
const UCHAR g_tim1[10]=
{
	0X14,		//1000Kbps
	0X16,		// 800Kbps
	0X1C,		// 500Kbps
	0X1C,		// 250Kbps
	0X1C,		// 125Kbps
	0X1C,		// 100Kbps
	0X1C,		// 50Kbps
	0X1C,		// 20Kbps
	0X1C		// 10Kbps
};
BOOL UsbCanInterface::OpenCan(Baud_TYPE baud)
{

	DWORD code=0;
	DWORD mask=0xffffffff; 
	UCHAR filtertype=0;
	UCHAR mode=0;
	UCHAR timing0=g_tim0[(int)baud];
	UCHAR timing1=g_tim1[(int)baud];
	INIT_CONFIG init_config;
	
	//	memset(&init_config,0,sizeof(VCI_INIT_CONFIG));
	//init_config.AccMask = 0xffffffff; //屏蔽码,表示全部接接收
	//init_config.Mode = 0; //正常模式
	//init_config.Timing0 = 0x00; //定时器0，和波特率有关，1M波特率
	//init_config.Timing1 = 0x14; //定时器1

	init_config.AccCode=code; 
	init_config.AccMask=mask;
	init_config.Filter=filtertype;
	init_config.Mode=mode;
	init_config.Timing0=(UCHAR)timing0;
	init_config.Timing1=(UCHAR)timing1;
	
	if(OpenDevice(DEV_TYPE,0,0)!=STATUS_OK)
	{
		CanTraceLog(_T("打开设备失败!"));
		return FALSE;
	}
	if(InitCAN(DEV_TYPE,0,0,&init_config)!=STATUS_OK)
	{
		CanTraceLog(_T("初始化CAN失败!"));
		CloseDevice(DEV_TYPE,0);
		return FALSE;
	}
	return TRUE;
}
BOOL UsbCanInterface::CloseCan()
{
	CloseDevice(DEV_TYPE,0);
	return TRUE;
}
BOOL UsbCanInterface::StarCan()
{
	if(StartCAN(DEV_TYPE,0,0)==1)
	{
		CanTraceLog(_T("启动成功"));
		return TRUE;
	}
	else
	{
		CanTraceLog(_T("启动失败"));
		return FALSE;
	}
}
BOOL UsbCanInterface::ResetCan()
{
/*	if(VCI_ResetCAN(DEV_TYPE,0,0)==1)
	{
		CanTraceLog("复位成功");
		return TRUE;
	}
	else
	{
		CanTraceLog("复位失败");
		return TRUE;
	}*/
	return TRUE;
}

