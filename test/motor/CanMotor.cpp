// CanMotor.cpp: implementation of the CCanMotor class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "CanMotor.h"
#include "../can/CanCom.h"



//////////////////////////////////////////////////////////////////////
// class CCanMotor
//////////////////////////////////////////////////////////////////////
CCanMotor::CCanMotor(UCHAR CtrID,UCHAR axisID):FinishSemp(TRUE)
{
	FASSERT(axisID<=3);
	FASSERT(axisID>0);
	m_pctrl=NULL;
	m_AxisID=axisID;
	m_CtrID=CtrID;
	//建立事件句柄
	g_Can.ResigterCtrl(CtrID,&m_pctrl);
	m_pctrl->SetChildMotor(axisID,this);
  	srand(wxDateTime::Now().GetTicks());
	m_SN=rand()%255;
	FinishSemp.SetEvent();
}

CCanMotor::~CCanMotor()
{

}
///////////////
//电机参数设置
//设置FINISH延时处理	
int CCanMotor::SetMotorPendEnable(BOOL able)
{
	m_pctrl->m_BNeedPendFinish[m_AxisID]=able;	
	return 0;
}

//设置脉冲类型
int CCanMotor::SetMotorPwmType(PWM_TYPE type)
{
	return m_pctrl->WritePWMType(m_AxisID,type);	
}

//设置电机细分，电流
int CCanMotor::SetMotorDriver(Current_TYPE current,SubDiv_TYPE subdiv,BOOL BModeA1_6)
{
	AxisDriverInfo info;
	info.Current=current;
	info.SubDiv=subdiv;
	info.ModeA1_6=BModeA1_6?Enable:Disable;
	return m_pctrl->WriteDriverInfo(m_AxisID,info);	
}
//设置电机锁定电流
int CCanMotor::SetMotorLockCurrent(BOOL BEnalbe,Current_TYPE current)
{
	LockCurrentInfo info(BEnalbe,current);
	return m_pctrl->WriteLockCurrent(m_AxisID,info);	
}
//设置液位减速度
int	CCanMotor::SetMotorLiquidDec(UINT DEC)
{
	return m_pctrl->WriteLiquidDec(m_AxisID,DEC);
}
//设置急停减速度,UrgentStopMotor用
int	CCanMotor::SetMotorUrgentStopDec(UINT DEC)
{
	return m_pctrl->WriteUrgentStopDec(m_AxisID,DEC);
}
//设置加速度方式:脉冲/秒*秒，加速时间
int	CCanMotor::SetMotorAccType(ACC_TYPE AccType)
{
	NotifyInfo info=m_pctrl->m_Notify[m_AxisID-1];
	info.AccType=AccType;
	return m_pctrl->WriteNotifyInfo(m_AxisID,info);
}
//设置轴位移完成后通知
int	CCanMotor::SetMotorStopNotify(BOOL BOrgStop,BOOL BLeftLimitStop,BOOL BRightLimitStop,BOOL BFinishStop,BOOL BLiquidStop)
{
	NotifyInfo info=m_pctrl->m_Notify[m_AxisID-1];
	info.BOrgNotify=BOrgStop;
	info.BLeftLimitNotify=BLeftLimitStop;
	info.BRightLimitNotify=BRightLimitStop;
	info.BLiqidNotify=BLiquidStop;
	info.BFinishNotify=BFinishStop;
	return m_pctrl->WriteNotifyInfo(m_AxisID,info);
}
//设置轴位移完成后通知
int	CCanMotor::SetMotorStopNotifyEX(BOOL BEncodeLimit,BOOL BEncodeZ,BOOL BFivePos)
{
	NotifyInfo info=m_pctrl->m_Notify[m_AxisID-1];
	info.BEncodeLimitNotify=BEncodeLimit;
	info.BEncodeZNotify=BEncodeZ;
	info.BFivePosNotify=BFivePos;
	return m_pctrl->WriteNotifyInfo(m_AxisID,info);
}


///////////////
//设置原点光耦有效电平
int	CCanMotor::SetMotorOrgLevel(Level_TYPE level)
{
	return m_pctrl->WriteLimitOrgLevel(m_AxisID,OrgLevel,level);
}
//设置右限位有效电平
int	CCanMotor::SetMotorRightLimitLevel(Level_TYPE level)
{
	return m_pctrl->WriteLimitOrgLevel(m_AxisID,RightLimitLevel,level);
}

//设置左限位有效电平
int	CCanMotor::SetMotorLeftLimitLevel(Level_TYPE level)
{
	return m_pctrl->WriteLimitOrgLevel(m_AxisID,LeftLimitLevel,level);
}

