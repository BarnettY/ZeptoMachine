// Serial.cpp: implementation of the CPCSocket class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "PCSocket.h"
#include "../trace.h"
#include "GenTcpIP.h"

#ifndef  WIN32
#include <netinet/tcp.h>
#include <netinet/in.h>


#ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
#endif

#ifndef SOCKET_ERROR
    #define SOCKET_ERROR (-1)
#endif
#endif
#include "../linux/fd1.h"

//------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPCSocket::CPCSocket():m_WriteRingBuf()
{
	m_psocket=NULL;
	m_pserver=NULL;
	m_BClose=FALSE;
 }

CPCSocket::~CPCSocket()
{
}

//------------------------------------------------
//与服务器端建立连接
BOOL CPCSocket::Connect()
{
	m_WriteRingBuf.SetWriteBuff();
	wxIPV4address addrReal;
	addrReal.Service(m_Tcpport);
	if ( !addrReal.Hostname(m_remoteHost))
	{
		TRACE("ERROR: couldn't get the address we bound to");
	}
	else
	{
		wxString strtmp=addrReal.IPAddress();
		TRACE("Server listening at %s:%u",strtmp.mb_str(wxConvUTF8), addrReal.Service());
	}
	// Create the socket
	m_pserver = new wxSocketServer(addrReal);
	// We use IsOk() here to see if the server is really listening
	if (! m_pserver->IsOk())
	{
		TRACE("Could not listen at the specified port !");
		return FALSE;
	}

	m_pserver->Notify(false);
	m_pserver->SetFlags(wxSOCKET_WAITALL|wxSOCKET_BLOCK);
	new CSocketThread(this);
	return TRUE;
}
BOOL CPCSocket::Close()
{
	m_BClose=TRUE;
	return TRUE;
}
int CPCSocket::WritePort(unsigned char *buf,int dwCharToWrite)
{
	m_WriteRingBuf.PutRingBuff(buf,dwCharToWrite);
	return TRUE;
}
/*
    This function will check for the events specified in the flags parameter,
    and it will return a mask indicating which operations can be performed.
 */
wxSocketEventFlags CPCSocket::Select(wxSocketEventFlags flags,const timeval *timeout)
{
    if ( m_fd == INVALID_SOCKET )
        return (wxSOCKET_LOST_FLAG & flags);

    struct timeval tv;
    if ( timeout )
        tv = *timeout;
    else
        tv.tv_sec = tv.tv_usec = 0;

    // prepare the FD sets, passing NULL for the one(s) we don't use
    fd_set *preadfds = NULL, *pwritefds = NULL;
    if ( flags & wxSOCKET_INPUT_FLAG )
        preadfds = &readfds;

    if ( flags & wxSOCKET_OUTPUT_FLAG )
        pwritefds = &writefds;


    if ( preadfds )
    {
        wxFD_ZERO(preadfds);
        wxFD_SET(m_fd, preadfds);
    }

    if ( pwritefds )
    {
        wxFD_ZERO(pwritefds);
        wxFD_SET(m_fd, pwritefds);
    }

    wxFD_ZERO(&exceptfds);
    wxFD_SET(m_fd, &exceptfds);

    const int rc = select(m_fd + 1, preadfds, pwritefds, &exceptfds, &tv);

    // check for errors first
    if ( rc == -1 || wxFD_ISSET(m_fd, &exceptfds) )
    {
         return wxSOCKET_LOST_FLAG & flags;
    }

    if ( rc == 0 )
        return 0;

//    wxASSERT_MSG( rc == 1, "unexpected select() return value" );

    wxSocketEventFlags detected = 0;
    if ( preadfds && wxFD_ISSET(m_fd, preadfds) )
    {
            detected |= wxSOCKET_INPUT_FLAG;
    }

    if ( pwritefds && wxFD_ISSET(m_fd, pwritefds) )
    {
            detected |= wxSOCKET_OUTPUT_FLAG;
    }
    return detected & flags;
}

