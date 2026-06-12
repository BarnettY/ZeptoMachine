// TcpIp.h: interface for the CAllTcpIp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPIP_H__BF9DE484_FEBA_46F6_AB6E_E3BFDFD4CB39__INCLUDED_)
#define AFX_TCPIP_H__BF9DE484_FEBA_46F6_AB6E_E3BFDFD4CB39__INCLUDED_

#include "GenTcpIP.h"
#include "../protocol/AllMsgClass.h"

#include "../test.h"
#define WAIT_TIME_SHORT			(6*1000)

class CMachine;
class CAllTcpIp:public CTcpAgent
{
public:
	CAllTcpIp();
	virtual ~CAllTcpIp();
	void SetFather(CMachine *pfather);

	BOOL Connect(UINT  port);
	//BOOL InitSocket(wxString strserver,int serverpt,int clientpt);

    BOOL InitSocket(BOOL BSever,wxString strIP,int wrport);
	BOOL DealWithMsg(MSG_TYPE type,CBaseMsg *pnew);//处理来的消息
	BOOL IsAllConnect();
	//------------CTcpAgent消息操作--------
	virtual	int		OnConnectMsg(UINT  port,BOOL connect);
	virtual	int		OnSynMsg(UINT  port,int reason);
	virtual	int		OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount);

	//------------------------通讯几大模式----------------
	BOOL SendInitState(BYTE status);							//设置成初始化模式
	//------------------------通讯子函数----------------
	//发送消息封装
	BOOL SendMsgNotWait(CBaseMsg *psendMsg);		//发送不等待返回
	BOOL SendMsgBuff(MsgBuff *pSendBuff);			//发送不等待返回

   
	BOOL   m_bCV ; // 标记CV连接。 
protected:
	CGenTcpIP 		*m_pSocket;
	CMachine 		*m_pST;
	//---------------------能接收消息类型----------------------------
	BOOL	IsMsgCanRcvNow(MSG_TYPE type);				//接收命令刷选
};

#endif // !defined(AFX_TCPIP_H__BF9DE484_FEBA_46F6_AB6E_E3BFDFD4CB39__INCLUDED_)