//原点光耦是否有效
int	CCanMotor::GetMotorOrg(BOOL &BVaild)
{
	int bit[]={11,8,5};
	int rtn=m_pctrl->ReadInput();
	if(rtn<0)
		return rtn;
	BVaild=(rtn&(1<<bit[(m_AxisID-1)]))>0;
	return 1;		
}

//左限位光耦是否有效
int	CCanMotor::GetMotorLeftLimit(BOOL &BVaild)
{
	int bit[]={10,7,4};
	int rtn=m_pctrl->ReadInput();
	if(rtn<0)
		return rtn;
	BVaild=(rtn&(1<<bit[(m_AxisID-1)]))>0;
	return 1;		
}
//右限位光耦是否有效
int	CCanMotor::GetMotorRightLimit(BOOL &BVaild)
{
	int bit[]={9,6,3};
	int rtn=m_pctrl->ReadInput();
	if(rtn<0)
		return rtn;
	BVaild=(rtn&(1<<bit[(m_AxisID-1)]))>0;
	return 1;		
}
//液位光耦是否有效
int	CCanMotor::GetMotorLiquid(BOOL &BVaild)
{
	int rtn=m_pctrl->ReadInput();
	if(rtn<0)
		return rtn;
	BVaild=(rtn&(1<<2))>0;
	return 1;		
}
//清空停止使能
int	CCanMotor::EmptyMotorStop(BOOL BSend)
{
	return m_pctrl->WriteLimitOrgEnable(m_AxisID,EmptyStop,FALSE,BSend);
}

//设置轴遇原点停止
int	CCanMotor::EnableMotorOrgStop(BOOL BOrgStop)
{
	return m_pctrl->WriteLimitOrgEnable(m_AxisID,OrgStop,BOrgStop);
}
//设置轴遇限位停
int	CCanMotor::EnableMotorLimitStop(BOOL BLimitStop)
{
	return m_pctrl->WriteLimitOrgEnable(m_AxisID,LimitStop,BLimitStop);
}
//设置编码器Z相停止
int	CCanMotor::EnableEncodeZStop(BOOL BEncodeZ)
{
	return m_pctrl->WriteLimitOrgEnable(m_AxisID,EncodeZStop,BEncodeZ);
}

//设置轴遇液位停
int	CCanMotor::EnableMotorLiquidStop(BOOL BLiquidStop)
{
	return m_pctrl->WriteLimitOrgEnable(m_AxisID,LiquidStop,BLiquidStop);
}

