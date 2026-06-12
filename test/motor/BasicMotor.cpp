// BasicMotor.cpp: implementation of the CBasicMotor class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "BasicMotor.h"
#include "../protocol/AllMsgClass.h"
#include "../Machine/Machine.h"
#include "../trace.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBasicMotor::CBasicMotor(UCHAR CtrID,UCHAR axisID):CCanMotor(CtrID,axisID)
{
	m_LastError=0;
	m_LastAct=ACT_NONE;
	m_LastDebugMode=0;
	m_SpeedInit=100;

	m_iCurrent =A0_2;
	m_iLockCurrent =A0_15;
	m_subDiv = Sub_16;

	memset(m_iSpeed,0,sizeof(m_iSpeed)); // 速度初始化
	memset(m_iSpeedStart,0,sizeof(m_iSpeedStart));
	memset(m_Offset,0,sizeof(m_Offset)); //位置初始化
}

CBasicMotor::~CBasicMotor()
{
}

///////////////
//参数设置
int CBasicMotor::InitMove()
{
	m_LastDebugMode=0;
	m_LastAct=ACT_InitHome;
	m_LastError=0;
	FinishSemp.SetEvent();
	return 0;
}

//设置成默认最低参数，防止用到没定义的参数而运动不能返回
void CBasicMotor::InitMotorParam()
{
	MotorActParam param;
	for(int i=0;i<10;i++)
	{
		ACT_TYPE type=ACT_TYPE(i);
		param.type=RELATIVE_POS;
		param.speedStar=m_SpeedInit;
		param.speedEnd=m_SpeedInit;
		param.step=10;
		param.ACCDec=ZERO_ACC;
		SetMotorActParam(type,param);
	}
	for(int i=0;i<OUTPARAMCNT;i++)
	{
		ACT_TYPE type=ACT_TYPE(i+ACT_OutTYPE1);
		param.type=RELATIVE_POS;
		param.speedStar=m_SpeedInit;
		param.speedEnd=m_SpeedInit;
		param.step=10;
		param.ACCDec=ZERO_ACC;
		SetMotorActParam(type,param);
	}
}

int CBasicMotor::SetMotorDefaultParam()
{
	//写入细分,电流，半电流
	m_LastError=SetMotorDriver(m_MotorParam.current,m_MotorParam.subDiv,m_MotorParam.BModeA1_6);
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetMotorLockCurrent(m_MotorParam.ableLockCurrent==Enable,m_MotorParam.lockCurrent);
	MOTOR_IF_ERROR_QUIT
		//写入急停减速度，液面减速度
		m_LastError=SetMotorLiquidDec(m_MotorParam.decLiquid);
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetMotorUrgentStopDec(m_MotorParam.decUrgentStop);
	MOTOR_IF_ERROR_QUIT
		//写入IO管教有效电平，一般默认0，无需修改
		m_LastError=m_pctrl->WriteInputValid();
	MOTOR_IF_ERROR_QUIT
		//写入原点，限位光耦配置
		m_pctrl->m_LimitLevel.SetAxisLimitOrgLevel(m_AxisID, m_InputParam.levelOrg, m_InputParam.levelLeftLimit, m_InputParam.levelRightLimit,m_InputParam.levelLiq);
	m_LastError=m_pctrl->WriteLimitValid(m_AxisID,m_pctrl->m_LimitLevel);
	MOTOR_IF_ERROR_QUIT
		//原点，限位是否使能?
		EmptyMotorStop(FALSE);
	EnableMotorOrgStop(m_InputParam.ableOrg);
	MOTOR_IF_ERROR_QUIT
		EnableMotorLimitStop(m_InputParam.ableLimit);
	MOTOR_IF_ERROR_QUIT
		EnableMotorLiquidStop(m_InputParam.ableLiq);
	MOTOR_IF_ERROR_QUIT

		//写入原点动作
		m_LastError=SetOrgAct(m_InputParam.OrgAction, m_InputParam.OrgResetAbs,m_InputParam.LimitAction);
	MOTOR_IF_ERROR_QUIT

		//写入停止通知
		m_LastError=SetMotorStopNotify(TRUE,TRUE,TRUE,TRUE,TRUE);
	MOTOR_IF_ERROR_QUIT

		//写入有效运动参数
		for(int i=0;i<10;i++)
		{
			//if(m_ActParamAry[i].IsVaild())
			{
				m_LastError=CCanMotor::WriteActParam(i,m_ActParamAry[i].speedStar,m_ActParamAry[i].speedEnd,m_ActParamAry[i].step,m_ActParamAry[i].ACCDec,m_ActParamAry[i].type);
				MOTOR_IF_ERROR_QUIT
			}
			/*		wxString strtemp;
			strtemp.Printf(_T("CBasicMotor[%d][%d] m_ActParamAry[%d]--star=%d,end=%d,step=%d,acc=%d"),
			m_CtrID,m_AxisID,i,m_ActParamAry[i].speedStar,m_ActParamAry[i].speedEnd,m_ActParamAry[i].step,m_ActParamAry[i].ACCDec);

			TRACE(_T("CBasicMotor::SetMotorDefaultParam %s"),strtemp);*/
		}
		m_LastError=LockMotor(TRUE,FALSE);
		MOTOR_IF_ERROR_QUIT
			return m_LastError;
}
int	CBasicMotor::WriteActParam(ACT_TYPE type,MotorActParam param)
{
	return CCanMotor::WriteActParam((UCHAR)type,param.speedStar,param.speedEnd,param.step,param.ACCDec,param.type);
}
//设置运动参数
int	CBasicMotor::WriteActParam(UCHAR group,int distence,POS_TYPE type)
{
	return CCanMotor::WriteActParam(group,distence,type);
}
//因为是10组外的参数，与ACT_InitAfterHome即时切换
int	CBasicMotor::WriteOuterActParam(ACT_TYPE type,ACT_TYPE outerdef)
{
	FASSERT(type>ACT_NONE);
	FASSERT(type<ACT_OutTNONE);
	return WriteActParam(outerdef,m_ActOutParamAry[type-ACT_OutTYPE1]);
}

