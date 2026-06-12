// MainRunState.cpp: implementation of the CMainRunState class.
//
//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MainRunState.h"
#include "Machine.h"
#include "../test.h"
#include "../comon.h"
#include "wx/listimpl.cpp"
#include "../trace.h"
#include "../protocol/PcDebugEX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMainRunState::CMainRunState():m_GoEvt(FALSE)
{
	m_Exit=FALSE;
    ResetReportErr();
	m_bNeedReset = TRUE ;
    m_bNeedResetEvent = FALSE;
	m_LastCheckDrawTime =0;
	m_LastCheckCoverTime =0;
	m_bNeedInit =FALSE;
}

CMainRunState::~CMainRunState()
{
	m_Exit=TRUE;
	wxMilliSleep(500);
}

// 初始化仪器状态--
BOOL CMainRunState::InitRuningState()
{
	g_MachineState =  MACHINE_INIT; // 开机初始化
	m_DEBUG_CMD_LIST.BindSemp(&m_pST->m_DealPCcmdSemp);
	return TRUE;
}

/****************************************************************************/
/*						仪器命令接口函数
/***************************************************************************/
// 命令处理
BOOL CMainRunState::DealWithMsg(MSG_TYPE type,CBaseMsg *pnew)
{
	if(!CanDealCmd(type,pnew))
	{
		CNakMsg *pNak=new CNakMsg;//
		m_pST->m_pAllTcpIp->SendMsgNotWait(pNak);
		delete pnew;
		return FALSE;
	}
	return TRUE;
}
BOOL CMainRunState::CanDealCmd(MSG_TYPE type,CBaseMsg *pnew)
{
	UINT cmdType=FromMsgTypeToCmdTyp(type);
	BYTE cmd=GET_CMD(cmdType);
	switch(cmd)
	{
	case MACHINE_CMD_DEBUG://调试
		if((MACHINE_READY==g_MachineState)||(MACHINE_DEBUG==g_MachineState)||(MACHINE_STOP==g_MachineState))
		{
			m_bNeedReset = TRUE;
			TrackSwitchState(MACHINE_DEBUG);
			return AddDebugCmd(pnew);
		}
		break;
	case  MACHINE_CMD_STOP:
		if((MACHINE_READY==g_MachineState)||(MACHINE_DEBUG==g_MachineState) ||(MACHINE_TEST==g_MachineState) ||(MACHINE_PAUSE==g_MachineState))
		{
			m_bNeedReset = TRUE;
		     MachineStateType MachineState =g_MachineState;
			TrackSwitchState(MACHINE_STOP,FALSE);	//回复停止成功
			if(MACHINE_PAUSE==MachineState)
			{
              	m_pPart->SetPauseEvent(); // 防止一直等待暂停信号
			}
			m_bNeedInit = TRUE;
			m_ProgramQueue.EmptyQueue();  //清空命令队列
			delete pnew;
			return TRUE;
		}
		break;
	case MACHINE_CMD_RESET://复位
		if (MACHINE_READY == g_MachineState || MACHINE_STOP == g_MachineState || MACHINE_DEBUG==g_MachineState || MACHINE_WAITINIT==g_MachineState)
		{
			delete pnew;
			TrackSwitchState(MACHINE_INIT,FALSE);
			CSndResetReq *pMsg = new CSndResetReq;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
			SndMachineState();
			return TRUE;
		}
		break;
	case MACHINE_CMD_PAUSE://复位  暂停 时间计时要继续
		if (MACHINE_TEST == g_MachineState || MACHINE_PAUSE == g_MachineState)
		{
			CPauseTestMsg  *pMsg=(CPauseTestMsg *)pnew; // 1 暂停  0 继续
			if (pMsg->m_Pause ==1 && MACHINE_TEST == g_MachineState )
			{
				m_pPart->ResetPauseEvent();
				TrackSwitchState(MACHINE_PAUSE,FALSE);
				delete pnew;
				CSndPauseTestMsg *pMsg1 = new CSndPauseTestMsg;
				g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);
				SndMachineState();
			}
			else if (pMsg->m_Pause ==0 && MACHINE_PAUSE == g_MachineState)//继续
			{
				delete pnew;
				CSndPauseTestMsg *pMsg1 = new CSndPauseTestMsg;
				BOOL bClose =FALSE ,bInPalce =FALSE  ;//检测舱门和抽屉    
				int ret=  m_pPart->m_pZMotor->GetMotorLiquid(bClose); //舱门
				wxMilliSleep(20);
				ret=  m_pPart->m_pZMotor->GetMotorRightLimit(bInPalce); //抽屉
                
				if (m_pPart->m_CheckFrontCoverMark == 0) //不检测舱门
				{
                      bClose =TRUE;
				}
				
				if (bClose && bInPalce)
				{
					g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);
					m_pPart->SetPauseEvent();
					TrackSwitchState(MACHINE_TEST,FALSE);
					SndMachineState();
				}
				else
				{

					pMsg1->m_Success =0;
					g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1); //通知失败
					TrackSwitchState(MACHINE_PAUSE,FALSE);
					SndMachineState();
					CSndQueryLampAndCoverStatus *pMsg2 =new CSndQueryLampAndCoverStatus ;
					m_pPart->GetLampAndCoverStaues(pMsg2->m_Lamp,pMsg2->m_UVLamp,pMsg2->m_FrontCover,pMsg2->m_Drawer);
					g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg2); //;

				}
			}
           	return TRUE;
		}
		break;
	case  MACHINE_CMD_RUN:	//启动
		if (MACHINE_READY == g_MachineState )
		{
			delete pnew;
			if(MACHINE_TEST!=g_MachineState)
			{
				TrackSwitchState(MACHINE_TEST,FALSE);
			}
			//回复
			CSndStartRunMsg*pMsg = new CSndStartRunMsg;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
            SndMachineState();
			return TRUE;
		}
		else if (MACHINE_STOP == g_MachineState || MACHINE_DEBUG==g_MachineState || MACHINE_WAITINIT==g_MachineState)
		{
			// 其他情况下需要初始化
			m_bNeedReset = TRUE;
			delete pnew;
			if(MACHINE_TEST!=g_MachineState)
			{
				TrackSwitchState(MACHINE_TEST,FALSE);
			}
			//回复
			CSndStartRunMsg*pMsg = new CSndStartRunMsg;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
			SndMachineState();
			return TRUE;
		}
		break;
	case  MACHINE_CMD_ALL_STATUS:

		if (type == MSG_RcvQueryStatus) // 查询状态
		{
			CMedchineStatus *pMsg = new CMedchineStatus ;
			pMsg->m_Status=g_MachineState;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
			delete pnew;
			return TRUE;
		}
		else if (type == MSG_RcvHeartBeat)
		{
			CSndHeartBeat *pMsg = new CSndHeartBeat ;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
			delete pnew;
			return TRUE;
		}
		else if (type == MSG_RcvQueryVersion)
		{
			CSndVersionInfo  *pMsg = new CSndVersionInfo ;
			pMsg->m_strVersion = SoftVersion;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
			delete pnew;
			return TRUE;
		}
		else if (type == MSG_RcvQueryIOInfo) // 查询IO
		{
			CSndAllIOMsg *pMsg = new CSndAllIOMsg ;
			ReadIO(pMsg->IO);
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
			delete pnew;
			return TRUE;
		}
		else if(type == MSG_RcvSetIO)
		{
			CRcvSetIO *pMsg = (CRcvSetIO *) pnew ;
			OpenCloseIO(pMsg->m_IO,pMsg->m_State); //
			CSndSetIO *pMsg1 =new CSndSetIO ;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1); //回复设置成功
			delete pnew;
			return TRUE;
		}
        else if (type ==MSG_RcvOpenCloseLamp)
        {
             CRcvOpenCloseLamp *pMsg = (CRcvOpenCloseLamp *) pnew ;
             m_pPart->OpenOrCloseLamp(pMsg->m_Open);
			 CSndOpenCloseLamp *pMsg1 =new CSndOpenCloseLamp ;
			 g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1); //回复设置成功
			 delete pnew;
			 return TRUE;
        }
		else if (type ==MSG_RcvQueryLampAndCoverStatus)
		{
			CSndQueryLampAndCoverStatus *pMsg =new CSndQueryLampAndCoverStatus ;
            m_pPart->GetLampAndCoverStaues(pMsg->m_Lamp,pMsg->m_UVLamp,pMsg->m_FrontCover,pMsg->m_Drawer);
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg); //回复设置成功;
			delete pnew;
			return TRUE;
		}
		else if (type ==MSG_Ack)
		{
			delete pnew;
			return TRUE;
		}
		break;

	case  MACHINE_CMD_EXCEPT_RUN:	 //运行中不设置参数

		if((MACHINE_READY==g_MachineState)||(MACHINE_DEBUG==g_MachineState)||(MACHINE_STOP==g_MachineState)||(MACHINE_WAITINIT==g_MachineState))
		{
			if (type == MSG_RcvProgramStep) //程序步骤
			{
				 CReciveProgramStep *pMsg = (CReciveProgramStep*)pnew;
                 CSndProgramStep *pMsg1 =new CSndProgramStep;
				 if (!RcvProgramStep(pMsg))
				 {
					 pMsg1->m_Success =0 ;
				 }
				 g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);	//回复指令
				 delete pnew;
				return TRUE;
			}
			else if (type == MSG_RcvOpenCloseUVLamp)
			{
				CRcvOpenCloseUVLamp *pMsg = (CRcvOpenCloseUVLamp*)pnew;
				m_pPart->OpenOrCloseUVLamp(pMsg->m_Open,pMsg->m_Time);

				delete pnew;
				return TRUE;
			}
			else if (type == MSG_RcvSuckMagneticWay)
			{
                CRcvSuckMagneticWay *pMsg = (CRcvSuckMagneticWay*)pnew;
				m_pPart->SetSuckMagneticWay(pMsg->m_Way);
				CSndSuckMagneticWay *pMsg1 =new CSndSuckMagneticWay;
				g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);	//回复指令
				delete pnew;
				return TRUE;
			}
			else if (type == MSG_RcvMotionParameters)
			{
		    	CRcvMotionParameters *pMsg = (CRcvMotionParameters*)pnew;
				if (pMsg->m_Mark==2)
				{
					m_pPart->SetMotionParameters(pMsg->m_HorizontalSpeed,pMsg->m_HorizontalMixSpeed,pMsg->m_HorizontalSuckMagneticSpeed,pMsg->m_UpDownSpeed,pMsg->m_UpDownMixSpeed,pMsg->m_SuckMagneticSpeed);
				}
				CSndMotionParameters *pMsg1 =new CSndMotionParameters;
                m_pPart->GetMotionParameters(pMsg1->m_HorizontalSpeed,pMsg1->m_HorizontalMixSpeed,pMsg1->m_HorizontalSuckMagneticSpeed,pMsg1->m_UpDownSpeed,pMsg1->m_UpDownMixSpeed,pMsg1->m_SuckMagneticSpeed);
				g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);	//回复指令
				delete pnew;
				return TRUE;
			}
			else if (type == MSG_RcvSetFactoryDefaultSpeed)
			{
				m_pPart->SetFactoryDefaultSpeed();
				CSndSetFactoryDefaultSpeed *pMsg1 =new CSndSetFactoryDefaultSpeed;
				g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);	//回复指令
				delete pnew;
				return TRUE;
			}
			else if(type == MSG_RcvSaveParam) // 保存参数
			{
				CSaveParamMsg *pMsg =(CSaveParamMsg *) pnew ;
				if (pMsg->m_btSave) //保存参数
				{
					SaveParamToIni();
					CSndSaveParamMsg  *pMsg1 = new CSndSaveParamMsg ;
					g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1); //回复成功
				}
				delete pnew;
				return TRUE;
			}
		}

		break;
	}
	return FALSE;
}

