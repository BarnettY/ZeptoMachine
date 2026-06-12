#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "CommonMsg.h"
#include "BaseMsg.h"
#include "PcDebugEX.h"


//----------------------------------------------------------
BOOL CPcDebugPart::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(Part,msgbuf);	
	PutBuffer(MotorIdx,msgbuf);	
	PutBuffer(act,msgbuf);	
	PutBuffer(param,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
BOOL CPcDebugPart::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(Part,msgbuf);	
	ReadBuffer(MotorIdx,msgbuf);	
	ReadBuffer(act,msgbuf);	
	ReadBuffer(param,msgbuf);	
	return msgbuf.PointNotExceed();
}
////----------------------------------------------------------

SdNO_ParseFromBuffer(CAckMsg)
SdNO_PackageBuffer(CAckMsg)
//----------------------------------------------------------
SdNO_ParseFromBuffer(CNakMsg)
SdNO_PackageBuffer(CNakMsg)
//----------------------------------------------------------

NO_ParseFromBuffer(CRcvAck)
NO_PackageBuffer(CRcvAck)
//----------------------------------------------------------
NO_ParseFromBuffer(CRcvEndAck)
NO_PackageBuffer(CRcvEndAck)
//----------------------------------------------------------

SdNO_ParseFromBuffer(CRcvHeartBeat)
NO_PackageBuffer(CRcvHeartBeat)

NO_ParseFromBuffer(CSndHeartBeat)
SdNO_PackageBuffer(CSndHeartBeat)


//--------------------------------------
SdNO_ParseFromBuffer(CStartRunMsg)
NO_PackageBuffer(CStartRunMsg)

SdNO_ParseFromBuffer(CSndStartRunMsg)

BOOL CSndStartRunMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}


SdNO_ParseFromBuffer(CStopRunMsg)
NO_PackageBuffer(CStopRunMsg)

SdNO_ParseFromBuffer(CSndStopRunMsg)
BOOL CSndStopRunMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}


BOOL CSndTestError::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_btPart,msgbuf);
	ReadBuffer(m_btMotor,msgbuf);
	ReadBuffer(m_btCode,msgbuf);	
	ReadBuffer(m_wErrinfo,15,msgbuf);	
	ReadBuffer(m_bLevel,msgbuf);	
	return msgbuf.PointNotExceed();
}
BOOL CSndTestError::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_btPart,msgbuf);	
	PutBuffer(m_btMotor,msgbuf);
	PutBuffer(m_btCode,msgbuf);	
	PutBuffer(m_wErrinfo,15,msgbuf);	
	PutBuffer(m_bLevel,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}


SdNO_ParseFromBuffer(CResetReq)
NO_PackageBuffer(CResetReq)

SdNO_ParseFromBuffer(CSndResetReq)
BOOL CSndResetReq::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}


//继续(暂停)测试----------------------------------------------------------
BOOL CPauseTestMsg::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Pause,msgbuf);
	return msgbuf.PointNotExceed();
}
BOOL CPauseTestMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Pause,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}

SdNO_ParseFromBuffer(CSndPauseTestMsg)

BOOL CSndPauseTestMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}

//反馈仪器状态----------------------------------------------------------
BOOL CMedchineStatus::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Status,msgbuf);	
	return msgbuf.PointNotExceed();
}
BOOL CMedchineStatus::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Status,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}

SdNO_ParseFromBuffer(CRcvQueryStatus)
NO_PackageBuffer(CRcvQueryStatus)



// 容器区混匀 ----------------------------------------------------------
BOOL CRcvMixSpeed::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_SpeedMode,msgbuf);	
	return msgbuf.PointNotExceed();
}
BOOL CRcvMixSpeed::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_SpeedMode,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
BOOL CSndMixSpeed::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Success,msgbuf);	
	return msgbuf.PointNotExceed();
}
BOOL CSndMixSpeed::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}


// 收到查询
SdNO_ParseFromBuffer(CRcvQueryMixSpeed)
NO_PackageBuffer(CRcvQueryMixSpeed)

//回复查询
NO_ParseFromBuffer(CSndQueryMixSpeed)
BOOL CSndQueryMixSpeed::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_SpeedMode,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}



BOOL CReciveProgramStep::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	int length=msgbuf.GetBufferCount()-7;
	int ncount=length/21;
    ReadBuffer(m_id,msgbuf);	//4
	ReadBuffer(m_Type,msgbuf);	//1
	ReadBuffer(m_SuckMagneticWay,msgbuf);	//1
	ReadBuffer(m_NumberOfSegments,msgbuf);	//1	

	for(int i=0;i<ncount;i++)
	{
		ProgramStep *Step =new ProgramStep;
		ReadBuffer(Step->StepId,msgbuf);
		ReadBuffer(Step->HolePos,msgbuf);
		ReadBuffer(Step->MixTime,msgbuf);
		ReadBuffer(Step->MagneticTime,msgbuf);
		ReadBuffer(Step->WaitTime,msgbuf);
		ReadBuffer(Step->Volume,msgbuf);
	    ReadBuffer(Step->SpeedLevel,msgbuf);
        ReadBuffer(Step->MixPerCent,msgbuf);
		ReadBuffer(Step->SuckMagLevel,msgbuf);
		m_ProgramList.Append(Step);
	}

	return msgbuf.PointNotExceed();
}
NO_PackageBuffer(CReciveProgramStep)


BOOL CSndProgramStep::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
NO_ParseFromBuffer(CSndProgramStep)


SdNO_ParseFromBuffer(CRcvQueryAllIOMsg)
NO_PackageBuffer(CRcvQueryAllIOMsg)



