// TrackerAssemble.cpp: implementation of the CInBox class.
//
//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "STMachine.h"
#include "../trace.h"

//单个动作调试
BOOL CSTMachine::SingleAct(int B,int a)
{
	GenerPart *pPart=NULL;
	B+=1;
	switch(B)
	{
	case 1:
		pPart=m_pPart1;
		break;
	case 2:
		pPart=m_pPart2;
		break;
	case 3:
		pPart=m_pPart3;
		break;
	case 4:
		pPart=m_pPart4;
		break;
	case 5:
		pPart=m_pPart5;
		break;
	case 6:
		pPart=m_pPart6;
		break;
	case 7:
		pPart=m_pPart7;
		break;
	default:
		break;
	}
	if(NULL==pPart)
		return FALSE;
	switch(a)
	{
 	case 0:
		pPart->Reset();
		break;
 	case 1:
		pPart->PushIn();
		break;
 	case 2:
		pPart->PopOut();
		break;
 	case 3:
		pPart->Emptying();
		break;
 	case 4:
		pPart->ShiftInner();
		break;
	default:
		break;
	}
	if((5==a)&&(4==B)) // 传过来B是3 ，上面+1
	{
		m_pPart3->ActAddSamplePos();
	}
   
    if((5==a)&&(3==B))// 移动到扫描位置
	{
	  m_pPart2->DebugReadBarCode();	 
	}

	return TRUE;
}

//单步动作调试
BOOL CInBox::DebugMotor(int movetype,int motorID)
{
	CTrackMotor *pmotor=NULL;
	motorID+=1;
	switch(motorID)
	{
	case 1:
		pmotor=m_pPart1->GetPartMotor(0);
		break;
	case 2:
		pmotor=m_pPart2->GetPartMotor(0);
		break;
	case 3:
		pmotor=m_pPart3->GetPartMotor(0);
		break;
	case 4:
		pmotor=m_pPart3->GetPartMotor(1);
		break;
	case 5:
		pmotor=m_pPart3->GetPartMotor(2);
		break;
	case 6:
         pmotor=m_pPart5->GetPartMotor(0); // 软件界面和接线统一
		 //pmotor=m_pPart4->GetPartMotor(0);
		break;
	case 7:
		pmotor=m_pPart4->GetPartMotor(0);
		//pmotor=m_pPart5->GetPartMotor(0);
		break;
	case 8:
		pmotor=m_pPart6->GetPartMotor(0);
		break;
	case 9:
		pmotor=m_pPart7->GetPartMotor(0);
		break;
	case 10:
		pmotor=m_pPart3->GetPartMotor(3);
		break;

	default:
		break;
	}
	if(NULL!=pmotor)
	{
		MotorActParam param;
		param.type=RELATIVE_POS;
		param.speedStar=1000;
		param.speedEnd=2000;
		param.step=100;
		param.ACCDec=120000;
		pmotor->WriteActParam(ACT_Debug,param);
		switch(movetype)
		{
			case 1:		//OnButtonStepBack
				pmotor->MoveRelativeAndWait(ACT_Debug,DIR_CCW);
				break;
			case 2:		//OnButtonStepGo
				pmotor->MoveRelativeAndWait(ACT_Debug,DIR_CW);
				break;
			case 3:		//OnUnlock
				pmotor->LockMotor(FALSE,FALSE);
				break;
			case 4:		//OnLock
				pmotor->LockMotor(TRUE,FALSE);
				break;
			default:
				break;
		}
	}
	return TRUE;
}
BOOL CInBox::StepDebugMotor(int movetype,int motorID,int DIR)		//单步动作调试
{
	CTrackMotor *pmotor=NULL;
	motorID+=1;
	switch(motorID)
	{
	case 1:
		pmotor=m_pPart1->GetPartMotor(0);
		break;
	case 2:
		pmotor=m_pPart2->GetPartMotor(0);
		break;
	case 3:
		pmotor=m_pPart3->GetPartMotor(0);
		break;
	case 4:
		pmotor=m_pPart3->GetPartMotor(1);
		break;
	case 5:
		pmotor=m_pPart3->GetPartMotor(2);
		break;
	case 6:
		pmotor=m_pPart5->GetPartMotor(0);
		//pmotor=m_pPart4->GetPartMotor(0);
		break;
	case 7:
	   //	pmotor=m_pPart5->GetPartMotor(0);
		pmotor=m_pPart4->GetPartMotor(0);
		break;
	case 8:
		pmotor=m_pPart6->GetPartMotor(0);
		break;
	case 9:
		pmotor=m_pPart7->GetPartMotor(0);
		break;
	case 10:
		pmotor=m_pPart3->GetPartMotor(3);
		break;
	default:
		break;
	}
	if(NULL!=pmotor)
	{
		FASSERT(movetype<10);
		pmotor->MoveRelativeAndWait(movetype,(DIR>0)?DIR_CW:DIR_CCW);
	}
	return TRUE;
}
BOOL CInBox::DebugBarcode(int motorID,wxString &strbar)		//单步动作调试
{
	/*strbar="";
	motorID+=1;
	switch(motorID)
	{
	case 2:
		return m_pPart2->DebugReadBarCode(strbar);
	case 4:
		return m_pPart4->DebugReadBarCode(strbar);
	default:
		break;
	}*/
	return TRUE;
}


