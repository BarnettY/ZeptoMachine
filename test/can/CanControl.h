// CanControl.h: interface for the CCanControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANCONTROL_H__2595EC1D_C506_410C_BD92_DF8E0C2516A7__INCLUDED_)
#define AFX_CANCONTROL_H__2595EC1D_C506_410C_BD92_DF8E0C2516A7__INCLUDED_



#include "AxisParamInfo.h"
#include "CanInterface.h"

//////////////////////////////////////////////////////////////////////
// class CCanControl
//////////////////////////////////////////////////////////////////////

class CCanMotor;
class CCanCom;

#define FINISH_ORG		1
#define FINISH_LEFT		2
#define FINISH_RIGHT		4
#define FINISH_LIQ		8
#define FINISH_STEP		16

enum LIMIT_EN_TYPE
{
	EmptyStop=0,
	OrgStop,
	LimitStop,
	EncodeZStop,
	LiquidStop,
	BarcodeStop
};

enum IO_INTER_TYPE
{
	EmptyInter=0,
	EmptyAllInter,
	OrgInter,
	LeftLimitInter,
	RightLimitInter,
	GenInter,
};

enum LIMIT_LEVEL_TYPE
{
	OrgLevel=0,
	LeftLimitLevel,
	RightLimitLevel,
};

enum  IO_ALL
{
	GenInter1=0,
    GenInter2,
	LiquidInter,
	RightLimitInter3,
	LeftLimitInter3,
	OrgInter3,
	RightLimitInter2,
	LeftLimitInter2,
	OrgInter2,
    RightLimitInter1,
	LeftLimitInter1,
	OrgInter1,
};
class CCanControl
{
public:
#ifdef _SIMULATOR_
	BOOL SimulateCallBack(BYTE AxisID);
	BOOL SimulateCallBack(UINT MASK,CCanMotor *pMotor1);
	BOOL CallBackMotor(BYTE AxisID);
#endif
friend class CCanMotor;
friend class CCanCom;
	void SetListen(UCHAR axis,BOOL BListin=TRUE)
	{
		FASSERT(axis>0);
		FASSERT(axis<=3);
		m_MotorListen[axis-1]=BListin;
	}