//设置轴遇条码5点停止
int	CCanMotor::EnableMotorBarcodeStop(BOOL BBarcodeStop,BOOL BSend)
{
	//因为加上原来的左右限位和原点，故8个点停止，故必须是轴1
	FASSERT(1==m_AxisID);
	return m_pctrl->WriteLimitOrgEnable(m_AxisID,BarcodeStop,BBarcodeStop);
}
int CCanMotor::SetOrgAct(Enable_STOP_NOW stop,Enable_ORG_RESETABSPOS reset,Enable_STOP_NOW limit)
{
	//写入原点动作
	m_pctrl->m_OrgAction.SetAxisOrgAction(m_AxisID,stop, reset,limit);
	return m_pctrl->WriteOrgAcion(m_pctrl->m_OrgAction);
}
//通用IO中断设置
//清空该轴附带的IO口中断
int	CCanMotor::EmptyMotorIOInter(BOOL BAllIO,BOOL BSend,Level_TYPE InterLevel)
{
	if(BAllIO)
		return m_pctrl->WriteInputInterrupt(m_AxisID,EmptyAllInter,FALSE,InterLevel,0,BSend);
	else	
		return m_pctrl->WriteInputInterrupt(m_AxisID,EmptyInter,FALSE,InterLevel,0,BSend);
}
//设置轴原点IO口中断
int	CCanMotor::SetMotorOrgInter(BOOL BInterrupt,Level_TYPE InterLevel)
{
	return m_pctrl->WriteInputInterrupt(m_AxisID,OrgInter,BInterrupt,InterLevel);
}
//设置轴左限位IO口中断
int	CCanMotor::SetMotorLeftLimitInter(BOOL BInterrupt,Level_TYPE InterLevel)
{
	return m_pctrl->WriteInputInterrupt(m_AxisID,LeftLimitInter,BInterrupt,InterLevel);
}
//设置轴右限位IO口中断
int	CCanMotor::SetMotorRightLimitInter(BOOL BInterrupt,Level_TYPE InterLevel)
{
	return m_pctrl->WriteInputInterrupt(m_AxisID,RightLimitInter,BInterrupt,InterLevel);
}
//设置通用输入IN1-IN3IO口中断
int	CCanMotor::SetGenIOInter(UCHAR INPUT,BOOL BInterrupt,Level_TYPE InterLevel)
{
	return m_pctrl->WriteInputInterrupt(m_AxisID,GenInter,TRUE,InterLevel,INPUT);
}
//禁用通用输入IN1-IN3IO口中断
int	CCanMotor::DisableGenIOInter(UCHAR INPUT)
{
	return m_pctrl->WriteInputInterrupt(m_AxisID,GenInter,FALSE,LevelLow,INPUT);
}
//IN1是否有效
int	CCanMotor::GetIOLevel1(BOOL &BLevel)
{
	int rtn=m_pctrl->ReadInput();
	if(rtn<0)
		return rtn;
	BLevel=(rtn&(1<<0))>0;
	return 1;		
}
//IN2是否有效
int	CCanMotor::GetIOLevel2(BOOL &BLevel)
{
	int rtn=m_pctrl->ReadInput();
	if(rtn<0)
		return rtn;
	BLevel=(rtn&(1<<1))>0;
	return 1;		
}
///////////////
//电机位置操作
//获取绝对位置
int	CCanMotor::GetAbsPos(int &pos)
{
	pos=0;
	int rtn=m_pctrl->ReadMotorPos(m_AxisID,ABSOLUTE_POS);
	if(rtn>=0)
	{
		pos=m_pctrl->m_ABSPos[m_AxisID-1];
		pos-=ENCODE_STAR;
	}
	return rtn;
}
//设置绝对位置
int	CCanMotor::SetAbsPos(int pos)
{
	pos+=ENCODE_STAR;
	return m_pctrl->WriteMotorPos(m_AxisID,ABSOLUTE_POS,pos);
}
///////////////
//电机状态查询
//获取当前速度
int	CCanMotor::GetSpeed()
{
	return m_pctrl->ReadMotorSpeed(m_AxisID);
}
//查询是否运行中
int	CCanMotor::IsRunning()
{
	return m_pctrl->ReadMotorRunning(m_AxisID);
}
///////////////
//电机运动功能
//锁轴/松轴
int	CCanMotor::LockMotor(BOOL BLock,BOOL BALL)
{
	if(BALL)
		return m_pctrl->LockMotor(BLock,0,BALL);
	else
		return m_pctrl->LockMotor(BLock,m_AxisID,BALL);
}
//持续运动
int	CCanMotor::MoveContinue(UCHAR group,DIR_TYPE DIR)	
{
	FASSERT(group<10);
	if(IsMotorBusy())
		return ERROR_MOVE_BUSY;
	MoveInfo moveinfo;
	moveinfo.Mode1=MOVE_CONTINUE;
	moveinfo.Group1=group;
	moveinfo.DIR1=DIR;
	moveinfo.POS1=RELATIVE_POS;
	int rtn=m_pctrl->WriteMotorParam(m_AxisID,moveinfo);
	if(rtn<0)
		return rtn;	
	return m_pctrl->RunMotor(m_AxisID,m_SN);	
}
//运行相对距离
int	CCanMotor::MoveRelative(UCHAR group,DIR_TYPE DIR)
{
	FASSERT(group<10);
	if(IsMotorBusy())
		return ERROR_MOVE_BUSY;
	MoveInfo moveinfo;
	moveinfo.Group1=group;
	moveinfo.DIR1=DIR;
	moveinfo.POS1=RELATIVE_POS;
	int rtn=m_pctrl->WriteMotorParam(m_AxisID,moveinfo);
	if(rtn<0)
		return rtn;	
	return m_pctrl->RunMotor(m_AxisID,m_SN);	
}

//运行两段相对距离
int	CCanMotor::MoveRelative(UCHAR group1,UCHAR group2,DIR_TYPE DIR)
{
	FASSERT(group1<10);
	FASSERT(group2<10);

	if(IsMotorBusy())
		return ERROR_MOVE_BUSY;
	MoveInfo moveinfo;
	moveinfo.Group1=group1;
	moveinfo.DIR1=DIR;
	moveinfo.POS1=RELATIVE_POS;
	moveinfo.BContinueMove=TRUE;
	moveinfo.Group2=group2;
	moveinfo.DIR2=DIR;
	moveinfo.POS2=RELATIVE_POS;
	int rtn=m_pctrl->WriteMotorParam(m_AxisID,moveinfo);
	if(rtn<0)
		return rtn;
	return m_pctrl->RunMotor(m_AxisID,m_SN);	

}


