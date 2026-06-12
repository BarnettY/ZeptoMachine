// WorkThread.cpp: implementation of the CWorkThread class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "WorkThread.h"
#include "../Machine/Machine.h"
#include "../trace.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(CmdIInfoList);

/****************************************************************************/
/*				 状态机的状态消息队列的定义					*/
/****************************************************************************/
//新增状态
BOOL CmdQueue::AddCmd(CMD_TYPE cmd,ProgramStep& Param,BOOL BHead)
{
	CmdInfo *addstate=new CmdInfo;
	addstate->m_Command=cmd;
	addstate->m_Param=Param;

	m_QueueMute.Enter();
	if(BHead)
	{
		m_StateLst.Insert(m_StateLst.GetFirst(), addstate);
	}
	else
	{
		m_StateLst.Append(addstate);
	}
	m_QueueMute.Leave();
	return TRUE;
}

//清空队列
BOOL CmdQueue::EmptyQueue()
{
	m_QueueMute.Enter();
	m_StateLst.clear();
	m_QueueMute.Leave();
	return TRUE;
}

//等待状态到来
CMD_TYPE CmdQueue::GetFisrtCmd(ProgramStep &Param)
{
	CMD_TYPE rtn=CMD_NONE;
	m_QueueMute.Enter();
	if(!m_StateLst.IsEmpty())
	{
		CmdInfo *state=m_StateLst.front();
		m_StateLst.pop_front();
		m_QueueMute.Leave();
		rtn=state->m_Command;
		Param=state->m_Param;
		delete state;
	}
	else
		m_QueueMute.Leave();
	return rtn;
}
/****************************************************************************/
/*				 线程托管的定义										*/
/****************************************************************************/
CWorkThread::CWorkThread()
{
	m_BStopSignalAll=FALSE;
}
CWorkThread::~CWorkThread()
{

}
BOOL CWorkThread::InitWork()
{
	if(!CreateHandle())
		return FALSE;
	return CreateAllThread();
}
const static TCHAR strpratAry[][20]={_T("PART_MACHINE"),_T("PART_ARM")};

TCHAR* CWorkThread::GetPartstring(PART_INDEX index)
{
	FASSERT(index<PART_END);
	return (TCHAR *)(strpratAry[index]);
}
int   CWorkThread:: JudgeIdel(PART_INDEX index)
{
	FASSERT(index<ALL_ACTION_PART);
	int rtn=WaitPartFinishEvent(index,0); //判断当前是否空闲
	if (rtn>=0)
	{
		TRACE(_T("CWorkThread::JudgeIdel index =%d,PART =%s"),index,strpratAry[index]);
	}

	return rtn;
}
BOOL  CWorkThread::IsAllPartIdel()
{
	BOOL bIdle =TRUE;
	for(int i=0;i<ALL_ACTION_PART;i++)
	{
		int  dwState =0 ;
		dwState=WaitPartFinishEvent((PART_INDEX)i,0);
		if(SUCCESS!=dwState)
		{
          bIdle =FALSE;
		  break;
		}
	}
   return bIdle;

}

//判断已经是正常完成,并发送下一步命令
int CWorkThread::JudgeIdelAndSendCmdWaitFinish(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param)
{
	int rtn=JudgeIdelAndSendPartCmdWaitRun(index,cmd,param);
	if(rtn<0)
		return rtn;
	rtn=WaitPartFinishAndReturn(index,WAIT_ARM_TIME);
	return rtn;
}

//判断已经是正常完成,并发送下一步命令
//这个函数一般是用来执行动作前获取控制权，不知道部件是否正常完成
int CWorkThread::JudgeIdelAndSendPartCmdWaitRun(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param)
{
	FASSERT(index<ALL_ACTION_PART);
	int rtn=WaitPartFinishEvent(index,WAIT_ARM_TIME);
	if(rtn<0)
	{
#ifdef  WIN32
		TRACE(_T("ERROR Wait CWorkThread::JudgeIdelAndSendPartCmdWaitRun PART_INDEX=%s cmd=%d,rtn=%d\n"),GetPartstring(index),cmd,rtn);
#endif
		return rtn;
	}
	rtn=GetPartReturn(index);
	if(rtn<0)
	{
#ifdef  WIN32	
		TRACE(_T("ERROR Return CWorkThread::JudgeIdelAndSendPartCmdWaitRun GetPartReturn PART_INDEX=%s cmd=%d,rtn=%d\n"),GetPartstring(index),cmd,rtn);
#endif
		//FIXME return rtn;
	}
	return SendPartCmdAndWaitRun(index,cmd,param);
}
//这个函数一般是在同一个功能区内，前面调用过JudgeIdelAndSendPartCmdRun,后面继续执行使用
int CWorkThread::SendPartCmdAndWaitRun(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param,BOOL BHead,int WaitTm)
{
	SendPartCmdAndRun(index,cmd,param,BHead);
	return WaitPartRuning(index,WaitTm);
}

