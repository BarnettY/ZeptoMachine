//---------------------------磁棒电机---------------------------------
#pragma once
#include "BasicMotor.h"

enum MagneticBarPos
{
	POS_MagneticBar_UNKNOW=-1,
	POS_MagneticBar_HOME,
	POS_MagneticBar_Idle,
	POS_MagneticBar_DOWN,
	POS_MagneticBar_Step,
	POS_MagneticBar_LiquidTop,
    POS_MagneticBar_Up,
};
class CMagneticBarMotor:public CBasicMotor
{
public:
	CMagneticBarMotor(UCHAR CtrID,UCHAR axisID);
	virtual ~CMagneticBarMotor();
	int  InitHome();  // 初始化回零
	int  GoToHome(UINT WaitTM =WAITTIME); //运动过程回零
	int  MoveIdlePos(UINT WaitTM =WAITTIME); // 空闲位置， 下降到Z轴（Z轴原点）底部，
    int  MoveDown(int &MoveStep,BYTE HolePos=1,  UINT WaitTM =WAITTIME,BYTE Mark =0); // 下降到底
	int  MoveDownStep(int &MoveStep,float Height=2 ,UINT WaitTM =WAITTIME,BYTE Mark =0); //下降脉冲
	int  MoveToLiquidTop(int &MoveStep,BYTE Type , BYTE HolePos=1,int Volume=10000, BOOL bFast = TRUE ,UINT WaitTM =WAITTIME,BYTE Mark =0); // 不同的类型液面高度。TOP位置不同。
	int  MoveDownSlow(int &MoveStep,BOOL bSegment=FALSE, float height=0,  UINT WaitTM =WAITTIME,BYTE Mark =0,BYTE suckLevel=10); // 缓慢下降吸磁
	int  MoveUpSlow(int &MoveStep,float height, UINT WaitTM =WAITTIME,BYTE Mark =0, int SpeedLevel=10);
	float GetHeightByVolumeInHolePos(BYTE Type ,BYTE Pos,int Volume);
	virtual int DebugMotor(BYTE act,int param,int relatepos=0);		//调试马达电机
    void SetDirDown(DIR_TYPE dir);
public:
	MagneticBarPos  m_MotorPos ;
 	int m_SuckmagneticLeftStep;
	int m_PerMMToPluse ;//每mm 对应脉冲
};