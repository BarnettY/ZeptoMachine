// WorkCtrl.cpp: implementation of the CWorkCtrl class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ItemList.h"
#include "wx/listimpl.cpp"
#include "../trace.h"

WX_DEFINE_LIST(ProgramInfoList); //程序队列

// ---------------程序步骤队列----------------------
ProgramQueue::ProgramQueue()
{
	m_ProgramLst.DeleteContents(true);
    m_Id=-1; //程序id  
	m_Type=0;
	m_SuckMagneticWay=0;
	m_NumberOfSegments =0;

}
//新增状态
BOOL ProgramQueue::AddStep(ProgramStep step)
{
	ProgramStep *pInfo=new ProgramStep;
	*pInfo = step;
	m_QueueMute.Enter();
	m_ProgramLst.Append(pInfo);
	m_QueueMute.Leave();
	return TRUE;
}

//清空队列
BOOL ProgramQueue::EmptyQueue()
{
	m_QueueMute.Enter();
	m_ProgramLst.Clear();
	m_Id=-1; //程序id  
    m_Type =0;
	m_SuckMagneticWay=0;
	m_NumberOfSegments =0;
	m_QueueMute.Leave();
	return TRUE;
}

int ProgramQueue::GetStepCount()
{
	int ncount= 0;
	m_QueueMute.Enter();
	ncount=m_ProgramLst.GetCount();
	m_QueueMute.Leave();
	return ncount;
}

ProgramStep ProgramQueue::GetProgramStep(BYTE index)
{
	//	FASSERT(index<MaxRackCapacity);
	ProgramStep info;
	int ncount=m_ProgramLst.GetCount();
	if(index<ncount)
	{
		m_QueueMute.Enter();
		ProgramInfoList::Node* node = m_ProgramLst.Item(index);
		ProgramStep*pinfo= node ->GetData();
		info=*pinfo;
		m_QueueMute.Leave();
	}
	return info;
}
// 返回index步骤开始到结束的，剩余结束时间
int ProgramQueue::GetFinishTimeByIndex(BYTE index)
{
    int iFinishTime = 0;
	m_QueueMute.Enter();
	int ncount=m_ProgramLst.GetCount(); 
	// 悬停吸磁需要孔1 增加下降和左右移动 时间 其他孔增加下降和上升时间，
	if(index<ncount)
	{
		for(int i=index;i<ncount;i++)
		{
			ProgramInfoList::Node* node = m_ProgramLst.Item(i);
			ProgramStep*pinfo= node ->GetData();
            if(!pinfo->IsEmpty())
			{           
			   if (pinfo->MixTime>0)
			   {
                   iFinishTime += pinfo->MixTime ; //
				   if (pinfo->HolePos==1)
				   {
                         iFinishTime +=HOLE_ONE_MIX_TIME;
				   }
				   else
				   {
                        iFinishTime +=HOLE_OTHER_MIX_TIME;
				   }
				   iFinishTime +=PART_HOME_TIME;
			   }
			   if (pinfo->MagneticTime >0)
			   {
                   iFinishTime += pinfo->MagneticTime ;
				   if (pinfo->HolePos==1)
				   {
					   iFinishTime +=HOLE_ONE_SUCKMAGNETIC_TIME;// TODO
				   }
				   else
				   {
					   iFinishTime +=HOLE_OTHER_SUCKMAGNETIC_TIME;
				   }
				   iFinishTime +=PART_HOME_TIME;
			   }
               iFinishTime += pinfo->WaitTime ;
			}
		}
         iFinishTime += INIT_HOME_TIME  ;//最后回零时间
         
	}
	m_QueueMute.Leave();
    return iFinishTime;
}
//int ProgramQueue:: GetNeedTimeToIndexStep(BYTE index) // 从开始到index步骤需要时间
//{
//	int iTime = 0;
//	m_QueueMute.Enter();
//	int ncount=m_ProgramLst.GetCount();
//	if(index<ncount)
//	{
//		for(int i=0;i<index;i++)
//		{
//			ProgramInfoList::Node* node = m_ProgramLst.Item(i);
//			ProgramStep*pinfo= node ->GetData();
//			if (!pinfo->IsEmpty())
//			{
//				/*iTime += pinfo->Seconds ;*/
//			}
//		}
//	}
//	m_QueueMute.Leave();
//	return iTime;
//}

void ProgramQueue::PrintProgramStepInfo()
{
	m_QueueMute.Enter();
	int ncount=m_ProgramLst.GetCount();
	for(int i=0;i<ncount;i++)
	{
		ProgramInfoList::Node* node = m_ProgramLst.Item(i);
		ProgramStep*pinfo= node ->GetData();
		if (!pinfo->IsEmpty())
		{
			TRACE(_T("ProgramQueue::index =%d, HolePos=%d, MixTime =%d,MagneticTime=%d,WaitTime=%d,SpeedLevel=%d,Volume=%d,MixPercent=%d,suckLevel=%d"),i,pinfo->HolePos, pinfo->MixTime,pinfo->MagneticTime,pinfo->WaitTime,pinfo->SpeedLevel,pinfo->Volume,pinfo->MixPerCent,pinfo->SuckMagLevel);
		}
	}
	m_QueueMute.Leave();
}

