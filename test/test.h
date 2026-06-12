// test.h : main header file for the TEST application
//

#if !defined(AFX_TEST_H__988D6DD6_C056_4921_AE9C_CF2997D6D8A2__INCLUDED_)
#define AFX_TEST_H__988D6DD6_C056_4921_AE9C_CF2997D6D8A2__INCLUDED_

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/textfile.h"

#include "TraceLogTcp.h"

/////////////////////////////////////////////////////////////////////////////
// CTestApp:
// See test.cpp for the implementation of this class
//
class CTestApp: public wxApp
{
public:
	CTestApp();
	virtual ~CTestApp();
	virtual bool OnInit();
	virtual int OnRun();
#ifdef LOG_FILE
	BOOL WriteLog(wxString &trace);

	BOOL CloseAllFile();
	BOOL WriteCanLog(TCHAR *trace);
#else
	CTraceLogTcp	*m_pTcpLog;
	BOOL	m_TcpLogConnect;
#endif
	BOOL WriteTrace(TCHAR *trace);
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void  ConnectAndDownParam();
private:


#ifdef LOG_FILE
	BOOL   BTcpLogFileClose;
	wxFile TcpLogfile;

	BOOL   BCanLogFileClose;
	wxFile CanLogfile;

	BOOL   BTraceFileClose;
	wxFile Tracefile;
#endif
};
extern CTestApp theApp;
extern wxCriticalSection CTraceMute;
#ifdef LOG_FILE
extern wxCriticalSection CLogMute;
extern wxCriticalSection CLogMuteCan;
#endif

class CMachine;
extern CMachine *g_ST;

extern MachineStateType g_MachineState;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST_H__988D6DD6_C056_4921_AE9C_CF2997D6D8A2__INCLUDED_)
