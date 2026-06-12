#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "AllMsgClass.h"

//收到命令解析用
CBaseMsg * FromMsgTypeToClass(MSG_TYPE msgType)
{
	CBaseMsg *prtn=NULL;
	switch(msgType)
	{
	case MSG_Ack:
		prtn = new  CAckMsg;
		break;
	case MSG_DebugPart:
		prtn = new  CPcDebugPart;
    	break;

	case MSG_Init:
		prtn = new  CResetReq;
		break;
	case MSG_TestStart:
		prtn = new  CStartRunMsg;
		break;
	case MSG_Pause:
		prtn = new  CPauseTestMsg;
		break;
	case MSG_Stop:
		prtn = new  CStopRunMsg;
		break;
	case MSG_RcvHeartBeat:
		prtn = new  CRcvHeartBeat;
		break;


	case MSG_RcvSaveParam:
		prtn = new  CSaveParamMsg;
		break;
	case MSG_RcvQueryStatus:
		prtn = new  CRcvQueryStatus;
		break;
	case MSG_RcvProgramStep:
		prtn = new  CReciveProgramStep;
		break;
	case MSG_RcvMixSpeed:
		prtn = new  CRcvMixSpeed;
		break;
	case MSG_RcvQueryMixSpeed:
		prtn = new  CRcvQueryMixSpeed;
		break;

	case MSG_RcvOpenCloseLamp:
		prtn = new  CRcvOpenCloseLamp;
		break;

	case MSG_RcvOpenCloseUVLamp:
		prtn = new  CRcvOpenCloseUVLamp;
		break;
	case MSG_RcvQueryLampAndCoverStatus:
		prtn = new  CRcvQueryLampAndCoverStatus;
		break;

	case MSG_RcvSuckMagneticWay:
		prtn = new  CRcvSuckMagneticWay;
		break;

	case MSG_RcvMotionParameters:
		prtn = new  CRcvMotionParameters;
		break;

	case MSG_RcvSetFactoryDefaultSpeed:
		prtn = new  CRcvSetFactoryDefaultSpeed;
		break;

	case MSG_RcvQueryIOInfo:
		prtn = new  CRcvQueryAllIOMsg;
		break;
	case MSG_RcvSetIO:
		prtn = new  CRcvSetIO;
		break;
	case MSG_RcvQueryVersion:
		prtn = new  CRcvQueryVersionInfo;
		break;
	default:
		FASSERT(FALSE);
		break;
	} 
	return prtn;
}
//收到命令判断命令类型用
UINT FromMsgTypeToCmdTyp(MSG_TYPE msgType)
{
	switch(msgType)
	{
	case MSG_DebugPart:
		return MACHINE_CMD_DEBUG;
	case MSG_Stop:
		return MACHINE_CMD_STOP;
	case MSG_Init:
		return MACHINE_CMD_RESET;
	case MSG_Pause:
		return MACHINE_CMD_PAUSE;
	case MSG_TestStart:
		return MACHINE_CMD_RUN;
	case MSG_RcvQueryStatus:  
	case  MSG_RcvQueryVersion:
	case  MSG_RcvQueryIOInfo:
	case  MSG_RcvSetIO:
	case  MSG_RcvQueryMixSpeed:  
	case  MSG_RcvQueryLampAndCoverStatus:
	case  MSG_RcvOpenCloseLamp:
	case  MSG_RcvHeartBeat:
	case  MSG_Ack:
		  return  MACHINE_CMD_ALL_STATUS;
//---------------------------------
	case MSG_RcvSaveParam:
	case MSG_RcvMixSpeed:
	case  MSG_RcvProgramStep:
	case  MSG_RcvOpenCloseUVLamp:
	case  MSG_RcvSuckMagneticWay:
	case  MSG_RcvMotionParameters:
	case  MSG_RcvSetFactoryDefaultSpeed:
       return  MACHINE_CMD_EXCEPT_RUN;
	default:
		return FALSE;
		break;
	}
	return FALSE;
}
//收到命令打TRACE用
wxString FromMsgTypeToString(MSG_TYPE msgType)
{
	CBaseMsg *prtn=NULL;
	switch(msgType)
	{
	case MSG_Ack:
		return _T("MSG_Ack");
	case MSG_NAK:
		return _T("MSG_NAK");
	case MSG_DebugPart:
		return _T("MSG_DebugPart");
	case MSG_Init:
		return _T("MSG_Init");
	case MSG_TestStart:
		return _T("MSG_TestStart");
	case MSG_Pause:
		return _T("MSG_Pause");
	case MSG_Stop:
		return _T("MSG_Stop");
	case MSG_RcvHeartBeat:
		return _T("MSG_RcvHeartBeat");
	case MSG_RcvQueryStatus:
         return _T("MSG_RcvQueryStatus");
	case MSG_RcvSaveParam:
		return _T("MSG_RcvSaveParam");
	case MSG_RcvProgramStep:
		return _T("MSG_RcvProgramStep");
	case MSG_RcvMixSpeed:
		return _T("MSG_RcvMixSpeed");
	case MSG_RcvQueryIOInfo:
		return _T("MSG_RcvQueryIOInfo");
	case MSG_RcvQueryMixSpeed:
         return _T("MSG_RcvQueryMixSpeed");
	case MSG_RcvQueryLampAndCoverStatus:
		return _T("MSG_RcvQueryLampAndCoverStatus");
	case MSG_RcvOpenCloseLamp:
		return _T("MSG_RcvOpenCloseLamp");
	case MSG_RcvOpenCloseUVLamp:
		return _T("MSG_RcvOpenCloseUVLamp");
	case MSG_RcvSetIO:
		return _T("MSG_RcvSetIO");
	case  MSG_RcvSuckMagneticWay:
	    return _T("MSG_RcvSuckMagneticWay");
	case  MSG_RcvMotionParameters:
    	return _T("MSG_RcvMotionParameters");
	case MSG_RcvQueryVersion:
		return _T("MSG_RcvQueryVersion");
	case MSG_RcvSetFactoryDefaultSpeed:
		return _T("MSG_RcvSetFactoryDefaultSpeed");
	default:
		FASSERT(FALSE);
		break;
	}
	return _T("");
}