//设置某组运动参数
void CBasicMotor::SetMotorActParam(ACT_TYPE type,MotorActParam param)
{
	if(type>ACT_NONE)
	{
		SaveOutParam(type,param);
	}
	else
	{
		FASSERT(type<ACT_NONE);
		UCHAR index=(UCHAR)type;
		m_ActParamAry[index]=param;
	}
}

void  CBasicMotor:: SetMotorParam(ACT_TYPE type,UINT speedStar,UINT speedEnd,int step,UINT ACCDec,POS_TYPE pos_type)
{
	MotorActParam param;
	param.type=pos_type;
	param.speedStar=speedStar; //
	param.speedEnd =speedEnd;
	param.step = step ; //
	param.ACCDec=ACCDec;
	SetMotorActParam(type,param);
}

//设置某组运动参数
int	CBasicMotor::SetMotorActDist(ACT_TYPE type,int distence)
{
	if(type>ACT_NONE)
	{
		FASSERT(type>ACT_NONE);
		FASSERT(type<ACT_OutTNONE);
		m_ActOutParamAry[type-ACT_OutTYPE1].step=distence;
		return 0;
	}
	else
	{
		FASSERT(type<ACT_NONE);
		UCHAR index=(UCHAR)type;
		m_ActParamAry[index].step=distence;
		return WriteActParam(type,distence,RELATIVE_POS);
	}
}

//保存10组参数外的运动参数，运动时提前发送
void	 CBasicMotor::SaveOutParam(ACT_TYPE type,MotorActParam param)
{
	FASSERT(type>ACT_NONE);
	FASSERT(type<ACT_OutTNONE);
	m_ActOutParamAry[type-ACT_OutTYPE1]=param;
}

