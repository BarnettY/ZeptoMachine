// CTcpCanInterface.cpp: implementation of the CTcpCanInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "TcpCanInterface.h"
#include "AxisParamInfo.h"
#include "CanCom.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////TCP2CAN/////////////////////////////////
CTcpCanInterface::CTcpCanInterface()
{
}

CTcpCanInterface::~CTcpCanInterface()
{

}
BOOL CTcpCanInterface::ReceiveCan(CAN_OBJ frameinfo[50],int &Count)
{
#ifdef _SIMULATOR_
	Count=0;
	wxMilliSleep(200);
	return FALSE;
#else
	return TcpReceiveCan(0,frameinfo, Count);
#endif
}
BOOL CTcpCanInterface::SendCan(CAN_OBJ frameinfo[50],int Count)
{
	for(int i=0;i<Count;i++)
	{
		BYTE ctrlID=FrameIDInfo::ParseCtrlID(frameinfo[i].ID);
		if(ctrlID<MAX_ACTION_CNT)
			TcpSendCan(0,&frameinfo[i], 1);
		else
			TcpSendCan(1,&frameinfo[i], 1);
	}
	return TRUE;
}

BOOL CTcpCanInterface::OpenCan(Baud_TYPE baud)
{
	return TcpOpenCan(0);//&TcpOpenCan(1)
}
BOOL CTcpCanInterface::CloseCan()
{
	return TcpCloseCan(0)&TcpCloseCan(1);
}
BOOL CTcpCanInterface::StarCan()
{
	return TRUE;
}
BOOL CTcpCanInterface::ResetCan()
{
	return TcpResetCan(0)&TcpResetCan(1);
}
/////////////////////TCP2COM/////////////////////////////////
CTcpComInterface::CTcpComInterface()
{
}

CTcpComInterface::~CTcpComInterface()
{

}
BOOL CTcpComInterface::ComClose(BYTE index)
{
	return TRUE;
}
BOOL CTcpComInterface::ComOpen(BYTE index)
{
	return TRUE;
}
BOOL CTcpComInterface::ComSend(BYTE index,BYTE *buff,int Count)
{
	return TcpComSend(index,buff,Count);
}
BOOL CTcpComInterface::ComReceive(BYTE index,BYTE *buff,int&Count)
{
#ifdef _SIMULATOR_
	return FALSE;
#else
	return TcpComReceive(index,buff,Count);
#endif
}
