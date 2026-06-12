// StateQueue.h: interface for the CStateQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATEQUEUE_H__1CC59948_3D77_44D5_863F_F44859F63DA7__INCLUDED_)
#define AFX_STATEQUEUE_H__1CC59948_3D77_44D5_863F_F44859F63DA7__INCLUDED_

#include "../comon.h"
// ----------------------------------------------未用------------------------------

//状态机的状态
#define STATE_WAIT					0
#define STATE_CONNECT				1
#define STATE_HANDSHAKE			    2
#define STATE_RESET					3
#define STATE_STOP					4

//-------仪器用
#define STATE_EMPTY_Q				11
#define STATE_ADD_Q					12
#define STATE_IN_SKIP_ANSWER		13
#define STATE_KEEPALIVE				14
#define STATE_SUCK_SKIP_S			15
//-------PC用
#define STATE_STATUS_REQ			6
#define STATE_QUERYTEST_REQ		7
#define STATE_SEND_STATE			8

/****************************************************************************/
/*				 状态机的状态消息队列的定义					*/  //未用
/****************************************************************************/
struct StateInfo
{
	StateInfo()
	{
		m_state=STATE_WAIT;			
		m_Param=NULL;				
	}
	int m_state;						//下一个要允许的状态
	void* m_Param;					//上面的参数
};
WX_DECLARE_LIST(StateInfo, StateInfoList);
class StateQueue
{
public:
	StateQueue();
	BOOL InitQueue();						//初始化队列
	BOOL EmptyQueue();					//清空队列
	BOOL AddState(int state,void* Param=NULL,BOOL BHead=FALSE);	//新增状态
	int WaitState(void *&param);				//等待状态到来
protected:
	CxwEvent		m_WaitEvt;
	wxCriticalSection m_QueueMute;				//读写锁
	StateInfoList m_StateLst;		//状态消息队列
};
#endif // !defined(AFX_STATEQUEUE_H__1CC59948_3D77_44D5_863F_F44859F63DA7__INCLUDED_)