///////////////
//基本运动
int CBasicMotor::MoveLimit(UCHAR group,DIR_TYPE dir,UINT WaitTM)
{
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
	m_LastError=EnableMotorLimitStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	m_LastError=MoveRelative(group,dir);
	if((m_LastError<0)||(0==WaitTM))
		return m_LastError;

	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EnableMotorLimitStop(FALSE);
	MOTOR_IF_ERROR_QUIT

		//检查必须是限位触发
	m_LastError=CheckActFinishMustRightLimit(notify);

	return m_LastError;
}
int CBasicMotor::MoveHome(UCHAR group,DIR_TYPE dir,UINT WaitTM)
{
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
	m_LastError=EnableMotorOrgStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	m_LastError=MoveRelative(group,dir);
	if((m_LastError<0)||(0==WaitTM))
	  return m_LastError;

	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EnableMotorOrgStop(FALSE);
	MOTOR_IF_ERROR_QUIT

	//检查必须是原点触发
	m_LastError=CheckActFinishMustOrg(notify);
    MOTOR_IF_ERROR_QUIT
	m_LastError=SetAbsPos(0);
	MOTOR_IF_ERROR_QUIT
	return m_LastError;
}
int CBasicMotor::InitHome()
{
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
		BOOL BOrg;
	m_LastError=GetMotorOrg(BOrg);
	MOTOR_IF_ERROR_QUIT
		//如果开始时候不是复位位置则先复位
	if(!BOrg)
	{
		m_LastError=MoveHome((UCHAR)ACT_InitHome,m_DirUp);
		if(m_LastError<0 )
		{
			ResetErr();
			m_LastError=MoveHome((UCHAR)ACT_InitHome,m_DirUp);
		}
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}
		//离开home
		m_LastError=LeaveHome((UCHAR)ACT_InitHome,m_DirDown);
		MOTOR_IF_ERROR_QUIT
			//运行到离开home一定距离
		m_LastError=MoveRelativeAndWait((UCHAR)ACT_InitLeaveHome,m_DirDown,WAITTIME);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(200);
		//运行到HOME
		m_LastError=MoveHome((UCHAR)ACT_InitHome,m_DirUp);
		if(m_LastError<0 )
		{
			ResetErr();
			m_LastError=MoveHome((UCHAR)ACT_InitHome,m_DirUp);
		}
		MOTOR_IF_ERROR_QUIT
		m_LastError=SetAbsPos(0);
		MOTOR_IF_ERROR_QUIT
		return m_LastError;
}
int CBasicMotor::BackHome(UCHAR group1,UCHAR group2,int step,int distance,UINT WaitTM)
{
	    MOTOR_IF_ERROR_QUIT
		if(step==0)
		{
			int currentpos;
			m_LastError=GetAbsPos(currentpos);
			MOTOR_IF_ERROR_QUIT
			step=abs(currentpos);
		}
		TRACE(_T("CBasicMotor::BackHome MotorName=%s ,step=%d"),m_strName,step);

		m_LastError=EmptyMotorStop(FALSE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EnableMotorOrgStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		if(step>distance)
		{
			step=step-distance;
			m_LastError=WriteActParam(group1,step,RELATIVE_POS);
			MOTOR_IF_ERROR_QUIT
			wxMilliSleep(50);
			m_LastError=MoveRelative((UCHAR)group1,(UCHAR)group2,m_DirUp);
			MOTOR_IF_ERROR_QUIT
		}
		else
		{
			m_LastError=MoveRelative((UCHAR)group2,m_DirUp);
			MOTOR_IF_ERROR_QUIT
		}
		if(WaitTM==0)
			return m_LastError;
		NotifyInfo notify;
		m_LastError=WaitMotorFinish(WaitTM,notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EnableMotorOrgStop(FALSE);
		MOTOR_IF_ERROR_QUIT

		//检查必须是原点触发
		m_LastError=CheckActFinishMustOrg(notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=SetAbsPos(0);
		MOTOR_IF_ERROR_QUIT

		return m_LastError;
}
int CBasicMotor::LeaveHome(UCHAR group,DIR_TYPE dir,UINT WaitTM)
{
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
	Level_TYPE level=(m_InputParam.levelOrg==LevelLow)?LevelHigh:LevelLow;

	m_LastError=SetMotorOrgLevel(level);
	MOTOR_IF_ERROR_QUIT

	m_LastError=EnableMotorOrgStop(TRUE);
	MOTOR_IF_ERROR_QUIT

	m_LastError=MoveRelative(group,dir);
	if((m_LastError<0)||(0==WaitTM))
		return m_LastError;

	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT

	m_LastError=EnableMotorOrgStop(FALSE);
	MOTOR_IF_ERROR_QUIT
	m_LastError=SetMotorOrgLevel(m_InputParam.levelOrg);
	MOTOR_IF_ERROR_QUIT

		//检查必须是原点触发
		m_LastError=CheckActFinishMustOrg(notify);
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetAbsPos(0);
	MOTOR_IF_ERROR_QUIT
		return m_LastError;
}

int  CBasicMotor::LeaveLeft(UCHAR group,DIR_TYPE dir,UINT WaitTM)
{
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
		Level_TYPE level=(m_InputParam.levelLeftLimit==LevelLow)?LevelHigh:LevelLow;

	m_LastError=SetMotorLeftLimitLevel(level);
	MOTOR_IF_ERROR_QUIT

		m_LastError=EnableMotorLimitStop(TRUE);
	MOTOR_IF_ERROR_QUIT

		m_LastError=MoveRelative(group,dir);
	if((m_LastError<0)||(0==WaitTM))
		return m_LastError;

	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT

		m_LastError=EnableMotorLimitStop(FALSE);
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetMotorLeftLimitLevel(m_InputParam.levelLeftLimit);
	MOTOR_IF_ERROR_QUIT

		//检查必须是做限位触发
		m_LastError=CheckActFinishMustLeftLimit(notify);
	MOTOR_IF_ERROR_QUIT
		//m_LastError=SetAbsPos(0);
		//MOTOR_IF_ERROR_QUIT
		return m_LastError;
}

int  CBasicMotor::LeaveRight(UCHAR group,DIR_TYPE dir,UINT WaitTM)
{
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
		Level_TYPE level=(m_InputParam.levelRightLimit==LevelLow)?LevelHigh:LevelLow;

	m_LastError=SetMotorRightLimitLevel(level);
	MOTOR_IF_ERROR_QUIT

		m_LastError=EnableMotorLimitStop(TRUE);
	MOTOR_IF_ERROR_QUIT

		m_LastError=MoveRelative(group,dir);
	if((m_LastError<0)||(0==WaitTM))
		return m_LastError;

	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT

		m_LastError=EnableMotorLimitStop(FALSE);
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetMotorRightLimitLevel(m_InputParam.levelLeftLimit);
	MOTOR_IF_ERROR_QUIT

		//检查必须是做限位触发
		m_LastError=CheckActFinishMustRightLimit(notify);
	MOTOR_IF_ERROR_QUIT
		//m_LastError=SetAbsPos(0);
		//MOTOR_IF_ERROR_QUIT
		return m_LastError;
}

int CBasicMotor::MoveToPos(int pos,UCHAR group,UINT WaitTM)
{
	int curpos;
	m_LastError=GetAbsPos(curpos);
	MOTOR_IF_ERROR_QUIT
		if(m_DirDown==DIR_CCW)
			curpos=-curpos;

	int step=pos-curpos;
	if(step==0)
		return m_LastError;
	DIR_TYPE bDir=m_DirDown;
	if(step<0)
	{
		step=-step;
		bDir=m_DirUp;
	}
	m_LastError=JogStep(step,bDir,group,WaitTM);

	return m_LastError;
}

//curPos 当前位置， pos 目标位置，正值
int CBasicMotor::MoveToPosRes(int curPos ,int pos,UCHAR group,UINT WaitTM)			//运行到绝对位置
{
	//int curpos;
	//m_LastError=GetAbsPos(curpos);
	//MOTOR_IF_ERROR_QUIT
	MOTOR_IF_ERROR_QUIT
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	if(m_DirDown==DIR_CCW)
	{
		curPos=-curPos; //  
	}
	int step=pos-curPos;
	if(step==0)
		return m_LastError;
	DIR_TYPE bDir=m_DirDown;
	if(step<0)
	{
		step=-step;
		bDir=m_DirUp;
	}
	m_LastError=JogStep(step,bDir,group,WaitTM);
	return m_LastError;
}

int CBasicMotor::JogStep(int step,DIR_TYPE DIR,UCHAR group,UINT WaitTM)
{
	MOTOR_IF_ERROR_QUIT
	m_LastError=WriteActParam(group,step,RELATIVE_POS);
	MOTOR_IF_ERROR_QUIT
	m_LastError=MoveRelative((UCHAR)group,DIR);
	MOTOR_IF_ERROR_QUIT
	if(WaitTM==0)
		return m_LastError;
	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT

	return CheckActFinishMustStepFinish(notify);
}
int CBasicMotor::MoveLiquid(UCHAR group,DIR_TYPE dir,int &POS,UINT WaitTM)
{
	FASSERT(WaitTM>0);	//必须等待动作完成
	POS=0;
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetAbsPos(0);//？？
	MOTOR_IF_ERROR_QUIT
		m_LastError=EnableMotorLiquidStop(TRUE);
	MOTOR_IF_ERROR_QUIT
		m_LastError=MoveRelative(group,dir);
	if((m_LastError<0)||(0==WaitTM))
		return m_LastError;

	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT

		m_LastError=EnableMotorLiquidStop(FALSE);
	MOTOR_IF_ERROR_QUIT
		int len;
	m_LastError=GetAbsPos(len);
	MOTOR_IF_ERROR_QUIT

		POS=abs(len);
	TRACE(_T("CBasicMotor::MoveLiquid length=%d"),POS);

	if(notify.BOrgNotify)
		return ERROR_MOVE_HOMEERR;
	if(notify.BLeftLimitNotify)
	{
		POS=-POS;
		return ERROR_MOVE_LEFTLIMITERR;
	}
	if(notify.BRightLimitNotify)
	{
		POS=-POS;
		return ERROR_MOVE_RIGHTLIMITERR;
	}
	if(notify.BEncodeLimitNotify)
		return NotifyFatherCmdFinish(ERROR_AFTER_ENCODEERR);
	if(notify.BEncodeZNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_HOMEERR);
	if(notify.BFivePosNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_BARCODE_END);
	if(!notify.BLiqidNotify)	//如果没有判断到液位，只是长度返回负值
	{
		POS=-POS;
	}
	return 1;
}

//运行相对距离
int CBasicMotor::MoveRelativeAndWait(UCHAR group,DIR_TYPE DIR,UINT WaitTM)
{
	FASSERT(WaitTM>0);	//必须等待动作完成
	NotifyInfo notify;
	m_LastError=MoveRelative(group,DIR);
	MOTOR_IF_ERROR_QUIT
		m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
		return CheckActFinishMustStepFinish(notify);
}

//运行编码器距离
int CBasicMotor::MoveEncodeAndWait(UCHAR group,UINT WaitTM)
{
	FASSERT(WaitTM>0);	//必须等待动作完成
	NotifyInfo notify;
	m_LastError=MoveEncode((UCHAR)group);
	MOTOR_IF_ERROR_QUIT
		m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
		return CheckActFinishMustStepFinish(notify);
}

int	CBasicMotor::WaitActFinish(UINT WaitTM)
{
	FASSERT(WaitTM>0);
	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
	return 1;
}

int  CBasicMotor:: WaitStepFinish(UINT WaitTM)
{
	FASSERT(WaitTM>0);
	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
	m_LastError=CheckActFinishMustStepFinish(notify);
	MOTOR_IF_ERROR_QUIT
	return m_LastError;
}

int  CBasicMotor:: WaitBackHome(UINT WaitTM)
{
	FASSERT(WaitTM>0);
	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
		//检查必须是原点触发
		m_LastError=CheckActFinishMustOrg(notify);
	MOTOR_IF_ERROR_QUIT
		m_LastError=SetAbsPos(0);
	MOTOR_IF_ERROR_QUIT
		return m_LastError;
}
int CBasicMotor::WaitRightLimit(UINT WaitTM)
{
	FASSERT(WaitTM>0);
	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
		//检查必须是原点触发
		m_LastError=CheckActFinishMustRightLimit(notify);
	MOTOR_IF_ERROR_QUIT
		return m_LastError;
}
int CBasicMotor::WaitLeftLimit(UINT WaitTM)
{
	FASSERT(WaitTM>0);
	NotifyInfo notify;
	m_LastError=WaitMotorFinish(WaitTM,notify);
	MOTOR_IF_ERROR_QUIT
		//检查必须是原点触发
		m_LastError=CheckActFinishMustLeftLimit(notify);
	MOTOR_IF_ERROR_QUIT
		return m_LastError;
}

int CBasicMotor::NotifyFatherCmdFinish(int param)
{
	if(param<0)
	{
		m_LastError=param;
		TRACE(_T("%s NotifyFatherCmdFinish =%d\r\n"),m_strName,param);
	}
	return param;
}
//检查电机停止的必须是步进走完
int CBasicMotor::CheckActFinishMustStepFinish(NotifyInfo notify)
{
#ifdef _SIMULATOR_
	return 0;
#endif
	if(notify.BOrgNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_HOMEERR);
	if(notify.BLeftLimitNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_LEFTLIMITERR);
	if(notify.BRightLimitNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_RIGHTLIMITERR);
	if(notify.BLiqidNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_LIQUIDERR);
	if(notify.BEncodeLimitNotify)
		return NotifyFatherCmdFinish(ERROR_AFTER_ENCODEERR);
	if(notify.BEncodeZNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_HOMEERR);
	if(notify.BFivePosNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_BARCODE_END);

	return 0;
}

//检查电机停止的必须是步进走完 或者 左限位
int CBasicMotor::CheckActFinishMustStepFinishOrLeftLimit(NotifyInfo notify)
{
#ifdef _SIMULATOR_
	return 0;
#endif
	if(notify.BOrgNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_HOMEERR);
	//if(notify.BLeftLimitNotify)
	//	return NotifyFatherCmdFinish(ERROR_MOVE_LEFTLIMITERR);
	if(notify.BRightLimitNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_RIGHTLIMITERR);
	if(notify.BLiqidNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_LIQUIDERR);
	if(notify.BEncodeLimitNotify)
		return NotifyFatherCmdFinish(ERROR_AFTER_ENCODEERR);
	if(notify.BEncodeZNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_HOMEERR);
	if(notify.BFivePosNotify)
		return NotifyFatherCmdFinish(ERROR_MOVE_BARCODE_END);

	return 0;
}

//检查电机停止的必须是右限位
int CBasicMotor::CheckActFinishMustRightLimit(NotifyInfo notify)
{
#ifdef _SIMULATOR_
	return 0;
#endif
	if(!notify.BRightLimitNotify)
		return NotifyFatherCmdFinish(ERROR_AFTER_NOT_RIGHTLIMITERR);
	return 0;
}
//检查电机停止的必须是右限位
int CBasicMotor::CheckActFinishMustLeftLimit(NotifyInfo notify)
{
#ifdef _SIMULATOR_
	return 0;
#endif
	if(!notify.BLeftLimitNotify)
		return NotifyFatherCmdFinish(ERROR_AFTER_NOT_LEFTLIMITERR);
	return 0;
}
//检查电机停止的必须是右限位
int CBasicMotor::CheckActFinishMustOrg(NotifyInfo notify)
{
#ifdef _SIMULATOR_
	return 0;
#endif
	if(!(notify.BOrgNotify||notify.BEncodeZNotify))
		return NotifyFatherCmdFinish(ERROR_AFTER_NOT_HOMEERR);
	return 0;
}
//检查电机停止的必须是液位
int CBasicMotor::CheckActFinishMustLiquid(NotifyInfo notify)
{
#ifdef _SIMULATOR_
	return 0;
#endif
	if(!notify.BLiqidNotify)
		return NotifyFatherCmdFinish(ERROR_AFTER_NOT_LIQUIDERR);
	return 0;
}

//调试马达电机
int	CBasicMotor::DebugMotor(BYTE act,int param,int relatepos)
{
	int rtn=0;
	m_LastDebugMode|=0x80;
	switch(act)
	{
	case DEBUG_ACT_RESET:
		m_LastDebugMode&=0xBF;
		rtn=InitMove();
		m_LastDebugMode|=0x80;
		if(rtn<0)	return rtn;
		return 0;
	case DEBUG_ACT_MODE:
		return 0;	//表示无处理
	case DEBUG_ACT_RUN:
		{
			m_LastDebugMode|=0x40;
			//除了距离，其他参数与复位相同
			m_ActOutParamAry[OUT_INDEX(ACT_Debug)]=m_ActParamAry[ACT_InitHome];
			m_ActOutParamAry[OUT_INDEX(ACT_Debug)].step=abs(param);
			//因为是10组外的参数，与ACT_InitAfterHome即时切换
			m_LastError=WriteOuterActParam(ACT_Debug);
			MOTOR_IF_ERROR_QUIT
				m_LastError=MoveRelative((UCHAR)ACT_InitAfterHome,(param>0)?m_DirDown:m_DirUp);
			MOTOR_IF_ERROR_QUIT
				return 0;
		}
		break;
	case DEBUG_ACT_LOCKUNLOCK:
		m_LastError=LockMotor((param>0));
		MOTOR_IF_ERROR_QUIT
			break;
	default:
		break;
	}
	return 0;
}

wxString CBasicMotor::SpanIncluding(wxString strbar,wxString str)
{
	const char*pstrbar=strbar.mb_str();
	int nlen=strlen(pstrbar);
	char *pchoice=new char[nlen+10];

	const char*pstrMask=str.mb_str();
	int nlenMask=strlen(pstrMask);
	int icount=0;
	for(int i=0;i<nlen;i++)
	{
		if(memchr(pstrMask,pstrbar[i],nlenMask)!=NULL)
		{
			pchoice[icount++]=pstrbar[i];
		}
	}
	pchoice[icount++]='\0'	;
	wxString strrtn(pchoice, wxConvUTF8);
	delete []pchoice;
	return strrtn;
}

//只收到IO通知
int	CBasicMotor::CallBackRcvNotify(NotifyInfo notify)
{
	TRACE(_T("CBasicMotor::CallBackRcvNotify MotorName=%s"),m_strName);
	return 1;
}
// 发送位置
void CBasicMotor::SendMotorPos()
{
	int Pos=0;
	m_LastError=GetAbsPos(Pos);
	CSndMotorPos  *pMsg = new CSndMotorPos ;
	pMsg->m_Pos =Pos;
	g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg); //
}