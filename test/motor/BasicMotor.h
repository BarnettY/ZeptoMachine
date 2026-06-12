// BasicMotor.h: interface for the CBasicMotor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASICMOTOR_H__3ED52DA2_B005_45FE_82D3_352468967E9C__INCLUDED_)
#define AFX_BASICMOTOR_H__3ED52DA2_B005_45FE_82D3_352468967E9C__INCLUDED_

#include "CanMotor.h"
//光耦配置
struct MotorInputParam
{
	MotorInputParam()
	{
		levelOrg=LevelLow;
		levelLeftLimit=LevelLow;
		levelRightLimit=LevelLow;
		levelLiq=LevelLow;
		ableOrg=Disable;
		ableLimit=Disable;
		ableLiq=Disable;
		OrgAction=StopNow;
		OrgResetAbs=OrgNotResetAbsPos;
		LimitAction=StopNow;
	}

	Level_TYPE levelOrg;						//原点有效电平
	Level_TYPE levelLeftLimit;
	Level_TYPE levelRightLimit;
	Level_TYPE levelLiq;
	Enable_TYPE ableOrg;						//原点停机使能
	Enable_TYPE ableLimit;
	Enable_TYPE ableLiq;
	Enable_STOP_NOW OrgAction;				//原点是否急停
	Enable_ORG_RESETABSPOS OrgResetAbs;		//原点是否清除绝对位置
	Enable_STOP_NOW LimitAction;				//限位是否急停
};
//电流细分配置
struct MotorDriveParam
{
	MotorDriveParam()
	{
		subDiv=Sub_16;
		current=A0_2;
		ableLockCurrent=Enable;
		lockCurrent=A0_15;
		decLiquid=500*1000;
		decUrgentStop=1*1000*1000;
		BModeA1_6=TRUE;
	}
	SubDiv_TYPE	subDiv;				//细分数目
	Current_TYPE	current;				//电流大小
	Enable_TYPE	ableLockCurrent;		//待机小电流使能
	Current_TYPE	lockCurrent;			//待机电流大小
	UINT		decLiquid;			//液位减速度
	UINT		decUrgentStop;		//急停减速度
	BOOL		BModeA1_6;			//开启1.6A模式，另一种是3.2A模式
};
//电机运动参数
struct MotorActParam
{
	MotorActParam()
	{
		type=RELATIVE_POS;
		step=1000;
		speedStar=1000;
		speedEnd=3000;
		ACCDec=0;					//默认无效
	}
	BOOL IsVaild()
	{
		return ACCDec>0;
	}
	POS_TYPE	type;
	int			step;
	UINT		speedStar;
	UINT		speedEnd;
	UINT		ACCDec;
};
//参数类型
enum ACT_TYPE
{
	ACT_TYPE1=0,
	ACT_TYPE2,
	ACT_TYPE3,
	ACT_TYPE4,
	ACT_TYPE5,
	ACT_TYPE6,
	ACT_TYPE7,
	ACT_TYPE8,
	ACT_TYPE9,
	ACT_TYPE10,
	ACT_NONE,
	ACT_OutTYPE1,
	ACT_OutTYPE2,
	ACT_OutTYPE3,
	ACT_OutTYPE4,
	ACT_OutTYPE5,
	ACT_OutTYPE6,
	ACT_OutTYPE7,
	ACT_OutTYPE8,
	ACT_OutTYPE9,
	ACT_OutTYPE10,
	ACT_OutTYPE11,
	ACT_OutTYPE12,
	ACT_OutTYPE13,
	ACT_OutTYPE14,
	ACT_OutTYPE15,
	ACT_OutTYPE16,
	ACT_OutTYPE17,
	ACT_OutTYPE18,
	ACT_OutTYPE19,
	ACT_OutTNONE
};
#define	OUTPARAMCNT			19
#define ACT_Move				ACT_TYPE1				//正常运行速度参数
//#define ACT_LowSpeed			ACT_TYPE2				//低速运行参数
#define	ACT_InitLeaveHome		ACT_TYPE8				//初始化离开HOME
#define	ACT_InitAfterHome		ACT_TYPE9				//外部参数使用
#define	ACT_InitHome			ACT_TYPE10				//初始化回HOME使用
#define	ACT_Debug				ACT_TYPE9			//只是调试用

#define ZERO_ACC				100000