//运行绝对距离
int	CCanMotor::MoveABS(UCHAR group)
{
	FASSERT(group<10);
	if(IsMotorBusy())
		return ERROR_MOVE_BUSY;
	MoveInfo moveinfo;
	moveinfo.Group1=group;
	moveinfo.POS1=ABSOLUTE_POS;
	int rtn=m_pctrl->WriteMotorParam(m_AxisID,moveinfo);
	if(rtn<0)
		return rtn;	
	return m_pctrl->RunMotor(m_AxisID,m_SN);	
}

//运行编码器距离
int	CCanMotor::MoveEncode(UCHAR group)
{
	FASSERT(group<10);
	if(IsMotorBusy())
		return ERROR_MOVE_BUSY;
	MoveInfo moveinfo;
	moveinfo.Group1=group;
	moveinfo.POS1=ENCODE_POS;
	int rtn=m_pctrl->WriteMotorParam(m_AxisID,moveinfo);
	if(rtn<0)
		return rtn;	
	return m_pctrl->RunMotor(m_AxisID,m_SN);	
}
//停止电机
int	CCanMotor::Stop()
{
	int rtn=m_pctrl->StopMotor(m_AxisID,m_SN);	
	if(rtn<0)
		return rtn;	
	FinishSemp.SetEvent();
	return 1;
}
//紧急停止电机
int	CCanMotor::UrgentStopMotor()
{
	return m_pctrl->UrgentStopMotor(m_AxisID,m_SN);	
}
//等待运动完成
int	CCanMotor::WaitMotorFinish(int time,NotifyInfo &notify)
{
	if(wxSEMA_NO_ERROR==FinishSemp.WaitTimeout(time))
	{
		notify=m_pctrl->m_RcvNotify[m_AxisID-1];
		return 1;
	}
	else 
		return ERROR_MOVE_TIMEOUT;
}
//等待运动离开HOME
int	CCanMotor::WaitLeaveHome(int time,NotifyInfo &notify)
{
	int ncount=time/200;
	for(int t=0;t<ncount;t++)
	{
		if(wxSEMA_NO_ERROR==FinishSemp.WaitTimeout(200))
		{
			notify=m_pctrl->m_RcvNotify[m_AxisID-1];
#ifndef _SIMULATOR_
			if(notify.BFinishNotify)
				return ERROR_MOVE_HOMEERR;
			if(notify.BLeftLimitNotify)
				return ERROR_MOVE_LEFTLIMITERR;
			if(notify.BRightLimitNotify)
				return ERROR_MOVE_RIGHTLIMITERR;
			if(notify.BLiqidNotify)
				return ERROR_MOVE_LIQUIDERR;	
#endif			
		}
		BOOL BOrg;
		int irtn=GetMotorOrg(BOrg);	
		if(irtn<0)
			return irtn;
		if(!BOrg)
		{
			return Stop();
		}
	}
	return ERROR_MOVE_TIMEOUT;
}

///////////////
//电机设定运动参数
//设置运动参数
int	CCanMotor::WriteActParam(UCHAR group,UINT SpeedStar,UINT SpeedEnd,int distence,UINT AccTm,POS_TYPE type)
{
	FASSERT(group<10);
	int rtn=0;
	if((ENCODE_POS==type)||(ABSOLUTE_POS==type))
		distence+=ENCODE_STAR;
	if((RELATIVE_POS==type)&&(distence<1))
		distence=2;
	PosInfo posinfo;
	posinfo.POS=distence;
	posinfo.Group=group;
	rtn=m_pctrl->WriteMotorParam(m_AxisID,posinfo,type);
	if(rtn<0)
		return rtn;	
	SpeedInfo StarInfo;
	StarInfo.Speed=SpeedStar;
	StarInfo.Group=group;
	rtn=m_pctrl->WriteMotorParam(m_AxisID,StarInfo,FALSE);
	if(rtn<0)
		return rtn;
	SpeedInfo EndInfo;
	EndInfo.Speed=SpeedEnd;
	EndInfo.Group=group;
	rtn=m_pctrl->WriteMotorParam(m_AxisID,EndInfo,TRUE);
	if(rtn<0)
		return rtn;	
	AccInfo Acc;
	Acc.Acc=AccTm;
	Acc.Group=group;
	return m_pctrl->WriteMotorParam(m_AxisID,Acc);
}
//设置运动参数
int	CCanMotor::WriteEncodeActPos(UCHAR group,int distence)
{
	FASSERT(group<10);
	distence+=ENCODE_STAR;
	PosInfo posinfo;
	posinfo.POS=distence;
	posinfo.Group=group;
	return m_pctrl->WriteMotorParam(m_AxisID,posinfo,ENCODE_POS);
}