BOOL CInBox:: DebugMotorAct(int motorID,int param)
{

	switch(motorID)
	{
	case 1: //
		if (param==0)
		{
		    m_pPart1->m_pMotor1->InitMove();
            m_pPart1->m_pMotor1->InitHome();
		}
		else if(param==1)
		{ 
			BOOL IsLiquid=FALSE;
            m_pPart1->m_pMotor1->MoveLiquidOrLimit(IsLiquid);
		}
		else if(param==2)
		{
             m_pPart1->m_pMotor1->MoveArmUp();  
		}
		else if(param==3)
		{
             m_pPart1->m_pMotor1->MoveArmDown();  
		}
		break;
	case 2:

		if (param==0)
		{
			m_pPart2->m_pMotor2->InitMove();
			m_pPart2->m_pMotor2->InitHome();
			m_pPart2->m_pMotor2->MoveBackStep();
		}
		else if(param==1)
		{  
            m_pPart2->m_pMotor2->MoveTrayBarcodeStar();
		}
		else if(param==2)
		{ 
		    wxString  strBar;
		    m_pPart2->m_pMotor2->ReadTrayBarCode(strBar);
			TRACE(_T("DebugMotorAct::ReadTrayBarCode strBar=%s"),strBar.mb_str(wxConvUTF8));
           
		}
		else if(param==3)
		{   
			wxString  strBar;
		    m_pPart2->m_pMotor2->ReadSampleBarCode(TRUE,strBar);
			TRACE(_T("DebugMotorAct::ReadSampleBarCode strBar=%s"),strBar.mb_str(wxConvUTF8));

		}
		else if(param==4)
		{
		  m_pPart2->m_pMotor2->MoveSpace();
		}
		else if(param==5)
		{
			m_pPart2->m_pMotor2->MoveToExit();
		}
		else if(param==6)
		{
			  m_pPart2->m_pMotor2->MoveOut();
		}
		break;
	case 3:

		if (param==0)
		{
			m_pPart3->m_pMotor3->InitMove();
			m_pPart3->m_pMotor3->InitHome();
		}
		else if(param==1)
		{ 
          	m_pPart3->m_pMotor3->MoveUp();
		}
	
		break;
	case 4:

		if (param==0)
		{
			m_pPart3->m_pMotor4->InitMove();
		}
		else if(param==1)
		{ 
           m_pPart3->m_pMotor4->MoveToOrg(); //();
		}
		else if(param==2)
		{
            m_pPart3->m_pMotor4->MoveToAddSamplePos(); 
		}
		else if(param==3)
		{
             m_pPart3->m_pMotor4->MoveHole();
		}
		else if(param==4)
		{
               m_pPart3->m_pMotor4->MoveEnd();
		}
		break;
	case 5: 

		if (param==0)
		{
			m_pPart3->m_pMotor5->InitMove();
		    m_pPart3->m_pMotor5->InitHome();
		}
		else if(param==1)
		{ 
              m_pPart3->m_pMotor5->PushInLimit();
		}
	
		break;
	case 6:

		if (param==0)
		{
			m_pPart5->m_pMotor6->InitMove();
			m_pPart5->m_pMotor6->InitHome();
		}
		else if(param==1)
		{ 
              m_pPart5->m_pMotor6->PushToRecheck();
		}
		else if(param==2)
		{
			   m_pPart5->m_pMotor6->PushToCollect();
		}
		break;
	case 7:

		if (param==0)
		{
			m_pPart4->m_pMotor7->InitMove();
			//m_pPart4->m_pMotor7->InitHome();
		}
		else if(param==1)
		{ 
			BOOL BLimit=FALSE;
			m_pPart4->m_pMotor7->MoveLimit(BLimit);
		}
		else if(param==2)
		{ 
			m_pPart4->m_pMotor7->MoveStep();
		}
		break;
	case 8:

		if (param==0)
		{
			m_pPart6-> m_pMotor8->InitMove();
			m_pPart6-> m_pMotor8->InitHome();
		}
		else if(param==1)
		{ 
           m_pPart6-> m_pMotor8->PushInLimit();
		}
		else if(param==2)
		{
            m_pPart6-> m_pMotor8->PushInStep();
		}
		break;
	case 9:

		if (param==0)
		{
			m_pPart7->m_pMotor9->InitMove();
			m_pPart7->m_pMotor9->InitHome();
		}
		else if(param==1)
		{ 
            m_pPart7->m_pMotor9->MoveLimit();
		}
		else if(param==2)
		{
			BOOL bFull=FALSE;
             m_pPart7->m_pMotor9->MoveHalf(bFull);
		}

		else if(param==3)
		{
			BOOL bFull=FALSE;
		    m_pPart7->m_pMotor9->MoveEnd(bFull);
		}
		else if(param==4)
		{
             m_pPart7->m_pMotor9->MoveStep(); 
		}
		break;
	case 10:
		if (param==0)
		{
			if(m_TackType ==1)
			{
				m_pPart3->m_pMotor10->InitMove();
				m_pPart3->m_pMotor10->InitHome();
				m_pPart3->m_pMotor10->MoveToOpenPos();

			}
		
		}
		else if(param==1)
		{ 
		   if(m_TackType ==1)
		   {
              m_pPart3->m_pMotor10->MoveToClosePos();
		   }

		}
		break;
	default:
		break;
	}

   return TRUE;
}









