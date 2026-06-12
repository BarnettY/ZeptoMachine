
#pragma once
#include "BasicMotor.h"


enum XMotorPos
{
	POS_XMotor_UNKNOW=-1,
	POS_XMotor_HOME,
	POS_XMotor_Hole1_Left,
	POS_XMotor_Hole1_Center,
	POS_XMotor_Hole1_Right,
	POS_XMotor_Hole1_Star,
	POS_XMotor_Hole2,
	POS_XMotor_Hole3,
	POS_XMotor_Hole4,
	POS_XMotor_Hole5,
	POS_XMotor_Hole6,
	POS_XMotor_Hole7,
	POS_XMotor_LastHole,
};

class CXMotor:public CBasicMotor
{
public:
	CXMotor(UCHAR CtrID,UCHAR axisID);
	virtual ~CXMotor();
	int InitHome();  // 初始化回零
	int GoToHome(UINT WaitTM=WAITTIME);
	int MoveToHoleOneBySegment(BYTE Type,BOOL Right= TRUE, BOOL bMix=TRUE, int SpeedLevel=0,int Segment=0, UINT WaitTM=WAITTIME*6,BYTE Mark =0);//吸磁满，分段
	int MoveToHoleOneCenter(BYTE Type,UINT WaitTM=WAITTIME); // 移动到1号孔中心 ，不同试剂盒起始位置不一致
	
	int MoveHoleOneDistance(BYTE Type,BOOL Right= TRUE, BOOL bMix=TRUE,   int SpeedLevel=0,   UINT WaitTM=WAITTIME*6 ); // 混匀用 ，间距 ，吸附磁珠，不同试剂盒起始位置不一致
    int MoveHalfHoleOneDistance(BYTE Type, BOOL Left= TRUE,BOOL bMix=TRUE,  int SpeedLevel=0, UINT WaitTM=WAITTIME*3);//混匀用，间距，吸附磁珠，不同试剂盒起始位置不一致
	int GetMoveHalfHoleOneDistanceTime(BYTE Type,int SpeedLevel,BOOL bMix=TRUE); 
	
	int MoveToHolePos(BYTE Type,int index,UINT WaitTM=WAITTIME); //index 从2 开始 ,定位用。 ，不同试剂盒孔2位置一致

	int MoveHoleDistance(BOOL Right= TRUE,UINT WaitTM=WAITTIME); //小孔间距
	
	int MoveLastHoleDistance(BOOL Right= TRUE,UINT WaitTM=WAITTIME) ; // 到最后孔间距 ，修改，走最后孔位置。
	int MoveToLastHolePos(BYTE Type,UINT WaitTM =WAITTIME);   //Type 不同类型的试剂盒，对应不同位置
    
	int MoveToSpecialHolePos(UINT WaitTM =WAITTIME);

	
	virtual int DebugMotor(BYTE act,int param,int relatepos=0);	//调试马达电机
	void SetDirDown(DIR_TYPE dir);
public:
	XMotorPos  m_MotorPos ;
	int m_PerMMToPluse ;//每mm 对应脉冲

};
