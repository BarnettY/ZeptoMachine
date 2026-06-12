// TraceLogTcp.h: interface for the CTraceLogTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACELOGTCP_H__BE718BBC_1412_460E_975A_70C392C7DDE6__INCLUDED_)
#define AFX_TRACELOGTCP_H__BE718BBC_1412_460E_975A_70C392C7DDE6__INCLUDED_

#include <wx/socket.h>
#include "comon.h"

class CTraceLogTcp
{
public:
	CTraceLogTcp();
	virtual ~CTraceLogTcp();
	BOOL	m_BConnect;
	BOOL	Connect(wxString insIP,int insports);
	BOOL	Close();
	void 	SeverProc();
	BOOL	WriteTrace(BYTE buf[],DWORD dwCharToWrite);
	void	OnConnect(BOOL Connect);
	int		ReadWriteSocket();
protected:
	volatile	BOOL 	m_BClose;
	wxSocketBase		*m_psocket;
	wxSocketServer	*m_pserver;
};
/////////////////////////////////////////////////////////////////////////////
class CLogTcpThread : public wxThread
{
public:
	CLogTcpThread(CTraceLogTcp* m_pfather);
	virtual ~CLogTcpThread();
	virtual void* Entry();
protected:
	CTraceLogTcp* m_pfather;
};

#endif // !defined(AFX_TRACELOGTCP_H__BE718BBC_1412_460E_975A_70C392C7DDE6__INCLUDED_)
