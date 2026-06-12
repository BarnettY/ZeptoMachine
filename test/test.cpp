// test.cpp : Defines the class behaviors for the application.
//

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "comon.h"
#include "test.h"
#include "./error/errcode.h"
#include "./can/CanCom.h"
#include "./can/CanControl.h"
#ifdef  WIN32
#include "./can/TcpCanInterface.h"
#include "./can/UsbCanInterface.h"
#else
#include "./linux/BoardCanInterface.h"
#include "./linux/PcSerialAux.h"
#endif

#include "./Machine/Machine.h"
#include "trace.h"
#include "wx/fileconf.h"
// Implements MyApp& GetApp()
DECLARE_APP(CTestApp)

// Give wxWidgets the means to create a MyApp object
IMPLEMENT_APP(CTestApp)
static TCHAR *Tracebuff;
static TCHAR *CanTracebuff;
/////////////////////////////////////////////////////////////////////////////
// CTestApp construction
CTestApp::CTestApp()
{
	Tracebuff=new TCHAR[6*1024];
	CanTracebuff=new TCHAR[6*1024];
	memset(Tracebuff,0,6*1024);
	memset(CanTracebuff,0,6*1024);
	delete wxLog::SetActiveTarget(new wxLogStderr(NULL));
#ifdef LOG_FILE
	BCanLogFileClose=TRUE;
	BTraceFileClose=TRUE;
#else
	m_TcpLogConnect=FALSE;
#endif
}
CTestApp::~CTestApp()
{
	delete [] Tracebuff;
	Tracebuff=NULL;
	delete [] CanTracebuff;
	CanTracebuff=NULL;
}
void ReportWarn(BYTE part,int errcode,wxString strinf)
{
    TRACE(_T("ReportWarn part=%d,errcode=%d"),part,errcode);
    m_pErrSet->SetPartErr(part,errcode);
}
static wxString GetTimeStr()
{
#ifdef WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	wxString strtemp;
	strtemp.Printf(_T("%0.2d/%0.2d/%0.4d %0.2d:%0.2d:%0.2d.%0.3d "),st.wMonth,st.wDay,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
	return strtemp;
#else
	DWORD TM=timeGetTime();
	DWORD TMM=TM/1000;
	DWORD TMK=TM%1000;
	wxString strtemp;
	strtemp.Printf(_T("%0.3d.%0.3d "),TMM,TMK);
	return strtemp;
#endif
}

void NOOP(const TCHAR *msg, ... )
{
#ifndef LOG_FILE
	if(!theApp.m_TcpLogConnect)
		return ;
#endif
	CTraceMute.Enter();
	va_list argp; // 定义保存函数参数的结构
	va_start( argp, msg );

	_vstprintf(Tracebuff, msg,argp);  // 函数原型为vsprintf(buff,format,va_list param),作用是将va_list 里面的可变参数按照format中的格式写入到buff中。效果与sprintf(char*,format,const char*)作用差不多，只不过它会返回写入字符串的长度，可能存在内存溢出的情况。建议使用vsnprintf().
	theApp.WriteTrace(Tracebuff);
	va_end( argp ); // 将argp置为NULL
	CTraceMute.Leave();
}
void CanTraceLog(const TCHAR *msg, ... )
{
#ifdef LOG_FILE
	CLogMuteCan.Enter();
	va_list argp; // 定义保存函数参数的结构
	va_start( argp, msg );

	_vstprintf(CanTracebuff, msg,argp);  // 函数原型为vsprintf(buff,format,va_list param),作用是将va_list 里面的可变参数按照format中的格式写入到buff中。效果与sprintf(char*,format,const char*)作用差不多，只不过它会返回写入字符串的长度，可能存在内存溢出的情况。建议使用vsnprintf().
	theApp.WriteCanLog(CanTracebuff);
	va_end( argp ); // 将argp置为NULL
	CLogMuteCan.Leave();
#else
	CTraceMute.Enter();
	va_list argp; // 定义保存函数参数的结构
	va_start( argp, msg );

	_vstprintf(Tracebuff, msg,argp);  // 函数原型为vsprintf(buff,format,va_list param),作用是将va_list 里面的可变参数按照format中的格式写入到buff中。效果与sprintf(char*,format,const char*)作用差不多，只不过它会返回写入字符串的长度，可能存在内存溢出的情况。建议使用vsnprintf().
	theApp.WriteTrace(Tracebuff);
	va_end( argp ); // 将argp置为NULL
	CTraceMute.Leave();
#endif
}

#ifndef LOG_FILE
void READLOG(int nport,const BYTE *buff,UINT len)
{
}
void WRITELOG(int nport,const BYTE *buff,UINT len)
{
}
#else
static const char *strReadPort[]={"WR:","R :"};
static const char *strWritePort[]={"WS:","S :"};
wxString GetBinString(const BYTE *buff,UINT len)
{
	wxString strtemp,strrtn;
	for(int i=0;i<len;i++)
	{
		if(0!=i)
			strrtn+=" ";
		strtemp.Printf("%0.2X",buff[i]);
		strrtn+=strtemp;
	}
	return strrtn;
}
void READLOG(int nport,const BYTE *buff,UINT len)
{
	wxString strtemp=GetBinString(buff,len);
	wxString strrtn;
	if(0==nport)
		strrtn=strReadPort[0];
	else
		strrtn=strReadPort[1];
	strrtn+=strtemp;
	CLogMute.Enter();
	theApp.WriteLog(strrtn);
	CLogMute.Leave();
}
void WRITELOG(int nport,const BYTE *buff,UINT len)
{
	wxString strtemp=GetBinString(buff,len);
	wxString strrtn;
	if(0==nport)
		strrtn=strWritePort[0];
	else
		strrtn=strWritePort[1];
	strrtn+=strtemp;

	CLogMute.Enter();
	theApp.WriteLog(strrtn);
	CLogMute.Leave();
}
#endif

#ifdef LOG_FILE
BOOL CTestApp::CloseAllFile()
{
	BTcpLogFileClose=TRUE;
	BCanLogFileClose=TRUE;
	BTraceFileClose=TRUE;
	return TRUE;
}
BOOL CTestApp::WriteLog(wxString &trace)
{
	if(!BTcpLogFileClose)
	{
		wxString str=GetTimeStr()+trace+_T("\n");
		TcpLogfile.Write(str);
		//TcpLogfile.Flush();
	}
	return TRUE;
}

BOOL CTestApp::WriteCanLog(TCHAR *trace)
{
	if(!BCanLogFileClose)
	{
		wxString str=GetTimeStr()+trace+_T("\n");
		CanLogfile.Write(str);
//		CanLogfile.Flush();
	}
	return TRUE;
}

BOOL CTestApp::WriteTrace(TCHAR *trace)
{
	static int strcount=0;
	if(!BTraceFileClose)
	{
		wxString str=GetTimeStr()+trace+_T("\n");
		Tracefile.Write(str);
//		Tracefile.Flush();
	}
	return TRUE;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only CTestApp object

CTestApp theApp;
wxCriticalSection CTraceMute;
#ifdef LOG_FILE
wxCriticalSection CLogMute;
wxCriticalSection CLogMuteCan;
#endif

CMachine * g_ST;
DWORD g_lastTime;
/////////////////////////////////////////////////////////////////////////////
// CTestApp initialization
BOOL CTestApp::InitInstance()
{
#ifdef  WIN32
	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);

#endif

#ifdef LOG_FILE
#ifdef WIN32
	TcpLogfile.Open("TCPIP.TXT",wxFile::write);
	CanLogfile.Open("CAN.TXT",wxFile::write);
	Tracefile.Open("TRACE.LOG",wxFile::write);
#endif
#ifndef	WIN32
	TcpLogfile.Open("/opt/tmp/TCPIP.TXT",wxFile::write);
	CanLogfile.Open("/opt/tmp/CAN.TXT",wxFile::write);
	Tracefile.Open("/opt/tmp/TRACE.LOG",wxFile::write);
#endif
	BTcpLogFileClose=FALSE;
	BCanLogFileClose=FALSE;
	BTraceFileClose=FALSE;
#endif
	wxSocketBase::Initialize();

#ifdef  WIN32
	g_Can.SetCanInterface(new UsbCanInterface);
#else
	g_Can.SetCanInterface(new BoardCanInterface);
#endif
	//轨道初始化
    g_ST  = new CMachine();
	g_ST->LoadSystemParam(); // 读取IP和端口
	//g_ST->SaveSystemParam();
    g_ST->Initial();
#ifndef LOG_FILE
	m_pTcpLog=new CTraceLogTcp;
	m_TcpLogConnect=m_pTcpLog->Connect(g_ST->strIP, g_ST->m_LogPort);
#endif
	return TRUE;
}

int CTestApp::ExitInstance()
{
#ifndef LOG_FILE
	m_pTcpLog->Close();
#endif
	// Terminate use of the WS2_32.DLL
	return TRUE;
}

#ifndef LOG_FILE
BOOL CTestApp::WriteTrace(TCHAR *trace)
{
	wxString strline=wxString(trace)+_T("\n");
	const char *buff =strline.mb_str();//wxStrdup(strline.mb_str());
	m_pTcpLog->WriteTrace((BYTE *)buff,strlen(buff));

	return TRUE;
}
#endif

void  CTestApp::ConnectAndDownParam()
{
	if(g_Can.Connect(K100))
	{
	}
#ifndef _SIMULATOR_
	wxMilliSleep(3000);

#ifdef  WIN32
	g_Can.ReConnect(K250);
	wxMilliSleep(1200);
#endif
	g_Can.ScanAllID();
	wxMilliSleep(1500);
#endif
	return;
}

// Initialize the application
bool CTestApp::OnInit()
{
	InitInstance();
	g_lastTime=timeGetTime();
	return true;
}
int CTestApp::OnRun()
{
	ConnectAndDownParam();
#ifndef _SIMULATOR_
	//wxMilliSleep(5*1000);
#endif
	m_pMainTask->ReadMotorParameter();
	g_ST->PartDownParam();
	m_pMainTask->SatePartRun();

	return 1;
}