#define 	DEBUG_ACT_RESET			0
#define 	DEBUG_ACT_MODE			1
#define 	DEBUG_ACT_RUN				2
#define 	DEBUG_ACT_LOCKUNLOCK	3
#define 	DEBUG_ACT_FOCUSPLATE		4

#define	STOP_MODE_ORG			0X1
#define	STOP_MODE_LEVEL			0X2
#define	OUT_INDEX(A)				(A-ACT_OutTYPE1)
class CBasicMotor:public CCanMotor
{
public:
	TCHAR	m_strName[25];
	BYTE	m_LastDebugMode;
	int		m_Offset[10];		//位置位置参数

	int  m_iCurrent ; //电机电流  // 参数文件中获取电流参数  待机和锁机电机  新增
	int  m_iLockCurrent ; //锁机电流  新增

    BYTE  m_subDiv ;// 细分数

	int  m_iSpeed[10] ;  // 运行速度  0 是回零速度  新增
	int  m_iSpeedStart[10] ; // 运行速度 起始值是回零速度 新增


	CBasicMotor(UCHAR CtrID,UCHAR axisID);
	virtual ~CBasicMotor();
	static wxString SpanIncluding(wxString strbar,wxString str);
	BOOL IsMotorDebug()
	{
		return m_LastDebugMode;
	}
	BOOL IsMotorDebugCallback()
	{
		return (0X40&m_LastDebugMode);
	}
	void ResetErr()
	{
		m_LastError=0;
		m_LastDebugMode=0;
	}
	//参数设置
	void SetInputParam(MotorInputParam inputparam)		//设置光耦配置
	{
		m_InputParam=inputparam;
	}
	void SetInputLevel(Level_TYPE org,Level_TYPE left,Level_TYPE right,Level_TYPE liq)
	{
		m_InputParam.levelOrg=org;
		m_InputParam.levelLeftLimit=left;
		m_InputParam.levelRightLimit=right;
		m_InputParam.levelLiq=liq;
	}
	void SetLimitStop(Enable_STOP_NOW limit)
	{
		m_InputParam.LimitAction=limit;
	}

	void SetCurrentParam(MotorDriveParam param)			//设置电流细分配置
	{
		m_MotorParam=param;
	}
	void SetCurrentParamRes(BOOL BModeA1_6)
	{
		if(m_subDiv!=0)
           m_MotorParam.subDiv = (SubDiv_TYPE)(m_subDiv%9);//    设置细分数
		m_MotorParam.BModeA1_6 = BModeA1_6;
        m_MotorParam.current =(Current_TYPE)(m_iCurrent%32) ;
		m_MotorParam.lockCurrent =(Current_TYPE)(m_iLockCurrent%32);

	}
	void	SetInitSpeed(UINT init)							//设置初始化速度
	{
		m_SpeedInit=init;
	}

	virtual int	DebugMotor(BYTE act,int param,int relatepos=0);					//调试马达电机
	virtual int	InitMove();
	virtual int	ApplayChangeParam(UCHAR index)							//应用修改好的参数
	{
		return 0;
	}

	BOOL IsMotorStanby()
	{
		return (0==IsMotorBusy());//FIXME:!IsRunning();
	}
	virtual void InitMotorParam();						//预备设置各动作参数组
	virtual int	SetMotorDefaultParam();					//把各参数组下发到控制器
	int	WriteActParam(ACT_TYPE type,MotorActParam param);			//直接把参数发达控制器
	int	WriteActParam(UCHAR group,int distence,POS_TYPE type);	//设置运动参数
	int	WriteOuterActParam(ACT_TYPE type,ACT_TYPE outerdef=ACT_InitAfterHome);		//因为是10组外的参数，与ACT_InitAfterHome即时切换
	int	VerifyActParam(ACT_TYPE type,int step);						//检查参数是否与保存的一致
	virtual void SetMotorActParam(ACT_TYPE type,MotorActParam param);		//设置某组运动参数

	void SetMotorParam(ACT_TYPE type,UINT speedStar,UINT speedEnd,int step,UINT ACCDec,POS_TYPE pos_type= RELATIVE_POS);


	virtual int SetMotorActDist(ACT_TYPE type,int distence);		//设置某组运动参数
	///////////////
	//基本运动
	int MoveLimit(UCHAR group,DIR_TYPE dir,UINT WaitTM=WAITTIME);		//运行到HOME
	int MoveHome(UCHAR group,DIR_TYPE dir,UINT WaitTM=WAITTIME);		//运行到HOME
	int LeaveHome(UCHAR group,DIR_TYPE dir,UINT WaitTM=WAITTIME);		//从HOME有效位置运行到非有效位

