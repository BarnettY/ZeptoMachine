#pragma once

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Part.h"
#include "MainRunState.h" 
#include "../tcpip/AllTcpIp.h"
#include "../error/errcode.h"
#include "../error/ErrorSet.h" // 错误信息
#include "../runinfo/WorkThread.h"

#define	 GLOBAL(X)	 X
extern	 CWorkThread   	  *m_pWorkThread;		//控制各自部件的线程
extern	 CErrorSet		   *m_pErrSet;			//仪器的出错信息
extern   CPart             *m_pPart;
extern   CMainRunState	   *m_pMainTask;	 
// ------------------------------仪器定义----------------------------------------------

class CMachine
{
public:
	friend class CMainRunState;
	friend class CErrorSet;
	CMachine();
	~CMachine(void);
	BOOL Initial();  // 初始化 设置 
	BOOL PartInitParam();  //初始网络连接
	BOOL PartDownParam(); // 电机参数
	BOOL StartRun(); //
	int  Reset(); //
	BOOL LoadSystemParam(); // 加载系统参数
	BOOL SaveSystemParam(); // 保存系统参数
	int   PartReset();		//复位
	void  Connect();        //连接

public:
	CAllTcpIp	 *m_pAllTcpIp;		//所有的TCPip连接
	CxwEvent	 m_DealPCcmdSemp;
	BOOL         m_bErrorStop;   // 报错停止
	BOOL         m_ThreadExit;   //线程是否退出， 
	wxString strIP;      //IP 地址
	int m_Port;          // 端口号
	int m_LogPort;       // 日志端口号
	int m_MachineType ;  //  //标记仪器版本  1 默认批量版本  2 是早期样机 

};
