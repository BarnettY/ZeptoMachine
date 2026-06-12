// WorkThread.h: interface for the CWorkThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKTHREAD_H__87068A3F_DBE0_4181_80E3_832856946D2A__INCLUDED_)
#define AFX_WORKTHREAD_H__87068A3F_DBE0_4181_80E3_832856946D2A__INCLUDED_
//
//#include "ArmTestInfo.h"

#include "../comon.h"
#include "ItemList.h"

//参数类型
enum CMD_TYPE
{
	CMD_NONE=0,
	CMD_RESET,					//复位
	CMD_ACTCYCLE,				//运行测试一个周期动作
	CMD_SINGLEACT,				//单步动作
	CMD_MOTORDEBUG,			//马达调试
	CMD_END
};
/****************************************************************************/
/*				 状态机的状态消息队列的定义					*/
/****************************************************************************/
struct CmdInfo
{
	CmdInfo()
	{
		m_Command=CMD_NONE;
	}
	CMD_TYPE			m_Command;					//下一个要允许的状态
	ProgramStep 		m_Param;					//上面的参数
};
WX_DECLARE_LIST(CmdInfo, CmdIInfoList);

class CmdQueue
{
public:
	BOOL EmptyQueue();					//清空队列
	BOOL AddCmd(CMD_TYPE cmd,ProgramStep &Param,BOOL BHead=FALSE);	//新增状态
	CMD_TYPE GetFisrtCmd(ProgramStep &Param);				//等待状态到来
protected:
	wxCriticalSection m_QueueMute;				//读写锁
	CmdIInfoList m_StateLst;		//状态消息队列
};

/****************************************************************************/
/*				 线程托管的定义										*/
/****************************************************************************/
#ifdef	_SIMULATOR_
#define WAIT_TIME					10000
#define WAIT_ARM_TIME				20000
#else
#define WAIT_TIME					100000
#define WAIT_ARM_TIME				100*1000
#endif
class CMachine;
class CWorkThread
{
public:
	CWorkThread();
	virtual ~CWorkThread();
	static TCHAR* GetPartstring(PART_INDEX index);
	void SetFather(CMachine *pfather)
	{
		m_pMachine=pfather;
	}
	BOOL    InitWork();
	int    JudgeIdel(PART_INDEX index);
	//判断已经是正常完成,并发送下一步命令
	int		JudgeIdelAndSendCmdWaitFinish(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param);
	int		JudgeIdelAndSendPartCmdWaitRun(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param);	//判断已经是正常完成,并发送下一步命令
	int		SendPartCmdAndWaitRun(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param,BOOL BHead=FALSE,int WaitTm=WAIT_TIME);	//直接发送运行下一步命令
	int		SendPartCmdAndRun(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param,BOOL BHead=FALSE);

	int		RunPart(PART_INDEX index);	//运行某个部件
	int		WaitPartRuning(PART_INDEX index,int WaitTm);//等待某个部件运行起来
	int		WaitPartFinishEvent(PART_INDEX index,int WaitTm);	//等待莫个部件完成
	int		WaitPartFinishAndReturn(PART_INDEX index,int WaitTm);	//等待莫个部件完成
	int		WaitAllPartFinish(int WaitTm);	//等待所有部件完成
	int		GetPartReturn(PART_INDEX index);	//得到某个部件的返回值


	BOOL   IsAllPartIdel();
	//--------------------------------仪器硬件运动线程----------------
    UINT ProcArm();
protected:
	//---------------------------------仪器硬件运动命令队列
	BOOL		EmptyPartCmdQueue(PART_INDEX index);
	BOOL		GetPartCmd(PART_INDEX index,CMD_TYPE &cmd,ProgramStep &param);
	//---------------------------------仪器硬件运动信号
	BOOL		SetPartRuningEvent(PART_INDEX index);
	BOOL		SetPartFinishEvent(PART_INDEX index,BOOL SaveErr=TRUE);
	BOOL		ResetPartFinishEvent(PART_INDEX index);
	//---------------------------------仪器硬件运动信号
	BOOL		CreateHandle();
	BOOL		InitHandle();
	BOOL		CreateAllThread();
	BOOL		CloseAllThread(int WaitTm);
protected:
	CMachine *m_pMachine;
	//运行某个线程的信号[0]表示线程需运行，[1]表示线程已经运行,[2]表示线程已经完成
	CxwEvent *m_pGoSignal[ALL_ACTION_PART][3];

	BOOL	m_BStopSignalAll; 	//停止所有线程的信号
	CmdQueue m_QueueAry[ALL_ACTION_PART];	//所有的命令集合
	int	m_PartRtnAry[ALL_ACTION_PART];
};

/////////////////////////////////////////////////////////////////////////////
class CPartThread : public wxThread
{
public:
    CPartThread(CWorkThread* m_pfather,PART_INDEX partid);
    virtual ~CPartThread();
    virtual void* Entry();
protected:
	CWorkThread* m_pfather;
	PART_INDEX	  m_partID;
};
#endif // !defined(AFX_WORKTHREAD_H__87068A3F_DBE0_4181_80E3_832856946D2A__INCLUDED_)