BOOL CSndAllIOMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(IO,ALL_IO_COUNT,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}

NO_ParseFromBuffer(CSndAllIOMsg)


BOOL CSaveParamMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_btSave,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
BOOL CSaveParamMsg::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_btSave,msgbuf);	
	return msgbuf.PointNotExceed();
}


NO_ParseFromBuffer(CSndSaveParamMsg)
BOOL CSndSaveParamMsg::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
//

// 水阀设置 ----------------------------------------------------------
BOOL CRcvSetIO::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_IO,msgbuf);	
    ReadBuffer(m_State,msgbuf);	
	return msgbuf.PointNotExceed();
}

NO_PackageBuffer(CRcvSetIO);

NO_ParseFromBuffer(CSndSetIO);
BOOL CSndSetIO::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}



//版本号
SdNO_ParseFromBuffer(CRcvQueryVersionInfo)
NO_PackageBuffer(CRcvQueryVersionInfo)

//发送版本号
BOOL CSndVersionInfo::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_strVersion,10,msgbuf);		
	msgbuf.SubmitPoint();
	return TRUE;
}
NO_ParseFromBuffer(CSndVersionInfo)



//发送 命令启动或结束
BOOL CSndActionStarOrEnd::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_StepId,msgbuf);
	PutBuffer(m_Mark,msgbuf);
	PutBuffer(m_ActType,msgbuf);
	PutBuffer(m_Time,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
NO_ParseFromBuffer(CSndActionStarOrEnd)




BOOL CRcvOpenCloseLamp::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Open,msgbuf);
	return msgbuf.PointNotExceed();
}

NO_PackageBuffer(CRcvOpenCloseLamp)

NO_ParseFromBuffer(CSndOpenCloseLamp)

BOOL CSndOpenCloseLamp::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}




BOOL CRcvOpenCloseUVLamp::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Open,msgbuf);
    ReadBuffer(m_Time,msgbuf);
	return msgbuf.PointNotExceed();
}

NO_PackageBuffer(CRcvOpenCloseUVLamp)

NO_ParseFromBuffer(CSndOpenCloseUVLamp)

BOOL CSndOpenCloseUVLamp::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}




SdNO_ParseFromBuffer(CRcvQueryLampAndCoverStatus)
NO_PackageBuffer(CRcvQueryLampAndCoverStatus)


NO_ParseFromBuffer(CSndQueryLampAndCoverStatus)


BOOL CSndQueryLampAndCoverStatus::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Lamp,msgbuf);
	PutBuffer(m_UVLamp,msgbuf);
	PutBuffer(m_FrontCover,msgbuf);
	PutBuffer(m_Drawer,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}



//发送所有步骤剩余时间
BOOL CSndLeftTime::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Time,msgbuf);	
	msgbuf.SubmitPoint();
	return TRUE;
}
NO_ParseFromBuffer(CSndLeftTime)


BOOL CRcvSuckMagneticWay::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Way,msgbuf);
	return msgbuf.PointNotExceed();
}
NO_PackageBuffer(CRcvSuckMagneticWay)

NO_ParseFromBuffer(CSndSuckMagneticWay)
BOOL CSndSuckMagneticWay::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}

//BYTE m_HorizontalSpeed ;  //  水平移动速度
//BYTE m_HorizontalMixSpeed ;  //	水平混匀高速
//BYTE m_HorizontalSuckMagneticSpeed ; //	水平移动吸磁速度
//
//BYTE m_UpDownSpeed ;//磁棒/磁棒套下降速度
//BYTE m_SuckMagneticSpeed ; // 吸磁速度 

BOOL CRcvMotionParameters::ParseFromBuffer(MsgBuff &msgbuf)
{
	msgbuf.ResetPoint();
	ReadBuffer(m_Mark,msgbuf);
	ReadBuffer(m_HorizontalSpeed,msgbuf);
	ReadBuffer(m_HorizontalMixSpeed,msgbuf);	
	ReadBuffer(m_HorizontalSuckMagneticSpeed,msgbuf);
	ReadBuffer(m_UpDownSpeed,msgbuf);
	ReadBuffer(m_UpDownMixSpeed,msgbuf);
	ReadBuffer(m_SuckMagneticSpeed,msgbuf);

	return msgbuf.PointNotExceed();
}
NO_PackageBuffer(CRcvMotionParameters)

NO_ParseFromBuffer(CSndMotionParameters)
BOOL CSndMotionParameters::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);
	PutBuffer(m_HorizontalSpeed,msgbuf);
	PutBuffer(m_HorizontalMixSpeed,msgbuf);	
	PutBuffer(m_HorizontalSuckMagneticSpeed,msgbuf);
	PutBuffer(m_UpDownSpeed,msgbuf);
	PutBuffer(m_UpDownMixSpeed,msgbuf);
	PutBuffer(m_SuckMagneticSpeed,msgbuf);

	msgbuf.SubmitPoint();
	return TRUE;
}

SdNO_ParseFromBuffer(CRcvSetFactoryDefaultSpeed)
NO_PackageBuffer(CRcvSetFactoryDefaultSpeed)

NO_ParseFromBuffer(CSndSetFactoryDefaultSpeed)
BOOL CSndSetFactoryDefaultSpeed::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Success,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}

NO_ParseFromBuffer(CSndMotorPos);
BOOL CSndMotorPos::PackageBuffer(MsgBuff &msgbuf)
{
	msgbuf.NewBuffer();
	PutBuffer(m_Pos,msgbuf);
	msgbuf.SubmitPoint();
	return TRUE;
}
