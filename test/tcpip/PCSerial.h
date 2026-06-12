// Serial.h: interface for the CPCSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_)
#define AFX_SERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_

#ifdef  WIN32
#include "../windows/PcSerialAux.h"
#else
#include "../linux/PcSerialAux.h"
#define CALLBACK
#endif
//-----------------------CSocketComm回调函数未使用-------------------
//定义连接事件
typedef void (CALLBACK* ONCONNECT)(void *pfather,BOOL Connect);
//定义当有数据接收事件
typedef void (CALLBACK* ONREAD)(void *pfather,const BYTE * buf,int len );


class CPCSerial:public CTcpAgent
{
public:
	CPCSerial();
	~CPCSerial();
   	int     	 m_Tcpport;						//远程主机端口
	BOOL 	Connect();						//与服务器端建立连接
	BOOL Close( void )
	{
		return	m_serial.Close();
	}
	int WritePort(unsigned char *buf,int dwCharToWrite);
	//---------------------CSocketComm未使用---------------------------
	wxString	 m_remoteHost;  					//远程主机IP地址
	void SetServerState(bool bServer)			// Run as server mode if true
	{
	}
	//-----------------------CSocketComm回调函数未使用-------------------
	ONCONNECT		OnCallConnect;			//建立连接事件，回调函数
	ONREAD       		OnCallRead;				//接收数据事件，回调函数
	void				*pfather;					//回调函数父类


	//------------CTcpAgent消息操作--------
	virtual	int		OnConnectMsg(UINT  port,BOOL connect);
	virtual	int		OnSynMsg(UINT  port,int reason);
	virtual	int		OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount);
protected:
	CPcSerialAux m_serial;
};

#endif // !defined(AFX_SERIAL_H__1F78EA22_307F_4862_A541_FEA74473C33F__INCLUDED_)
