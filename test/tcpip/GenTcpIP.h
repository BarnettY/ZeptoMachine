//
//////////////////////////////////////////////////////////////////////
//[fan 11-04-01] TCP_IP 连接
#if !defined(AFX_GenTcpIPServer_H__7D32D812_22B4_416A_A502_99C73A11DCCA__INCLUDED_)
#define AFX_GenTcpIPServer_H__7D32D812_22B4_416A_A502_99C73A11DCCA__INCLUDED_

#include "GenCommunicate.h"
#include "PCSerial.h"

class CTcpAgent;
class CGenTcpIP: public CGenCommunicate 
{
public:
	CTcpAgent*	m_pAgent;			//消息代理
	friend class CAllTcpIp;
	friend class CTraceLogTcp;
	CGenTcpIP();
	virtual ~CGenTcpIP();
	void SetNeedParse(BOOL bparse)
	{
		m_NeedParseDate=bparse;
	}
	//////////////////////////////////////////////////////////////////////////////
	//	接口函数
	//////////////////////////////////////////////////////////////////////////////
	//建立连接
	BOOL SetAddress(BOOL BSever,wxString strIP,int tcpipport);		//建立连接
	//断开连接
	BOOL ClosePort();
	//写端口数据
	virtual DWORD WritePort(BYTE buf[],DWORD dwCharToWrite);

	//////////////////////////////////////////////////////////////////////////////
	//	SOCKET 函数
	//////////////////////////////////////////////////////////////////////////////
public:
	//建立一个新的连接
	virtual BOOL ReConnect();
	virtual BOOL SendSocketDate(const BYTE * buf,int len );
	virtual BOOL OnCannotRcvAck();					//发送到数据没有收到ACK
	virtual BOOL OnParseData(BYTE *pBuff,UINT  ncount);			//已经解析好的数据
	//////////////////////////////////////////////////////////////////////////////
	//	SOCKET 消息触发
	//////////////////////////////////////////////////////////////////////////////
	static void CALLBACK	OnConnect(void *pfather,BOOL Connect);			//连接建立回调函数
	static void CALLBACK	OnRead(void *pfather,const BYTE * buf,int len );		//接收数据事件，回调函数
protected:
	//CPCSerial m_socket;			//定义CSocket对象
	BOOL		m_NeedParseDate;	//收到的消息需要解析	
	//	wxCriticalSection *m_WriteMute;									//读写锁
};

#endif // !defined(AFX_GenTcpIPServer_H__7D32D812_22B4_416A_A502_99C73A11DCCA__INCLUDED_)
