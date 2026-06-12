// TcpIpSerial.cpp: implementation of the CGenTcpIP class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "../comon.h"
#include "GenTcpIP.h"
#include "../trace.h"

CGenTcpIP::CGenTcpIP()
{
	m_NeedParseDate=TRUE;
	m_pAgent=NULL;
//	m_WriteMute=new wxCriticalSection();
}
CGenTcpIP::~CGenTcpIP()
{
//	delete m_WriteMute;
}

//建立连接
BOOL CGenTcpIP::SetAddress(BOOL BSever,wxString strIP,int tcpipport)
{
	TRACE(_T("CGenTcpIP::SetAddress m_nPort=%d BSever=%d ,port=%d"),m_nPort,BSever,tcpipport);
	InitCommunicate();

	m_bServer =BSever;
	m_remoteHost = strIP;
	m_Tcpport =tcpipport;

	//设置m_tcpClient属性
	//m_socket.m_remoteHost = strIP;
	//m_socket.m_Tcpport = tcpipport;
	//m_socket.pfather=this;
	//m_socket.OnCallRead = OnRead;
	//m_socket.OnCallConnect = OnConnect;
	//m_socket.SetServerState(BSever>0);	// run as server
	//建立与服务器端连接
	return TRUE;
}

//断开连接
BOOL CGenTcpIP::ClosePort()
{
	TRACE(_T("CGenTcpIP SN=%d ClosePort"),m_nPort);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////

//写端口数据
DWORD CGenTcpIP::WritePort(BYTE buf[],DWORD dwCharToWrite)
{
	//WRITELOG(m_nPort,buf,dwCharToWrite);
	return WriteDate(buf,dwCharToWrite);
/*	m_WriteMute->Enter();
	int rtn=m_socket.WritePort(buf,dwCharToWrite);
	m_WriteMute->Leave();
	return rtn;*/

}

//////////////////////////////////////////////////////////////////////////////
//	SOCKET 函数
//////////////////////////////////////////////////////////////////////////////
//建立一个新的连接
BOOL CGenTcpIP::ReConnect()
{
//	m_socket.Connect();
	TRACE(_T("CGenTcpIP SN=%d ReConnect"),m_nPort);
	
	CGenCommunicate::ConnectPort();
	//如果上次没有关闭连接则关闭连接
	return TRUE;
}

//发送到数据没有收到ACK
BOOL CGenTcpIP::OnCannotRcvAck()
{
	TRACE(_T("CGenTcpIP SN=%d OnCannotRcvAck"),m_nPort);
	//没有收到ACK
	FASSERT(m_pAgent);
	return TRUE;
}

//已经解析好的数据
BOOL CGenTcpIP::OnParseData(BYTE *pBuff,UINT  ncount)
{
	FASSERT(m_pAgent);
	return m_pAgent->OnReceiveMsg(m_nPort,pBuff,ncount);
}
//向SOCKET发送数据
BOOL CGenTcpIP::SendSocketDate(const BYTE * buf,int len )
{
	TRACE(_T("CGenTcpIP::SendSocketDate Len=%d,data:%0.2x %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x"),
		len,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8]);
//	return m_socket.WritePort((BYTE * )buf,len);//SendData(buf,len);
	return CGenCommunicate::SendSocketDate(buf,len);

}

//////////////////////////////////////////////////////////////////////////////
//	SOCKET 消息触发
//////////////////////////////////////////////////////////////////////////////
//接收数据事件，回调函数
void CALLBACK CGenTcpIP::OnRead(void *pfather1,const BYTE * buf,int len )
{
	CGenTcpIP *pfather=(CGenTcpIP *)pfather1;
	TRACE(_T("\nCGenTcpIP::OnRead=%d len=%d"),pfather->m_nPort,len);
	if(pfather->m_NeedParseDate)
		pfather->ReadDate(buf,len );
	else	//LOG类数据无需解析
		pfather->OnParseData((BYTE *)buf,len );
}

//连接建立回调函数
void  CALLBACK CGenTcpIP::OnConnect(void *pfather1,BOOL Connect1)
{
	CGenTcpIP *pfather=(CGenTcpIP *)pfather1;
	TRACE(_T("\nCGenTcpIP SN=%d %s"),pfather->m_nPort,Connect1?_T("OnConnect"):_T("DisConnect"));
	pfather->m_Connect=Connect1;
	FASSERT(pfather->m_pAgent);
	pfather->m_pAgent->OnConnectMsg(pfather->m_nPort,Connect1);
}

