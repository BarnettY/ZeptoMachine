

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "XMotor.h"
#include "../trace.h"

CXMotor::CXMotor(UCHAR CtrID,UCHAR axisID):CBasicMotor(CtrID,axisID)
{
	m_DirDown=DIR_CW;
	m_DirUp=(m_DirDown==DIR_CCW)?DIR_CW:DIR_CCW;
	m_MotorPos =POS_XMotor_UNKNOW;
}
CXMotor::~CXMotor()
{
}
//初始化回零
int CXMotor ::InitHome()
{
	TRACE(_T("CXMotor::InitHome MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	m_LastError=CBasicMotor::InitHome();
	MOTOR_IF_ERROR_QUIT
	m_MotorPos=POS_XMotor_HOME;
	m_LastAct=ACT_NONE;
	return m_LastError;
}
int CXMotor::GoToHome(UINT WaitTM)
{
	TRACE(_T("CXMotor::GoToHome MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	m_LastError=BackHome(ACT_Move,ACT_InitHome,0,100,WaitTM);
	if(m_LastError<0)
	{ 
		ResetErr();
		m_LastError=BackHome(ACT_Move,ACT_InitHome,0,100,WaitTM); 
	}
	MOTOR_IF_ERROR_QUIT
    m_MotorPos =POS_XMotor_HOME;
	m_LastAct=ACT_NONE;
	return	m_LastError;
}

// 移动到1号孔中心 ,走绝对位置，孔1中心根据孔2位置确定  Type 标记试剂盒类型
int CXMotor:: MoveToHoleOneCenter(BYTE Type,UINT WaitTM) 
{
	TRACE(_T("CXMotor::MoveToHoleOneCenter MotorName=%s,m_MotorPos=%d,Type=%d"),m_strName,m_MotorPos,Type);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	int Pos=0,NowPos =0;
	if(Type== KIT_TYPE_ONE || Type== KIT_TYPE_THREE)
	{ 
        Pos =m_Offset[3]-m_Offset[1]; //孔1中心根据孔2位置确定
	}
	else if (Type == KIT_TYPE_TWO)
	{
        Pos =m_Offset[3]-m_Offset[7];
	}
	m_LastError=GetAbsPos(NowPos);
    MOTOR_IF_ERROR_QUIT

	m_LastError=MoveToPosRes(NowPos,Pos,ACT_TYPE2,WaitTM);
	TRACE(_T("CXMotor::MoveToHoleOneCenter,Type =%d,NowPos=%d,TargetPos=%d "),Type,NowPos,Pos);
	MOTOR_IF_ERROR_QUIT
    m_MotorPos =POS_XMotor_Hole1_Center;
	return m_LastError;
}
// 分段运动走孔
int  CXMotor::MoveToHoleOneBySegment(BYTE Type,BOOL Right, BOOL bMix, int SpeedLevel,int Segment, UINT WaitTM,BYTE Mark)
{
	TRACE(_T("CXMotor::MoveHoleOneDistance MotorName=%s,m_MotorPos=%d,Type=%d,Right=%d,bMix=%d,SpeedLevel=%d,Segment=%d,Mark=%d"),m_strName,m_MotorPos,Type,Right,bMix,SpeedLevel,Segment,Mark);
	MOTOR_IF_ERROR_QUIT
	if (Mark==1 || Mark==0)
	{
		m_LastError=EmptyMotorStop(TRUE);
		MOTOR_IF_ERROR_QUIT
		MotorActParam actParam;
		actParam.type=RELATIVE_POS;
		if(SpeedLevel>=10) SpeedLevel=10;
		if (SpeedLevel<=1) SpeedLevel=1;
		if (bMix) //混匀
		{
			actParam.speedStar= m_iSpeedStart[3] *SpeedLevel/10; //  SpeedLevel 区分
			actParam.speedEnd= m_iSpeed[3] *SpeedLevel/10;   
		}
		else //吸磁
		{
			actParam.speedStar= m_iSpeedStart[4]*SpeedLevel/10 ; //
			actParam.speedEnd= m_iSpeed[4]*SpeedLevel/10 ;       //
		}
		if(Type == KIT_TYPE_ONE || Type == KIT_TYPE_THREE)
		{ 
			actParam.step=m_Offset[2] /Segment; //长孔间距1/次数 ，TODO 是否考虑脉冲过小情形
		}
		else if (Type == KIT_TYPE_TWO)
		{
			actParam.step=m_Offset[8]/Segment; //长孔间距2/次数
		}
		actParam.ACCDec=120000;
		m_LastError=WriteActParam(ACT_TYPE3,actParam);
		wxMilliSleep(50);
	}
	if (Mark==2 || Mark==0)
	{
		if (Right) 
		{
			m_LastError=MoveRelativeAndWait(ACT_TYPE3,m_DirDown,WaitTM);	//向右
			m_MotorPos =POS_XMotor_Hole1_Right;
		}
		else
		{
			m_LastError=MoveRelativeAndWait(ACT_TYPE3,m_DirUp,WaitTM);	//	向左
			m_MotorPos =POS_XMotor_Hole1_Left;
		}
		MOTOR_IF_ERROR_QUIT
	}
	return m_LastError;

}


 // 混匀用 ，间距 ，吸附磁珠
int CXMotor:: MoveHoleOneDistance(BYTE Type,BOOL Right, BOOL bMix, int SpeedLevel, UINT WaitTM )
{
	TRACE(_T("CXMotor::MoveHoleOneDistance MotorName=%s,m_MotorPos=%d,Type=%d,Right=%d,bMix=%d,SpeedLevel=%d"),m_strName,m_MotorPos,Type,Right,bMix,SpeedLevel);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	MotorActParam actParam;
	actParam.type=RELATIVE_POS;
 
    if(SpeedLevel>=10) SpeedLevel=10;
	if (SpeedLevel<=1) SpeedLevel=1;

    if (bMix) //混匀
    {
		actParam.speedStar= m_iSpeedStart[3] *SpeedLevel/10; //  SpeedLevel 区分
		actParam.speedEnd= m_iSpeed[3] *SpeedLevel/10;  
    }
	else //吸磁
	{
		actParam.speedStar= m_iSpeedStart[4] *SpeedLevel/10;   ;
		actParam.speedEnd= m_iSpeed[4] *SpeedLevel/10;   ;
	}

	if(Type==KIT_TYPE_ONE || Type== KIT_TYPE_THREE)
	{ 
		actParam.step=m_Offset[2]; //圆孔间距
	}
	else if (Type==KIT_TYPE_TWO)
	{
		actParam.step=m_Offset[8];
	}
	actParam.ACCDec=120000;
	m_LastError=WriteActParam(ACT_TYPE3,actParam);
	wxMilliSleep(50);

    if (Right)
    {
		m_LastError=MoveRelative(ACT_TYPE3,m_DirDown);
    }
	else
	{
    	m_LastError=MoveRelative(ACT_TYPE3,m_DirUp);
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
	m_MotorPos =POS_XMotor_Hole1_Right;
	if (Right)
	{
		m_MotorPos =POS_XMotor_Hole1_Right;
	}
	else
	{
       	m_MotorPos =POS_XMotor_Hole1_Left;
	}
	MOTOR_IF_ERROR_QUIT
	return m_LastError;
}

//混匀用，一半间距，吸附磁珠
int CXMotor:: MoveHalfHoleOneDistance(BYTE Type,BOOL Left,BOOL bMix, int SpeedLevel, UINT WaitTM )
{
	TRACE(_T("CXMotor::MoveHalfHoleOneDistance MotorName=%s,m_MotorPos=%d,Type=%d"),m_strName,m_MotorPos,Type);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	MotorActParam actParam;
	actParam.type=RELATIVE_POS;
	if(SpeedLevel>=10) SpeedLevel=10;
	if (SpeedLevel<=1) SpeedLevel=1;

	if (bMix) //混匀
	{
		actParam.speedStar= m_iSpeedStart[3] *SpeedLevel/10; //  SpeedLevel 区分
		actParam.speedEnd= m_iSpeed[3] *SpeedLevel/10;  
	}
	else //吸磁
	{
		actParam.speedStar= m_iSpeedStart[4] *SpeedLevel/10 ;
		actParam.speedEnd= m_iSpeed[4] *SpeedLevel/10;   ;
	}
	if(Type==KIT_TYPE_ONE || Type== KIT_TYPE_THREE)
	{ 
		actParam.step=m_Offset[2]/2;
	}
	else if (Type==KIT_TYPE_TWO)
	{
		actParam.step=m_Offset[8]/2;
	}
	actParam.ACCDec=120000;
	m_LastError=WriteActParam(ACT_TYPE3,actParam);

	wxMilliSleep(50);
	if (!Left)
	{
		m_LastError=MoveRelativeAndWait(ACT_TYPE3,m_DirDown,WaitTM);	//
		m_MotorPos =POS_XMotor_Hole1_Star;
	}
	else
	{
		m_LastError=MoveRelativeAndWait(ACT_TYPE3,m_DirUp,WaitTM);	//	
		m_MotorPos =POS_XMotor_Hole1_Center;
	}
	MOTOR_IF_ERROR_QUIT

	return m_LastError;

}
//返回毫秒
int CXMotor::GetMoveHalfHoleOneDistanceTime(BYTE Type, int SpeedLevel,BOOL bMix)
{
    DWORD Time =0;
    int Speed =0,Step=0;
	if (bMix) //混匀
	{
		if(SpeedLevel>=10) SpeedLevel=10;
		if (SpeedLevel<=1) SpeedLevel=1;
	  	Speed= m_iSpeed[3] *SpeedLevel/10;  
	}
	else
	{
		Speed= m_iSpeed[4] *SpeedLevel/10;
	}
	if(Type==KIT_TYPE_ONE || Type== KIT_TYPE_THREE)
	{ 
		Step=m_Offset[2]/2;
	}
	else if (Type==KIT_TYPE_TWO)
	{
		Step=m_Offset[8]/2;
	}
	Time =Step*1000/ Speed ;
    return Time ;
}

// 孔2 的位置+加上间距组成，  移动孔位置
int CXMotor:: MoveToHolePos(BYTE Type,int index,UINT WaitTM)
{
    int diatance = m_Offset[3]; //孔2位置,不同类型的试剂盒，孔2 位置一致
	int Pos = diatance + m_Offset[4]*(index-2); 
	int NowPos =0;
	m_LastError=GetAbsPos(NowPos);
	MOTOR_IF_ERROR_QUIT
	TRACE(_T("CXMotor::MoveToHolePos,Type =%d,index =%d ,NowPos=%d,TargetPos=%d "),Type,index,NowPos,Pos);
	m_LastError=MoveToPosRes(NowPos,Pos,ACT_TYPE4,WaitTM);
	MOTOR_IF_ERROR_QUIT
	m_LastError=GetAbsPos(NowPos);
	MOTOR_IF_ERROR_QUIT
	TRACE(_T("CXMotor::MoveToHolePos ,index =%d ,NowPos=%d "),index,NowPos);
    return m_LastError;
}

//小孔间距 不同类型一致
int  CXMotor:: MoveHoleDistance(BOOL Right,UINT WaitTM)
{
	TRACE(_T("CXMotor::MoveHoleDistance MotorName=%s,m_MotorPos=%d,Right=%d"),m_strName,m_MotorPos,Right);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	if (Right)
	{
		m_LastError=MoveRelativeAndWait(ACT_TYPE5,m_DirDown,WaitTM);	//	
	}
	else
	{
		m_LastError=MoveRelativeAndWait(ACT_TYPE5,m_DirUp,WaitTM);	//	
	}
	MOTOR_IF_ERROR_QUIT
	m_MotorPos =POS_XMotor_Hole2;
	return m_LastError;

}

// 移动最后孔间距 相对位置
int CXMotor::MoveLastHoleDistance(BOOL Right,UINT WaitTM) 
{
	TRACE(_T("CXMotor::MoveHalfHoleOneDistance MotorName=%s,m_MotorPos=%d"),m_strName,m_MotorPos);
	MOTOR_IF_ERROR_QUIT
	m_LastError=EmptyMotorStop(TRUE);
	MOTOR_IF_ERROR_QUIT
	if (Right)
	{
		m_LastError=MoveRelativeAndWait(ACT_TYPE6,m_DirDown,WaitTM);	//	
	}
	else
	{
		m_LastError=MoveRelativeAndWait(ACT_TYPE6,m_DirUp,WaitTM);	//	
	}
	MOTOR_IF_ERROR_QUIT
	m_MotorPos =POS_XMotor_LastHole;
	return m_LastError;


}
// 最后孔位置
int CXMotor::MoveToLastHolePos(BYTE Type,UINT WaitTM )
{
 
	int Pos =0,NowPos =0;
	if(Type==KIT_TYPE_ONE )
	{ 
		Pos=m_Offset[3]+m_Offset[4]*2+m_Offset[5];
	}
	else if (Type==KIT_TYPE_TWO)
	{
		Pos=m_Offset[3]+m_Offset[4]*4+m_Offset[5];
	}
	else if (Type==KIT_TYPE_THREE)
	{
		Pos=m_Offset[3]+m_Offset[4]*4+m_Offset[5];
	}

	m_LastError=GetAbsPos(NowPos);
	TRACE(_T("CXMotor::MoveToLastHolePos,Type =%d ,NowPos=%d,TargetPos=%d "),Type,NowPos,Pos);

	m_LastError=MoveToPosRes(NowPos,Pos,ACT_TYPE7,WaitTM);
	MOTOR_IF_ERROR_QUIT

	m_LastError=GetAbsPos(NowPos);
	MOTOR_IF_ERROR_QUIT

	TRACE(_T("CXMotor::MoveToLastHolePos ,Type =%d ,NowPos=%d "),Type,NowPos);
	return m_LastError;

}
// 10ml试剂盒 特殊孔位
int  CXMotor:: MoveToSpecialHolePos(UINT WaitTM )
{
	int Pos =0,NowPos =0;
	Pos=m_Offset[3]+m_Offset[4]*4+m_Offset[5]; // 10ml试剂盒的6号孔 实际是其他类型的7号位置
	m_LastError=GetAbsPos(NowPos);
	TRACE(_T("CXMotor::MoveToSpecialHolePos ,NowPos=%d,TargetPos=%d "),NowPos,Pos);

	m_LastError=MoveToPosRes(NowPos,Pos,ACT_TYPE7,WaitTM);
	MOTOR_IF_ERROR_QUIT
	m_LastError=GetAbsPos(NowPos);
	MOTOR_IF_ERROR_QUIT

    TRACE(_T("CXMotor::MoveToSpecialHolePos ,NowPos=%d "),NowPos);
	return m_LastError;

}

int CXMotor::DebugMotor(BYTE act,int param,int relatepos)
{
	TRACE(_T("CXMotor::DebugMotor act=%d, param=%d,"),act,param);
	int rtn=0;
	m_LastDebugMode|=0x80;
	switch(act)
	{
	case DEBUG_ACT_RESET:
		m_LastDebugMode&=0xBF;
		rtn=InitMove();
		rtn=InitHome();
		m_LastDebugMode|=0x80;
		SendMotorPos();
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
				rtn=MoveToHoleOneCenter(KIT_TYPE_ONE);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 2:
				rtn=MoveToHoleOneCenter(KIT_TYPE_TWO);
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 3:
				rtn = MoveHoleOneDistance(KIT_TYPE_ONE); 
				if(rtn<0) return rtn;
				MODE++;
				break;	

			case 4:
				rtn = MoveHalfHoleOneDistance(KIT_TYPE_ONE); 
				if(rtn<0) return rtn;
				MODE++;
				break;	
			case 5:
				rtn = MoveHoleOneDistance(KIT_TYPE_TWO); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 6:
				rtn = MoveHalfHoleOneDistance(KIT_TYPE_TWO); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 7:
				rtn = MoveToHolePos(0,2); 
				if(rtn<0) return rtn;
				MODE++;
				break;

			case 8:
				rtn = MoveHoleDistance(TRUE); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 9:
				rtn = MoveLastHoleDistance(); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 10:
				rtn = MoveToLastHolePos(KIT_TYPE_ONE); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 11:
				rtn = MoveToLastHolePos(KIT_TYPE_TWO); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 12:
				rtn = GoToHome(); 
				if(rtn<0) return rtn;
				MODE++;
				break;
			case 13:
				rtn = MoveToSpecialHolePos();  //类型1 的新增孔位，
				if(rtn<0) return rtn;
				MODE++;
				break;
			default:
				rtn=InitMove();
				if(rtn<0) return rtn;
				MODE=1;
				break;
			}
            SendMotorPos();
			m_LastDebugMode=MODE|0x80;
		}
		return 0;	//表示无处理
	case DEBUG_ACT_RUN:
		{
			m_LastDebugMode|=0x40;
			BYTE MODE=(m_LastDebugMode&0x0f)-1;
			switch(MODE)
			{
			case 1:
				m_Offset[1]-=param;  //孔1 中心距离孔2位置
				if (m_Offset[1]<0) m_Offset[1] =0;
				SetMotorActDist(ACT_TYPE2,m_Offset[1]);
				MOTOR_IF_ERROR_QUIT
				break;
			case 2:
				m_Offset[7]-=param; //孔1 中心距离孔2位置
				if (m_Offset[7]<0) m_Offset[7] =0;
				break;
			case 3:
				m_Offset[2]+=param; //孔1间距
				if (m_Offset[2]<0) m_Offset[2] =0;
				SetMotorActDist(ACT_TYPE3,m_Offset[2]);
				MOTOR_IF_ERROR_QUIT
				break;
	        case 4:
               break;
			case 5:
				m_Offset[8]+=param;//孔1间距（短）
				if (m_Offset[8]<0) m_Offset[8] =0;
				break;
			case 6:
				break;

			case 7: //孔2位置
				m_Offset[3]+=param;
				if (m_Offset[3]<0) m_Offset[3] =0;
				SetMotorActDist(ACT_TYPE4,m_Offset[3]);
				MOTOR_IF_ERROR_QUIT
				break;

			case 8:
				m_Offset[4]+=param; //圆孔间距
				if (m_Offset[4]<0) m_Offset[4] =0;
				SetMotorActDist(ACT_TYPE5,m_Offset[4]);
				MOTOR_IF_ERROR_QUIT
				break;
			case 9: // 最后孔间距
				m_Offset[5]+=param;
				if (m_Offset[5]<0) m_Offset[5] =0;
				SetMotorActDist(ACT_TYPE6,m_Offset[5]);
				MOTOR_IF_ERROR_QUIT
				break;
			default:
				break;
			}
			MotorActParam actParam;
			actParam.type=RELATIVE_POS;
			actParam.speedStar=1000;
			actParam.speedEnd=2000;
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
void CXMotor :: SetDirDown(DIR_TYPE dir)
{
	m_DirDown=dir;
	m_DirUp=(m_DirDown==DIR_CCW)?DIR_CW:DIR_CCW;
}