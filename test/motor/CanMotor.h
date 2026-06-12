// CanMotor.h: interface for the CCanMotor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANMOTOR_H__1240947D_FE80_413C_AEDD_8891AAB94A9B__INCLUDED_)
#define AFX_CANMOTOR_H__1240947D_FE80_413C_AEDD_8891AAB94A9B__INCLUDED_

#include "../can/CanControl.h"
#include "../comon.h"

//////////////////////////////////////////////////////////////////////
// class CCanMotor
//////////////////////////////////////////////////////////////////////
#define	ENCODE_STAR				2000000
#define	ENCODE_MAX				6000000
#define	ENCODE_MIN				-1500000

class CCanMotor
{
public:
friend class CCanControl;
	CCanMotor(UCHAR CtrID,UCHAR axisID);
	virtual ~CCanMotor();
	CCanControl *GetControl()
	{
		return m_pctrl;
	}
	virtual BOOL	IsControlBusy();
	UCHAR GetCtrlID()
	{
		return m_CtrID;
	}
	UCHAR GetAxisID()
	{
		return m_AxisID;
	}
	virtual void InitMotorParam()=0;
	virtual int	InitMove()=0;
	virtual int	SetMotorDefaultParam()=0;
	///////////////
	//电机参数设置
	int	SetMotorPendEnable(BOOL able);							//设置FINISH延时处理	
	int	SetMotorPwmType(PWM_TYPE type);						//设置脉冲类型
	int	SetMotorDriver(Current_TYPE current,SubDiv_TYPE subdiv,BOOL BModeA1_6);	//设置电机细分，电流
	int	SetMotorLockCurrent(BOOL BEnalbe,Current_TYPE current);		//设置电机锁定电流
	int	SetMotorLiquidDec(UINT DEC);							//设置液位减速度
	int	SetMotorUrgentStopDec(UINT DEC);					//设置急停减速度,UrgentStopMotor用
	int	SetMotorAccType(ACC_TYPE AccType=ACC_SPEED);		//设置加速度方式:脉冲/秒*秒，加速时间
	int	SetMotorStopNotify(BOOL BOrgStop,BOOL BLeftLimitStop,BOOL BRightLimitStop,BOOL BFinishStop,BOOL BLiquidStop);				//设置轴位移完成后通知
	int	SetMotorStopNotifyEX(BOOL BEncodeLimit,BOOL BEncodeZ,BOOL BFivePos);				//设置轴位移完成后通知
	///////////////
	//电机光耦功能设置
	int	SetMotorRightLimitLevel(Level_TYPE level);			//设置右限位有效电平
	int	SetMotorLeftLimitLevel(Level_TYPE level);				//设置左限位有效电平
	int	SetMotorOrgLevel(Level_TYPE level);					//设置原点光耦有效电平
	int	GetMotorOrg(BOOL &BVaild);							//原点光耦是否有效
	int	GetMotorLeftLimit(BOOL &BVaild);						//左限位光耦是否有效
	int	GetMotorRightLimit(BOOL &BVaild);					//右限位光耦是否有效
	int	GetMotorLiquid(BOOL &BVaild);							//液位光耦是否有效

	int	GetMotorOrgFromValue(BOOL &BVaild,int InputValue );		//原点光耦是否有效
	int	GetMotorLeftLimitFromValue(BOOL &BVaild,int InputValue );						//左限位光耦是否有效
	int	GetMotorRightLimitFromValue(BOOL &BVaild,int InputValue );					//右限位光耦是否有效
	int	GetMotorLiquidFromValue(BOOL &BVaild,int InputValue );							//液位光耦是否有效

	int	GetIOLevel1FromValue(BOOL &BLevel,int InputValue );							//IN1是否有效
	int	GetIOLevel2FromValue(BOOL &BLevel,int InputValue );							//IN2是否有效

	int	EmptyMotorStop(BOOL BSend=FALSE);					//清空停止使能
	int	EnableEncodeZStop(BOOL BEncodeZ=FALSE);				//设置编码器Z相停止
	int	EnableMotorOrgStop(BOOL BOrgStop=FALSE);			//设置轴遇原点停止
	int	EnableMotorLimitStop(BOOL BLimitStop=FALSE);		//设置轴遇限位停
	int	EnableMotorLiquidStop(BOOL BLiquidStop=FALSE);		//设置轴遇液位停
	int	SetOrgAct(Enable_STOP_NOW stop,Enable_ORG_RESETABSPOS reset,Enable_STOP_NOW limit);