//设置运动参数
int	CCanMotor::WriteActParam(UCHAR group,int distence,POS_TYPE type)
{
	FASSERT(group<10);
	if((RELATIVE_POS==type)&&(distence<1))
		distence=2;
	PosInfo posinfo;
	posinfo.POS=distence;
	posinfo.Group=group;
	return m_pctrl->WriteMotorParam(m_AxisID,posinfo,type);
}
//设置通用IO
//outPutIndex脚输出BHIGH电平
int CCanMotor::WriteOutput(UCHAR outPutIndex,BOOL BHIGH,UINT WaitTM)
{
	return m_pctrl->WriteOutput(outPutIndex,BHIGH,WaitTM);
}

int CCanMotor::WriteAllOutPut(UCHAR outPutIndex,BOOL BHIGH,UINT WaitTM)
{
	UCHAR ctrlID=(outPutIndex-1)/4+1;
	UCHAR IOBit=(outPutIndex-1)%4;
	return m_pctrl->WriteAllOutput(ctrlID,IOBit,BHIGH,WaitTM);
}
//收到电机停止的通知
int	CCanMotor::CallBackActFinish(NotifyInfo notify)
{
	FinishSemp.SetEvent();
	return 1;
}

//只收到IO通知
int	CCanMotor::CallBackRcvNotify(NotifyInfo notify)
{
	return 1;
}
int CCanMotor::IsMotorBusy()
{
	if(FinishSemp.m_BVaild)
		return 0;
	else 
	{
		//CanTraceLog(_T("ERROR:IsMotorBusy"));
		return 1;
	}
}
BOOL CCanMotor::IsControlBusy()
{
	for(int i=0;i<3;i++)
	{
		if((NULL!=m_pctrl->m_pMotor[i])&&(m_pctrl->m_pMotor[i]->IsMotorBusy()))
			return TRUE;
	}
	return FALSE;
}

///////////////
//编码器接口
//设置编码器线数
int CCanMotor::WriteEncodeCount(UINT count)
{
	return m_pctrl->WriteEncodeCount(m_AxisID,count);
}
//获取编码器位置
int	CCanMotor::GetEncodePos(int &pos)
{
	pos=0;
	int rtn=m_pctrl->ReadMotorPos(m_AxisID,ENCODE_POS);
	if(rtn>=0)
	{
		pos=m_pctrl->m_EncodePos[m_AxisID-1];
		pos-=ENCODE_STAR;
	}
	return rtn;
}
//设置编码器位置
int	CCanMotor::SetEncodePos(int pos)
{
	pos+=ENCODE_STAR;
	return m_pctrl->WriteMotorPos(m_AxisID,ENCODE_POS,pos);
}


int	CCanMotor::GetMotorOrgFromValue(BOOL &BVaild,int InputValue)
{
	int bit[]={11,8,5};
	BVaild=(InputValue&(1<<bit[(m_AxisID-1)]))>0;
	return 1;		
}
//左限位光耦是否有效
int	CCanMotor::GetMotorLeftLimitFromValue(BOOL &BVaild,int InputValue)
{
	int bit[]={10,7,4};
	BVaild=(InputValue&(1<<bit[(m_AxisID-1)]))>0;
	return 1;		
}
//右限位光耦是否有效
int	CCanMotor::GetMotorRightLimitFromValue(BOOL &BVaild,int InputValue)
{
	int bit[]={9,6,3};

	BVaild=(InputValue&(1<<bit[(m_AxisID-1)]))>0;
	return 1;		
}
//液位光耦是否有效
int	CCanMotor::GetMotorLiquidFromValue(BOOL &BVaild,int InputValue)
{
	BVaild=(InputValue&(1<<2))>0;
	return 1;		
}

//IN1是否有效
int	CCanMotor::GetIOLevel1FromValue(BOOL &BLevel,int InputValue)
{

	BLevel=(InputValue&(1<<0))>0;
	return 1;		
}
//IN2是否有效
int	CCanMotor::GetIOLevel2FromValue(BOOL &BLevel,int InputValue)
{

	BLevel=(InputValue&(1<<1))>0;
	return 1;		
}