//直接发送运行下一步命令
int CWorkThread::SendPartCmdAndRun(PART_INDEX index,CMD_TYPE cmd,ProgramStep &param,BOOL BHead)
{
	FASSERT(index<ALL_ACTION_PART);
//	TRACE(_T("CWorkThread::SendPartCmdAndRun PART_INDEX=%s,cmd=%d"),GetPartstring(index),cmd);
	m_QueueAry[(BYTE)index].AddCmd(cmd,param,BHead);
	return RunPart(index);
}
//运行某个部件
int CWorkThread::RunPart(PART_INDEX index)
{
	FASSERT(index<ALL_ACTION_PART);
	m_pGoSignal[index][0]->SetEvent();
	return SUCCESS;
}
//等待某个部件运行起来
int CWorkThread::WaitPartRuning(PART_INDEX index,int WaitTm)
{
	FASSERT(index<ALL_ACTION_PART);
	DWORD dwState=m_pGoSignal[index][1]->WaitTimeout(WaitTm);
	//接收数据超时
	if(dwState != 0)
	{
		TRACE(_T("CWorkThread::WaitPartRuning ERROR_RUNPART_TIMEOUT"));
		return ERROR_RUNPART_TIMEOUT-index;
	}
//	TRACE(_T("CWorkThread::WaitPartRuning OK PART_INDEX=%s\n"),GetPartstring(index));
	return SUCCESS;
}
//等待莫个部件完成
int CWorkThread::WaitPartFinishEvent(PART_INDEX index,int WaitTm)
{
	FASSERT(index<ALL_ACTION_PART);
	DWORD dwState=m_pGoSignal[index][2]->WaitTimeout(WaitTm);
	//接收数据超时
	if(dwState != 0)
	{
		return ERROR_PART_TIMEOUT-index;
	}
//	TRACE(_T("CWorkThread::WaitPartFinishEvent OK PART_INDEX=%s\n"),GetPartstring(index));
	return SUCCESS;
}
int CWorkThread::WaitPartFinishAndReturn(PART_INDEX index,int WaitTm)
{
	int rtn=WaitPartFinishEvent(index,WaitTm);
	if(rtn<0)
	{
//		TRACE(_T("ERROR Wait CWorkThread::WaitPartFinishAndReturn PART_INDEX=%s,rtn=%d\n"),GetPartstring(index),rtn);
		return rtn;
	}
//	TRACE(_T("CWorkThread::WaitPartFinishAndReturn OK PART_INDEX=%s,rtn=%d\n"),GetPartstring(index),rtn);
	return GetPartReturn(index);
}
//等待所有部件完成
int CWorkThread::WaitAllPartFinish(int WaitTm)
{
	int i;
	DWORD dwState =0;
	for(i=0;i<ALL_ACTION_PART;i++)
	{
		dwState=WaitPartFinishEvent((PART_INDEX)i,WaitTm);
		if(SUCCESS!=dwState)
			return dwState;
	}
	return SUCCESS;
}
//得到某个部件的返回值
int CWorkThread::GetPartReturn(PART_INDEX index)
{
	FASSERT(index<ALL_ACTION_PART);
	return m_PartRtnAry[index];
}

//命令相关
BOOL CWorkThread::EmptyPartCmdQueue(PART_INDEX index)
{
	FASSERT(index<ALL_ACTION_PART);
//	TRACE(_T("CWorkThread::EmptyPartCmdQueue PART_INDEX=%d"),index);
	return m_QueueAry[(BYTE)index].EmptyQueue();
}
BOOL CWorkThread::GetPartCmd(PART_INDEX index,CMD_TYPE &cmd,ProgramStep &param)
{
	FASSERT(index<ALL_ACTION_PART);
	cmd=m_QueueAry[(BYTE)index].GetFisrtCmd(param);
//	TRACE(_T("CWorkThread::GetPartCmd PART_INDEX=%d,cmd=%d"),index,cmd);
	return (cmd>CMD_NONE);
}
BOOL CWorkThread::SetPartRuningEvent(PART_INDEX index)
{
	FASSERT(index<ALL_ACTION_PART);
//	TRACE(_T("CWorkThread::SetPartRuningEvent OK PART_INDEX=%s\n"),GetPartstring(index));
	m_pGoSignal[index][1]->SetEvent();
	return TRUE;
}