	//通用IO中断设置
	int	EmptyMotorIOInter(BOOL BAllIO=TRUE,BOOL BSend=FALSE,Level_TYPE InterLevel=LevelHigh);			//清空该轴附带的IO口中断
	int	SetMotorOrgInter(BOOL BInterrupt=FALSE,Level_TYPE InterLevel=LevelHigh);		//设置轴原点IO口中断
	int	SetMotorLeftLimitInter(BOOL BInterrupt=FALSE,Level_TYPE InterLevel=LevelHigh);	//设置轴左限位IO口中断
	int	SetMotorRightLimitInter(BOOL BInterrupt=FALSE,Level_TYPE InterLevel=LevelHigh);	//设置轴右限位IO口中断
	int	SetGenIOInter(UCHAR INPUT,BOOL BInterrupt=FALSE,Level_TYPE InterLevel=LevelHigh);		//设置通用输入IN1-IN3IO口中断
	int	DisableGenIOInter(UCHAR INPUT);		//禁用通用输入IN1-IN3IO口中断
	int	EnableMotorBarcodeStop(BOOL BBarcodeStop=FALSE,BOOL BSend=TRUE);		//设置轴遇条码5点停止
	int	GetIOLevel1(BOOL &BLevel);							//IN1是否有效
	int	GetIOLevel2(BOOL &BLevel);							//IN2是否有效
	///////////////
	//电机位置操作
	int	GetAbsPos(int &pos);								//获取绝对位置
	int	SetAbsPos(int pos);								//设置绝对位置
	///////////////
	//电机状态查询
	int	GetSpeed();										//获取当前速度
	int	IsRunning();										//查询是否运行中
	///////////////
	//电机运动功能
	int	LockMotor(BOOL BLock=TRUE,BOOL BALL=FALSE);		//锁轴/松轴
	int	MoveContinue(UCHAR group,DIR_TYPE DIR=DIR_CW);	//持续运动
	int	MoveRelative(UCHAR group,DIR_TYPE DIR=DIR_CW);	//运行相对距离
	int	MoveRelative(UCHAR group1,UCHAR group2,DIR_TYPE DIR);	//运行两段相对距离
	int	MoveABS(UCHAR group);							//运行绝对距离
	int	MoveEncode(UCHAR group);						//运行编码器距离
	int	Stop();												//停止电机
	int	UrgentStopMotor();								//紧急停止电机
	int	WaitMotorFinish(int time,NotifyInfo &notify);			//等待运动完成
	int	WaitLeaveHome(int time,NotifyInfo &notify);			//等待运动离开HOME
	///////////////
	//电机设定运动参数
	int	WriteActParam(UCHAR group,UINT SpeedStar,UINT SpeedEnd,int distence,UINT AccTm,POS_TYPE type);	//设置运动参数
	int	WriteActParam(UCHAR group,int distence,POS_TYPE type);	//设置运动参数
	int	WriteEncodeActPos(UCHAR group,int distence);			//设置运动参数
	//设置通用IO
	int WriteOutput(UCHAR outPutIndex,BOOL BHIGH=TRUE,UINT WaitTM=0);		//outPutIndex脚输出BHIGH电平
	//编码器接口
	int WriteEncodeCount(UINT count);						//设置编码器线数
	int	GetEncodePos(int &pos);							//获取编码器位置
	int	SetEncodePos(int pos);							//设置编码器位置

	int WriteAllOutPut(UCHAR outPutIndex,BOOL BHIGH=TRUE,UINT WaitTM=1000);	//控制所有控制器输出  从1开始

protected:
	virtual int	CallBackActFinish(NotifyInfo notify);		//收到电机停止的通知
	virtual int	CallBackRcvNotify(NotifyInfo notify);		//只收到IO通知
	virtual int IsMotorBusy();

	CCanControl	*m_pctrl;
	UCHAR		m_CtrID;
	UCHAR		m_AxisID;
	CxwEvent	 FinishSemp;						//表示动作完成的事件
	UCHAR		 m_SN;
  
};

#endif // !defined(AFX_CANMOTOR_H__1240947D_FE80_413C_AEDD_8891AAB94A9B__INCLUDED_)
