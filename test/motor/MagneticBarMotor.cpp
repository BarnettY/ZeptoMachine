#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MagneticBarMotor.h"
#include "../trace.h"

//--------------------------- 磁棒电机------------
CMagneticBarMotor::CMagneticBarMotor(UCHAR CtrID,UCHAR axisID):CBasicMotor(CtrID,axisID)
{
	m_DirDown=DIR_CW;
	m_DirUp=(m_DirDown==DIR_CCW)?DIR_CW:DIR_CCW;
	m_MotorPos =POS_MagneticBar_UNKNOW;
}
CMagneticBarMotor::~CMagneticBarMotor()
{
}
// 初始化回零
int CMagneticBarMotor:: InitHome()
{
	TRACE(_T("CMagneticBarMotor::InitHome MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	m_LastError= CBasicMotor::InitHome();
	MOTOR_IF_ERROR_QUIT
    m_MotorPos=POS_MagneticBar_HOME;
	m_LastAct=ACT_NONE;
	return m_LastError;
}
int CMagneticBarMotor::GoToHome(UINT WaitTM)
{
	TRACE(_T("CMagneticBarMotor::GoToHome MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	m_LastError=Stop();//出现过GoToHome 失败情形
	InitMove();
	m_LastError= BackHome(ACT_Move,ACT_InitHome,0,500,WaitTM);
	if(m_LastError<0)
	{
		TRACE(_T("CMagneticBarMotor::GoToHome Again "));
		InitMove();
		wxMilliSleep(200);
		m_LastError=BackHome(ACT_Move,ACT_InitHome,0,500,WaitTM);
	}
	MOTOR_IF_ERROR_QUIT
	m_MotorPos = POS_MagneticBar_HOME;
	m_LastAct=ACT_NONE;
	TRACE(_T("CMagneticBarMotor::GoToHome End"));
	return	m_LastError;
}
//走绝对位置 ,回到空闲位置，
int  CMagneticBarMotor::MoveIdlePos(UINT WaitTM)
{
	int Pos = m_Offset[3];
	int NowPos =0;
	m_LastError=GetAbsPos(NowPos);
	MOTOR_IF_ERROR_QUIT
	TRACE(_T("CMagneticBarMotor::MoveIdlePos NowPos=%d,TargetPos=%d "),NowPos,Pos);
	m_LastError=MoveToPosRes(NowPos,Pos,ACT_TYPE4,WaitTM);
	MOTOR_IF_ERROR_QUIT
	m_MotorPos =POS_MagneticBar_Idle;
	return m_LastError;
}
// 下降到底 改成移动到绝对位置 ，MARK
int CMagneticBarMotor:: MoveDown(int &MoveStep, BYTE HolePos, UINT WaitTM ,BYTE Mark)
{
	TRACE(_T("CMagneticBarMotor::MoveDown MotorName=%s,m_MotorPos=%d,Mark=%d,HolePos=%d"),m_strName,m_MotorPos,Mark,HolePos);
	MOTOR_IF_ERROR_QUIT

	if (Mark==1 || Mark==0)
	{
		int Pos = m_Offset[1];  // 孔1 和孔2 高度不一致
		int NowPos =0;
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=GetAbsPos(NowPos);
	    MOTOR_IF_ERROR_QUIT
		MoveStep =Pos-abs(NowPos);
		if (MoveStep==0)
		{
            TRACE(_T("CMagneticBarMotor::MoveDown step=0"));
			return  m_LastError;
		}
		m_LastError=WriteActParam(ACT_TYPE2,MoveStep,RELATIVE_POS);
		MOTOR_IF_ERROR_QUIT
	    wxMilliSleep(50);
	}
    if (Mark==2 || Mark==0) //
    {
		m_LastError=MoveRelative(ACT_TYPE2,m_DirDown);
		MOTOR_IF_ERROR_QUIT
		if(0==WaitTM)
			return m_LastError;
		NotifyInfo notify;
		m_LastError=WaitMotorFinish(WaitTM,notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=CheckActFinishMustStepFinish(notify);
		MOTOR_IF_ERROR_QUIT
	    m_MotorPos =POS_MagneticBar_DOWN;
    }
	return m_LastError;
}

//下降脉冲--TODO 增加判断 不能低于最低位置。 孔1中下降高度
int  CMagneticBarMotor:: MoveDownStep(int & MoveStep,float Height,UINT WaitTM,BYTE Mark)
{
	TRACE(_T("CMagneticBarMotor::MoveDownStep MotorName=%s,m_MotorPos=%d,Height=%fmm,Mark=%d"),m_strName,m_MotorPos,Height,Mark);
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
	if (Mark==1 || Mark==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		int Step= Height*m_PerMMToPluse;
		int NowPos =0;
		m_LastError = GetAbsPos(NowPos); //当前位置
	    MOTOR_IF_ERROR_QUIT
		int Temp = m_Offset[1]-abs(NowPos)-(m_SuckmagneticLeftStep*(m_PerMMToPluse/ZMOTOR_MM_TO_PULSE));  //m_SuckmagneticLeftStep 是Z轴50脉冲，磁棒要转换
		if (Step>Temp)
		{
    		Step =Temp;
    		TRACE(_T("CMagneticBarMotor::MoveDownStep Step=%d"),Step);
		}
        MoveStep =Step;
		m_LastError=WriteActParam(ACT_TYPE3,Step,RELATIVE_POS);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}
	if (Mark==2 || Mark==0)
	{
		m_LastError=MoveRelative(ACT_TYPE3,m_DirDown);
		MOTOR_IF_ERROR_QUIT
		if(0==WaitTM)
			return m_LastError;
		NotifyInfo notify;
		m_LastError=WaitMotorFinish(WaitTM,notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=CheckActFinishMustStepFinish(notify);
		MOTOR_IF_ERROR_QUIT
		m_MotorPos =POS_MagneticBar_Step;
	}
	return m_LastError;
}

//不同的类型液面高度 位置不同。
int  CMagneticBarMotor::  MoveToLiquidTop(int &MoveStep,BYTE Type, BYTE HolePos,int Volume, BOOL bFast, UINT WaitTM ,BYTE Mark)
{
	TRACE(_T("CMagneticBarMotor::MoveToLiquidTop MotorName=%s,m_MotorPos=%d,Mark=%d"),m_strName,m_MotorPos,Mark);
	MOTOR_IF_ERROR_QUIT
	int Pos = 0,NowPos =0;
	m_LastError=GetAbsPos(NowPos);
    MOTOR_IF_ERROR_QUIT
	if (HolePos==1)
	{
		if (Type == KIT_TYPE_ONE || Type==KIT_TYPE_THREE)
		{
			if(Volume>10000)
			{
				Volume=10000;
			}

			if(Volume>1000)
			{
				Pos= m_Offset[1]-(TYPE1_MM_1ML_1*m_PerMMToPluse)-(Volume*(TYPE1_MM_1ML_2*m_PerMMToPluse)/1000)+(TYPE1_MM_1ML_2*m_PerMMToPluse);	//  高度从底部往上
			}
			else
			{
				Pos= m_Offset[1]-(Volume*(TYPE1_MM_1ML_1*m_PerMMToPluse)/1000);
			}
		}
		else
		{
			if(Volume>5000)
			{
				Volume=5000;
			}

			if(Volume>1000)
			{
				Pos= m_Offset[1]-(TYPE2_MM_1ML_1*m_PerMMToPluse)-(Volume*(TYPE2_MM_1ML_2*m_PerMMToPluse)/1000)+(TYPE2_MM_1ML_2*m_PerMMToPluse);
			}
			else
			{
				Pos= m_Offset[1]-(Volume*(TYPE2_MM_1ML_1*m_PerMMToPluse)/1000);
			}
		}
	}
    else
	{
		if(Volume>2000)
		{
			Volume=2000;
		}
		Pos= m_Offset[1]-(TYPE_MM_1ML_HOLE*m_PerMMToPluse)*Volume/1000; // 体积确定高度;
	}
	if (Pos<0) Pos =0;
	TRACE(_T("CMagneticBarMotor::MoveToLiquidTop NowPos=%d,TargetPos=%d "),NowPos,Pos);

	if(m_DirDown==DIR_CCW) //
	{
		NowPos=-NowPos;
	}
	int step=Pos-NowPos;
	MoveStep =step;
	if(step==0)
		return m_LastError;
	DIR_TYPE bDir=m_DirDown;
	if(step<0)
	{
		step=-step;
		bDir=m_DirUp;
	}
	if (bFast)
	{
		if (Mark ==1 ||Mark ==0)
		{
			m_LastError=EmptyMotorStop(TRUE);
			MOTOR_IF_ERROR_QUIT
			m_LastError=WriteActParam(ACT_TYPE5,step,RELATIVE_POS);
			MOTOR_IF_ERROR_QUIT
			wxMilliSleep(50);
		}
		if (Mark ==2 ||Mark ==0)
		{
			m_LastError=MoveRelative(ACT_TYPE5,bDir);
			MOTOR_IF_ERROR_QUIT
		}
	}
	else
	{
		if (Mark ==1 ||Mark ==0)
		{
			m_LastError=EmptyMotorStop(TRUE);
			MOTOR_IF_ERROR_QUIT
			m_LastError=WriteActParam(ACT_TYPE6,step,RELATIVE_POS);
			MOTOR_IF_ERROR_QUIT
			wxMilliSleep(50);
		}
		if (Mark ==2 ||Mark ==0)
		{
			m_LastError=MoveRelative(ACT_TYPE6,bDir);
			MOTOR_IF_ERROR_QUIT
		}
	}
	if (Mark ==2 ||Mark ==0)
	{
		if(0==WaitTM)
			return m_LastError;
		NotifyInfo notify;
		m_LastError=WaitMotorFinish(WaitTM,notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=CheckActFinishMustStepFinish(notify);
		MOTOR_IF_ERROR_QUIT
		m_MotorPos =POS_MagneticBar_LiquidTop;
	}
	return m_LastError;
}

// 除孔1 其他孔中吸磁---缓慢下降吸磁
int CMagneticBarMotor:: MoveDownSlow(int &MoveStep,BOOL bSegment, float height,  UINT WaitTM ,BYTE Mark,BYTE suckLevel)
{
	TRACE(_T("CMagneticBarMotor::MoveDownSlow MotorName=%s,m_MotorPos=%d,Mark=%d,bSegment=%d,height=%f"),m_strName,m_MotorPos,Mark,bSegment,height);
	MOTOR_IF_ERROR_QUIT
	DWORD StartTime=timeGetTime();
	if (Mark ==1 ||Mark ==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		int Step = m_Offset[1]-(m_SuckmagneticLeftStep*(m_PerMMToPluse/ZMOTOR_MM_TO_PULSE));
		int NowPos =0;
		m_LastError=GetAbsPos(NowPos);
        MOTOR_IF_ERROR_QUIT
		if (bSegment && height>0)
		{
			Step = height*m_PerMMToPluse;
		}
		else
		{
			Step-=abs(NowPos); //需要走的脉冲
			if(Step<0)
				Step =0;
		}
		MoveStep =Step;
		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		actParam.speedStar=m_iSpeedStart[7]*suckLevel/10; // 快速振荡 ,根据
		if(actParam.speedStar<10)
		 actParam.speedStar =10;
		actParam.speedEnd=m_iSpeed[7]*suckLevel/10;
		actParam.step=Step;
		actParam.ACCDec=300;
        TRACE(_T("CMagneticBarMotor::MoveDownSlow Step=%d,speedEnd=%d,speedStar=%d"),Step,actParam.speedEnd, actParam.speedStar);
		m_LastError=WriteActParam(ACT_TYPE7,actParam);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}
	if (Mark ==2 ||Mark ==0)
	{
		m_LastError=MoveRelative(ACT_TYPE7,m_DirDown);
		MOTOR_IF_ERROR_QUIT
		if(0==WaitTM)
			return m_LastError;
		NotifyInfo notify;
		m_LastError=WaitMotorFinish(WaitTM,notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=CheckActFinishMustStepFinish(notify);
		MOTOR_IF_ERROR_QUIT
		m_MotorPos =POS_MagneticBar_DOWN;
	}
    DWORD EndTime=timeGetTime();
	DWORD Span =EndTime -StartTime;
	if (Mark!=1)
	{
	 TRACE(_T("CMagneticBarMotor MoveDownSlow::Span=%d"),Span);
	}
	return m_LastError;
}
//缓慢上升吸磁
int CMagneticBarMotor:: MoveUpSlow(int &MoveStep,float height, UINT WaitTM ,BYTE Mark, int SpeedLevel)
{
	TRACE(_T("CMagneticBarMotor::MoveUpSlow MotorName=%s,m_MotorPos=%d,Mark=%d,height=%f,SpeedLevel=%d"),m_strName,m_MotorPos,Mark,height,SpeedLevel);
	//如果上次是出错的，保护退出
	MOTOR_IF_ERROR_QUIT
	if (Mark ==1 ||Mark ==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		int Step= height*m_PerMMToPluse;
		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		actParam.speedStar=m_iSpeedStart[7]*SpeedLevel/10; // 快速振荡 ,根据
		actParam.speedEnd=m_iSpeed[7]*SpeedLevel/10;
		actParam.step=Step;
		actParam.ACCDec=30;
		MoveStep =Step;
		m_LastError=WriteActParam(ACT_TYPE7,actParam);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}
	if (Mark ==2 ||Mark ==0)
	{
		m_LastError=MoveRelative(ACT_TYPE7,m_DirUp);
		MOTOR_IF_ERROR_QUIT
		if(0==WaitTM)
			return m_LastError;
		NotifyInfo notify;
		m_LastError=WaitMotorFinish(WaitTM,notify);
		MOTOR_IF_ERROR_QUIT
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=CheckActFinishMustStepFinish(notify);
		MOTOR_IF_ERROR_QUIT
		m_MotorPos =POS_MagneticBar_Up;
	}
	return m_LastError;
}
float  CMagneticBarMotor:: GetHeightByVolumeInHolePos(BYTE Type,BYTE Pos, int Volume)
{
	float Height = 0;
	if (Pos==1)
	{
		if (Type==KIT_TYPE_ONE || Type==KIT_TYPE_THREE )
		{
			if(Volume>10000)
			{
				Volume=10000;
			}

			if(Volume>1000) // 1ml 底部不规则
			{
				Height=(float)(TYPE1_MM_1ML_1+(float)(Volume*TYPE1_MM_1ML_2/1000-TYPE1_MM_1ML_2));
			}
			else
			{
				Height=(float) (Volume*TYPE1_MM_1ML_1/1000);
			}
		}
		else
		{
			if(Volume>5000)
			{
				Volume=5000;
			}
			if(Volume>1000) // 1ml 底部不规则
			{
				Height= (float)TYPE2_MM_1ML_1+(float)(Volume*TYPE2_MM_1ML_2/1000-TYPE2_MM_1ML_2);
			}
			else
			{
				Height= (float)(Volume*TYPE2_MM_1ML_1/1000);
			}
		}
	}
	else
	{
		if(Volume>2000)
		{
			Volume=2000;
		}

     	Height =(float)(Volume*TYPE_MM_1ML_HOLE/1000);
	}
	return Height;
}

int CMagneticBarMotor::DebugMotor(BYTE act,int param,int relatepos)
{
	TRACE(_T("CMagneticBarMotor::DebugMotor act=%d, param=%d,"),act,param);
	int rtn=0,Step=0;
	m_LastDebugMode|=0x80;
	switch(act)
	{
	case DEBUG_ACT_RESET:
		m_LastDebugMode&=0xBF;
		rtn=InitMove();
		rtn=InitHome();
	     SendMotorPos();
		m_LastDebugMode|=0x80;
		if(rtn<0)	return rtn;
		return 1;
	case DEBUG_ACT_MODE:
		m_LastDebugMode&=0xBF;
		{
			BYTE MODE=0;
			if(param==0xff)
				MODE=m_LastDebugMode&0x0f;
			else
				MODE=param;
			switch(MODE)
			{
			case 1:
				rtn = MoveDown(Step);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 2:
				rtn = MoveDownStep(Step,2);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 3:
				rtn = MoveIdlePos();
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 4:
				rtn = MoveToLiquidTop(Step,KIT_TYPE_ONE);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 5:
				rtn = MoveToLiquidTop(Step,KIT_TYPE_TWO);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 6:
				rtn = MoveDownSlow(Step,FALSE,0,WAITTIME*50,0,1); // 下降吸磁
				if(rtn<0) return rtn;
				MODE++;
				break;

			default:
				rtn=InitMove();
				if(rtn<0) return rtn;
				MODE=1;
				break;
			}
			m_LastDebugMode=MODE|0x80;
		   SendMotorPos();
		}
		return 0;	//表示无处理
	case DEBUG_ACT_RUN:
		{
			m_LastDebugMode|=0x40;
			BYTE MODE=(m_LastDebugMode&0x0f)-1;
			switch(MODE)
			{
			case 1: // 靠计算得来
			/*	m_Offset[1]+=param;
				if (m_Offset[1]<0) m_Offset[1] =0;
				SetMotorActDist(ACT_TYPE2,m_Offset[1]);
				MOTOR_IF_ERROR_QUIT*/
				break;
			case 2:
				//m_Offset[2]+=param;
				//if (m_Offset[2]<0) m_Offset[2] =0;
				//SetMotorActDist(ACT_TYPE3,m_Offset[2]);
				//MOTOR_IF_ERROR_QUIT
				break;
			case 3:
				m_Offset[3]+=param;
				if (m_Offset[3]<0) m_Offset[3] =0;
				SetMotorActDist(ACT_TYPE4,m_Offset[3]);
				MOTOR_IF_ERROR_QUIT
				break;
			case 4:
			/*	m_Offset[4]+=param;
				if (m_Offset[4]<0) m_Offset[4] =0;
				SetMotorActDist(ACT_TYPE5,m_Offset[4]);
				MOTOR_IF_ERROR_QUIT*/
				break;
			case 5:
				/*m_Offset[5]+=param;
				if (m_Offset[5]<0) m_Offset[5] =0;
				SetMotorActDist(ACT_TYPE6,m_Offset[5]);
				MOTOR_IF_ERROR_QUIT*/
				break;
			case 6:
			/*	m_Offset[6]+=param;
				if (m_Offset[6]<0) m_Offset[6] =0;
				SetMotorActDist(ACT_TYPE7,m_Offset[6]);
				MOTOR_IF_ERROR_QUIT*/
				break;
			default:
				break;
			}
			MotorActParam actParam;
			actParam.type=RELATIVE_POS;
			actParam.speedStar=1000;
			actParam.speedEnd=10000;
			actParam.step=abs(param);
			actParam.ACCDec=120000;
			m_LastError=WriteActParam(ACT_Debug,actParam);
			MOTOR_IF_ERROR_QUIT
			m_LastError=MoveRelativeAndWait((UCHAR)ACT_Debug,(param>0)?m_DirDown:m_DirUp);
			MOTOR_IF_ERROR_QUIT
			SendMotorPos();
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

void CMagneticBarMotor :: SetDirDown(DIR_TYPE dir)
{
	m_DirDown=dir;
	m_DirUp=(m_DirDown==DIR_CCW)?DIR_CW:DIR_CCW;
}
