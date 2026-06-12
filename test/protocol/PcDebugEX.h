#ifndef	_PCDEBUGEX_H_
#define	_PCDEBUGEX_H_

//----------------------------------------命令定义------------------------------------------------

#include "../runinfo/ItemList.h"
//发送ACK
class CAckMsg: public CBaseMsg
{
public:
	CAckMsg()
	{
		m_MessageType=MSG_Ack;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	

};

//----------------------------------------------------------
class CNakMsg: public CBaseMsg
{
public:
	CNakMsg()
	{
		m_MessageType=MSG_NAK;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	

};
////----------------------------------------------------------
//计算机要求机器启动
class CStartRunMsg: public CBaseMsg
{
public:
	CStartRunMsg()
	{
		m_MessageType=MSG_TestStart;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//回复启动
class CSndStartRunMsg: public CBaseMsg
{
public:
	CSndStartRunMsg()
	{
		m_MessageType=MSG_SndTestStart;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//停止 
class CStopRunMsg: public CBaseMsg
{
public:
	CStopRunMsg()
	{
		m_MessageType=MSG_Stop;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

class CSndStopRunMsg: public CBaseMsg
{
public:
	CSndStopRunMsg()
	{
		m_MessageType=MSG_Stop;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//暂停 继续
class CPauseTestMsg: public CBaseMsg
{
public:
	CPauseTestMsg()
	{
		m_MessageType=MSG_Pause;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
    BYTE m_Pause; // 1 暂停  0 继续
};

class CSndPauseTestMsg: public CBaseMsg
{
public:
	CSndPauseTestMsg()
	{
		m_MessageType=MSG_SndPause;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//----------------------------------------------------------

//计算机要求机器复位
class CResetReq: public CBaseMsg
{
public:
	CResetReq()
	{
		m_MessageType=MSG_Init;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};


class CSndResetReq: public CBaseMsg
{
public:
	CSndResetReq()
	{
		m_MessageType=MSG_SndInit;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//ACK
class CRcvAck: public CBaseMsg
{
public:
	CRcvAck()
	{
		m_MessageType=(MSG_TYPE)(0x01);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//----------------------------------------------------------
//ACK
class CRcvEndAck: public CBaseMsg
{
public:
	CRcvEndAck()
	{
		m_MessageType=(MSG_TYPE)(0x02);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//发送心跳包
class CSndHeartBeat: public CBaseMsg
{
public:
	CSndHeartBeat()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndHeartBeat);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

//收到心跳包
class CRcvHeartBeat: public CBaseMsg
{
public:
	CRcvHeartBeat()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvHeartBeat);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};


////测试中的错误信息
class CSndTestError: public CBaseMsg
{
public:
	CSndTestError()
	{
		m_MessageType=MSG_Error;
		m_btPart=0;
		m_btMotor=0;
		m_btCode=0;
		m_wErrinfo=_T("");
		m_bLevel =0;
	}
	BYTE m_btPart;
	BYTE m_btMotor;
	BYTE m_btCode;
	wxString  m_wErrinfo;
	///WORD m_wErrinfo;

	BYTE m_bLevel;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	

};

// 调试
class CPcDebugPart: public CBaseMsg
{
public:
	CPcDebugPart()
	{
		m_MessageType=MSG_DebugPart;
		Part=0;
		MotorIdx=0xff;
		act=0;
		param=0;
	}
	BYTE	Part;
	BYTE	MotorIdx;
	BYTE	act;
	int 		param;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};

//
//反馈仪器状态
class CMedchineStatus: public CBaseMsg
{
public:
	CMedchineStatus()
	{
		m_MessageType=(MSG_TYPE)(MSG_MedchineStatus);
		m_Status=0;
	}
	BYTE m_Status;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


//查询状态
class CRcvQueryStatus: public CBaseMsg
{
public:
	CRcvQueryStatus()
	{
		m_MessageType=MSG_RcvQueryStatus;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

// 混匀速度设置
class CSndMixSpeed: public CBaseMsg
{
public:
	CSndMixSpeed()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndMixSpeed);
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


//设置摇匀速度
class CRcvMixSpeed: public CBaseMsg
{
public:
	CRcvMixSpeed()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvQueryMixSpeed);
		m_SpeedMode=0;
	}
	BYTE m_SpeedMode; // 0 禁用搅拌     5速度最大
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


// 反馈 混匀速度设置
class CSndQueryMixSpeed: public CBaseMsg
{
public:
	CSndQueryMixSpeed()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndQueryMixSpeed);
		m_SpeedMode=0;
	}
	BYTE m_SpeedMode;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


//查询摇匀速度设置
class CRcvQueryMixSpeed: public CBaseMsg
{
public:
	CRcvQueryMixSpeed()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvMixSpeed);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};

// 程序反应流程 程序步骤 
class CReciveProgramStep: public CBaseMsg
{
public:
	CReciveProgramStep()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvProgramStep);
		m_ProgramList.DeleteContents(true);
        m_id=0;
		m_Type =0;
		m_SuckMagneticWay=0;
		m_NumberOfSegments=0;
	}
	ProgramInfoList m_ProgramList;

    int  m_id;
	BYTE  m_Type;
	BYTE  m_SuckMagneticWay;
	BYTE  m_NumberOfSegments;
	
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};
class CSndProgramStep: public CBaseMsg
{
public:
	CSndProgramStep()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndProgramStep);
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};




//IO 信息
class CRcvQueryAllIOMsg: public CBaseMsg
{
public:
	CRcvQueryAllIOMsg()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvQueryIOInfo);

	}
	//BYTE m_Mark ;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

class CSndAllIOMsg: public CBaseMsg
{
public:
	CSndAllIOMsg()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndAllIOInfo);
	}
	BYTE  IO[ALL_IO_COUNT];
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};



class CSaveParamMsg:public CBaseMsg  //保存参数
{
public:
	CSaveParamMsg()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvSaveParam);
	}
	BYTE m_btSave; //1 保存参数
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

class CSndSaveParamMsg:public CBaseMsg  //保存参数
{
public:
	CSndSaveParamMsg()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndSaveParam);
		m_Success =1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};



// 程序步骤 命令 // 试剂 位置信息
//设置输出
class CRcvSetIO:public CBaseMsg
{
public:
	CRcvSetIO()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvSetIO);
		m_State =0;
		m_IO =0;
	}
	BYTE m_State;  // 0 关闭  1 打开
	BYTE m_IO;  // 1-6  对应6个水阀
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


class CSndSetIO: public CBaseMsg
{
public:
	CSndSetIO()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndSetIO);
		m_Success=0;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};
// 查询版本信息
class CRcvQueryVersionInfo:  public CBaseMsg
{
public:
	CRcvQueryVersionInfo()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvQueryVersion);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};


class CSndVersionInfo: public CBaseMsg
{
public:
	CSndVersionInfo()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndSoftVersion);
		m_strVersion="1.0.000";
	}
	wxString m_strVersion;

	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	

};



//发送命令开始结束
class CSndActionStarOrEnd: public CBaseMsg
{
public:
	CSndActionStarOrEnd()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndActionStarOrEnd);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
	BYTE m_StepId; // 步骤ID
	BYTE m_Mark ; //  1动作开始 2 动作结束
	BYTE m_ActType ;//  1 混匀  2  吸磁   3 等待 
	int  m_Time; // 时长;秒
};



// 打开/关闭 照明灯
class CRcvOpenCloseLamp : public CBaseMsg
{
public:
	CRcvOpenCloseLamp()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvOpenCloseLamp) ;
	}
	BYTE m_Open ;  //  1 打开 2 关闭
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


class CSndOpenCloseLamp: public CBaseMsg
{
public:
	CSndOpenCloseLamp()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndOpenCloseLamp) ;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


// 打开/关闭紫外灯
class CRcvOpenCloseUVLamp: public CBaseMsg
{
public:
	CRcvOpenCloseUVLamp()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvOpenCloseUVLamp) ;
	}
	BYTE m_Open ;  //  1 打开 2 关闭
	int  m_Time;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


class CSndOpenCloseUVLamp: public CBaseMsg
{
public:
	CSndOpenCloseUVLamp()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndOpenCloseUVLamp) ;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


// 状态反馈
class CSndQueryLampAndCoverStatus: public CBaseMsg
{
public:
	CSndQueryLampAndCoverStatus()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndQueryLampAndCoverStatus);
		m_Lamp=0;
		m_UVLamp =0;
		m_FrontCover =0; 
        m_Drawer=0;
	}
	BYTE m_Lamp;  // 0关  1开
	BYTE m_UVLamp;
	BYTE m_FrontCover;
	BYTE m_Drawer ;

	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};



class CRcvQueryLampAndCoverStatus: public CBaseMsg
{
public:
	CRcvQueryLampAndCoverStatus()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvQueryLampAndCoverStatus) ;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


class CSndLeftTime: public CBaseMsg
{
public:
	CSndLeftTime()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndLeftTime);
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
	int  m_Time; // 时长;秒
};


