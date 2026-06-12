#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ZMotor.h"
#include "../trace.h"

//--------------------------- 磁棒套电机-------------------------------

CZMotor::CZMotor(UCHAR CtrID,UCHAR axisID):CBasicMotor(CtrID,axisID)
{
	m_DirDown=DIR_CCW;
	m_DirUp=(m_DirDown==DIR_CCW)?DIR_CW:DIR_CCW;
	m_MotorPos =POS_ZMotor_UNKNOW;
}

CZMotor::~CZMotor()
{
}

// 初始化回零
int CZMotor:: InitHome()
{
	TRACE(_T("CZMotor::InitHome MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	m_LastError= CBasicMotor::InitHome();
	MOTOR_IF_ERROR_QUIT
	m_MotorPos=POS_ZMotor_HOME;
	m_LastAct=ACT_NONE;
	return m_LastError;
}
int CZMotor::GoToHome(UINT WaitTM)
{
	TRACE(_T("CZMotor::GoToHome MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
    m_LastError=BackHome(ACT_Move,ACT_InitHome,0,100,WaitTM);
	if(m_LastError<0)
	{
		ResetErr();
		m_LastError=BackHome(ACT_Move,ACT_InitHome,0,100,WaitTM);
	}
	MOTOR_IF_ERROR_QUIT
	m_LastAct=ACT_NONE;
	m_MotorPos=POS_ZMotor_HOME;
	return	m_LastError;
}
int CZMotor:: GoToHome1( UINT WaitTM )
{
	TRACE(_T("CZMotor::GoToHome1 MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	m_LastError=MoveHome(ACT_Move,m_DirUp,WaitTM);
	if(m_LastError<0)
	{
		ResetErr();
		m_LastError=MoveHome(ACT_Move,m_DirUp,WaitTM);
	}
	MOTOR_IF_ERROR_QUIT
	m_LastAct=ACT_NONE;
	m_MotorPos=POS_ZMotor_HOME;
	return	m_LastError;
}
// 下降到底,孔1 和其他孔高度不一致，已经修改一致,保存参数和运动分开执行  MARK=1 只保存参数
int CZMotor:: MoveDown(int& MoveStep , BYTE HolePos,UINT WaitTM ,BYTE Mark)
{
	TRACE(_T("CZMotor::MoveDown MotorName=%s,m_MotorPos=%d,Mark=%d"),m_strName,m_MotorPos,Mark);
	MOTOR_IF_ERROR_QUIT
	int Pos = m_Offset[1];  // 孔1 和孔2 高度不一致
	int NowPos =0;
	if (Mark==1 || Mark==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		m_LastError=GetAbsPos(NowPos);
		int Step =Pos-abs(NowPos);
		MoveStep = Step;
		if (Step==0)
		{
			return  m_LastError;
		}
		m_LastError=WriteActParam(ACT_TYPE2,Step,RELATIVE_POS);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}
	if (Mark==2 || Mark==0)
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
		m_MotorPos =POS_ZMotor_Down;
	}

	return m_LastError;
}

// 快速振荡，相对距离。
int CZMotor::  MoveDownFast(BOOL bDown ,int Volume,UINT WaitTM,BYTE Mark,int SpeedLevel )
{
	TRACE(_T("CZMotor::MoveDownFast MotorName=%s,m_MotorPos=%d,bDown=%d,Volume=%d,Mark=%d,SpeedLevel=%d"),m_strName,m_MotorPos,bDown,Volume,Mark,SpeedLevel);
	MOTOR_IF_ERROR_QUIT
	if (Mark==1 || Mark ==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT

		if (Volume>2000) //限定2ml
		{
			Volume =2000;
		}
		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		actParam.speedStar=m_iSpeedStart[3]*SpeedLevel/10; // 快速振荡 ,根据
		actParam.speedEnd=m_iSpeed[3]*SpeedLevel/10;

		actParam.step=(TYPE_MM_1ML_HOLE*ZMOTOR_MM_TO_PULSE)*Volume/1000; // 体积确定高度
		if(actParam.step<100 && SpeedLevel>5)
		{
			actParam.speedStar=m_iSpeedStart[3]*5/10; // 快速振荡 ,根据 ,距离短，速度上不去
			actParam.speedEnd=m_iSpeed[3]*5/10;
		}
		actParam.ACCDec=30; //加速时间
		m_LastError=WriteActParam(ACT_TYPE3,actParam);
		MOTOR_IF_ERROR_QUIT
	    wxMilliSleep(50); // 增加延时
	}
	if (Mark ==2 || Mark ==0)
	{
		if (bDown)
		{
			m_LastError=MoveRelativeAndWait(ACT_TYPE3,m_DirDown,WaitTM);	//
		}
		else
		{
			m_LastError=MoveRelativeAndWait(ACT_TYPE3,m_DirUp,WaitTM);	//
		}
		MOTOR_IF_ERROR_QUIT
		m_MotorPos =POS_ZMotor_DownFast;
	}
	return m_LastError;
}


int CZMotor::  MoveDownStep(int &MoveStep,float Height,UINT WaitTM,BYTE Mark)//下降脉冲
{
	TRACE(_T("CZMotor::MoveDownStep MotorName=%s,m_MotorPos=%d,Height=%f ,Mark=%d"),m_strName,m_MotorPos,Height,Mark);
	MOTOR_IF_ERROR_QUIT
	if (Mark ==1||Mark ==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		int Step = Height*ZMOTOR_MM_TO_PULSE;
		int NowPos =0;
		m_LastError = GetAbsPos(NowPos); //当前位置
		int Temp = m_Offset[1] -abs(NowPos)-m_SuckmagneticLeftStep;
		if (Step >Temp)	//底部判断
		{
			 Step =Temp;
			 TRACE(_T("CZMotor::MoveDownStep Step=%d"),Step);
		}
		MoveStep = Step ;
		m_LastError=WriteActParam(ACT_TYPE4,Step,RELATIVE_POS);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}

	if (Mark ==2 || Mark ==0)
	{
		m_LastError=MoveRelative(ACT_TYPE4,m_DirDown);
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
		m_MotorPos =POS_ZMotor_DownStep;
	}
	return m_LastError;
}
// 获取下降时间 ms
int  CZMotor::MoveDownStepTime(float Height) //TODO
{
	DWORD  Time=0;
    Time = (Height*ZMOTOR_MM_TO_PULSE/m_iSpeed[4])*1000;
    return Time ;
}


int  CZMotor:: MoveToLiquidTop(int &MoveStep,BYTE Type,BYTE HolePos,int Volume,BOOL bFast ,UINT WaitTM,BYTE Mark)
{
	TRACE(_T("CZMotor::MoveToLiquidTop MotorName=%s,m_MotorPos=%d,Type=%d,Volume=%d,bFast=%d,Mark=%d"),m_strName,m_MotorPos,Type,Volume,bFast,Mark);
	MOTOR_IF_ERROR_QUIT
	int Pos = 0,NowPos =0;
	m_LastError=GetAbsPos(NowPos);
	if (HolePos==1)
	{
		if (Type==KIT_TYPE_ONE || Type==KIT_TYPE_THREE )
		{
			if(Volume>10000)
			{
				Volume=10000;
			}
			if(Volume>1000)
			{
				Pos= m_Offset[1]-(TYPE1_MM_1ML_1*ZMOTOR_MM_TO_PULSE)-(Volume*(TYPE1_MM_1ML_2*ZMOTOR_MM_TO_PULSE)/1000)+(TYPE1_MM_1ML_2*ZMOTOR_MM_TO_PULSE);  //  高度从底部往上算
			}
			else
			{
				Pos= m_Offset[1]-(Volume*(TYPE1_MM_1ML_1*ZMOTOR_MM_TO_PULSE)/1000);
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
				Pos= m_Offset[1]-(TYPE2_MM_1ML_1*ZMOTOR_MM_TO_PULSE)-(Volume*(TYPE2_MM_1ML_2*ZMOTOR_MM_TO_PULSE)/1000)+(TYPE2_MM_1ML_2*ZMOTOR_MM_TO_PULSE);
			}
			else
			{
				Pos= m_Offset[1]-(Volume*(TYPE2_MM_1ML_1*ZMOTOR_MM_TO_PULSE)/1000);
			}
		}
	}
	else  //其他孔位计算一致
	{
		if(Volume>2000)	Volume=2000;
		 Pos= m_Offset[1]-(TYPE_MM_1ML_HOLE*ZMOTOR_MM_TO_PULSE)*Volume/1000; // 体积确定高度;
	}
 	if (Pos<0) Pos =0;
    TRACE(_T("CZMotor::MoveToLiquidTop NowPos=%d,TargetPos=%d "),NowPos,Pos);
	if(m_DirDown==DIR_CCW)
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
		if (Mark ==1||Mark ==0)
		{
			m_LastError=EmptyMotorStop(TRUE);
			MOTOR_IF_ERROR_QUIT
			m_LastError=WriteActParam(ACT_TYPE5,step,RELATIVE_POS);
			MOTOR_IF_ERROR_QUIT
			wxMilliSleep(50);
		}
		if (Mark ==2||Mark ==0)
		{
			m_LastError=MoveRelative(ACT_TYPE5,bDir);
			MOTOR_IF_ERROR_QUIT
		}
	}
	else
	{
		if (Mark ==2||Mark ==0)
		{
			m_LastError=EmptyMotorStop(TRUE);
		    MOTOR_IF_ERROR_QUIT
			m_LastError=WriteActParam(ACT_TYPE6,step,RELATIVE_POS);
			MOTOR_IF_ERROR_QUIT
			wxMilliSleep(50);
		}
		if (Mark ==2||Mark ==0)
		{
			m_LastError=MoveRelative(ACT_TYPE6,bDir);
			MOTOR_IF_ERROR_QUIT
		}
	}
	if (Mark ==2||Mark ==0)
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
		m_MotorPos =POS_ZMotor_Top1;
	}
	return m_LastError;
}

//圆孔中缓慢下降吸磁
int CZMotor:: MoveDownSlow(int &MoveStep,BOOL bSegment,  float height,  UINT WaitTM ,BYTE Mark,BYTE suckLevel)// 下降到底
{
	TRACE(_T("CZMotor::MoveDownSlow MotorName=%s,m_MotorPos=%d,Mark=%d,bSegment=%d,height=%f"),m_strName,m_MotorPos,Mark,bSegment,height);
	DWORD StartTime=timeGetTime();
	if (Mark ==1 ||Mark ==0)
	{
		MOTOR_IF_ERROR_QUIT
		m_LastError=EmptyMotorStop(TRUE);
		int Step= m_Offset[1]-m_SuckmagneticLeftStep ;
		int NowPos = 0;
		m_LastError = GetAbsPos(NowPos); //当前位置
		MOTOR_IF_ERROR_QUIT
		if (bSegment && height>0 )
		{
			 Step= height*ZMOTOR_MM_TO_PULSE;
		}
		else
		{
			Step-=abs(NowPos); //需要走的脉冲
			if(Step<0)
				Step =0;
		}
		MoveStep = Step ;

		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		actParam.speedStar=m_iSpeedStart[7]*suckLevel/10; //
        if(actParam.speedStar<10)
		 actParam.speedStar =10;
		actParam.speedEnd=m_iSpeed[7]*suckLevel/10;

		actParam.step=Step;
		actParam.ACCDec=300;
		TRACE(_T("CZMotor::MoveDownSlow Step=%d,speedEnd=%d,speedStar=%d"),Step,actParam.speedEnd, actParam.speedStar);
		m_LastError=WriteActParam(ACT_TYPE7,actParam);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}

	if (Mark ==2||Mark ==0)
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
		m_MotorPos =POS_ZMotor_Down;
	}
	DWORD EndTime=timeGetTime();
	DWORD Span =EndTime -StartTime;
	if (Mark!=1)
	{
	  TRACE(_T("CZMotor MoveDownSlow::Span=%d"),Span);
	}

	return m_LastError;
}
//缓慢上升吸磁
int CZMotor:: MoveUpSlow(int &MoveStep,float height, UINT WaitTM ,BYTE Mark, int SpeedLevel)
{
	TRACE(_T("CZMotor::MoveUpSlow MotorName=%s,m_MotorPos=%d,Mark=%d,height=%f,SpeedLevel=%d"),m_strName,m_MotorPos,Mark,height,SpeedLevel);
	MOTOR_IF_ERROR_QUIT
	if (Mark ==1 ||Mark ==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		int Step= height*ZMOTOR_MM_TO_PULSE;
		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		actParam.speedStar=m_iSpeedStart[7]*SpeedLevel/10;
		actParam.speedEnd=m_iSpeed[7]*SpeedLevel/10;
		actParam.step=Step;
		actParam.ACCDec=30;
		MoveStep =Step ;
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
		m_MotorPos =POS_ZMotor_Up;
	}
	return m_LastError;
}

// 通过体积获取高度
float  CZMotor:: GetHeightByVolumeInHolePos(BYTE Type ,BYTE Pos,int Volume)
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
             Height=(float)(TYPE1_MM_1ML_1)+(float)(Volume*TYPE1_MM_1ML_2/1000-TYPE1_MM_1ML_2);
		   }
		   else
		   {
                 Height=(float) Volume*TYPE1_MM_1ML_1/1000; //Volume 单位是微升
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
               Height= (float)Volume*TYPE2_MM_1ML_1/1000;
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

int CZMotor::MoveUpDownFast(BOOL bDown,float height, UINT WaitTM ,BYTE Mark,int SpeedLevel)
{
	TRACE(_T("CZMotor::MoveUpDownFast MotorName=%s,m_MotorPos=%d,bDown=%d,height=%f,Mark=%d,SpeedLevel=%d"),m_strName,m_MotorPos,bDown,height,Mark,SpeedLevel);
	MOTOR_IF_ERROR_QUIT
	if (Mark==1 || Mark==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		actParam.speedStar=m_iSpeedStart[3]*SpeedLevel/10; // 快速振荡 ,根据
		actParam.speedEnd=m_iSpeed[3]*SpeedLevel/10;
		actParam.step=height*ZMOTOR_MM_TO_PULSE;
		if(actParam.step<100 && SpeedLevel>5)
		{
			actParam.speedStar=m_iSpeedStart[3]*5/10; // 快速振荡 ,根据 ,距离短，速度上不去
			actParam.speedEnd=m_iSpeed[3]*5/10;
		}
		actParam.ACCDec=30;
		m_LastError=WriteActParam(ACT_TYPE3,actParam);
		MOTOR_IF_ERROR_QUIT
		wxMilliSleep(50);
	}

	if (Mark ==2 || Mark ==0)
	{
		if (bDown)
		{
			m_LastError=MoveRelative(ACT_TYPE3,m_DirDown);	//
		}
		else
		{
			m_LastError=MoveRelative(ACT_TYPE3,m_DirUp);	//
		}
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
		m_MotorPos =POS_ZMotor_DownFast;
	}
	return m_LastError;
}

int CZMotor::DebugMotor(BYTE act,int param,int relatepos)
{
	TRACE(_T("CZMotor::DebugMotor act=%d, param=%d,"),act,param);
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
				rtn = MoveDownFast(FALSE,1000); //上升
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 3:
				rtn = MoveDownFast(TRUE,1000); //下降
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 4:
				rtn = MoveDownStep(Step,2);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 5:
				rtn = MoveToLiquidTop(Step,KIT_TYPE_ONE); // 液面顶部
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 6:
				rtn = MoveToLiquidTop(Step,KIT_TYPE_TWO);
				//rtn = MoveDownSlow(Step,FALSE,0,WAITTIME,0,1); // 下降吸磁
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
			case 1:
				m_Offset[1]+=param;
				if (m_Offset[1]<0) m_Offset[1] =0;
				SetMotorActDist(ACT_TYPE2,m_Offset[1]);
				MOTOR_IF_ERROR_QUIT
					break;
	/*		case 2:
			   	m_Offset[2]+=param;
				if (m_Offset[2]<0) m_Offset[2] =0;
				SetMotorActDist(ACT_TYPE3,m_Offset[2]);
				MOTOR_IF_ERROR_QUIT
				break;
			case 3:
				m_Offset[3]+=param;
				if (m_Offset[3]<0) m_Offset[3] =0;
				SetMotorActDist(ACT_TYPE4,m_Offset[3]);
				MOTOR_IF_ERROR_QUIT
					break;
			case 4:
				m_Offset[4]+=param;
				if (m_Offset[4]<0) m_Offset[4] =0;
				SetMotorActDist(ACT_TYPE5,m_Offset[4]);
				MOTOR_IF_ERROR_QUIT
					break;
			case 5:
				m_Offset[5]+=param;
				if (m_Offset[5]<0) m_Offset[5] =0;
				SetMotorActDist(ACT_TYPE6,m_Offset[5]);
				MOTOR_IF_ERROR_QUIT
					break;*/
			default:
				break;
			}
			MotorActParam actParam;
			actParam.type=RELATIVE_POS;
			actParam.speedStar=1000;
			actParam.speedEnd=2000;
			actParam.step=abs(param);
			actParam.ACCDec=12000;
			m_LastError=WriteActParam(ACT_Debug,actParam);
			//m_LastError=WriteActParam(ACT_Debug,abs(param),RELATIVE_POS);
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
