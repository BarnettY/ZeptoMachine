// Serial.cpp: implementation of the CTemptureSerial class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "TemptureSerial.h"
#include "../Machine/Machine.h"
#include "../comon.h"
#include "../trace.h"


//------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTemptureSerial::CTemptureSerial():m_TempACKSemp(FALSE)
{
	m_serial.m_pAgent=this;
	m_ReadCnt=0;
	m_temp =0;
}

CTemptureSerial::~CTemptureSerial()
{
}

//------------消息操作--------
int	CTemptureSerial::OnConnectMsg(UINT  port,BOOL connect)
{
	return 0;
}
int	CTemptureSerial::OnSynMsg(UINT  port,int reason)
{
	return 0;
}
int	CTemptureSerial::OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount)
{
//	TRACE(_T("CTemptureSerial::OnReceiveMsg m_ReadCnt=%d,ncount=%d"),m_ReadCnt,ncount);
	if(m_ReadCnt+ncount>=29)
	{
		TRACE(_T("CTemptureSerial::OnReceiveMsg discard !Len=%d"),m_ReadCnt);
		m_ReadCnt=0;
		return FALSE;
	}
	if(ncount>0)
	{
		memmove((void *)(m_ReadBuffer+m_ReadCnt),pBuff,ncount);
		m_ReadCnt+=ncount;
		//开始校验收到的命令
		//6C 01 72 01 18 60 72 01 69 64 
		if(10==m_ReadCnt)
		{
			WORD add=0;
			for(int i=0;i<4;i++)
			{
				WORD temp=(m_ReadBuffer[2*i+1]<<8)+m_ReadBuffer[2*i];
				add+=temp;
			}
			add+=1;
			WORD check=(m_ReadBuffer[9]<<8)+m_ReadBuffer[8];
			if(add==check) //校验成功
			{
				int temp=(m_ReadBuffer[1]<<8)+m_ReadBuffer[0];
//				GLOBAL(m_pRunCondition)->m_RRVTemp=temp/10.0f;
				m_temp =temp/10.0f;
				m_TempACKSemp.SetEvent();
			}
		}
	}
	return 0;
}

