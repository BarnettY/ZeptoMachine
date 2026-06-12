// AxisParamInfo.h: interface for the CAxisParamInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AXISPARAMINFO_H__9FD02D68_0DF2_441E_9069_46D3AFEE4E26__INCLUDED_)
#define AFX_AXISPARAMINFO_H__9FD02D68_0DF2_441E_9069_46D3AFEE4E26__INCLUDED_

#include "../error/errcode.h"
#include "../comon.h"

//////////////////////////////////////////////////////////////////////
// class PosInfo
//////////////////////////////////////////////////////////////////////
struct PosInfo
{
	BOOL 	BLock;
	UINT	POS;
	UCHAR Group;
	PosInfo()
	{
		BLock=FALSE;
		POS=0;
		Group=0;
	}
	BOOL VerifyData();
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
};

//////////////////////////////////////////////////////////////////////
// class SpeedInfo
//////////////////////////////////////////////////////////////////////
struct SpeedInfo
{
	BOOL 	BLock;
	UINT 		Speed;
	UCHAR Group;
	SpeedInfo()
	{
		BLock=FALSE;
		Speed=0;
		Group=0;
	}
	BOOL VerifyData();
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
};

//////////////////////////////////////////////////////////////////////
// class AccInfo
//////////////////////////////////////////////////////////////////////
struct AccInfo
{
	BOOL 	BLock;
	UINT 		Acc;
	UCHAR Group;
	AccInfo()
	{
		BLock=FALSE;
		Acc=0;
		Group=0;
	}
	BOOL VerifyData();
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
};

//////////////////////////////////////////////////////////////////////
// class AxisParamInfo
//////////////////////////////////////////////////////////////////////
struct AxisParamInfo
{
	AxisParamInfo()
	{
		HaveEncode=FALSE;
		for(int i=0;i<9;i++)
		{
			PosInfo empty;
			AbsPos[i]=empty;
			AbsPos[i].Group=i;
			RelaPos[i]=empty;
			RelaPos[i].Group=i;
			EncodePos[i]=empty;
			EncodePos[i].Group=i;
			SpeedInfo Zero;
			SpeedHigh[i]=Zero;
			SpeedHigh[i].Group=i;
			SpeedLow[i]=Zero;
			SpeedLow[i].Group=i;
			AccInfo ZeroAcc;
			AccSpeed[i]=ZeroAcc;
		}
	}
	PosInfo AbsPos[9];
	PosInfo RelaPos[9];
	PosInfo EncodePos[9];
	SpeedInfo SpeedHigh[9];
	SpeedInfo SpeedLow[9];
	AccInfo AccSpeed[9];
	BOOL HaveEncode;

	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);

};
enum Current_TYPE
{
	A0=0,
	A0_05,	//_1
	A0_1,	//_2
	A0_15,	//_3
	A0_2,	//_4
	A0_25,	//_5
	A0_3,	//_6
	A0_35,	//_7
	A0_4,	//_8
	A0_45,	//_9
	A0_5,	//_10
	A0_6,	//_11
	A0_7,	//_12
	A0_8,	//_13
	A0_9,	//_14
	A0_95,	//_15
	A1,		//_16
	A1_1,	//_17
	A1_15,	//_18
	A1_2,	//_19
	A1_25,	//_20
	A1_3,	//_21
	A1_35,	//_22
	A1_4,	//_23
	A1_45,	//_24
	A1_5,	//_25
	A1_55,	//_26
	A1_6,	//_27
	A1_7,	//_28
	A1_8,	//_29
	A1_9,	//_30
	A2,		//_31
};
enum SubDiv_TYPE
{
	Sub_1=0,
	Sub_2,
	Sub_4,
	Sub_8,
	Sub_16,
	Sub_32,
	Sub_64,
	Sub_128,
	Sub_256
};
enum Enable_TYPE
{
	Disable=0,
	Enable
};
struct AxisDriverInfo
{
	AxisDriverInfo()
	{
		LockMotor=TRUE;
		Current=A0_1;
		SubDiv=Sub_16;
		ModeA1_6=Enable;
	}
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
	BOOL 	LockMotor;
	Current_TYPE Current;
	SubDiv_TYPE SubDiv;
	Enable_TYPE ModeA1_6;
};
enum ACC_TYPE
{
	ACC_SPEED=0,
	ACC_TIME
};

