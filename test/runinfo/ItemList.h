// ItemList.h: interface for the CRequestItemList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMLIST_H__1BCFC8F0_7BAE_4075_A353_3BC0454D47CA__INCLUDED_)
#define AFX_ITEMLIST_H__1BCFC8F0_7BAE_4075_A353_3BC0454D47CA__INCLUDED_

#include "../comon.h"
struct ProgramStep  //步骤流程
{  
	BYTE StepId ; //步骤序号           
    BYTE HolePos ;//  孔位  1-7 
    int  MixTime ; // 混合时间（min) 改成sec  取值>0
    int  MagneticTime ;// 吸磁时间(sec)   取值>=0
    int  WaitTime ; // 等待时间（min) 改成sec    取值>=0
    int  Volume ;   //容积(微升)         取值>0
	BYTE  SpeedLevel ; //混合速度 1-10
	BYTE  MixPerCent; // 混合幅度
	BYTE  SuckMagLevel ; //吸磁速度 1-10
    BYTE  MarkFinish ; //  0 初始值， 1 标记完成。 
	ProgramStep()
	{
		Empty();
	}
	void Empty()
	{
		StepId =-1;
		HolePos =0 ;
		MixTime =0 ; 
		MagneticTime=0  ;
		WaitTime =0 ;
		Volume =0 ; 
		SpeedLevel =0 ; 
		MarkFinish =0 ;
		MixPerCent =80;
		SuckMagLevel=0;
	}
	BOOL IsEmpty()
	{
		BOOL bEmpty =TRUE;
		if (StepId !=0 && HolePos!=0)
		{
			bEmpty = FALSE;
		}
		return bEmpty;
	}
};

//存储 程序步骤队列

WX_DECLARE_LIST(ProgramStep,ProgramInfoList);

class ProgramQueue
{
public:
	ProgramQueue();
	BOOL EmptyQueue();							//清空队列
	BOOL AddStep(ProgramStep step);	            //添加程序步骤
	ProgramStep GetProgramStep(BYTE index);     //获取步骤
	int GetStepCount();                        //程序步数
	int GetFinishTimeByIndex(BYTE index);     // 从当前步骤到结束预计时间
	void PrintProgramStepInfo();  //打印步骤信息

public:
	int   m_Id; //程序id  
    BYTE  m_Type ;  //类型 ，0 ， 1--10ML  2--5MML   3--新的10ML
	BYTE  m_SuckMagneticWay ; //吸磁方式  
	BYTE  m_NumberOfSegments; //分段吸磁 ，段数

protected:
	wxCriticalSection m_QueueMute;				//读写锁
	ProgramInfoList m_ProgramLst;				//程序队列

};


#endif // !defined(AFX_ITEMLIST_H__1BCFC8F0_7BAE_4075_A353_3BC0454D47CA__INCLUDED_)