	int LeaveLeft(UCHAR group,DIR_TYPE dir,UINT WaitTM=WAITTIME);      ////从LEFT有效位置运行到非有效位  有问题 反向不停
	int LeaveRight(UCHAR group,DIR_TYPE dir,UINT WaitTM=WAITTIME);      ////从Right有效位置运行到非有效位 有问题 反向不停

	int MoveLiquid(UCHAR group,DIR_TYPE dir,int &POS,UINT WaitTM=WAITTIME);	//判断液位
	int MoveRelativeAndWait(UCHAR group,DIR_TYPE DIR=DIR_CW,UINT WaitTM=WAITTIME);	//运行相对距离
	int MoveEncodeAndWait(UCHAR group,UINT WaitTM=WAITTIME);						//运行编码器距离

	int InitHome();																				//初始化回原点
	int BackHome(UCHAR group1=ACT_Move,UCHAR group2=ACT_InitHome,int step=0,int distance=100,UINT WaitTM=WAITTIME);	//运行中返回原点

	int MoveToPos(int pos,UCHAR group=ACT_Move,UINT WaitTM=WAITTIME);			//运行到绝对位置

	int MoveToPosRes(int curPos ,int pos,UCHAR group=ACT_Move,UINT WaitTM=WAITTIME);			//运行到绝对位置
	int JogStep(int step,DIR_TYPE DIR,UCHAR group=ACT_Move,UINT WaitTM=WAITTIME); //相对位置

	virtual int	 WaitActFinish(UINT WaitTM=WAITTIME);

    virtual   int WaitStepFinish(UINT WaitTM=WAITTIME);
	virtual   int WaitBackHome(UINT WaitTM=WAITTIME);
    virtual   int WaitRightLimit(UINT WaitTM=WAITTIME);
    virtual   int WaitLeftLimit(UINT WaitTM=WAITTIME);

	//运动后检查
	int NotifyFatherCmdFinish(int param);

	//检查电机停止的必须是步进走完
	int CheckActFinishMustStepFinish(NotifyInfo notify);
	//检查电机停止的必须是右限位
	int CheckActFinishMustRightLimit(NotifyInfo notify);
	//检查电机停止的必须是左限位
	int CheckActFinishMustLeftLimit(NotifyInfo notify);
	//检查电机停止的必须是原点
	int CheckActFinishMustOrg(NotifyInfo notify);
	//检查电机停止的必须是液位
	int CheckActFinishMustLiquid(NotifyInfo notify);


	virtual int	CallBackRcvNotify(NotifyInfo notify);		//只收到IO通知

    int  CheckActFinishMustStepFinishOrLeftLimit(NotifyInfo notify);
   /* int  CheckActFinishMustStepFinishOrLiquid(NotifyInfo notify);*/
	void SendMotorPos();
protected:
	void	 SaveOutParam(ACT_TYPE type,MotorActParam param);//保存10组参数外的运动参数，运动时提前发送

	UINT	m_SpeedInit;
	int	m_LastError;
	ACT_TYPE 		m_LastAct;									//上一个未完成动作

	MotorInputParam		m_InputParam;					//光耦特性参数
	MotorDriveParam		m_MotorParam;					//电流细分参数
	MotorActParam			m_ActParamAry[10];				//使用到的运动参数
	MotorActParam			m_ActOutParamAry[OUTPARAMCNT];			//10组外的运动参数,与ACT_InitAfterHome同一组，即时切换
	DIR_TYPE m_DirDown;
	DIR_TYPE m_DirUp;											//m_DirDown的反方向




};

#define MOTOR_IF_ERROR_QUIT  	if(m_LastError<0) {/*FASSERT(FALSE);*/	TRACE(_T("IF_ERROR_QUIT MotorName=%s, m_LastError=%d"),m_strName,m_LastError);return m_LastError;}
#define PART_IF_ERROR_QUIT  	if(m_LastError<0) {FASSERT(FALSE);	TRACE(_T("IF_ERROR_QUIT MotorName=%s, m_LastError=%d"),m_strName,m_LastError);return m_LastError;}
//#endif

#endif // !defined(AFX_BASICMOTOR_H__3ED52DA2_B005_45FE_82D3_352468967E9C__INCLUDED_)