BOOL CWorkThread::SetPartFinishEvent(PART_INDEX index,BOOL SaveErr)
{
//	TRACE(_T("CWorkThread::SetPartRuningEvent OK PART_INDEX=%s\n"),GetPartstring(index));
	FASSERT(index<ALL_ACTION_PART);
	//设置错误
	if(SaveErr&&(m_PartRtnAry[index]<0))
		GLOBAL(m_pErrSet)->SetPartErr(index,m_PartRtnAry[index]);     //  TODO  报错
	m_pGoSignal[index][2]->SetEvent();
	return TRUE;
}
BOOL CWorkThread::ResetPartFinishEvent(PART_INDEX index)
{
//	TRACE(_T("CWorkThread::ResetPartFinishEvent OK PART_INDEX=%s\n"),GetPartstring(index));
	FASSERT(index<ALL_ACTION_PART);
	m_pGoSignal[index][2]->ResetEvent();
	return TRUE;
}
BOOL CWorkThread::CloseAllThread(int WaitTm)
{
	m_BStopSignalAll=TRUE;
	wxSleep(1);
	return TRUE;
}
BOOL CWorkThread::CreateAllThread()
{
	InitHandle();
	new CPartThread(this,PART_ARM);
	return TRUE;
}
// 创建事件

BOOL CWorkThread::CreateHandle()
{
	for(int i=0;i<ALL_ACTION_PART;i++)
	{
		m_pGoSignal[i][0] = new CxwEvent(FALSE);
		m_pGoSignal[i][1] = new CxwEvent(FALSE);
		m_pGoSignal[i][2] = new CxwEvent(TRUE);
	}
	m_BStopSignalAll=FALSE;
	return TRUE;
}
BOOL CWorkThread::InitHandle()
{
	for(int i=0;i<ALL_ACTION_PART;i++)
	{
		m_pGoSignal[i][0]->ResetEvent();
		m_pGoSignal[i][1]->ResetEvent();
		m_pGoSignal[i][2]->SetEvent();
	}
	memset(m_PartRtnAry,0,sizeof(m_PartRtnAry));
	m_BStopSignalAll=FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// 加载抽屉
UINT CWorkThread::ProcArm()
{
	CMD_TYPE	m_Command;						//下一个要允许的状态
    ProgramStep 		m_Param;	
	while (!m_BStopSignalAll)
	{
		if(wxSEMA_NO_ERROR!=m_pGoSignal[PART_ARM][0]->WaitTimeout(20000))
			continue;
		ResetPartFinishEvent(PART_ARM);
		BOOL brtn=GetPartCmd(PART_ARM,m_Command,m_Param);
		FASSERT(brtn);
		SetPartRuningEvent(PART_ARM);
    	//如果部件已经出错,除了复位不再执行
		if((m_PartRtnAry[PART_ARM]<0) &&(CMD_RESET!=m_Command))
		{
			SetPartFinishEvent(PART_ARM,FALSE);
			continue;
		}
		switch(m_Command)
		{
			case CMD_NONE:
				break;
			case CMD_RESET:
				EmptyPartCmdQueue(PART_ARM);
				m_PartRtnAry[PART_ARM]=GLOBAL(m_pPart)->InitPart();
				break;
			case CMD_ACTCYCLE:
				m_PartRtnAry[PART_ARM] = GLOBAL(m_pPart)->WholeAction(m_Param);
			    break;
			case CMD_SINGLEACT:
                // m_PartRtnAry[PART_ARM]=GLOBAL(m_pPartLoadDrawer)->SingleAct(xyz);
				break;
			case CMD_MOTORDEBUG:
				{
		/*			MOTOR_DEBUG_INFO debug=m_Param.GetDebug();
					m_PartRtnAry[PART_ARM]=GLOBAL(m_pPartLoadDrawer)->DebugPartMotor(debug.index,debug.act,debug.param);*/
				}
				break;
			default:
				FASSERT(FALSE);
				break;
		}
		SetPartFinishEvent(PART_ARM); // 里面处理报错
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////
CPartThread::CPartThread(CWorkThread* pfather,PART_INDEX partid)
{
	m_pfather=pfather;
	m_partID=partid;
	Create();
//#ifdef  WIN32
	SetPriority(100);
//#endif
	Run();
}
void* CPartThread::Entry()
{
	switch(m_partID)
	{
	case PART_ARM:
        m_pfather->ProcArm();
		break;
	default:
		FASSERT(FALSE);
		break;
	}
	return NULL;
}
CPartThread::~CPartThread()
{
}

