
// Protocol.cpp : implementation file
//

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "AllMsgClass.h"

/////////////////////////////////////////////////////////////////////////////
// CProtocol
CProtocol::~CProtocol()
{
	if(m_DeleteMsgBuffer)
		ReleaseMsgBuffer();		
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProtocol::ReadBuffer(WORD &Date,MsgBuff &msgbuf)
{
	UINT temp=0;
	if(msgbuf.m_MsgPoint+2>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}
BOOL CProtocol::ReadBuffer(BYTE &Date,MsgBuff &msgbuf)
{
	BYTE temp=0;
	if(msgbuf.m_MsgPoint+1>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}
BOOL  CProtocol::ReadBufferP(BYTE* &Date,UINT len,MsgBuff &msgbuf)
{
	if(msgbuf.m_MsgPoint+len>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	Date=new BYTE [len+10];
	memcpy(Date,msgbuf.m_MsgBuf+msgbuf.m_MsgPoint,len);
	msgbuf.m_MsgPoint+=len;
	return TRUE;
}

//-------------------------------------------------------------------
BOOL CProtocol::PutBuffer(WORD Date,MsgBuff &msgbuf)
{
	UINT temp=Date;
	if(msgbuf.m_MsgPoint+2>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp>>8)&0xFF;
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp)&0xFF;
	return TRUE;
}
BOOL CProtocol::PutBuffer(BYTE Date,MsgBuff &msgbuf)
{
	BYTE temp=Date;
	if(msgbuf.m_MsgPoint+1>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp)&0xFF;
	return TRUE;
}
BOOL CProtocol::PutBufferP(BYTE* &Date,UINT len,MsgBuff &msgbuf)
{
	if(msgbuf.m_MsgPoint+len>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	if(len>0)
	{
		memcpy(msgbuf.m_MsgBuf+msgbuf.m_MsgPoint,Date,len);
		msgbuf.m_MsgPoint+=len;
	}
	return TRUE;
}

// CProtocol message handlers
//-------------------------------------------------------------------
BOOL CProtocol::AddMsgIntoBuffer(MsgBuff &msgbuf,int RESN,BOOL DeleteMsgBuffer)
{
	m_ReturnID=RESN;
	m_Msglen=msgbuf.m_MsgCount;
	m_MsgBuffer=msgbuf.m_MsgBuf;
	m_DeleteMsgBuffer=DeleteMsgBuffer;
	return TRUE;
}

BOOL CProtocol::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	if(msgbuf.GetBufferCount()<6)
		return FALSE;
	
 	ReadBuffer(m_COM_SYN1,msgbuf);
	FASSERT(0X33==m_COM_SYN1);
 	ReadBuffer(m_COM_SYN2,msgbuf);
 	FASSERT(0X44==m_COM_SYN2);
	ReadBuffer(m_DST,msgbuf);
 	FASSERT(0X10==m_DST);		//是发给下位机的
	//cmd有些有扩展,需特殊对待
	BYTE	CMD;
 	ReadBuffer(CMD,msgbuf);
	ReadBuffer(m_Msglen,msgbuf);	//Message Length
 	if(msgbuf.GetBufferCount()-2<m_Msglen)
		return FALSE;
	//对某些命令的扩展
	switch(CMD)
	{
		case 0xec:
		case 0x41:
		case 0xff:
		case 0xb6:
			{
				BYTE	childCMD;
 				ReadBuffer(childCMD,msgbuf);
				m_MsgType=CMD;
				m_MsgType<<=8;
				m_MsgType+=childCMD;
				m_Msglen-=1;				
			}
			break;
		case 0x15:		//这个指令扩展出两位，但实际只用到1位
			{
				BYTE	childCMD;
 				ReadBuffer(childCMD,msgbuf);
				ReadBuffer(childCMD,msgbuf);
				m_MsgType=CMD;
				m_MsgType<<=8;
				m_MsgType+=childCMD;
				m_Msglen-=2;				
			}
			break;			
		default:
			m_MsgType=CMD;
			break;
	}
	ReadBufferP(m_MsgBuffer,m_Msglen,msgbuf);	//Message
	return msgbuf.PointNotExceed();
}

BOOL CProtocol::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();

 	PutBuffer(m_COM_SYN1,msgbuf);	
 	PutBuffer(m_COM_SYN2,msgbuf);	
 	PutBuffer(m_DST,msgbuf);	
	BYTE ExternCmd=(m_MsgType>>8)&0xff;
	BYTE CMD=m_MsgType&0xff;
	if(0x15==ExternCmd)	//这个指令扩展出两位，但实际只用到1位
	{
		PutBuffer(ExternCmd,msgbuf);
		m_Msglen+=2;
	}
	else if(ExternCmd>0)
	{
		PutBuffer(ExternCmd,msgbuf);
		m_Msglen+=1;
	}
	else
	{
		PutBuffer(CMD,msgbuf);
	}
	PutBuffer(m_Msglen,msgbuf);	//Message Length
	if(0x15==ExternCmd)	//这个指令扩展出两位，但实际只用到1位
	{
		BYTE Zero=0;
		PutBuffer(Zero,msgbuf);
		PutBuffer(CMD,msgbuf);
	}
	else if(ExternCmd>0)
	{
		PutBuffer(CMD,msgbuf);
	}
	
	//Message Type determines this content
	PutBufferP(m_MsgBuffer,m_Msglen,msgbuf);	//Message
	
	msgbuf.SubmitPoint();
	WORD CHECK=msgbuf.GetCheckSum(msgbuf.m_MsgPoint);
	PutBuffer(CHECK,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}
