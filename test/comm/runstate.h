//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNSTATE_H__FD1D81C9_7DF4_4D47_A6F3_4F8C7ED39525__INCLUDED_)
#define AFX_RUNSTATE_H__FD1D81C9_7DF4_4D47_A6F3_4F8C7ED39525__INCLUDED_

#include "../comon.h"

//////////////////////////////////////////////////////////////////////
//命令类型
enum CMD_TYPE
d{
//	CMD_NONE=0,
//	CMD_RESET,					//复位
//	CMD_EMPTYING,				//清空样本架
//	CMD_PUSHIN,
//	CMD_POPOUT,
//	CMD_SHIFTINNER,			//跟在CMD_POPOUT动作后面的内部移动
//	//CMD_MOVEOUT,
//	CMD_END
//};
//状态类型
enum STATUS_TYPE
{
	STATUS_IDEL=0,
	STATUS_CAN_1,
	STATUS_CAN_2,
	STATUS_CAN_3,
	STATUS_WAIT_1,
	STATUS_WAIT_2,
	STATUS_WAIT_3,
	STATUS_ACT_1,
	STATUS_ACT_2,
	STATUS_ACT_3,

	STATUS_WAITOUT,//3
	STATUS_WAITIN,//1
	STATUS_POPOUT,//2
	STATUS_PUSHIN,//1
	STATUS_SHIFTINNER,//2
	STATUS_POPOUTING,//3
	STATUS_PUSHINING,//1


	STATUS_RESETING,
	STATUS_EMPTYING,
	STATUS_END
};
//WX_DECLARE_LIST(CMD_TYPE, CMD_TYPEList);
class CmdQueue
{
public:
	CmdQueue();
	BOOL EmptyQueue();					//清空队列
	BOOL AddCmd(CMD_TYPE cmd,BOOL BHead=FALSE);	//新增状态
	CMD_TYPE	GetFisrtCmd();				//等待状态到来
	BOOL IsCmdEmpty();						//没有命令
protected:
	wxCriticalSection m_QueueMute;				//读写锁
	CMD_TYPEList m_StateLst;		//状态消息队列
};
class RunState
{
public:
	RunState();
	~RunState();
	//完成
	virtual BOOL IsCmdFinish();
	//初始化
	BOOL Init();
	//清空命令
	BOOL EmptyCmd();
	//启动线程
	BOOL CreateTread();
	//运行命令
	BOOL RunCmd(CMD_TYPE cmd,BOOL BHead=FALSE,int WaitTm=WAITTIME);
	STATUS_TYPE GetStatus()
	{
		return m_Status;
	}
	//运行某个部件
	int		SetPartRuning();
	UINT SatePartRun();					//部件线程主函数
protected:
	//运行某个部件
	int		RunPart();
	//等待某个部件运行起来
	int		WaitPartRuning(int WaitTm=WAITTIME);
	//等待莫个部件完成
	int		WaitPartFinishEvent(int WaitTm=WAITTIME);
	//virtual BOOL RunFuncAndReportErr(CMD_TYPE cmd)=0;					//部件运行函数
	CxwEvent		m_GoEvt;
	CxwEvent		m_RunningEvt;
	CxwEvent		m_FinishEvt;
	CmdQueue	    m_QueueAry;		//命令列表
	STATUS_TYPE 	m_Status;		//目前状态
};

enum MessageType
{
	MSG_NONE1=0,
	//以下消息，零部件都会有
	MSG_INOK,				//表示接收完成
	MSG_OUTOK,				//表示推出完成
	MSG_HALFFULL,			//表示已经半满
	MSG_FULL,				//表示已经全满
	MSG_EMPTY,				//部件才会有的，整个部件空的消息
	MSG_RESET,				//部件才会有的，整个部件空的消息
	MSG_MOVEOUT,            //试管架从皮带上移出
};

/////////////////////////////////////////////////////////////////////////////
class CPartThread : public wxThread
{
public:
    CPartThread(RunState* m_pfather);
    virtual ~CPartThread();
    virtual void* Entry();
protected:
	RunState* m_pfather;
};
#endif // !defined(AFX_RUNSTATE_H__FD1D81C9_7DF4_4D47_A6F3_4F8C7ED39525__INCLUDED_)
