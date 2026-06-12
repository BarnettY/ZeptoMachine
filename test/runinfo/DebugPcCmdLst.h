#ifndef		_DEBUG_PC_CMD_LIST_H_
#define		_DEBUG_PC_CMD_LIST_H_
#include "../runinfo/datadefine.h"
#include "../protocol/AllMsgClass.h"

/****************************************************************************/
/*						 与事件绑定的队列定义						*/
/****************************************************************************/
WX_DECLARE_LIST(CBaseMsg, PCMDMSGList);

class CDebugPcCmdLst
{
public:
	CDebugPcCmdLst();
	~CDebugPcCmdLst();
	void BindSemp(CxwEvent *psemp);
	BOOL ResetSemp();
	void AddTail(CBaseMsg* cmd);
	void CopyTo(PCMDMSGList* pNewList);
	void Empty();
	BOOL IsEmpty();
	CBaseMsg* RemoveHead();
protected:
	PCMDMSGList		m_List;
	wxCriticalSection	m_ListOpMutex;				//互斥锁
	CxwEvent		*m_pListSemp;
};
#endif
