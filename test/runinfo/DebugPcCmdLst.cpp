// WorkCtrl.cpp: implementation of the CWorkCtrl class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "DebugPcCmdLst.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(PCMDMSGList);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugPcCmdLst::CDebugPcCmdLst()
{
//	m_List.DeleteContents(true);
	m_pListSemp=NULL;
}
CDebugPcCmdLst::~CDebugPcCmdLst()
{
}
void CDebugPcCmdLst::BindSemp(CxwEvent *psemp)
{
	m_pListSemp=psemp;
}

BOOL CDebugPcCmdLst::ResetSemp()
{
	BOOL brtn=FALSE;
	if(IsEmpty())
	{
		brtn=TRUE;
		if(m_pListSemp)
			m_pListSemp->ResetEvent();
	}
	return brtn;
}
void CDebugPcCmdLst::AddTail(CBaseMsg* cmd)
{
	m_ListOpMutex.Enter();
	m_List.Append(cmd);
	m_ListOpMutex.Leave();
	if(m_pListSemp)
		m_pListSemp->SetEvent();
}
void CDebugPcCmdLst::CopyTo(PCMDMSGList* pNewList)
{
	pNewList->clear();
	m_ListOpMutex.Enter();
	PCMDMSGList::Node* node = m_List.GetFirst();
	while (node)
	{
		CBaseMsg *info = node->GetData();
		pNewList->Append(info);
		node = node->GetNext();
	}
	m_List.clear();
	m_ListOpMutex.Leave();
}
void CDebugPcCmdLst::Empty()
{
	m_ListOpMutex.Enter();
	m_List.clear();
	m_ListOpMutex.Leave();
	ResetSemp();
}
BOOL CDebugPcCmdLst::IsEmpty()
{
	m_ListOpMutex.Enter();
	BOOL Brtn=(0==m_List.size());
	m_ListOpMutex.Leave();
	return Brtn;
}
CBaseMsg*  CDebugPcCmdLst::RemoveHead()
{
	m_ListOpMutex.Enter();
	CBaseMsg* returnValue =m_List.front();
	m_List.pop_front();
	m_ListOpMutex.Leave();
	ResetSemp();
	return returnValue;
}	

