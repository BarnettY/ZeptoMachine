// StateQueue.cpp: implementation of the CStateQueue class.
//
//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "StateQueue.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(StateInfoList);
/****************************************************************************/
/*				 状态机的状态消息队列的定义					*/
/****************************************************************************/
StateQueue::StateQueue():m_WaitEvt(FALSE)
{
	m_StateLst.DeleteContents(true);
}
//初始化队列
BOOL StateQueue::InitQueue()
{
	return TRUE;
}
//新增状态
BOOL StateQueue::AddState(int state,void* Param,BOOL BHead)
{
	StateInfo *addstate=new StateInfo;
	addstate->m_state=state;
	addstate->m_Param=Param;

	m_QueueMute.Enter();
	if(BHead)
	{
		m_StateLst.Insert(addstate);
	}
	else
	{
		m_StateLst.Append(addstate);
	}
	m_QueueMute.Leave();
	m_WaitEvt.SetEvent();
	return TRUE;
}
	
//清空队列
BOOL StateQueue::EmptyQueue()
{
	m_QueueMute.Enter();
	m_StateLst.Clear();
	m_QueueMute.Leave();
	m_WaitEvt.ResetEvent();
	return TRUE;
}
	
//等待状态到来
int StateQueue::WaitState(void *&param)
{
	m_WaitEvt.WaitTimeout(200);
	//接收数据超时
//	if(dwState == WAIT_TIMEOUT)
//	{
//		return STATE_WAIT;
//	}
//	else if(dwState == WAIT_OBJECT_0)
	{
		int rtn=STATE_WAIT;
		m_QueueMute.Enter();
		if(!m_StateLst.IsEmpty())
		{
			StateInfo *state=m_StateLst.front();
			rtn=state->m_state;		
			param=state->m_Param;		
			m_StateLst.pop_front();
		}		
		m_QueueMute.Leave();
		return rtn;
	}
	return STATE_WAIT;
}

