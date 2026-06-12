#pragma once
#include "../comm/GeneralPart.h"
#include "../motor/XMotor.h"
#include "../motor/ZMotor.h"
#include "../motor/MagneticBarMotor.h"
#include "../runinfo/ItemList.h"

////////////////////////////////////////////////////////////////////////
// class CPart                动作模块
//////////////////////////////////////////////////////////////////////
class CPart:public GenerPart
{
public:
	CPart();
	virtual ~CPart();
	virtual BOOL DownPartParam();   //下载运动参数
protected:
	virtual void InitMotorParam();	//预备设置各动作参数组

public:
	//---------------------动作---------------------
	int   InitPart(BOOL bCheckStop=TRUE);//  模块初始化
	virtual int WholeAction(ProgramStep Info);
	void  SndActionStarOrEndMsg(BYTE StepID, BYTE Mark,BYTE ActType ,int Time); // 通知命令开始结束
	void  SetPauseEvent(); //暂停事件
	void  ResetPauseEvent();//暂停事件
	int   WaitPauseEventOrStop();
	int   GetLastHolePos(BYTE Type); //获取最后孔位置
	void  OpenOrCloseLamp(BYTE Open);
	void  OpenOrCloseUVLamp(BYTE Open,int Time);
	void  GetLampAndCoverStaues(BYTE &Lamp,BYTE & UVLamp,BYTE& FrontCover,BYTE & Drawer);
	int   PartHome();
	//---------------------磁棒和磁棒套动作---------------

	int  MoveHoleOneMix(BYTE Type, int SpeedLevel, BOOL bHalfMove,DWORD &LastTime);  //孔1混匀，水平加上下运动
	int  MoveHoleOneSuckMagnetic(BYTE Type, int SpeedLevel, BOOL bHalfMove,BOOL bRight);  //孔1吸磁
    int  MoveHalfHoleOneSuckMagnetic(BYTE Type, int SpeedLevel,BOOL bRight); //运动一半孔1吸磁

	int  MoveToHolePos(BYTE Type,BYTE HolePos);// 移动到孔位置
	int  MoveToLiquidTop(BYTE Type,BYTE HolePos,int Volume,BOOL bFast  ,BOOL bZMotorFirst) ;//下降Z轴先

	int  MoveDownHeight(float Height); //孔1中下降吸磁
    int  MoveDown(BYTE HolePos); //孔 快速下降
	int  MoveDownSlow(BOOL bSegment=FALSE, float Height=0,BYTE suckLevel =10); // 其他孔缓慢下降吸磁 ,增加速度

	int  MoveUpSlow(float Height, int SpeedLevel); //慢速上升

	float GetMoveHeight(BYTE Type,BYTE HolePos,int Volume);
	void  SendLampAndCoverStaues();  //发送灯和舱门状态
	BOOL  CreateUVLampTread();
	void  SetSuckMagneticWay(BYTE  SuckMagneticWay);
	void  SetMotionParameters(BYTE HorizontalSpeed,int HorizontalMixSpeed,BYTE HorizontalSuckMagneticSpeed,BYTE UpDownSpeed,int UpDownMixSpeed, BYTE SuckMagneticSpeed );
	void  GetMotionParameters(BYTE &HorizontalSpeed,int &HorizontalMixSpeed,BYTE & HorizontalSuckMagneticSpeed,BYTE &UpDownSpeed,int  &UpDownMixSpeed, BYTE &SuckMagneticSpeed);

	void  SetFactoryDefaultSpeed(); //设置
	void  SaveSpeedParameters(); //保存速度参数
    int   StopMotor(); // 停止电机
	int   PartDebug(UCHAR ACTION); 
	int	  DebugPartMotor(BYTE index,BYTE act,int param);	//调试马达电机
	UINT  UVLamp(); // 紫外灯线程
public:
	CxwEvent	  m_UVLampEvt;  // 紫外灯
	BOOL          m_ThreadExit; //线程是否退出， 0 混匀  1 加样  2 扫描
	CXMotor      *m_pXMotor; // X轴横向移动
	CZMotor      *m_pZMotor; // Z轴升降 磁棒套
	CMagneticBarMotor *m_pMagBarMotor; // 磁棒轴

	BYTE m_SpeedLevel; // 混匀的速度级别  对应 5组数据
	BYTE m_LampStatus;  // 0关  1开
	BYTE m_UVLampStatus; //紫外灯  1开 0关
	DWORD  m_UVTime;  // 紫外灯打开时间

	BYTE m_FrontCoverStatus;  // 0 关闭  1打开
	BYTE m_DrawerStatus ;     // 0 关闭  1打开
   	DWORD m_StateQueryTime ;    // 上次状态查询时间，定时查询
	BYTE  m_SuckMagneticWay ; //吸磁方式
	BYTE  m_NumberOfSegments; //分段吸磁 ，段数
    // 出厂速度设置

	float m_FactoryHorizontalSpeed;    //  水平移动速度
	float m_FactoryHorizontalMixSpeed ;  //	水平混匀高速
	float m_FactoryHorizontalSuckMagneticSpeed ; //	水平移动吸磁速度
	float m_FactoryUpDownSpeed ;//磁棒/磁棒套下降速度
	float m_FactoryUpDownMixSpeed;
	float m_FactorySuckMagneticSpeed ; // 吸磁速度

    int m_StepHeight ;
	int m_SuckmagneticLeftStep ; //吸磁底部到瓶底的脉冲数，
	BOOL m_bAlreadySuckMagnetic ; // 标记已经吸磁 ，如果动作先要 去磁
	int m_ReturnValue;
	int  m_XSegment;
    int m_MixHeight ;   //混匀时，上下幅度
	int m_MixUpDownMark;
	int m_MixUpDownTimes1;  
	int m_MixUpDownTimes2; 
	int m_CompensateValue1;
	int m_CompensateValue2;
	int m_CheckFrontCoverMark ; // 检查舱门标记

private:
	CxwEvent	m_PauseEvent;      // 暂停事件
};

//紫外灯
class CUVLampThread : public wxThread
{
public:
	CUVLampThread(CPart* m_pfather);
	virtual ~CUVLampThread();
	virtual void* Entry();
protected:
	CPart* m_pfather;
};