//------------------------------------------------
void CPCSocket::OnConnect(BOOL Connect)
{
	CGenTcpIP::OnConnect(m_pfather,Connect);
}
void CPCSocket::OnRead(BYTE * buf,int len )
{
	CGenTcpIP::OnRead(m_pfather,buf,len);
}
#define BUFFMAXLEN		(1000)			//最大1K
int CPCSocket::ReadWriteSocket()
{
	BYTE *buf=new BYTE[BUFFMAXLEN];
	BYTE *wrbuff;
	int wrcnt=0;
	if (!m_psocket->IsConnected())
	{
		TRACE("ThreadWorker: not connected");
		return 0;
	}
	m_fd=m_psocket->GetSocket();
	while(!m_BClose)
	{

		if (!m_psocket->IsConnected())
		{
			TRACE("ThreadWorker: not connected");
			return 0;
		}
        timeval tv = { 0, 100*1000 };
        wxSocketEventFlags inputflag=wxSOCKET_INPUT_FLAG|wxSOCKET_LOST_FLAG;
        if(m_WriteRingBuf.IsReadBuff())
                inputflag|=wxSOCKET_OUTPUT_FLAG;

		wxSocketEventFlags flag=Select(inputflag,&tv);
		//TRACE("ThreadWorker: Select flag=%d",flag);
		if(flag&wxSOCKET_LOST_FLAG)
		{
			TRACE("ThreadWorker: wxSOCKET_LOST_FLAG error");
			return 0;
		}
		if(flag&wxSOCKET_INPUT_FLAG)
		{
			m_psocket->Read(buf, BUFFMAXLEN);
			if (m_psocket->Error())
			{
                int errid=m_psocket->LastError();
                if((wxSOCKET_TIMEDOUT!=errid)&&(wxSOCKET_WOULDBLOCK!=errid))
                {
                    TRACE("ThreadWorker: Read error=%d",errid);
                    return 0;
                }
            }
			int iCount = m_psocket->LastCount();
			if(iCount>0)
				OnRead(buf,iCount);
		}
		if(flag&wxSOCKET_OUTPUT_FLAG)
		{
			while(m_WriteRingBuf.IsReadBuff())
			{
				if(m_WriteRingBuf.GetRingBuff(wrbuff,wrcnt))
				{
                    TRACE("m_psocket->Write wrcnt=%d",wrcnt);
					m_psocket->Write(wrbuff,wrcnt);
                }
			}
 		}
	}
	m_BClose=FALSE;
	return 0;
}

void CPCSocket::SeverProc()
{

	while(1)
	{
		wxString s = _("OnSocketEvent: ");
		wxSocketBase *sock;
		sock = m_pserver->Accept(true);
		if (!sock)
		{
			TRACE("Error: couldn't accept a new connection");
			continue;
		}

		wxIPV4address addr;
		if ( !sock->GetPeer(addr) )
		{
			TRACE("New connection from unknown client accepted.");
			continue;
		}
		TRACE("New client connection from %s:%u accepted",addr.IPAddress().mb_str(wxConvUTF8), addr.Service());
		sock->Notify(false);
		int  flag=1;
		sock->SetFlags(wxSOCKET_NOWAIT);
#ifndef  WIN32
		sock->SetOption(IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
#endif
		m_psocket=sock;
		OnConnect(TRUE);
		ReadWriteSocket();
		m_psocket->Destroy();
		OnConnect(FALSE);
		m_psocket=NULL;
	}
}
//////////////////////////////////////////////////////////////////////
CSocketThread::CSocketThread(CPCSocket* pfather)
{
	m_pfather=pfather;
	Create();
	SetPriority(100);
	Run();
}

CSocketThread::~CSocketThread()
{
}
void* CSocketThread::Entry()
{
	m_pfather->SeverProc();
	return NULL;
}
