// TraceLogTcp.cpp: implementation of the CTraceLogTcp class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "test.h"

#ifndef  WIN32
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif

#ifndef		LOG_FILE

#include "TraceLogTcp.h"

#ifdef TRACE
#undef TRACE
#endif

#define TRACE	wxLogMessage

#define	VER_H	1
#define	VER_M	0
#define	VER_L	43

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTraceLogTcp::CTraceLogTcp()
{
	m_psocket=NULL;
	m_pserver=NULL;
	m_BConnect=FALSE;
	m_BClose=FALSE;
}

CTraceLogTcp::~CTraceLogTcp()
{

}
BOOL CTraceLogTcp::Connect(wxString insIP,int insports)
{
	wxIPV4address addrReal;
	addrReal.Service(insports);
	//if ( !addrReal.Hostname(insIP))
	//{
	//	TRACE("ERROR: couldn't get the address we bound to");
	//}
	//else
	//{
		//wxString strtmp=addrReal.IPAddress();
		//TRACE("Server listening at %s:%u",strtmp.mb_str(wxConvUTF8), addrReal.Service());
	//}
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
	new CLogTcpThread(this);
	return TRUE;
}
BOOL CTraceLogTcp::Close()
{
	m_BClose=TRUE;
	m_BConnect=FALSE;
	return TRUE;
}
BOOL  CTraceLogTcp::WriteTrace(BYTE buf[],DWORD dwCharToWrite)
{
	if(m_BConnect&&(NULL!=m_psocket))
	{
		m_psocket->Write(buf,dwCharToWrite);
	}
	return TRUE;
}
void CTraceLogTcp::OnConnect(BOOL Connect)
{
	m_BConnect=Connect;
}
#define BUFFMAXLEN		(1000)			//×î´ó1K
int CTraceLogTcp::ReadWriteSocket()
{
	BYTE *buf=new BYTE[BUFFMAXLEN];
	BYTE *wrbuff;
	int wrcnt=0;
	while(!m_BClose)
	{
		if (!m_psocket->IsConnected())
		{
			TRACE("ThreadWorker: not connected");
			return 0;
		}
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
		wxMilliSleep(10);
	}
	delete []buf; // FIX 
	m_BClose=FALSE;
	return 0;
}
void CTraceLogTcp::SeverProc()
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
		sock->SetFlags(wxSOCKET_WAITALL_READ);
#ifndef  WIN32
		sock->SetOption(IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
#endif
		m_psocket=sock;
		OnConnect(TRUE);
		//TRACE("INBOX VER=%d.%d.%d",VER_H,VER_M,VER_L);
		ReadWriteSocket();
		m_psocket->Destroy();
		OnConnect(FALSE);
		m_psocket=NULL;
	}
}
//////////////////////////////////////////////////////////////////////
CLogTcpThread::CLogTcpThread(CTraceLogTcp* pfather)
{
	m_pfather=pfather;
	Create();
	SetPriority(20);
	Run();
}

CLogTcpThread::~CLogTcpThread()
{
}
void* CLogTcpThread::Entry()
{
	m_pfather->SeverProc();
	return NULL;
}

#endif