//单步动作调试
BOOL CInBox::MonitorIO(BYTE input[ALL_PS_COUNT])
{
	memset(input,0,ALL_PS_COUNT);
	BOOL BVaild[ALL_PS_COUNT]={0};

	// IO 信号
	m_pPart1->GetPartMotor(0)->GetMotorOrg(BVaild[0]);   //M1原点
	m_pPart1->GetPartMotor(0)->GetMotorLeftLimit(BVaild[1]);   //M1左限位
    m_pPart1->GetPartMotor(0)->GetMotorLiquid(BVaild[2]);   //M1 液位  到位检测
    
	m_pPart1->GetPartMotor(0)->GetIOLevel1(BVaild[3]);     // M1 托盘检测
	
	m_pPart2->GetPartMotor(0)->GetMotorOrg(BVaild[4]);		//M2 原点
	m_pPart2->GetPartMotor(0)->GetIOLevel2(BVaild[5]);	   // M2 试管检测 Ps6
	
	m_pPart3->GetPartMotor(0)->GetMotorOrg(BVaild[6]) ; // M3 原点
	m_pPart3->GetPartMotor(1)->GetMotorOrg(BVaild[7]) ; // M4 原点 对射传感
	m_pPart3->GetPartMotor(1)->GetMotorLeftLimit(BVaild[8]) ; // M4  左限位  微动开关
	m_pPart3->GetPartMotor(2)->GetMotorOrg(BVaild[9]) ; // M5  原点
	m_pPart3->GetPartMotor(2)->GetMotorRightLimit(BVaild[10]) ; // M5  限位

    m_pPart4->GetPartMotor(0)->GetMotorLeftLimit(BVaild[11]);  // M7 限位  调试界面上 标注的是M6。

	m_pPart5->GetPartMotor(0)->GetMotorOrg(BVaild[12]);  // M6 原点
	m_pPart5->GetPartMotor(0)->GetMotorLiquid(BVaild[13]);  //M6 液位
	m_pPart5->GetPartMotor(0)->GetMotorLeftLimit(BVaild[14]); //M6 左限位


	m_pPart6->GetPartMotor(0)->GetMotorOrg(BVaild[15]);  //M8  原点
	m_pPart6->GetPartMotor(0)->GetMotorRightLimit(BVaild[16]); //M8 右限位

	m_pPart7->GetPartMotor(0)->GetMotorOrg(BVaild[17]);  //M9  原点
	m_pPart7->GetPartMotor(0)->GetMotorLiquid(BVaild[18]); //M9 中间检测
	m_pPart7->GetPartMotor(0)->GetMotorRightLimit(BVaild[19]); //M9 右限位
   	m_pPart7->GetPartMotor(0)->GetIOLevel1(BVaild[20]); //M9 托盘检测

	m_pPart2->GetPartMotor(0)->GetMotorLeftLimit(BVaild[21]);	   // M2 试管检测
 
	if (m_TackType==1)
	{
		m_pPart3->GetPartMotor(3)->GetMotorOrg(BVaild[22]) ; // M10 原点
	}
	

	for(int i=0;i<ALL_PS_COUNT;i++)
	{
		if(BVaild[i])
		{
			input[i]=1;
		}

	}
	return TRUE;
}


void CSTMachine::DeBugAct()
{
 
	m_pPart1->ActReset();
	m_pPart2->ActReset();
	m_pPart3->ActReset();
	m_pPart4->ActReset();
    m_pPart5->ActReset();
    m_pPart6->ActReset();
    m_pPart7->ActReset();
}