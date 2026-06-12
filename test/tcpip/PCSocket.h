// Serial.h: interface for the CPCSocket class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_SERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_)
//#define AFX_SERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_
#pragma once
#include "wx/socket.h"
#include "../comon.h"
#include "RingBuff.h"


class CGenTcpIP;
#define FD_HANDLER_ENABLED	1
#define FD_HANDLER_DISABLED	0

class CPCSocket
{
public:
	friend class CSocketThread;
	CPCSocket();
	~CPCSocket();
 	wxString	 m_remoteHost;  					//远程主机IP地址
  	int     	 m_Tcpport;						//远程主机端口
  	CGenTcpIP	*m_pfather;
	BOOL 	Connect();					//与服务器端建立连接
	BOOL	Close();
	int		WritePort(unsigned char *buf,int dwCharToWrite);
	wxSocketEventFlags Select(wxSocketEventFlags flags,const timeval *timeout = NULL);
protected:

	void		OnConnect(BOOL Connect);
	void		OnRead(BYTE * buf,int len );
	int		ReadWriteSocket();
	void 	SeverProc();
	volatile	BOOL 	m_BClose;
	wxSocketBase		*m_psocket;
	wxSocketServer	*m_pserver;
	CRingBuff		m_WriteRingBuf;						//环形写缓冲
	fd_set	readfds;
	fd_set	writefds;
	fd_set	exceptfds;
	wxSOCKET_T	m_fd;
};

/////////////////////////////////////////////////////////////////////////////
class CSocketThread : public wxThread
{
public:
    CSocketThread(CPCSocket* m_pfather);
    virtual ~CSocketThread();
    virtual void* Entry();
protected:
	CPCSocket* m_pfather;
}; // !defined(AFX_SERIAL_H__1F78EA22_307F_4862_A541_FEA74473C33F__INCLUDED_)