//------------------------------------------------
//与服务器端建立连接
BOOL CTemptureSerial::Connect(int port)
{
#ifndef WIN32
	m_serial.OpenPort(port,9600);
#else
	
#endif
	return TRUE;
}
BOOL CTemptureSerial::ReadTemp(float &temp)
{
	m_ComeMute.Enter();
	m_ReadCnt=0;
	m_TempACKSemp.ResetEvent();
	unsigned char buf[]={0x81 ,0x81 ,0x52 ,0x00 ,0x00 ,0x00 ,0x53 ,0x00};
	m_serial.WritePort(buf,8);
	DWORD dwState=m_TempACKSemp.WaitTimeout(1*1000);
	m_ComeMute.Leave();
	if(wxSEMA_NO_ERROR!=dwState)
		return FALSE;
	temp= m_temp;
	return TRUE;
}
BOOL CTemptureSerial::SetTemp(float temp)
{
	m_ComeMute.Enter();
	WORD settemp=temp*10;
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	cmd[4]=settemp%256;
	cmd[5]=settemp/256;
	cmd[6]=(67+settemp+1)%256;
	cmd[7]=(67+settemp+1)/256;
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}
BOOL CTemptureSerial::SetOffset(float fOffset)
{
	m_ComeMute.Enter();
	int offset=fOffset*10;
	WORD setoffset=offset;
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x10 ,0x00 ,0x00 ,0x00 ,0x00};
	cmd[4]=setoffset%256;
	cmd[5]=setoffset/256;
	cmd[6]=(0x10*256+67+offset+1)%256;
	cmd[7]=(0x10*256+67+offset+1)/256;
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}
BOOL CTemptureSerial::SetCHYS(float fCHYS)
{
	m_ComeMute.Enter();
	int chys=fCHYS*10;
	WORD setchys=chys;
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x1C ,0x00 ,0x00 ,0x00 ,0x00};
	cmd[4]=setchys%256;
	cmd[5]=setchys/256;
	cmd[6]=(0x1C*256+67+setchys+1)%256;
	cmd[7]=(0x1C*256+67+setchys+1)/256;
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}
BOOL CTemptureSerial::SetPID(WORD P,WORD I,WORD D)
{
	m_ComeMute.Enter();
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	cmd[3]=0x07;
	cmd[4]=P%256;
	cmd[5]=P/256;
	cmd[6]=(0x07*256+67+P+1)%256;
	cmd[7]=(0x07*256+67+P+1)/256;
	WritePort(cmd,8);
	wxMilliSleep(100);
	cmd[3]=0x08;
	cmd[4]=I%256;
	cmd[5]=I/256;
	cmd[6]=(0x08*256+67+I+1)%256;
	cmd[7]=(0x08*256+67+I+1)/256;
	WritePort(cmd,8);
	wxMilliSleep(100);
	cmd[3]=0x09;
	cmd[4]=D%256;
	cmd[5]=D/256;
	cmd[6]=(0x09*256+67+D+1)%256;
	cmd[7]=(0x09*256+67+D+1)/256;
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}
BOOL CTemptureSerial::ReadPID(WORD &P,WORD &I,WORD &D)
{
	m_ComeMute.Enter();
	m_ReadCnt=0;
	m_TempACKSemp.ResetEvent();
	unsigned char cmd[]={0x81 ,0x81 ,0x52 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
	cmd[3]=0x07;
	cmd[6]=0x53;
	cmd[7]=0x07;
	m_serial.WritePort(cmd,8);
	DWORD dwState=m_TempACKSemp.WaitTimeout(1*1000);
	if(wxSEMA_NO_ERROR!=dwState)
	{
		m_ComeMute.Leave();
		return FALSE;
	}
	P=(m_ReadBuffer[7]<<8)+m_ReadBuffer[6];

	m_ReadCnt=0;
	m_TempACKSemp.ResetEvent();
	cmd[3]=0x08;
	cmd[6]=0x53;
	cmd[7]=0x08;
	m_serial.WritePort(cmd,8);
	dwState=m_TempACKSemp.WaitTimeout(1*1000);
	if(wxSEMA_NO_ERROR!=dwState)
	{
		m_ComeMute.Leave();
		return FALSE;
	}
	I=(m_ReadBuffer[7]<<8)+m_ReadBuffer[6];

	m_ReadCnt=0;
	m_TempACKSemp.ResetEvent();
	cmd[3]=0x09;
	cmd[6]=0x53;
	cmd[7]=0x09;
	m_serial.WritePort(cmd,8);
	dwState=m_TempACKSemp.WaitTimeout(1*1000);
	if(wxSEMA_NO_ERROR!=dwState)
	{
		m_ComeMute.Leave();
		return FALSE;
	}
	D=(m_ReadBuffer[7]<<8)+m_ReadBuffer[6];

	m_ComeMute.Leave();
	return TRUE;
}
int CTemptureSerial::WritePort(unsigned char *buf,int dwCharToWrite)
{
	return m_serial.WritePort(buf,dwCharToWrite);
}

//地址代号+43H（67）+要写的参数代号+写入数低字节+写入数高字节+校验码
BOOL   CTemptureSerial:: StarHeat()  // 1BH   0 运行  1 停止
{
	m_ComeMute.Enter();
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x1b ,0x00 ,0x00 ,0x00 ,0x00}; //81 81 43 1B 00 00 44 1B
    
	cmd[6]=(67+27*256+0+1)%256;  //要写的参数代号×256+67+要写的参数值+ADDR   44
	cmd[7]=(67+27*256+0+1)/256;  // 27   1b
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}

   
BOOL   CTemptureSerial::  StopHeat()
{
	m_ComeMute.Enter();
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x1b ,0x01 ,0x00 ,0x00 ,0x00};//81 81 43 1B 01 00 45 1B

	cmd[6]=(67+27*256+1+1)%256; // 6981%256  69    45H
	cmd[7]=(67+27*256+1+1)/256;  //27     1BH 
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}


BOOL  CTemptureSerial::   SetPT100()
{
	m_ComeMute.Enter();
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x0b ,0x15 ,0x00 ,0x00 ,0x00}; //81 81 43 0B 15 00 59 0B
	cmd[6]=(67+11*256+21+1)%256; // 89
	cmd[7]=(67+11*256+21+1)/256;  //11
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;
}



///PonP   设置上电STOP      1BH  设置启动停止
BOOL   CTemptureSerial::  SetPonP(BYTE Value )
{

	m_ComeMute.Enter();
	BYTE cmd[8]={0x81 ,0x81 ,0x43 ,0x2C ,0x00 ,0x00 ,0x00 ,0x00}; //81 81 43 2C 01 00 45 2C
    cmd[4]=Value%256;

	cmd[6]=(67+0x2C*256+1+1)%256; // 69
	cmd[7]=(67+0x2C*256+1+1)/256;  //44
	WritePort(cmd,8);
	m_ComeMute.Leave();
	return TRUE;




}