enum PWM_TYPE
{
	PWM_CP_DIR=0,
	PWM_A_B
};
struct NotifyInfo
{
	NotifyInfo()
	{
		AccType=ACC_SPEED;
		BOrgNotify=FALSE;
		BLeftLimitNotify=FALSE;
		BRightLimitNotify=FALSE;
		BLiqidNotify=FALSE;
		BFinishNotify=FALSE;
		BEncodeLimitNotify=FALSE;
		BEncodeZNotify=FALSE;
		BFivePosNotify=FALSE;
	}
	void Combine(NotifyInfo a)
	{
		if(a.BOrgNotify)
			BOrgNotify=TRUE;
		if(a.BLeftLimitNotify)
			BLeftLimitNotify=TRUE;
		if(a.BRightLimitNotify)
			BRightLimitNotify=TRUE;
		if(a.BLiqidNotify)
			BLiqidNotify=TRUE;
		if(a.BFinishNotify)
			BFinishNotify=TRUE;
		if(a.BEncodeLimitNotify)
			BEncodeLimitNotify=TRUE;
		if(a.BEncodeZNotify)
			BEncodeZNotify=TRUE;
		if(a.BFivePosNotify)
			BFivePosNotify=TRUE;
	}
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);

	ACC_TYPE AccType;
	BOOL BOrgNotify;
	BOOL BLeftLimitNotify;
	BOOL BRightLimitNotify;
	BOOL BLiqidNotify;
	BOOL BFinishNotify;
	BOOL BEncodeLimitNotify;
	BOOL BEncodeZNotify;
	BOOL BFivePosNotify;
};

enum Level_TYPE
{
	LevelLow=0,
	LevelHigh
};

struct InputValidLevel
{
	InputValidLevel()
	{
		for(int i=0;i<12;i++)
			InputLevel[i]=LevelLow;
	}
	void SetAxisInput(UCHAR index,Level_TYPE org,Level_TYPE left,Level_TYPE right,Level_TYPE liq);
	int PackageBufferForX21(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
	Level_TYPE InputLevel[12];
};
struct InputInterEnableInfo
{
	InputInterEnableInfo()
	{
		for(int i=0;i<12;i++)
			InputEnable[i]=Disable;
	}
	void SetAxisInput(UCHAR index,Enable_TYPE org,Enable_TYPE left,Enable_TYPE right,Enable_TYPE liq);
	BOOL GetAxisSignal(UCHAR &SN,NotifyInfo &tmpNotify);
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
	Enable_TYPE InputEnable[12];
};

struct LimitOrgLevel
{
	LimitOrgLevel()
	{
		for(int i=0;i<12;i++)
			LimitLevel[i]=LevelLow;
	}
	void SetAxisLimitOrgLevel(UCHAR index,Level_TYPE org,Level_TYPE leftlimit,Level_TYPE rightlimit,Level_TYPE liq);
	int PackageBufferForX25(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
	Level_TYPE LimitLevel[12];
};

struct LimitOrgEnableInfo
{
	LimitOrgEnableInfo()
	{
		for(int i=0;i<13;i++)
			InputEnable[i]=Disable;
	}
	void SetAxisInput(UCHAR index,Enable_TYPE org,Enable_TYPE limit,Enable_TYPE liq);
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
	Enable_TYPE InputEnable[13];
};
enum Enable_STOP_NOW
{
	StopNow=0,
	StopDec
};
enum Enable_ORG_RESETABSPOS
{
	OrgNotResetAbsPos=0,
	OrgResetAbsPos
};
struct OrgActionInfo
{
	OrgActionInfo()
	{
		for(int i=0;i<3;i++)
		{
			OrgAction[i]=StopNow;
			OrgResetAbs[i]=OrgNotResetAbsPos;
			LimitDec[i]=StopNow;
		}
	}
	void SetAxisOrgAction(UCHAR index,Enable_STOP_NOW stop,Enable_ORG_RESETABSPOS resetABS,Enable_STOP_NOW limit);
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);
	Enable_STOP_NOW OrgAction[3];
	Enable_ORG_RESETABSPOS OrgResetAbs[3];
	Enable_STOP_NOW LimitDec[3];
};


//////////////////////////////////////////////////////////////////////
// class LockCurrentInfo
//////////////////////////////////////////////////////////////////////
struct LockCurrentInfo
{
	LockCurrentInfo()
	{
		BEnalbe=TRUE;
		LockCurrent=A0_05;
	}
	LockCurrentInfo(BOOL able,Current_TYPE Current)
	{
		BEnalbe=able;
		LockCurrent=Current;
	}
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);

