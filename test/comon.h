#ifndef _COMMON_H_
#define _COMMON_H_

/////////////////////////////////////////////////////////////////////////////
//以下是LOG函数
#ifdef  WIN32
#pragma  warning (disable:4018)
#pragma  warning (disable:4100)
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define WINVER			0x0500
#define _WIN32_WINNT	0x0500
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <Mmsystem.h>
#pragma comment(lib,"winmm.lib")
#else
typedef unsigned int UINT;
typedef unsigned int BOOL;
typedef unsigned char BYTE;
typedef char TCHAR;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char UCHAR;

#define _tcscpy strcpy
#define _vstprintf vsprintf
#include <time.h>
unsigned int inline timeGetTime()
{
        unsigned int uptime = 0;
        struct timespec on;
        if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
                 uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
        return uptime;
}
#endif
/////////////////////////////////////////////////////////////////////////////
#define	WAIT_FOREVER	0X0fffffff

////同步的原因
//#define REASON_NOTACK		101				//没有收到ACK
//#define REASON_RECONNECT	102				//需要重新连接
//#define REASON_HANKSHAKE	103				//需要同步握手

class CTcpAgent
{
public:
	//------------消息操作--------
	virtual	int		OnConnectMsg(UINT  port,BOOL connect)=0;
	virtual	int		OnSynMsg(UINT  port,int reason)=0;
	virtual	int		OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount)=0;
	//virtual	int		OnAckMsg(UINT  port,int  ack)=0;
};

// 仪器状态定义
enum MachineStateType
{
     MACHINE_WAITINIT=0, // 等待初始化
     MACHINE_INIT=1,
	 MACHINE_READY=2,
	 MACHINE_DEBUG=3,
	 MACHINE_TEST=4,
     MACHINE_PAUSE=5,
	 MACHINE_STOP=6,
	 MACHINE_ERROR=7,// 不用
};

#ifdef  WIN32
#pragma  warning (disable:4996)
#pragma  warning (disable:4390)
#endif // WIN32

#define ALL_IO_COUNT	5
#define WAITTIME		20*1000
#define SPANTIME		15*1000

//#define	TEST_FINSIH		1       //测试退出结果
#define SoftVersion   "1.0.0.8"         //

#include "wx/thread.h"
//extern BOOL	g_BFirstRest;
#define FASSERT wxASSERT
extern void NOOP(const TCHAR *msg, ... );
extern void CanTraceLog(const TCHAR *msg, ... );
extern void ReportWarn(BYTE part,int errcode,wxString strinf=_T(""));

extern void READLOG(int nport,const BYTE *buff,UINT len);
extern void WRITELOG(int nport,const BYTE *buff,UINT len);

class CxwEvent
{
public:
	CxwEvent(BOOL manual):m_semp(0,1)
	{
		m_BManual=manual;
		m_BVaild=FALSE;
	}
	void SetEvent()
	{
		m_BVaild=TRUE;
		m_semp.Post();
	}
	void ResetEvent()
	{
		m_BVaild=FALSE;
		m_semp.WaitTimeout(1);
	}
	wxSemaError WaitTimeout(unsigned long milliseconds)
	{
		if(m_BManual&&m_BVaild)
			return wxSEMA_NO_ERROR;
		wxSemaError rtn=m_semp.WaitTimeout(milliseconds);
		if(!m_BManual&&(wxSEMA_NO_ERROR==rtn))
			m_BVaild=FALSE;
		return rtn;
	}
	wxSemaphore		m_semp;
	BOOL	m_BManual;
	volatile BOOL	m_BVaild;
};

enum PART_INDEX
{
	 PART_MACHINE=0,
	 PART_ARM=1,
	 PART_END,
};
//
//#define  SUCKMAGNETIC_LEFT_STEP 50   // 吸磁底部到试剂瓶的底部
#define ALL_PART						0xFFFFFFFF
#define RESET_OK_MASK				((1<<PART_ARM))

enum KIT_TYPE
{
	KIT_TYPE_ONE=1,   // 1长孔+3圆孔+ 1圆孔
	KIT_TYPE_TWO=2,   // 1短孔+5圆孔+ 1圆孔
	KIT_TYPE_THREE=3, // 1长孔+5圆孔+ 1圆孔
};

//动作类型-- 1 混匀  2  吸磁   3 等待
enum ACTION_TYPE
{
   ACT_MIX=1,
   ACT_MAGNETIC=2,
   ACT_WAIT=3,
};




enum ACTION_STATUS
{
	ACT_STATUS_START=1,
	ACT_STATUS_END=2,
	ACT_STATUS_ALL_END=3,
};

//吸磁方式

enum SUCKMAGNETIC_WAY
{
	XUAN_TING=1,
    WANG_FU=2,
    FENG_DUAN=3,
};

//运动补偿时间
#define  PART_HOME_TIME  3
#define  INIT_HOME_TIME  3
#define  HOLE_ONE_MIX_TIME 6
#define  HOLE_ONE_SUCKMAGNETIC_TIME 3
#define  HOLE_OTHER_MIX_TIME 8
#define  HOLE_OTHER_SUCKMAGNETIC_TIME 2

//定义毫米mm对应的脉冲比值
#define XMOTOR_MM_TO_PULSE 267  //x轴  3200/12
#define ZMOTOR_MM_TO_PULSE 100  //Z轴 没变
#define MMOTOR_MM_TO_PULSE 800  // 磁棒轴 3200/4


#define   XMOTOR_MM_TO_PULSE_1     100//样机的x轴  3200/32

#define   MMOTOR_MM_TO_PULSE_1    640 //      3200/5
//这个通用
#define  TYPE1_MM_1ML_1  4 // 小于1ml    1ml对应高度 4mm
#define  TYPE1_MM_1ML_2  2.2  // 大于1ml .每毫升对应高度
#define  TYPE2_MM_1ML_1  3.8 // 小于1ml对应  类型2
#define  TYPE2_MM_1ML_2  3.6
#define  TYPE_MM_1ML_HOLE  19.5 // 圆孔每毫升对应高度


#define  LAST_HOLE_POS_TYPE_1  5
#define  LAST_HOLE_POS_TYPE_2_3  7

#define  KIT_TYPE_ONE_SPECIAL_POS  6

//  IO 定义
enum  IODefine
{
	IO_VALVE1 =1,
	IO_VALVE2,
	IO_VALVE3,
	IO_VALVE4,
	IO_VALVE5,
	IO_VALVE6,
	IO_VALVE7,
	IO_VALVE8,
	IO_VALVE9,
	IO_VALVE10,
	IO_VALVE11,
	IO_VALVE12,
	IO_VALVE13,
	IO_VALVE14,
};

#define  ALL_ACTION_PART		PART_END	//所有运动部件
#define  IF_EMER_STOP  if(g_MachineState == MACHINE_STOP) {return 2;} //   监听停止
#endif