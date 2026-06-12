// TcpIp.cpp: implementation of the CAllTcpIp class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "AllTcpIp.h"
#include "../Machine/Machine.h"

#include "../trace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAllTcpIp::CAllTcpIp()
{
	m_pSocket=new CGenTcpIP;
	m_pSocket->SetPort(1);
	m_pSocket->SetWriteReadMode(3);	//
}

CAllTcpIp::~CAllTcpIp()
{
	delete m_pSocket;

}

void CAllTcpIp::SetFather( CMachine*pfather)
{
	m_pSocket->m_pAgent=this;
    m_pST=pfather;
}

BOOL CAllTcpIp::IsAllConnect()
{
	return m_pSocket->IsConnect();
}


BOOL CAllTcpIp:: InitSocket(BOOL BSever,wxString strIP,int wrport)
{
	m_pSocket->SetAddress(BSever,strIP,wrport);
	return TRUE;
}
//BOOL CAllTcpIp::InitSocket(wxString strserver,int serverpt,int clientpt)
//{
//	m_pSocket->SetAddress(FALSE,strserver,serverpt);
//	return TRUE;
//}

BOOL CAllTcpIp::Connect(UINT  port)
{
	if(1==port)
	{
		return m_pSocket->ReConnect();
	}
//#ifndef WIN32
//    else
//    { 
//       new CReadThread(); //串口	   串口没有用到
//    }
//#endif
	return TRUE;
}

//处理来的消息
BOOL CAllTcpIp::DealWithMsg(MSG_TYPE type,CBaseMsg *pnew)
{
    return m_pMainTask->DealWithMsg(type,pnew);
 
}

//------------------------通讯几大模式----------------
//设置成初始化模式
BOOL CAllTcpIp::SendInitState(BYTE status)
{
	//CSndInitResult *pinfo=new CSndInitResult;
	//pinfo->m_btCheckStatus=status;
	//return SendMsgNotWait(pinfo);
	return TRUE ;
}
//----------------------------------------
//发送并等待返回
BOOL CAllTcpIp::SendMsgNotWait(CBaseMsg *psendMsg)
{
#ifdef	_NO_CONNECT_PC_
	delete psendMsg;
	return TRUE;
#endif

	MsgBuff tmpBuff;
	if(!psendMsg->PackageBuffer(tmpBuff))
	{
		FASSERT(FALSE);
		return FALSE;
	}
	CProtocol Sendpro;
	Sendpro.AddMsgIntoBuffer(tmpBuff,0,FALSE);
	MsgBuff SendBuff;
	Sendpro.m_MsgType=psendMsg->m_MessageType;
	if(!Sendpro.PackageBuffer(SendBuff))
	{
		FASSERT(FALSE);
		return FALSE;
	}
	m_pSocket->WritePort((BYTE *)SendBuff.GetBuffer(),SendBuff.GetBufferCount());
	SendBuff.ReleaseBuffer();
	delete psendMsg;
	//Sendpro.ReleaseMsgBuffer();
    tmpBuff.ReleaseBuffer();
	return TRUE;
}
BOOL  CAllTcpIp::SendMsgBuff(MsgBuff *pSendBuff)
{
	m_pSocket->WritePort((BYTE *)pSendBuff->GetBuffer(),pSendBuff->GetBufferCount());
	return pSendBuff->ReleaseBuffer();
}

//---------------------能接收消息类型----------------------------
//接收命令筛选
BOOL CAllTcpIp::IsMsgCanRcvNow(MSG_TYPE type)
{
//	if(MSG_PCActiveItem==type)		return TRUE;
	return FALSE;
}

//------------CTcpAgent消息操作--------
int	CAllTcpIp::OnConnectMsg(UINT  port,BOOL connect)
{
	return 0;
}
int	CAllTcpIp::OnSynMsg(UINT  port,int reason)
{
	return 0;
}
int	CAllTcpIp::OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount)
{
	//TRACE(_T("CAllTcpIp OnRead!Port=%d Len=%d"),port,ncount);
	READLOG(port,pBuff,ncount);
	MsgBuff AllMsg(pBuff,ncount);
	CProtocol proto;
	if(!proto.ParseFromBuffer(AllMsg))
	{
		FASSERT(FALSE);
		return FALSE;
	}
	if ((MSG_TYPE)proto.m_MsgType!=MSG_RcvHeartBeat)
	{
		TRACE(_T("CAllTcpIp  OnReceiveMsg type=%s"),FromMsgTypeToString((MSG_TYPE)proto.m_MsgType).mb_str(wxConvUTF8));
	}
	//处理消息
	CBaseMsg *pnew=FromMsgTypeToClass((MSG_TYPE)proto.m_MsgType);
	if(NULL==pnew)		//不知道的命令也要ACK
	{
		CAckMsg *pAck=new CAckMsg;
		SendMsgNotWait(pAck);
		return TRUE;
	}
	////除了ACK,NAK全部应答
	if((MSG_Ack!=proto.m_MsgType)&&(MSG_NAK!=proto.m_MsgType))
	{
		CAckMsg *pAck=new CAckMsg;
		SendMsgNotWait(pAck);
	}
	MsgBuff mBuff(proto.m_MsgBuffer,proto.m_Msglen);
	if(!pnew->ParseFromBuffer(mBuff))
		return TRUE;
		//	FASSERT(FALSE);
	//处理消息
	DealWithMsg((MSG_TYPE)proto.m_MsgType,pnew);
	//delete  pnew;
	proto.m_DeleteMsgBuffer = TRUE;
    proto.ReleaseMsgBuffer(); //FIX 
	return 0;
}