// 吸磁方式
class CRcvSuckMagneticWay: public CBaseMsg
{
public:
	CRcvSuckMagneticWay()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvSuckMagneticWay) ;
	}
	BYTE m_Way ;  // 吸磁方式
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};

class CSndSuckMagneticWay: public CBaseMsg
{
public:
	CSndSuckMagneticWay()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndSuckMagneticWay) ;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};

// 运动参数
class CRcvMotionParameters: public CBaseMsg
{
public:
	CRcvMotionParameters()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvMotionParameters) ;
	}
	//BYTE m_HorizontalSpeed ;  //  水平移动速度
	//BYTE m_ZUpDownSpeed ;// 磁棒套升降速度
	//BYTE m_MixedFrequency ;//混合频率  
 //   BYTE m_MagneticUpDownSpeed ; //磁棒升降速度mm/s
 //   BYTE m_SuckMagneticSpeed ; // 吸磁速度 
    
	BYTE m_Mark ;
	BYTE m_HorizontalSpeed ;  //  水平移动速度
	int m_HorizontalMixSpeed ;  //	水平混匀高速
	BYTE m_HorizontalSuckMagneticSpeed ; //	水平移动吸磁速度

	BYTE m_UpDownSpeed ;//磁棒/磁棒套下降速度
    int m_UpDownMixSpeed ;//圆孔中上下混匀速度
	BYTE m_SuckMagneticSpeed ; // 吸磁速度 

	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};

