#ifndef		_CommonMsg_H_
#define		_CommonMsg_H_
#include "../comon.h"

enum MACHINE_CMD_TYPE
{
	MACHINE_CMD_NONE=0,
	MACHINE_CMD_STOP,					//停止
	MACHINE_CMD_RESET,					//复位
	MACHINE_CMD_RUN,				    //启动
    MACHINE_CMD_PAUSE,                   // 暂停  
	MACHINE_CMD_DEBUG,					//调试
	MACHINE_CMD_EXCEPT_RUN,             //  运行状态  MACHINE_READY  MACHINE_DEBUG  MACHINE_STOP  MACHINE_WAITINIT
	MACHINE_CMD_ALL_STATUS,              // 任何状态满足
	MACHINE_CMD_END 
};


enum MSG_TYPE
{
	MSG_NONE=-1,
	MSG_Ack=0x01,  
	MSG_NAK=0x02,  
	MSG_SndAck=0x01,					//CSndAck
	MSG_SndNAK=0x04,					//CSndNAK
	MSG_RcvHeartBeat=0x06,			//CRcvHeartBeat
	MSG_SndHeartBeat=0x07,			//CSndHeartBeat
	MSG_Init =0x08,
    MSG_SndInit =0x08,

	MSG_TestStart =0x09,
	MSG_SndTestStart  =0x09,
    MSG_Pause= 0x0A,
    MSG_SndPause= 0x0A,
	MSG_Stop= 0x0B,

    MSG_SndStop= 0x0B,
	MSG_DebugPart =0x0C,

    MSG_MedchineStatus =0x0E,
    MSG_RcvQueryStatus =0x0E, // 查询状态
	MSG_Error =0x13,
	MSG_RcvOpenCloseLamp =0x14,
    MSG_SndOpenCloseLamp =0x14,

	MSG_RcvMixSpeed =0x16,
	MSG_SndMixSpeed =0x16,

    MSG_RcvQueryLampAndCoverStatus=0x17,
    MSG_SndQueryLampAndCoverStatus=0x17,

	MSG_RcvProgramStep =0x18, //程序步骤
	MSG_SndProgramStep =0x18,
	MSG_SndLeftTime =0x19,  // 发送程序预计时间

	MSG_RcvSaveParam=0x1d, // 保存参数
    MSG_SndSaveParam=0x1d, // 保存参数成功

	MSG_RcvQueryMixSpeed =0x1A,
	MSG_SndQueryMixSpeed =0x1A,

	MSG_RcvQueryIOInfo =0x21, 
	MSG_SndAllIOInfo =0x21,

	//MSG_SndRackInfo =0x22, //玻片架信息
	MSG_RcvSetIO =0x23, // IO设置
	MSG_SndSetIO =0x23, // IO设置

    MSG_SndActionStarOrEnd=0x24, // 通知命令开始执行或者结束。

	MSG_RcvOpenCloseUVLamp =0x26,
	MSG_SndOpenCloseUVLamp =0x26,


	MSG_RcvSuckMagneticWay =0x27, //吸磁方式
	MSG_SndSuckMagneticWay =0x27,

	MSG_RcvQueryVersion =0x28,
	MSG_SndSoftVersion =0x28,

	MSG_RcvMotionParameters=0x29,
	MSG_SndMotionParameters=0x29,

	MSG_RcvSetFactoryDefaultSpeed=0x30,
	MSG_SndSetFactoryDefaultSpeed=0x30,


	MSG_SndMotorPos =0x32, //调试状态下发送电机位置。
	//case 0xec:   不使用
	//case 0x41:
	//case 0xff:
	//case 0xb6:
	//case 0x15:	
};



class CBaseMsg;
CBaseMsg * FromMsgTypeToClass(MSG_TYPE msgType);
UINT FromMsgTypeToCmdTyp(MSG_TYPE msgType);
wxString FromMsgTypeToString(MSG_TYPE msgType);


//
//#define CMD_DEBUG_BACK		((1<<8)+CMD_DEBUG)
//#define CMD_DEBUG_AACK		(CMD_DEBUG)
//#define CMD_DEBUG_FINISH		((3<<8)+CMD_DEBUG)
//#define CMD_DEBUG_NOFINISH	((1<<8)+CMD_DEBUG)
//
//#define CMD_AD_DEBUG_GET		((1<<8)+CMD_AD_DEBUG)
//#define CMD_AD_DEBUG_SET		(CMD_AD_DEBUG)
//#define CMD_STATUS_BACK		((1<<8)+CMD_STATUS)
//#define CMD_STATUS_AACK		(CMD_STATUS)
//#define CMD_PARAM_GET			((1<<8)+CMD_PARAM)
//#define CMD_PARAM_SET			(CMD_PARAM)
//#define CMD_TEST_GET			((1<<8)+CMD_TEST)
//#define CMD_TEST_SET			(CMD_TEST)
//
#define	GET_CMD(A)				(A&0XFF)
//#define	GET_BEFACK(A)			((A>>8)&0X01)
//#define	GET_JOBFINISH(A)		((A>>9)&0X01)

#endif
