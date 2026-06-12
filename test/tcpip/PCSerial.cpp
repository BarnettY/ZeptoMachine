// Serial.cpp: implementation of the CPCSerial class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "PCSerial.h"
#include "../comon.h"
#include "../trace.h"


//------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPCSerial::CPCSerial()
{
	OnCallConnect = NULL;
	OnCallRead = NULL;
	m_serial.m_pAgent=this;
}

CPCSerial::~CPCSerial()
{
}

//------------消息操作--------
int	CPCSerial::OnConnectMsg(UINT  port,BOOL connect)
{
	return 0;
}
int	CPCSerial::OnSynMsg(UINT  port,int reason)
{
	return 0;
}
int	CPCSerial::OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount)
{
	OnCallRead(pfather,pBuff,ncount);
	return 0;
}

//------------------------------------------------
//与服务器端建立连接
BOOL CPCSerial::Connect()
{
#ifndef WIN32
    CPcSerialAux::EmptyAllSerial();
#endif
	m_serial.OpenPort(m_Tcpport,38400);
	OnCallConnect(pfather,m_serial.m_bOpened);
	return TRUE;
}
int CPCSerial::WritePort(unsigned char *buf,int dwCharToWrite)
{
	return m_serial.WritePort(buf,dwCharToWrite);
}