class CSndMotionParameters: public CBaseMsg
{
public:
	CSndMotionParameters()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndMotionParameters) ;
		m_Success=1;
	}
	BYTE m_Success;
	BYTE m_HorizontalSpeed ;  //  水平移动速度
	int m_HorizontalMixSpeed ;  //	水平混匀高速
	BYTE m_HorizontalSuckMagneticSpeed ; //	水平移动吸磁速度

	BYTE m_UpDownSpeed ;//磁棒/磁棒套下降速度
	int m_UpDownMixSpeed ;//圆孔中上下混匀速度
	BYTE m_SuckMagneticSpeed ; // 吸磁速度 
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};

class CRcvSetFactoryDefaultSpeed: public CBaseMsg
{
public:
	CRcvSetFactoryDefaultSpeed()
	{
		m_MessageType=(MSG_TYPE)(MSG_RcvSetFactoryDefaultSpeed) ;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

class CSndSetFactoryDefaultSpeed: public CBaseMsg
{
public:
	CSndSetFactoryDefaultSpeed()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndSetFactoryDefaultSpeed) ;
		m_Success=1;
	}
	BYTE m_Success;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);	
};

class CSndMotorPos: public CBaseMsg
{
public:
	CSndMotorPos()
	{
		m_MessageType=(MSG_TYPE)(MSG_SndMotorPos);
		m_Pos =0;
	}
	int  m_Pos;
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
};


#endif