/****************************************************************************/
/*						仪器命令接口函数
/***************************************************************************/

BOOL CMainRunState::AddDebugCmd(CBaseMsg* cmd)
{
	m_DEBUG_CMD_LIST.AddTail(cmd);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//	删除所有的PC调试命令
//返回	1		成功
//		0		失败 PCMDMSGList
//////////////////////////////////////////////////////////////////////////////
BOOL CMainRunState::EmptyPCDebugCmd(void)
{
	//把要处理的调试队列COPY一份,再把队列清空,需要互斥
	PCMDMSGList Deal_PC_CMD_LIST;
	//Deal_PC_CMD_LIST.DeleteContents(true);
	m_DEBUG_CMD_LIST.CopyTo(&Deal_PC_CMD_LIST);

	PCMDMSGList::Node* node = Deal_PC_CMD_LIST.GetFirst();
	while (node)
	{
		CBaseMsg* pCmd = node->GetData();
		delete pCmd;
		node = node->GetNext();
	}
	Deal_PC_CMD_LIST.clear();
	return TRUE;
}

/****************************************************************************/
/*						 工作线程的仪器状态运行函数			*/
/****************************************************************************/
//状态机主函数
UINT CMainRunState::SatePartRun()
{
	while(!m_Exit)
	{
		m_GoEvt.WaitTimeout(10);
		WorkProc();
	};
	return 1;
}

//
UINT CMainRunState::WorkProc()
{
	if ( m_pST->m_bErrorStop)
	{
		//等待所有动作停止
		TRACE(_T("CMainRunState::WorkProc  StopMotor1"));
		m_pPart->StopMotor();
		if (m_pWorkThread->WaitAllPartFinish(10*1000))    //等待部件停止
		{
			g_MachineState = MACHINE_STOP;
			m_pMainTask->TrackSwitchState(MACHINE_STOP);
			m_bNeedInit = TRUE;
			m_pST->m_bErrorStop = FALSE;
		}
	}
	else
	{
		switch(g_MachineState)
		{
		case MACHINE_WAITINIT:
			 wxMilliSleep(2000); //等待初始化
			 break ;
		case MACHINE_INIT: // 开机是否直接初始化
			RunAllReset();
			break ;
		case MACHINE_READY: // 就绪
			RunIdleProc();
			break;
		case MACHINE_TEST: // 启动
		case MACHINE_PAUSE:// 暂停
			RunTestProc();
			break;
		case MACHINE_DEBUG:// 调试
			RunDebugProc();
			break;
		case MACHINE_STOP: // 停止
			RunStopProc();
			break;
		default:
			FASSERT(FALSE);
			break;
		}
		if (g_MachineState!=MACHINE_DEBUG &&  g_MachineState!=MACHINE_INIT &&  g_MachineState!=MACHINE_WAITINIT)
		{
			m_pPart->SendLampAndCoverStaues(); //发送灯、舱门状态、时间间隔1秒
		}
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////////
//	调试流程
//返回	1		成功
//		0		失败
//////////////////////////////////////////////////////////////////////////////
UINT CMainRunState::RunDebugProc(void)
{
	if(!m_DEBUG_CMD_LIST.IsEmpty())
	{
		CBaseMsg* pcmd=m_DEBUG_CMD_LIST.RemoveHead();
		//  调试命令处理
		UINT cmdType=FromMsgTypeToCmdTyp(pcmd->m_MessageType);
		BYTE cmd=GET_CMD(cmdType);
		if(MACHINE_CMD_DEBUG==cmd)
		{
			int rtn=ExecPCDebugCmd(pcmd); //--命令执行之前已经回复ACK
			delete pcmd;
			return rtn;
		}
	}
	return 0;
}

// 执行调试
UINT CMainRunState::ExecPCDebugCmd(CBaseMsg* pcmd)
{
	switch(pcmd->m_MessageType)
	{
	case MSG_DebugPart:
		{
			CPcDebugPart *pMsg=(CPcDebugPart *)pcmd;
			return ExecPartMotorDebug(pMsg->Part,pMsg->MotorIdx,pMsg->act,pMsg->param);
		}
	default:
		FASSERT(FALSE);
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	空闲流程
// 空闲状态下 可以接受 调试命令

//////////////////////////////////////////////////////////////////////////////
UINT CMainRunState::RunIdleProc(void)
{
	DWORD dwState=m_pST->m_DealPCcmdSemp.WaitTimeout(2000);
	if(dwState == wxSEMA_NO_ERROR) //调试命令
	{
		RunDebugProc();
	}
	else 	//测试信息
	{
		wxMilliSleep(100);
		return  0;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	停止流程
//返回	1		成功
//		0		失败
//////////////////////////////////////////////////////////////////////////////
UINT CMainRunState::RunStopProc(void)
{
	m_pST->m_DealPCcmdSemp.WaitTimeout(500);
	if(m_bNeedInit)
	{
        m_bNeedInit =FALSE;
        m_pPart-> StopMotor();
		CSndStopRunMsg *pMsg = new CSndStopRunMsg;
		if(m_pWorkThread->WaitAllPartFinish(20*1000)) //等待部件停止
		{
			pMsg->m_Success =1;
		}
		else
		{
			pMsg->m_Success =0;
		}
		m_pPart->InitPart(FALSE);
		g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);
		SndMachineState();
	}

	return TRUE;
}

//电机复位
int CMainRunState:: RunAllReset()
{
	//m_FirstRun = TRUE;
	m_RunStep =0;
	m_StepRunIndex =0;
    ResetReportErr();
	//m_ProgramQueue.EmptyQueue(); //清空队列
    m_bNeedResetEvent =FALSE;
	m_bSendLeftTime = FALSE;
	m_LastCheckDrawTime =0;
	m_LastCheckCoverTime =0;
	m_pST->Reset();
	EmptyPCDebugCmd(); //清空命令
	if (!m_pST->PartReset()) // 动作复位
	{
		m_pErrSet->SetPartErr(PART_MACHINE,ERROR_PART_RESET);//报错复位失败
		TrackSwitchState(MACHINE_STOP);
	}
	else
	{
		TrackSwitchState(MACHINE_READY); //准备就绪
		m_bNeedReset =FALSE;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	测试流程
//返回	1		成功
//		0		停止测试
//////////////////////////////////////////////////////////////////////////////
UINT CMainRunState::RunTestProc(void)
{
	if (m_bNeedReset) //非就绪状态接受启动测试先要进行复位
	{
		m_RunStep =0;
		m_StepRunIndex =0;
		ResetReportErr();
		m_pST->Reset();
		m_bNeedResetEvent =FALSE;
		EmptyPCDebugCmd(); //清空命令
		if (!m_pST->PartReset()) // 动作复位
		{
			m_pErrSet->SetPartErr(PART_MACHINE,ERROR_PART_RESET);//报错复位失败
			TrackSwitchState(MACHINE_STOP);
			return 0;
		}
		m_bNeedReset = FALSE;
        m_bSendLeftTime = FALSE;
	}
	if(m_pST->m_bErrorStop)  return 0;
	if(g_MachineState == MACHINE_PAUSE)
	{
    	wxMilliSleep(200);
	    return 0;
	}
	int iRtn=0;
//	TRACE(_T("CMainRunState::RunTestProc  m_RunStep =%d"),m_RunStep);
#ifdef  _SIMULATOR_
	if (m_RunStep==0)
	{
		//AddProgramSimulator();
		//AddVesselsInfoSimulator();
		//m_VesselsInfoQueue.PrintVesselsInfoInfo(); // 打印容器信息
		m_ProgramQueue.PrintProgramStepInfo();// 打印程序步骤信息
	}
#endif

	if(m_ProgramQueue.GetStepCount()<=0 ) //没有执行步骤
	{
        TrackSwitchState(MACHINE_STOP);
		return 0;
	}
	if (m_ProgramQueue.GetStepCount()>0 && !m_bSendLeftTime && m_StepRunIndex == 0 ) //开始发送剩余时间
	{
		CSndLeftTime *pMsg = new CSndLeftTime ;
		pMsg->m_Time = m_ProgramQueue.GetFinishTimeByIndex(m_StepRunIndex);
	    DWORD Time  =GetCompensateTime(m_StepRunIndex); //吸磁补偿时间
        pMsg->m_Time+=(Time/1000); //毫秒转秒
		m_pST-> m_pAllTcpIp->SendMsgNotWait(pMsg);
		m_bSendLeftTime =TRUE;
	}
	//判断当前是否前舱门是否打开
    BOOL bClose =FALSE ;
	bClose =CheckFrontCover() ;

	if (!bClose && !m_bNeedResetEvent)
	{
	    m_pPart->ResetPauseEvent(); //暂停运行
		//TrackSwitchState(MACHINE_PAUSE);
		m_bNeedResetEvent = TRUE;
	}

    BOOL bInPalce =FALSE ;
    bInPalce = CheckDrawerInPlace();
	if (!bInPalce && !m_bNeedResetEvent)
	{
	    m_pPart->ResetPauseEvent(); //暂停运行
		//TrackSwitchState(MACHINE_PAUSE);
		m_bNeedResetEvent = TRUE;
	}
	if(bInPalce && bClose)
	{
		if (m_bNeedResetEvent) // 复位暂停事件
		{
			m_pPart->SetPauseEvent();
            //TrackSwitchState(MACHINE_TEST);
			m_bNeedResetEvent = FALSE;
		}
	}
    // 继续上一次反应。
	if (bClose && bInPalce)
	{
		//等待当前部件是否是空闲状态，是否需要复位
		if(  m_ProgramQueue.GetStepCount()>0 && GLOBAL(m_pWorkThread)->JudgeIdel(PART_ARM)>0 )  //判断当前步骤执行完成，执行完成变成完成状态、
		{
			if (m_ProgramQueue.GetStepCount()>0 && !m_bSendLeftTime && m_StepRunIndex != 0  ) //开始发送剩余时间
			{
				CSndLeftTime *pMsg = new CSndLeftTime ;
				pMsg->m_Time = m_ProgramQueue.GetFinishTimeByIndex(m_StepRunIndex);
				DWORD Time  =GetCompensateTime(m_StepRunIndex); //吸磁补偿时间
				pMsg->m_Time+=(Time/1000); //毫秒转秒
				m_pST-> m_pAllTcpIp->SendMsgNotWait(pMsg);
				m_bSendLeftTime =TRUE;
			}
	    	if(m_StepRunIndex<m_ProgramQueue.GetStepCount())
			{
				ProgramStep stepInfo=  m_ProgramQueue.GetProgramStep(m_StepRunIndex);//下一步反应信息
				if(!stepInfo.IsEmpty()) //步骤非空。
				{
					GLOBAL(m_pWorkThread)->SendPartCmdAndWaitRun(PART_ARM,CMD_ACTCYCLE,stepInfo); //执行命令
					m_StepRunIndex++;
					m_bSendLeftTime = FALSE; // 每步都发送剩余时间
				}
				else
				{
					if (MACHINE_STOP!=g_MachineState)
					{
						m_pST->PartReset();
					}
	                m_pPart->SndActionStarOrEndMsg(0,ACT_STATUS_ALL_END,0,0); 	//	//通知所有结束
					TrackSwitchState(MACHINE_STOP);
					m_StepRunIndex =0;
				}
			}
			else
			{
				if (MACHINE_STOP!=g_MachineState)
				{
				   m_pST->PartReset();
				}
				m_StepRunIndex =0;
			    m_pPart->SndActionStarOrEndMsg(0,ACT_STATUS_ALL_END,0,0); 	//	//通知所有结束
		       	TrackSwitchState(MACHINE_STOP);
			}
		}
	}
    wxMilliSleep(1000);//
	if (m_RunStep>=INT_MAX)
	{
		m_RunStep =0 ;
	}
	m_RunStep++;
	return 0;
}
DWORD CMainRunState:: GetCompensateTime(int StepRunIndex)
{
	DWORD Time  =0;
	// 补偿吸磁运动时间
	int ncount=m_ProgramQueue.GetStepCount();
	// 悬停吸磁需要孔1 增加下降和左右移动 时间 其他孔增加下降和上升时间，
	for(int i=m_StepRunIndex;i<ncount;i++)
	{
		ProgramStep stepInfo=  m_ProgramQueue.GetProgramStep(i);//下一步反应信息
		if(!stepInfo.IsEmpty()) //步骤非空。
		{
			if (stepInfo.MagneticTime >0)
			{
				float Height =m_pPart->GetMoveHeight(m_ProgramQueue.m_Type,stepInfo.HolePos,stepInfo.Volume);
				if(m_ProgramQueue.m_SuckMagneticWay ==XUAN_TING) //悬停吸磁
				{
					if (stepInfo.HolePos==1)
					{
						DWORD  MoveHoleOneTime = m_pPart->m_pXMotor->GetMoveHalfHoleOneDistanceTime(m_ProgramQueue.m_Type,stepInfo.SuckMagLevel,FALSE)*2;
						int MaxTimes = Height/m_pPart->m_StepHeight;  // 计算次数，高度/每次下降到高度
						DWORD TempTime  = MoveHoleOneTime*(MaxTimes+2) ;
						Time+= m_pPart->m_pZMotor->MoveDownStepTime(Height);        /*   (Height*100/m_pPart->m_pZMotor->m_iSpeed[4])*1000; */ //下降时间
						Time+=TempTime;
					}
					else
					{
						// 其他孔增加下降和上升时间
						Time+=Height*(ZMOTOR_MM_TO_PULSE)*10*1000*2/(m_pPart->m_pZMotor->m_iSpeed[7]*stepInfo.SuckMagLevel);  //2 因为上升和下降
					}
				}
			}
		}
	}

    return  Time;
}

const static TCHAR strpratAry[][20]={_T("MACHINE_WAITINIT"),_T("MACHINE_INIT"),_T("MACHINE_READY"),_T("MACHINE_DEBUG"),_T("MACHINE_TEST"),_T("MACHINE_PAUSE"),_T("MACHINE_STOP"),_T("MACHINE_ERROR")};

void CMainRunState::TrackSwitchState(MachineStateType mode,BOOL BSend)
{
	TRACE(_T("CMainRunState::TrackSwitchState  LastState =%s ,State=%s"),strpratAry[g_MachineState],strpratAry[mode]);
	g_MachineState =mode ;
	if(BSend)
	{
		CMedchineStatus *pMsg = new CMedchineStatus ;
		pMsg->m_Status=g_MachineState;
		m_pST->m_pAllTcpIp->SendMsgNotWait(pMsg);
	}
}
// 发送仪器状态
void CMainRunState::SndMachineState()
{
	TRACE(_T("CMainRunState::SndMachineState  State=%s"),strpratAry[g_MachineState]);
	CMedchineStatus *pMsg = new CMedchineStatus ;
	pMsg->m_Status=g_MachineState;
	m_pST->m_pAllTcpIp->SendMsgNotWait(pMsg);
}

//事件复位
BOOL CMainRunState::Reset()
{
	m_GoEvt.ResetEvent();
	return TRUE;
}

/**
    @brief  通过当前下一步反应信息
    @param ProgramIndex:对应程序序号
    @param Step:当前步骤
    @return ProgramStep  反应步骤信息.
    @date 2024.

*/
ProgramStep CMainRunState:: GetProgramNextStepByStep(int step)
{
	ProgramStep stepInfo= m_ProgramQueue.GetProgramStep(step+1);//下一步反应信息
	return stepInfo;
}
//  IO 输出操作
void  CMainRunState::OpenCloseIO(BYTE IO, BYTE State)
{
	TRACE(_T("CMainRunState:: OpenCloseIO IO =%d,State=%d "),IO,State);
	BOOL BHIGH= (BOOL) State;
	switch (IO)
	{
	case IO_VALVE1: //紫外灯
		m_pPart->m_pXMotor->WriteOutput(0,BHIGH);
		break;
	case IO_VALVE2: //日光灯
		m_pPart->m_pXMotor->WriteOutput(1,BHIGH);
		break;
	default:
		break;
	}
}

//清空程序步骤
void CMainRunState:: EmptyProgramStep()
{
	TRACE(_T("CMainRunState::EmptyProgramStep"));
	m_ProgramQueue.m_Id = -1;
	m_ProgramQueue.m_Type = 0;
	m_ProgramQueue.EmptyQueue();
}

// 接收程序步骤

BOOL CMainRunState:: RcvProgramStep(CReciveProgramStep * pMsg)
{
	BOOL bRet = TRUE ;
	 m_ProgramQueue.EmptyQueue(); // 清空之前步骤
	int  ncount = pMsg->m_ProgramList.GetCount();
    TRACE(_T("CMainRunState  RcvProgramStep:: ncount =%d"),ncount);
	if(ncount>0 && (pMsg->m_Type==KIT_TYPE_ONE || pMsg->m_Type==KIT_TYPE_TWO || pMsg->m_Type==KIT_TYPE_THREE ))
	{
		if (m_ProgramQueue.GetStepCount()==0) //  处理运行的程序发来 ，其他的不用发
		{
			m_ProgramQueue.m_Id = pMsg->m_id ; // 程序ID
		    m_ProgramQueue.m_Type = pMsg->m_Type ; // 试剂盒类型
			m_ProgramQueue.m_SuckMagneticWay =pMsg->m_SuckMagneticWay;
			m_ProgramQueue.m_NumberOfSegments =pMsg->m_NumberOfSegments;
			if(m_ProgramQueue.m_SuckMagneticWay<XUAN_TING || m_ProgramQueue.m_SuckMagneticWay>FENG_DUAN)
			{
				m_ProgramQueue.m_SuckMagneticWay =XUAN_TING;
			}
			if(m_ProgramQueue.m_SuckMagneticWay== FENG_DUAN ) //界面设置输入范围
			{
				if (m_ProgramQueue.m_NumberOfSegments>9 ||m_ProgramQueue.m_NumberOfSegments <2)
				{
					m_ProgramQueue.m_NumberOfSegments =5;
				}
			}
			ProgramStep info;
			for(int j=0;j<ncount;j++)
			{
				ProgramInfoList::Node* node =  pMsg->m_ProgramList.Item(j);
				ProgramStep*pinfo= node ->GetData();
				info=*pinfo;
		    	if (!info.IsEmpty())
				{
					m_ProgramQueue.AddStep(info);
				}
			}

		  m_ProgramQueue.PrintProgramStepInfo(); // 打印步骤信息
		}
	}
	else
	{
		bRet = FALSE;
	}
    return  bRet ;
}
//检测前盖板打开 报警
BOOL  CMainRunState::CheckFrontCover()
{
	BOOL bClose = FALSE;
	DWORD now=timeGetTime();
	DWORD SpanTM=now-m_LastCheckCoverTime;

#ifdef  _SIMULATOR_
	return TRUE;
#endif
   if(m_pPart->m_CheckFrontCoverMark==1)
   {
	   if (SpanTM>1000)
	   {
		   m_LastCheckCoverTime=timeGetTime();

		   int ret=   m_pPart->m_pZMotor->GetMotorLiquid(bClose); //上盖
		   if (ret>0)
		   {
			   if (!bClose && !m_bReportFrontCoverErr)
			   {
				   m_bReportFrontCoverErr =TRUE;
				   m_pErrSet->SetPartErr(PART_MACHINE,ERROR_FRONT_COVER_IS_OPEN,"",1); // TODO  打开上盖 是否仪器切换到停止状态
			   }
			   else  if (bClose && m_bReportFrontCoverErr)
			   {
				   m_bReportFrontCoverErr = FALSE;
			   }
		   }
	   }
   }
   else
   {
       bClose  =TRUE ;
   }

	return bClose;
}

 //检查卸载抽屉是 到位，不在重复报警
BOOL  CMainRunState::CheckDrawerInPlace()
{
#ifdef  _SIMULATOR_
  return TRUE;
#endif
	BOOL bInPlace = FALSE;

	DWORD now=timeGetTime();
	DWORD SpanTM=now-m_LastCheckDrawTime;

	if (SpanTM>1000)
	{
	     m_LastCheckDrawTime =timeGetTime();
		 int ret=  m_pPart->m_pZMotor->GetMotorRightLimit(bInPlace); //
		 if (ret>0)
		 {
			 if (!bInPlace && !m_bReportDrawerErr)
			 {
				 m_bReportDrawerErr = TRUE;
				 m_pErrSet->SetPartErr(PART_MACHINE,ERROR_DRAWER_NOT_IN_PLACE,"",1);
			 }
			 else  if (bInPlace && m_bReportDrawerErr)
			 {
				 m_bReportDrawerErr = FALSE;
			 }
		 }
	}

	return bInPlace; //
}

//----------复位是否已经报警标志 ----------
void  CMainRunState:: ResetReportErr()
{
	m_bReportFrontCoverErr =FALSE ;
    m_bReportDrawerErr =FALSE;
}