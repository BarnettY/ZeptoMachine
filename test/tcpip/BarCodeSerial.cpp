// Serial.cpp: implementation of the CBarCodeSerial class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BarCodeSerial.h"
//#include "../AutoSamplingSys/AutoSamplingSystem.h"
#include "../comon.h"
#include "../trace.h"


//------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBarCodeSerial::CBarCodeSerial():m_CodeACKSemp(FALSE)
{
	m_serial.m_pAgent=this;
	m_ReadCnt=0;
	m_Start = FALSE;
}

CBarCodeSerial::~CBarCodeSerial()
{
}

//------------消息操作--------
int	CBarCodeSerial::OnConnectMsg(UINT  port,BOOL connect)
{
	return 0;
}
int	CBarCodeSerial::OnSynMsg(UINT  port,int reason)
{
	return 0;
}
int	CBarCodeSerial::OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount)
{
	TRACE(_T("CBarCodeSerial::OnReceiveMsg m_ReadCnt=%d,ncount=%d"),m_ReadCnt,ncount);
	
	if(ncount>0)
	{
		if (m_Start)
		{
		memmove((void *)(m_ReadBuffer+m_ReadCnt),pBuff,ncount);
		m_ReadCnt+=ncount;
		}
		m_CodeACKSemp.SetEvent();
	}
	return 0;
}

//------------------------------------------------
//与服务器端建立连接
BOOL CBarCodeSerial::OpenPort(int port,int speed)
{
	return m_serial.OpenPort(port,speed);
}
BOOL CBarCodeSerial::StartRead()
{
	m_ReadCnt=0;
	m_Start = TRUE;
	m_CodeACKSemp.ResetEvent();
	unsigned char buf[]={0x01,0x54,0x04};         //01 54 04
	m_serial.WritePort(buf,3);
	return TRUE;
}
BOOL CBarCodeSerial::StopRead()
{
	unsigned char buf[]={0x1B ,0x50,0x04};
	m_serial.WritePort(buf,3);
	m_Start = FALSE;
	return TRUE;
}
BOOL CBarCodeSerial::ReadBarCode(wxString& strBarCode,int iWaitTime)
{
	strBarCode="";
	DWORD dwState=m_CodeACKSemp.WaitTimeout(iWaitTime);
	if(wxSEMA_NO_ERROR!=dwState)
		return FALSE;
	strBarCode=wxString(m_ReadBuffer,m_ReadCnt);
	strBarCode.Trim();
	return TRUE;
}
int CBarCodeSerial::WritePort(unsigned char *buf,int dwCharToWrite)
{
	return m_serial.WritePort(buf,dwCharToWrite);
}
