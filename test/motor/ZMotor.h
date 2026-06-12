

//---------------------------Y轴 57升降电机---------------------------------

#pragma once
#include "BasicMotor.h"

enum ZMotorPos
{
	POS_ZMotor_UNKNOW=-1,
	POS_ZMotor_HOME,
    POS_ZMotor_Down,
	POS_ZMotor_DownFast,
    POS_ZMotor_Top1,
    POS_ZMotor_Top2,
	POS_ZMotor_DownStep,
    POS_ZMotor_Up,

};
class CZMotor:public CBasicMotor
{
public:
	CZMotor(UCHAR CtrID,UCHAR axisID);
	virtual ~CZMotor();
	int  InitHome();  // 初始化回零
	int  GoToHome( UINT WaitTM =WAITTIME); //运动过程回零 
	int  GoToHome1( UINT WaitTM =WAITTIME); /// 为和磁棒运动保持一致

	int  MoveDown(int& MoveStep ,BYTE HolePos=1, UINT WaitTM =WAITTIME,BYTE Mark =0); // 下降到底 ,不同试剂盒的底部一致。 //设置下降速度。
	int  MoveToLiquidTop(int &MoveStep,BYTE Type ,BYTE HolePos=1,int Volume=10000, BOOL bFast = TRUE ,UINT WaitTM =WAITTIME,BYTE Mark =0); //不同的类型TOP位置不同。下降到液面
	
	int  MoveDownStep(int &MoveStep,float Height=2,UINT WaitTM =WAITTIME,BYTE Mark =0); //下降脉冲 //孔1 下降 吸磁
	int  MoveDownStepTime(float Height);
	
	int  MoveDownSlow(int &MoveStep,BOOL bSegment=FALSE,  float height=0,  UINT WaitTM =WAITTIME,BYTE Mark =0,BYTE suckLevel=10); // 缓慢下降吸磁 除孔1 外
	int  MoveUpSlow(int &MoveStep,float height, UINT WaitTM =WAITTIME,BYTE Mark =0, int SpeedLevel=10 );
  
	int  MoveDownFast(BOOL bDown= TRUE,int Volume=1000, UINT WaitTM =WAITTIME,BYTE Mark =0,int SpeedLevel=10); // 下降到底，上下振荡 
	int  MoveUpDownFast(BOOL bDown= TRUE,float height=2, UINT WaitTM =WAITTIME,BYTE Mark =0,int SpeedLevel=10);
	
	float GetHeightByVolumeInHolePos(BYTE Type ,BYTE Pos, int Volume);
	virtual int DebugMotor(BYTE act,int param,int relatepos=0);		//调试马达电机
public:
	ZMotorPos  m_MotorPos ;
	int m_SuckmagneticLeftStep ;
};
