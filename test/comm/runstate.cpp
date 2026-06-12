//
//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "runstate.h"
#include "../motor/BasicMotor.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(CMD_TYPEList);
CmdQueue::CmdQueue()
{
	m_StateLst.DeleteContents(true);
}
//新增状态
BOOL CmdQueue::AddCmd(CMD_TYPE cmd,BOOL BHead)
{
	CMD_TYPE *pcmd=new CMD_TYPE;
	*pcmd=cmd;
	m_QueueMute.Enter();
	if(BHead)
	{
		m_StateLst.Insert(pcmd);
	}
	else
	{
		m_StateLst.Append(pcmd);
	}
	m_QueueMute.Leave();
	return TRUE;
}
//没有命令
BOOL CmdQueue::IsCmdEmpty()
{
	BOOL BEmpty=FALSE;
	m_QueueMute.Enter();
	BEmpty=m_StateLst.IsEmpty();
	m_QueueMute.Leave();
	return BEmpty;
}

//清空队列
BOOL CmdQueue::EmptyQueue()
{
	m_QueueMute.Enter();
	m_StateLst.Clear();
	m_QueueMute.Leave();
	return TRUE;
}

//等待状态到来
CMD_TYPE CmdQueue::GetFisrtCmd()
{
	CMD_TYPE rtn=CMD_NONE;
	m_QueueMute.Enter();
	if(!m_StateLst.IsEmpty())
	{
		CMD_TYPE *state=m_StateLst.front();
		rtn=*state;
		m_StateLst.pop_front();
	}
	m_QueueMute.Leave();
	return rtn;
}

RunState::RunState():m_GoEvt(FALSE),m_RunningEvt(FALSE),m_FinishEvt(TRUE)
{
	m_Status=STATUS_IDEL;
}
RunState::~RunState()
{
}

//初始化
BOOL RunState::Init()
{
	m_GoEvt.ResetEvent();
	m_RunningEvt.ResetEvent();
	m_FinishEvt.SetEvent();
	m_Status=STATUS_IDEL;
	return TRUE;
}
//清空命令
BOOL RunState::EmptyCmd()
{
	return m_QueueAry.EmptyQueue();
}

//运行命令
BOOL RunState::RunCmd(CMD_TYPE cmd,BOOL BHead,int WaitTm)
{
	BOOL Brtn=IsCmdFinish();
	FASSERT(Brtn);
	m_QueueAry.AddCmd(cmd,BHead);
	RunPart();
	Brtn=WaitPartRuning(2000); 
	FASSERT(Brtn>0);
	if(Brtn<0)
	{
		return ERROR_RUNPART_TIMEOUT;
	}
	if(WaitTm>0)
	{
		return WaitPartFinishEvent(WaitTm);
	}
	return Brtn;
}

//运行某个部件
int RunState::RunPart()
{
	m_GoEvt.SetEvent();
	return SUCCESS;
}
//运行某个部件
int RunState::SetPartRuning()
{
	m_RunningEvt.SetEvent();
	return SUCCESS;
}

//完成
BOOL RunState::IsCmdFinish()
{
	return SUCCESS==WaitPartFinishEvent(0);
}

//等待某个部件运行起来
int RunState::WaitPartRuning(int WaitTm)
{
	//接收数据超时
	if(wxSEMA_NO_ERROR!=m_RunningEvt.WaitTimeout(WaitTm))
	{
		return ERROR_RUNPART_TIMEOUT;
	}
	return SUCCESS;
}
//等待莫个部件完成
int RunState::WaitPartFinishEvent(int WaitTm)
{
	//接收数据超时
	if(wxSEMA_NO_ERROR!=m_FinishEvt.WaitTimeout(WaitTm))
	{
		return ERROR_PART_TIMEOUT;
	}
	return SUCCESS;
}
//状态机主函数
UINT RunState::SatePartRun()
{
	while(1)
	{
		if(wxSEMA_NO_ERROR!=m_GoEvt.WaitTimeout(WAIT_FOREVER))
			continue;
		//m_GoEvt.WaitTimeout(WAIT_FOREVER);
		m_FinishEvt.ResetEvent();
		m_RunningEvt.SetEvent();
		CMD_TYPE cmd=m_QueueAry.GetFisrtCmd();
		RunFuncAndReportErr(cmd);
		m_FinishEvt.SetEvent();
	};
	return 1;
}

//启动线程
BOOL RunState::CreateTread()
{
	new CPartThread(this);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
CPartThread::CPartThread(RunState* pfather)
{
	m_pfather=pfather;
	Create();
	SetPriority(100);
	Run();
}
void* CPartThread::Entry()
{
	m_pfather->SatePartRun();
	return NULL;
}
CPartThread::~CPartThread()
{
}