	virtual ~CCanControl();
	void SetChildMotor(UCHAR index,CCanMotor *pMotor);
	CCanMotor *GetChildMotor(UCHAR index);
	int ParseNotifyBuffer(FrameIDInfo frame,UCHAR *buff,UINT Len);
	///////////////////////////
	//读操作
	int ReadVersion(UINT WaitTM=2000);
	int ReadBaud(UINT WaitTM=2000);
	int ReadInputValid(UINT WaitTM=2000);			//读取输入的有效电平
	int ReadInput(UINT WaitTM=2000);				//读取全部输入是否有效
	int ReadInputEnable(UINT WaitTM=2000);		//读取输入使能
	int ReadOutput(UINT WaitTM=2000);				//读取全部输出高低电平
	int ReadOrgAcion(UINT WaitTM=2000);			//读取原点对应动作
	int ReadMotorPos(UCHAR axis,POS_TYPE postype,UINT WaitTM=2000);	//读取当前位置
	int ReadMotorSpeed(UCHAR axis,UINT WaitTM=2000);	//读取速度
	int ReadMotorParam(UCHAR axis,PosInfo &pos,POS_TYPE postype,UINT WaitTM=2000);
	int ReadMotorParam(UCHAR axis,SpeedInfo &speed,BOOL BHigh,UINT WaitTM=2000);
	int ReadMotorParam(UCHAR axis,AccInfo &ACC,UINT WaitTM=2000);
	int ReadMotorRunning(UCHAR axis,UINT WaitTM=2000);		//查询电机是否运行中
	int ReadLiqDec(UCHAR axis,UINT WaitTM=2000);				//查询液位减速度
	int ReadStopDec(UCHAR axis,UINT WaitTM=2000);						//查询急停减速度
	int ReadNotifyInfo(UCHAR axis,UINT WaitTM=2000);					//查询通知信号有效位
	int ReadPWMType(UCHAR axis,UINT WaitTM=2000);					//查询指定站点指定轴的脉冲模式
	int ReadAllParam(UINT WaitTM=2000);							//查询全部参数
	int ReadEncodeCount(UCHAR axis,UINT WaitTM=2000);			//查询编码器线数
	int ReadEncodeErrorLimit(UCHAR axis,UINT WaitTM=2000);		//查询编码器超差
	int ReadEncodeDir(UCHAR axis,UINT WaitTM=2000);				//查询编码器方向
	int ReadMotorCircleCount(UCHAR axis,UINT WaitTM=2000);		//查询电机一圈步进
	int GetInputValid(IO_ALL Index ,BOOL& BVaild); // 取IO输入信号
	//通用IO中断
	int WriteInputInterrupt(UCHAR axis,IO_INTER_TYPE type,BOOL enable,Level_TYPE InterLevel,UCHAR INPUT=0,BOOL send=TRUE,UINT WaitTM=2000);		//设置IO中断触发电平
	//写操作
	int WriteBaud(Baud_TYPE Baud,UINT WaitTM=2000);
	int WriteDriverInfo(UCHAR axis,AxisDriverInfo drive,UINT WaitTM=2000);
	int LockMotor(BOOL BLock=TRUE,UCHAR axis=0,BOOL BALL=TRUE,UINT WaitTM=2000);
	int WriteLockCurrent(UCHAR axis,LockCurrentInfo lock,UINT WaitTM=2000);
	int WriteInputValid(UINT WaitTM=2000);
	int WriteLimitValid(UCHAR axis,LimitOrgLevel &input,UINT WaitTM=2000);
	int WriteLimitOrgLevel(UCHAR axis,LIMIT_LEVEL_TYPE type,Level_TYPE level,UINT WaitTM=2000);
	int WriteLimitOrgEnable(UCHAR axis,LIMIT_EN_TYPE type,BOOL enable,BOOL send=TRUE,UINT WaitTM=2000);
	int WriteOrgAcion(OrgActionInfo	Action,UINT WaitTM=2000);				//读取原点对应动作
	int WriteOutput(UCHAR outPutIndex,BOOL BHIGH=TRUE,UINT WaitTM=2000);		//outPutIndex脚输出BHIGH电平
	int WriteAllOutput(UCHAR ctrlID,UCHAR outPutIndex,BOOL BHIGH=TRUE,UINT WaitTM=2000);
	int WriteDefaultOutput(UINT WaitTM=2000);								//outPutIndex脚输出BHIGH电平
	int WriteMotorPos(UCHAR axis,POS_TYPE postype,int POS,UINT WaitTM=2000);	//设置当前位置
	int WriteMotorParam(UCHAR axis,PosInfo pos,POS_TYPE postype,UINT WaitTM=2000);
	int WriteMotorParam(UCHAR axis,SpeedInfo speed,BOOL BHigh,UINT WaitTM=2000);
	int WriteMotorParam(UCHAR axis,AccInfo ACC,UINT WaitTM=2000);
	int WriteMotorParam(UCHAR axis,MoveInfo Mode,UINT WaitTM=2000);
	int RunMotor(UCHAR axis,UCHAR &SN,UINT WaitTM=2000);
	int StopMotor(UCHAR axis,UCHAR &SN,UINT WaitTM=2000);
	int UrgentStopMotor(UCHAR axis,UCHAR &SN,UINT WaitTM=2000);
	int WriteLiquidDec(UCHAR axis,UINT DEC,UINT WaitTM=2000);
	int WriteUrgentStopDec(UCHAR axis,UINT DEC,UINT WaitTM=2000);							//设置急停减速度
	int WriteNotifyInfo(UCHAR axis,NotifyInfo notify,UINT WaitTM=2000);					//设置通知信号有效位
	int WritePWMType(UCHAR axis,PWM_TYPE type,UINT WaitTM=2000);					//设置指定站点指定轴的脉冲模式
	int WriteAllParam(UINT WaitTM=2000);											//设置全部参数
	int DisAllMotor(UINT WaitTM=2000);												//所有轴驱动电路禁止
	int ResetAllMotorPos(UINT WaitTM=2000);										//所有站点所有轴当前位置为原点
	int WriteEncodeCount(UCHAR axis,UINT count,UINT WaitTM=2000);					//设置编码器线数
	int WriteEncodeErrorLimit(UCHAR axis,UINT count,UINT WaitTM=2000);				//设置编码器超差
	int WriteEncodeDir(UCHAR axis,UINT DIR,UINT WaitTM=2000);						//设置编码器方向
	int WriteMotorCircleCount(UCHAR axis,UINT count,UINT WaitTM=2000);				//设置电机一圈步进
protected:
	CCanControl(UINT ctrlID);
	int SendCanAndWait(FrameIDInfo frame,UCHAR *buff,int len,UINT WaitTM=2000);
	int ParseWriteBuffer(FrameIDInfo frame,UCHAR *buff,UINT Len);
	int ParseReadBuffer(FrameIDInfo frame,UCHAR *buff,UINT Len);
	///////////////////////////
	int PackageBuffer(UINT date,int len,UCHAR *buff);
	int ReadBuffer(UINT &date,int len,UCHAR *buff);
	int PackageBuffer(UCHAR *buff);
	int ReadBuffer(UCHAR *buff);
	int DealPend(WORD RCVID);
	int DealFinish(UCHAR AxisID);
	CCanMotor *m_pMotor[3];
public:
	UINT		m_ControlID;		//_2	拨码
	UINT		m_ControlVer;		//_4	版本
	Baud_TYPE	m_Baud;			//_2	波特率
	AxisDriverInfo m_DriverInfo[3];		//_4x3	细分及电流
	LockCurrentInfo m_LockInfo[3];		//_2x3	锁定时候的衰减
	LimitOrgLevel	m_LimitLevel;		//_4		原点限位有效电平
	LimitOrgEnableInfo m_LimitOrgEnable;		//_2		输入停止使能
	OrgActionInfo	m_OrgAction;		//_4		原点对应动作
	AxisParamInfo m_Param[3];			//_240x2+200	//10组运动参数
	UINT		m_LiqDec[3];			//_4		碰到液位的减速度
	UINT		m_StopDec[3];		//_4		急停减速度
	NotifyInfo 	m_Notify[3];			//_4		原点限位停止通知
	PWM_TYPE 	m_PWMType[3];		//_2		脉冲类型
	UINT		m_XEncodeCnt;		//_4		x轴编码器线数
	UINT		m_YEncodeCnt;		//_4		Y轴编码器线数

	//零时变量
	UINT		m_OutPutLevel;			//当前输出电平
	int			m_ABSPos[3];			//当前位置
	int			m_EncodePos[3];			//当前编码器
	UINT		m_CurrentSpeed[3];		//当前速度
	MoveInfo		m_MoveMode[3];			//运动模式
	volatile  BOOL		m_MotorRunning[3];		//是否正在运行
	NotifyInfo 	m_RcvNotify[3];			//刚收到的通知
	InputValidLevel m_InputLevel;			//_4		输入有效电平,运行时不改动
	InputValidLevel m_InputInterruptLevel;		//_4		输入触发电平,运行时不改动
	InputInterEnableInfo m_InputInterruptEnable;	//_2		输入停止使能
protected:
	volatile  BOOL		m_MotorListen[3];		//马达是否在监听
	volatile  WORD	m_PendFinishID[3];		//CAN已经收到FINISH信号		
	volatile  BOOL		m_BNeedPendFinish[3];	//已经收到FINISH信号后延时处理			
	wxCriticalSection AxisMute0;				//0号公共轴锁
};
#endif // !defined(AFX_CANCONTROL_H__2595EC1D_C506_410C_BD92_DF8E0C2516A7__INCLUDED_)