	BOOL BEnalbe;
	Current_TYPE LockCurrent;
};
//////////////////////////////////////////////////////////////////////
// class FrameIDInfo
//////////////////////////////////////////////////////////////////////
struct FrameIDInfo
{
	FrameIDInfo()
	{
		BRcv=FALSE;
		ID=0X7F;
		AxisID=0;
		BContinue=FALSE;
		BRead=FALSE;
		TCW=0;
		Channel=0;
	}
	FrameIDInfo(UCHAR id,UCHAR axis,UCHAR tcw,BOOL read)
	{
		BRcv=FALSE;
		BContinue=FALSE;
		ID=id;
		AxisID=axis;
		TCW=tcw;
		BRead=read;
		Channel=0;
	}
	UINT PackageFrameID();
	static FrameIDInfo ParseFrameID(UINT ID);
	static BYTE ParseCtrlID(UINT ID)
	{
		BYTE CtrlID=(ID>>18)&0x7f;
		return CtrlID;
	}

	BOOL BRcv;
	UCHAR ID;
	UCHAR AxisID;
	BOOL BContinue;
	BOOL BRead;
	UCHAR TCW;
	BYTE Channel;
};

enum MOVE_TYPE
{
	MOVE_RT=0,
	MOVE_CONTINUE
};
enum DIR_TYPE
{
	DIR_CW=0,
	DIR_CCW
};
enum POS_TYPE
{
	ABSOLUTE_POS=0,
	RELATIVE_POS,
	ENCODE_POS
};

struct MoveInfo
{
	MoveInfo()
	{
		Mode1=MOVE_RT;
		Group1=0;
		POS1=ABSOLUTE_POS;
		DIR1=DIR_CW;
		BContinueMove=FALSE;
		Mode2=MOVE_RT;
		Group2=0;
		POS2=ABSOLUTE_POS;
		DIR2=DIR_CW;
	}
	int PackageBuffer(UCHAR*buff);
	int ReadBuffer(UCHAR*buff);

	MOVE_TYPE	Mode1;
	UCHAR		Group1;
	POS_TYPE	POS1;
	DIR_TYPE	DIR1;
	BOOL		BContinueMove;
	MOVE_TYPE	Mode2;
	UCHAR		Group2;
	POS_TYPE	POS2;
	DIR_TYPE	DIR2;
};
///////////////////////////////////////////////////////////////
#define ACTION_SPLITSTEP			400		//动作分步完成开始
#define ACTION_SPLITSTEPFINISH		410		//动作分步完成结束



#endif // !defined(AFX_AXISPARAMINFO_H__9FD02D68_0DF2_441E_9069_46D3AFEE4E26__INCLUDED_)
