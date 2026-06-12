// CanControl.cpp: implementation of the CCanControl class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "CanControl.h"
#include "../motor/CanMotor.h"
#include "CanCom.h"
#include "../trace.h"


//////////////////////////////////////////////////////////////////////
// class CCanControl
//////////////////////////////////////////////////////////////////////
CCanControl::CCanControl(UINT ctrlID)
{
	int i;
	m_ControlID=ctrlID;
	m_ControlVer=0;
	for(i=0;i<3;i++)
	{
		m_pMotor[i]=NULL;
		m_StopDec[i]=10;//ms
		m_LiqDec[i]=100;//ms
		m_MotorListen[i]=FALSE;
		m_PendFinishID[i]=0;
		m_BNeedPendFinish[i]=FALSE;
	}
	m_Param[0].HaveEncode=TRUE;
	m_Param[1].HaveEncode=TRUE;
	m_Baud=K100;
	m_XEncodeCnt=1000;
	m_YEncodeCnt=2000;

	//零时变量
	for(i=0;i<3;i++)
	{
		m_ABSPos[i]=0;
		m_EncodePos[i]=0;
		m_CurrentSpeed[i]=0;
		m_MotorRunning[i]=FALSE;
	}
	m_OutPutLevel=0X0F;
}

CCanControl::~CCanControl()
{
/*	for(int i=0;i<3;i++)
	{
		if(NULL!=m_pMotor[i])
			delete m_pMotor[i];
	}*/
}

void CCanControl::SetChildMotor(UCHAR index,CCanMotor *pMotor)
{
	FASSERT(index<=3);
	FASSERT(index>0);
	if((index<1)||(index>3))
		return;
	FASSERT(NULL==m_pMotor[index-1]);
	m_pMotor[index-1]=pMotor;
}

CCanMotor *CCanControl::GetChildMotor(UCHAR index)
{
	FASSERT(index<=3);
	FASSERT(index>0);
	if((index<1)||(index>3))
		return NULL;
	return m_pMotor[index-1];
}
///////////////////////////
//读操作
int CCanControl::ReadVersion(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x02,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
int CCanControl::ReadBaud(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x03,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
//读取输入的有效电平
int CCanControl::ReadInputValid(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x21,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
inline int ReverseBit(int value,int bit)
{
	if(bit<=0)
		return value;
	UCHAR BIT=(value>>bit)&0X1;
	if(BIT)
	{
		int temp=value-(1<<bit);
		return temp;
	}
	else
	{
		int temp=value+(1<<bit);
		return temp;
	}
}
//读取全部输入是否有效
int CCanControl::ReadInput(UINT WaitTM)
{
	AxisMute0.Enter();
	FrameIDInfo frame(m_ControlID,0,0x22,TRUE);
	int irtn=SendCanAndWait(frame,NULL,0,WaitTM);
	if (irtn>=0)
	{
		//这个项目中，配置成LevelLow代表是挡光即程序逻辑高电平，
		//配置成LevelHigh代表不挡光即程序逻辑低电平
		//结合原点限位高低电平来判断是否电平有效
		static const int shit[]={11,10,9,-1,8,7,6,-1,5,4,3,-1};
		for(int i=0;i<12;i++)
		{
			if(LevelHigh==m_LimitLevel.LimitLevel[i])
			{
				irtn=ReverseBit(irtn,shit[i]);
			}
		}
		//因为三个探针只要设置一个，就需要反
		if(m_LimitLevel.LimitLevel[3]||m_LimitLevel.LimitLevel[7]||m_LimitLevel.LimitLevel[11])
			irtn=ReverseBit(irtn,2);
	}
	AxisMute0.Leave();
	return irtn;
}
int  CCanControl::GetInputValid(IO_ALL Index,BOOL& BVaild )
{
	int rtn=ReadInput();
	if(rtn<0)
		return rtn;
	BVaild=(rtn&(1<<Index))>0;
	return 1;	
}
//读取输入使能
int CCanControl::ReadInputEnable(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x23,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
//读取全部输出高低电平
int CCanControl::ReadOutput(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x24,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	rtn=~rtn;
	AxisMute0.Leave();
	return rtn;
}
//读取原点对应动作
int CCanControl::ReadOrgAcion(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x26,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}

//读取当前位置
int CCanControl::ReadMotorPos(UCHAR axis,POS_TYPE postype,UINT WaitTM)
{
	FASSERT(RELATIVE_POS!=postype);
	if(ENCODE_POS==postype)
		FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,(ABSOLUTE_POS==postype)?0x40:0x41,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//读取速度
int CCanControl::ReadMotorSpeed(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x42,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//读取电机运动参数
int CCanControl::ReadMotorParam(UCHAR axis,PosInfo &pos,POS_TYPE postype,UINT WaitTM)
{
	FrameIDInfo frame;
	switch(postype)
	{
		case  ABSOLUTE_POS:
			 frame=FrameIDInfo(m_ControlID,axis,0x43,TRUE);
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=3);
			break;
		case  RELATIVE_POS:
			 frame=FrameIDInfo(m_ControlID,axis,0x44,TRUE);
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=3);
			break;
		case  ENCODE_POS:
			 frame=FrameIDInfo(m_ControlID,axis,0x45,TRUE);
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=2);
			break;
		default:
			FASSERT(FALSE);
			break;
	}
	UCHAR buff[8]={0};
	UINT rtnLen=pos.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		switch(postype)
		{
			case  ABSOLUTE_POS:
				pos=m_Param[axis-1].AbsPos[pos.Group];
				break;
			case  RELATIVE_POS:
				pos=m_Param[axis-1].RelaPos[pos.Group];
				break;
			case  ENCODE_POS:
				pos=m_Param[axis-1].EncodePos[pos.Group];
				break;
			default:
				FASSERT(FALSE);
				break;
		}
	}
	return rtn;
}
int CCanControl::ReadMotorParam(UCHAR axis,SpeedInfo &speed,BOOL BHigh,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame;
	if(BHigh)
	{
		 frame=FrameIDInfo(m_ControlID,axis,0x46,TRUE);
	}
	else
	{
		 frame=FrameIDInfo(m_ControlID,axis,0x47,TRUE);
	}
	UCHAR buff[8]={0};
	UINT rtnLen=speed.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		if(BHigh)
		{
			speed=m_Param[axis-1].SpeedHigh[speed.Group];
		}
		else
		{
			speed=m_Param[axis-1].SpeedLow[speed.Group];
		}
	}
	return rtn;
}
int CCanControl::ReadMotorParam(UCHAR axis,AccInfo &ACC,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x48,TRUE);
	UCHAR buff[8]={0};
	UINT rtnLen=ACC.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		ACC=m_Param[axis-1].AccSpeed[ACC.Group];
	}
	return rtn;
}

int CCanControl::ReadMotorRunning(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	AxisMute0.Enter();
	FrameIDInfo frame(m_ControlID,axis,0x53,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
//查询液位减速度
int CCanControl::ReadLiqDec(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x54,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//查询急停减速度
int CCanControl::ReadStopDec(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x68,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//查询通知信号有效位
int CCanControl::ReadNotifyInfo(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x69,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//查询指定站点指定轴的脉冲模式
int CCanControl::ReadPWMType(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x6a,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}

//查询全部参数
int CCanControl::ReadAllParam(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x80,TRUE);
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
//查询编码器线数
int CCanControl::ReadEncodeCount(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x90,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}

//查询编码器超差
int CCanControl::ReadEncodeErrorLimit(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x91,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//查询编码器方向
int CCanControl::ReadEncodeDir(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x93,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//查询电机原始一圈步进
int CCanControl::ReadMotorCircleCount(UCHAR axis,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x92,TRUE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}

///////////////////////////
//写操作
int CCanControl::WriteBaud(Baud_TYPE Baud,UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x03,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=PackageBuffer((UINT)Baud,2,buff);
	if(rtnLen<2)
		return 0;
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	AxisMute0.Leave();
	return rtn;
}
int CCanControl::WriteDriverInfo(UCHAR axis,AxisDriverInfo drive,UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,axis,0x10,FALSE);
	FASSERT(frame.AxisID>0);
	FASSERT(frame.AxisID<=3);
	UCHAR buff[8]={0};
	UINT rtnLen=drive.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;
	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_DriverInfo[axis-1]=drive;
	}
	return rtn;
}

int  CCanControl::LockMotor(BOOL BLock,UCHAR axis,BOOL BALL,UINT WaitTM)
{
	FrameIDInfo frame(BALL?0x7f:m_ControlID,axis,0x11,FALSE);
	UCHAR buff[8]={0};
	buff[0]=BLock?1:0;
	int rtn=SendCanAndWait(frame,buff,2,WaitTM);
	return rtn;
}

int CCanControl::WriteLockCurrent(UCHAR axis,LockCurrentInfo lock,UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,axis,0x12,FALSE);
	FASSERT(frame.AxisID>0);
	FASSERT(frame.AxisID<=3);
	UCHAR buff[8]={0};
	UINT rtnLen=lock.PackageBuffer(buff);
	if(rtnLen<2)
		return 0;
	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_LockInfo[axis-1]=lock;
	}
	return rtn;
}
//设置通用IO有效值电平，0低有效，1高有效，第一步设置
int CCanControl::WriteInputValid(UINT WaitTM)
{
	//输入端口有效电平0x21
	FrameIDInfo frame(m_ControlID,0,0x21,FALSE);
	AxisMute0.Enter();
	UCHAR buff[8]={0};
	UINT rtnLen=m_InputLevel.PackageBufferForX21(buff);
	if(rtnLen<4)
		return 0;
//	buff[0]=0xff;
//	buff[1]=0x0f;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	AxisMute0.Leave();
	if(rtn<0)
		return rtn;
	return rtn;
}
//设置IO中断触发电平
int CCanControl::WriteInputInterrupt(UCHAR axis,IO_INTER_TYPE type,BOOL BInterrupt,Level_TYPE InterLevel,UCHAR INPUT,BOOL send,UINT WaitTM)
{
	Enable_TYPE enabled=BInterrupt?Enable:Disable;
	AxisMute0.Enter();
	switch(type)
	{
		case EmptyAllInter:
			{
				BInterrupt=FALSE;
				InputInterEnableInfo emptye;
				InputValidLevel emptyl;
				m_InputInterruptEnable=emptye;
				for(int i=0;i<12;i++)
					m_InputInterruptLevel.InputLevel[i]=InterLevel;
			}
			break;
		case EmptyInter:
			BInterrupt=FALSE;
			m_InputInterruptEnable.SetAxisInput(axis,Disable,Disable,Disable,Disable);
			m_InputInterruptLevel.SetAxisInput(axis,InterLevel,InterLevel,InterLevel,InterLevel);
			break;
		case OrgInter:
			if(1==axis)
			{
				m_InputInterruptEnable.InputEnable[11]=enabled;
				m_InputInterruptLevel.InputLevel[11]=InterLevel;
			}
			else if(2==axis)
			{
				m_InputInterruptEnable.InputEnable[8]=enabled;
				m_InputInterruptLevel.InputLevel[8]=InterLevel;
			}
			else if(3==axis)
			{
				m_InputInterruptEnable.InputEnable[5]=enabled;
				m_InputInterruptLevel.InputLevel[5]=InterLevel;
			}
			else
				FASSERT(FALSE);
			break;
		case LeftLimitInter:
			if(1==axis)
			{
				m_InputInterruptEnable.InputEnable[10]=enabled;
				m_InputInterruptLevel.InputLevel[10]=InterLevel;
			}
			else if(2==axis)
			{
				m_InputInterruptEnable.InputEnable[7]=enabled;
				m_InputInterruptLevel.InputLevel[7]=InterLevel;
			}
			else if(3==axis)
			{
				m_InputInterruptEnable.InputEnable[4]=enabled;
				m_InputInterruptLevel.InputLevel[4]=InterLevel;
			}
			else
				FASSERT(FALSE);
			break;
		case RightLimitInter:
			if(1==axis)
			{
				m_InputInterruptEnable.InputEnable[9]=enabled;
				m_InputInterruptLevel.InputLevel[9]=InterLevel;
			}
			else if(2==axis)
			{
				m_InputInterruptEnable.InputEnable[6]=enabled;
				m_InputInterruptLevel.InputLevel[6]=InterLevel;
			}
			else if(3==axis)
			{
				m_InputInterruptEnable.InputEnable[3]=enabled;
				m_InputInterruptLevel.InputLevel[3]=InterLevel;
			}
			else
				FASSERT(FALSE);
			break;
		case GenInter:
			FASSERT(INPUT>0);
			FASSERT(INPUT<4);
			INPUT-=1;
			m_InputInterruptEnable.InputEnable[INPUT]=enabled;
			m_InputInterruptLevel.InputLevel[INPUT]=InterLevel;
			break;
		default:
			FASSERT(FALSE);
			break;
	}

	if(!send)
	{
		AxisMute0.Leave();
		return 0;
	}

	//输入端口IO中断触发电平0x28
	FrameIDInfo frame(m_ControlID,0,0x28,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=m_InputInterruptLevel.PackageBufferForX21(buff);
	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn<0)
	{
		AxisMute0.Leave();
		return rtn;
	}
	if(BInterrupt)
		SetListen(axis,BInterrupt);

	FrameIDInfo frame1(m_ControlID,0,0x27,FALSE);
	rtnLen=m_InputInterruptEnable.PackageBuffer(buff);
	rtn=SendCanAndWait(frame1,buff,rtnLen,WaitTM);
	AxisMute0.Leave();
	return rtn;
}

//限位点/原点/探针有效电平设置寄存器,0x25
int CCanControl::WriteLimitValid(UCHAR axis,LimitOrgLevel &input,UINT WaitTM)
{
	//限位点/原点/探针有效电平设置寄存器,0x25
	FrameIDInfo frame1(m_ControlID,0,0x25,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=input.PackageBufferForX25(buff);
	if(rtnLen<2)
		return 0;
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame1,buff,rtnLen,WaitTM);
	AxisMute0.Leave();
	return rtn;
}

int CCanControl::WriteLimitOrgLevel(UCHAR axis,LIMIT_LEVEL_TYPE type,Level_TYPE level,UINT WaitTM)
{
	AxisMute0.Enter();
	switch(type)
	{
		case OrgLevel:
			if(1==axis)
			{
				m_LimitLevel.LimitLevel[0]=level;
			}
			else if(2==axis)
			{
				m_LimitLevel.LimitLevel[4]=level;
			}
			else if(3==axis)
			{
				m_LimitLevel.LimitLevel[8]=level;
			}
			else
				FASSERT(FALSE);
			break;
		case LeftLimitLevel:
			if(1==axis)
			{
				m_LimitLevel.LimitLevel[1]=level;
			}
			else if(2==axis)
			{
				m_LimitLevel.LimitLevel[5]=level;
			}
			else if(3==axis)
			{
				m_LimitLevel.LimitLevel[9]=level;
			}
			else
				FASSERT(FALSE);
			break;
		case RightLimitLevel:
			if(1==axis)
			{
				m_LimitLevel.LimitLevel[2]=level;
			}
			else if(2==axis)
			{
				m_LimitLevel.LimitLevel[6]=level;
			}
			else if(3==axis)
			{
				m_LimitLevel.LimitLevel[10]=level;
			}
			else
				FASSERT(FALSE);
			break;
		default:
			FASSERT(FALSE);
			break;
	}
	//限位点/原点/探针有效电平设置寄存器,0x25
	FrameIDInfo frame1(m_ControlID,0,0x25,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=m_LimitLevel.PackageBufferForX25(buff);
	int rtn=SendCanAndWait(frame1,buff,rtnLen,WaitTM);
	AxisMute0.Leave();
	return rtn;

}

int CCanControl::WriteLimitOrgEnable(UCHAR axis,LIMIT_EN_TYPE type,BOOL enable,BOOL send,UINT WaitTM)
{
	Enable_TYPE enabled=enable?Enable:Disable;
	AxisMute0.Enter();
	switch(type)
	{
		case EmptyStop:
			m_LimitOrgEnable.InputEnable[(axis-1)*4+0]=enabled;
			m_LimitOrgEnable.InputEnable[(axis-1)*4+1]=enabled;
			m_LimitOrgEnable.InputEnable[(axis-1)*4+3]=enabled;
			break;
		case OrgStop:
			m_LimitOrgEnable.InputEnable[(axis-1)*4+0]=enabled;
			break;
		case LimitStop:
			m_LimitOrgEnable.InputEnable[(axis-1)*4+1]=enabled;
			break;
		case EncodeZStop:
			m_LimitOrgEnable.InputEnable[(axis-1)*4+2]=enabled;
			break;
		case LiquidStop:
			m_LimitOrgEnable.InputEnable[(axis-1)*4+3]=enabled;
			break;
		case BarcodeStop:
			if(enable)
			{
				//如果条码打开，则必须把Z原点,Z限位关闭
				m_LimitOrgEnable.InputEnable[12]=Enable;
				m_LimitOrgEnable.InputEnable[8]=Disable;
				m_LimitOrgEnable.InputEnable[9]=Disable;
			}
			else
			{
				m_LimitOrgEnable.InputEnable[12]=Disable;
			}
			break;
		default:
			FASSERT(FALSE);
			break;
	}
	if(!send)
	{
		AxisMute0.Leave();
		return 0;
	}
	FrameIDInfo frame(m_ControlID,0,0x23,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=m_LimitOrgEnable.PackageBuffer(buff);
	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	AxisMute0.Leave();
	return rtn;
}

//outPutIndex=(0~3),BSet=TRUE高电平，FALSE 低电平
int CCanControl::WriteOutput(UCHAR outPutIndex,BOOL BHIGH,UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x24,FALSE);
	AxisMute0.Enter();
	UINT nowOut=m_OutPutLevel;
	FASSERT(outPutIndex<=3);
	//根据现在的输出推出变化后的电平掩码
	if(BHIGH)
	{
		UINT TEMP=(1<<outPutIndex);
		nowOut&=~TEMP;
	}
	else
	{
		nowOut|=(1<<outPutIndex);
	}
	UCHAR buff[8]={0};
	buff[0]=nowOut;
	int rtn=SendCanAndWait(frame,buff,4,WaitTM);
	if(rtn>=0)
	{
		m_OutPutLevel=nowOut;
	}
	AxisMute0.Leave();
	return rtn;
}
int CCanControl::WriteAllOutput(UCHAR ctrlID,UCHAR outPutIndex,BOOL BHIGH,UINT WaitTM)
{
	FASSERT(outPutIndex<=3);
	CCanControl *outControl;
	outControl=g_Can.GetControl(ctrlID);
	return outControl->WriteOutput(outPutIndex,BHIGH,WaitTM);
}
int  CCanControl::WriteDefaultOutput(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x24,FALSE);
	UCHAR buff[8]={0};
	buff[0]=0x0f;
	AxisMute0.Enter();
	int rtn=SendCanAndWait(frame,buff,4,WaitTM);
	if(rtn>=0)
	{
		m_OutPutLevel=0X0F;
	}
	AxisMute0.Leave();
	return rtn;
}
//写原点对应动作
int CCanControl::WriteOrgAcion(OrgActionInfo Action,UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x26,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=Action.PackageBuffer(buff);
	if(rtnLen<2)
		return 0;
	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_OrgAction=Action;
	}
	return rtn;
}

//设置当前位置
int CCanControl::WriteMotorPos(UCHAR axis,POS_TYPE postype,int POS,UINT WaitTM)
{
	FASSERT(RELATIVE_POS!=postype);
	if(ENCODE_POS==postype)
		FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,(ABSOLUTE_POS==postype)?0x40:0x41,FALSE);

	PosInfo posinfo;
	posinfo.POS=POS;

	UCHAR buff[8]={0};
	UINT rtnLen=posinfo.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		switch(postype)
		{
			case  ABSOLUTE_POS:
				m_ABSPos[axis-1]=POS;
				break;
			case  ENCODE_POS:
				m_EncodePos[axis-1]=POS;
				break;
			default:
				FASSERT(FALSE);
				break;
		}
	}
	return rtn;
}

int CCanControl::WriteMotorParam(UCHAR axis,PosInfo pos,POS_TYPE postype,UINT WaitTM)
{
	FrameIDInfo frame;
	switch(postype)
	{
		case  ABSOLUTE_POS:
			 frame=FrameIDInfo(m_ControlID,axis,0x43,FALSE);
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=3);
			break;
		case  RELATIVE_POS:
			 frame=FrameIDInfo(m_ControlID,axis,0x44,FALSE);
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=3);
			break;
		case  ENCODE_POS:
			 frame=FrameIDInfo(m_ControlID,axis,0x45,FALSE);
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=2);
			break;
		default:
			FASSERT(FALSE);
			break;
	}
	UCHAR buff[8]={0};
	UINT rtnLen=pos.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		switch(postype)
		{
			case  ABSOLUTE_POS:
				m_Param[axis-1].AbsPos[pos.Group]=pos;
				break;
			case  RELATIVE_POS:
				m_Param[axis-1].RelaPos[pos.Group]=pos;
				break;
			case  ENCODE_POS:
				m_Param[axis-1].EncodePos[pos.Group]=pos;
				break;
			default:
				FASSERT(FALSE);
				break;
		}
	}
	return rtn;
}
int CCanControl::WriteMotorParam(UCHAR axis,SpeedInfo speed,BOOL BHigh,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame;
	if(BHigh)
	{
		 frame=FrameIDInfo(m_ControlID,axis,0x46,FALSE);
	}
	else
	{
		 frame=FrameIDInfo(m_ControlID,axis,0x47,FALSE);
	}
	UCHAR buff[8]={0};
	UINT rtnLen=speed.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		if(BHigh)
		{
			m_Param[axis-1].SpeedHigh[speed.Group]=speed;
		}
		else
		{
			m_Param[axis-1].SpeedLow[speed.Group]=speed;
		}
	}
	return rtn;
}
int CCanControl::WriteMotorParam(UCHAR axis,AccInfo ACC,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x48,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=ACC.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_Param[axis-1].AccSpeed[ACC.Group]=ACC;
	}
	return rtn;
}
int CCanControl::WriteMotorParam(UCHAR axis,MoveInfo Mode,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x49,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=Mode.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_MoveMode[axis-1]=Mode;
	}
	return rtn;
}

int CCanControl::RunMotor(UCHAR axis,UCHAR &SN,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x50,FALSE);
	UCHAR buff[8]={0};
	buff[0]=1;
	buff[1]=SN;
	SN+=2;
	m_pMotor[axis-1]->FinishSemp.ResetEvent();
	NotifyInfo empty;
	m_RcvNotify[frame.AxisID-1]=empty;
	m_MotorListen[frame.AxisID-1]=TRUE;
	m_PendFinishID[frame.AxisID-1]=0;
#ifdef _SIMULATOR_
	SimulateCallBack(axis);
#endif
	int rtn=SendCanAndWait(frame,buff,4,WaitTM);
	return rtn;
}

int CCanControl::StopMotor(UCHAR axis,UCHAR &SN,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x51,FALSE);
	UCHAR buff[8]={0};
	buff[0]=1;
	buff[1]=SN;
	SN+=2;
	int rtn=SendCanAndWait(frame,buff,4,WaitTM);
	return rtn;
}

int CCanControl::UrgentStopMotor(UCHAR axis,UCHAR &SN,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x52,FALSE);
	UCHAR buff[8]={0};
	buff[0]=1;
	buff[1]=SN;
	SN+=2;
	int rtn=SendCanAndWait(frame,buff,4,WaitTM);
	return rtn;
}
int CCanControl::WriteLiquidDec(UCHAR axis,UINT DEC,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x54,FALSE);
	UCHAR buff[8]={0};
	AccInfo ACC;
	ACC.Acc=DEC;
	UINT rtnLen=ACC.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_LiqDec[axis-1]=DEC;
	}
	return rtn;
}
//设置急停减速度
int CCanControl::WriteUrgentStopDec(UCHAR axis,UINT DEC,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x68,FALSE);
	UCHAR buff[8]={0};
	AccInfo ACC;
	ACC.Acc=DEC;
	UINT rtnLen=ACC.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_StopDec[axis-1]=DEC;
	}
	return rtn;
}

//设置通知信号有效位
int CCanControl::WriteNotifyInfo(UCHAR axis,NotifyInfo notify,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x69,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=notify.PackageBuffer(buff);
	if(rtnLen<4)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		m_Notify[axis-1]=notify;
	}
	return rtn;
}

//设置指定站点指定轴的脉冲模式
int CCanControl::WritePWMType(UCHAR axis,PWM_TYPE type,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=3);
	FrameIDInfo frame(m_ControlID,axis,0x6a,FALSE);
	UCHAR buff[8]={0};
	buff[0]=(UCHAR)type;

	int rtn=SendCanAndWait(frame,buff,2,WaitTM);
	if(rtn>=0)
	{
		m_PWMType[axis-1]=type;
	}
	return rtn;
}

//设置全部参数
int CCanControl::WriteAllParam(UINT WaitTM)
{
	FrameIDInfo frame(m_ControlID,0,0x80,FALSE);
	UCHAR buff[1024]={0};
	UINT rtnLen=PackageBuffer(buff);
	if(rtnLen<718)
		return 0;

	int rtn=SendCanAndWait(frame,buff,rtnLen,WaitTM);
	return rtn;
}
//所有轴驱动电路禁止
int CCanControl::DisAllMotor(UINT WaitTM)
{
	FrameIDInfo frame(0x7f,0,0x81,FALSE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//所有站点所有轴当前位置为原点
int CCanControl::ResetAllMotorPos(UINT WaitTM)
{
	FrameIDInfo frame(0x7f,0,0x82,FALSE);
	int rtn=SendCanAndWait(frame,NULL,0,WaitTM);
	return rtn;
}
//设置编码器线数
int CCanControl::WriteEncodeCount(UCHAR axis,UINT count,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x90,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=PackageBuffer((UINT)count,4,buff);
	if(rtnLen<4)
		return 0;
	int rtn= SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
		if(1==axis)
			m_XEncodeCnt=count;
		else
			m_YEncodeCnt=count;
	}
	return rtn;
}

//设置编码器超差
int CCanControl::WriteEncodeErrorLimit(UCHAR axis,UINT count,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x91,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=PackageBuffer((UINT)count,4,buff);
	if(rtnLen<4)
		return 0;
	int rtn= SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
	}
	return rtn;
}

//设置编码器方向
int CCanControl::WriteEncodeDir(UCHAR axis,UINT DIR,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x93,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=PackageBuffer((UINT)DIR,4,buff);
	if(rtnLen<4)
		return 0;
	int rtn= SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
	}
	return rtn;
}
//设置电机原始一圈步进
int CCanControl::WriteMotorCircleCount(UCHAR axis,UINT count,UINT WaitTM)
{
	FASSERT(axis>0);
	FASSERT(axis<=2);
	FrameIDInfo frame(m_ControlID,axis,0x92,FALSE);
	UCHAR buff[8]={0};
	UINT rtnLen=PackageBuffer((UINT)count,4,buff);
	if(rtnLen<4)
		return 0;
	int rtn= SendCanAndWait(frame,buff,rtnLen,WaitTM);
	if(rtn>=0)
	{
	}
	return rtn;
}

///////////////////////////
int CCanControl::SendCanAndWait(FrameIDInfo frame,UCHAR *buff,int len,UINT WaitTM)
{

#ifdef _SIMULATOR_
	return 0;
#endif
	int rtn=0;
	int seg=0;
	int i=0;
	UINT ID=0;
	UINT len1=0;
	UCHAR *rcvbuff=NULL;

	g_Can.EmptyRcv(frame);

	if(len<8)
	{
		UINT ID=frame.PackageFrameID();
		rtn=g_Can.SendSingleFrame(ID,buff,len);
		if(rtn<0)
			return rtn;
	}
	else
	{
		seg=(len+7)/8;
		for(i=0;i<seg;i++)
		{
			frame.BContinue=(i<seg-1)?1:0;
			ID=frame.PackageFrameID();
			if(i<seg-1)
			{
				rtn=g_Can.SendSingleFrame(ID,buff,8);
				buff+=8;
			}
			else
			{
				//最后一包,buff无需变化
				rtn=g_Can.SendSingleFrame(ID,buff,(0==len%8)?8:len%8);
			}
			if(rtn<0)
				return rtn;
		}
	}
	if(0==WaitTM)
	{
		rtn=0;
		return rtn;
	}
	rtn=g_Can.WaitCanRcv(frame, WaitTM); // 等待命令回复，命令处理在 RarseFrameAry();
//	CanTraceLog("WaitCanRcv");
	if(rtn<0)
		return rtn;
	rcvbuff=g_Can.GetCanRcvBuffer(frame,len1,rtn); // rtn 1 
	if(rtn<0)
		return rtn;
	if(frame.BRead)
		rtn=ParseReadBuffer(frame,rcvbuff,len1);
	else
		rtn=ParseWriteBuffer(frame,rcvbuff,len1); // 不起作用
	delete []rcvbuff;

	return rtn;
}
int CCanControl::DealPend(WORD RCVID)
{
	for(int t=0;t<3;t++)
	{
		if(m_BNeedPendFinish[t]&&(m_PendFinishID[t]>0)&&(m_MotorListen[t]))
		{
			int offset=RCVID-m_PendFinishID[t];
			if((offset>3)||(offset<-3))
			{
				DealFinish(t);
			}
		}
	}
	return TRUE;
}
int CCanControl::DealFinish(UCHAR AxisID)
{
	FASSERT(AxisID<3);
	//CanTraceLog(_T("DealFinish CTRL_ID=%d,AXIS=%d"),m_ControlID,AxisID);
	//取消监听
	m_PendFinishID[AxisID]=0;
	m_MotorListen[AxisID]=FALSE;
	m_RcvNotify[AxisID].BFinishNotify=1;
	m_pMotor[AxisID]->CallBackActFinish(m_RcvNotify[AxisID]);//停止通知
	return TRUE;
}

int CCanControl::ParseNotifyBuffer(FrameIDInfo frame,UCHAR *buff,UINT Len)
{
	switch(frame.TCW)
	{
		case 0xf0:		//接收到的通知
			{
				//通用IO中断
				if(0==frame.AxisID)
				{
					InputInterEnableInfo tmpInputInter;
					tmpInputInter.ReadBuffer(buff);
					NotifyInfo tempNotify;
					UCHAR SN;
					if(tmpInputInter.GetAxisSignal(SN,tempNotify))
					{
						if(NULL!=m_pMotor[SN])
						{
							m_pMotor[SN]->CallBackRcvNotify(tempNotify);
							CanTraceLog(_T("Monitor IO=%d"),frame.TCW);
						}
					}
					return 0;
				}
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);

				NotifyInfo tempNotify;
				tempNotify.ReadBuffer(buff);
				//PMC运动停止原因不区分左还是右限位
				if(tempNotify.BLeftLimitNotify)
					tempNotify.BRightLimitNotify=TRUE;

				if(tempNotify.BFinishNotify)
				{
					if(m_MotorListen[frame.AxisID-1]&&(NULL!=m_pMotor[frame.AxisID-1]))
					{
						if(m_BNeedPendFinish[frame.AxisID-1])
						{
							//收到FINISH,标记好
							m_PendFinishID[frame.AxisID-1]=g_Can.m_recvcount;
							//CanTraceLog(_T("3ParseNotifyBuffer CTRL_ID=%d,AXIS=%d,TCW=%d"),m_ControlID,frame.AxisID,frame.TCW);
						}
						else
						{
							DealFinish(frame.AxisID-1);
							//CanTraceLog(_T("1ParseNotifyBuffer CTRL_ID=%d,AXIS=%d,TCW=%d"),m_ControlID,frame.AxisID,frame.TCW);
						}
					}
				}
				else
				{
					m_RcvNotify[frame.AxisID-1]=tempNotify;
					if(m_MotorListen[frame.AxisID-1]&&(NULL!=m_pMotor[frame.AxisID-1]))
					{
						//CanTraceLog(_T("2ParseNotifyBuffer CTRL_ID=%d,AXIS=%d,TCW=%d"),m_ControlID,frame.AxisID,frame.TCW);
					}
				}
			}
			break;
		default:
			break;
	}
	return 0;
}
int CCanControl::ParseWriteBuffer(FrameIDInfo frame,UCHAR *buff,UINT Len)
{
	UINT TEMP=0;
	return	TEMP;
}
int CCanControl::ParseReadBuffer(FrameIDInfo frame,UCHAR *buff,UINT Len)
{

	UINT TEMP=0;
	switch(frame.TCW)
	{
		case 0x3:		//波特率
			ReadBuffer(TEMP,2,buff);
			m_Baud=(Baud_TYPE)TEMP;
			break;
		case 0x10:		//细分及电流
			FASSERT(frame.AxisID>0);
			FASSERT(frame.AxisID<=3);
			m_DriverInfo[frame.AxisID-1].ReadBuffer(buff);
			break;
		case 0x21:		//输入有效电平
			m_InputLevel.ReadBuffer(buff);
			break;
		case 0x22:		//当前输入是否有效
			ReadBuffer(TEMP,4,buff);
			TEMP&=0xffff;
			break;
		case 0x23:		//输入使能
			m_LimitOrgEnable.ReadBuffer(buff);
			break;
		case 0x24:		//输出电平
			ReadBuffer(TEMP,4,buff);
			TEMP&=0xF;
			m_OutPutLevel=TEMP;
			break;
		case 0x25:		//原点限位有效电平
			m_LimitLevel.ReadBuffer(buff);
			break;
		case 0x26:		//原点对应动作
			m_OrgAction.ReadBuffer(buff);
			break;
		case 0x27:		//IO口监控信息
			m_InputInterruptEnable.ReadBuffer(buff);
			break;
		case 0x28:		//IO口监控信息
			m_InputInterruptLevel.ReadBuffer(buff);
			break;
		case 0x40:		//当前绝对位置
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				PosInfo posinfo;
				posinfo.ReadBuffer(buff);
				m_ABSPos[frame.AxisID-1]=posinfo.POS;
			}
			break;
		case 0x41:		//当前编码器位置
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=2);
				PosInfo posinfo;
				posinfo.ReadBuffer(buff);
				m_EncodePos[frame.AxisID-1]=posinfo.POS;
			}
			break;
		case 0x42:		//当前速度
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				SpeedInfo speed;
				speed.ReadBuffer(buff);
				m_CurrentSpeed[frame.AxisID-1]=speed.Speed;
				TEMP=speed.Speed;
			}
			break;
		case 0x43:		//电机绝对目标位置的10组参数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				PosInfo posinfo;
				posinfo.ReadBuffer(buff);
				m_Param[frame.AxisID-1].AbsPos[posinfo.Group]=posinfo;
			}
			break;
		case 0x44:		//电机增量目标位置的10组参数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				PosInfo posinfo;
				posinfo.ReadBuffer(buff);
				m_Param[frame.AxisID-1].RelaPos[posinfo.Group]=posinfo;
			}
			break;
		case 0x45:		//电机编码器目标位置的10组参数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=2);
				PosInfo posinfo;
				posinfo.ReadBuffer(buff);
				m_Param[frame.AxisID-1].EncodePos[posinfo.Group]=posinfo;
			}
			break;
		case 0x46:		//电机最高速的10组参数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				SpeedInfo speed;
				speed.ReadBuffer(buff);
				m_Param[frame.AxisID-1].SpeedHigh[speed.Group]=speed;
			}
			break;
		case 0x47:		//电机最低速的10组参数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				SpeedInfo speed;
				speed.ReadBuffer(buff);
				m_Param[frame.AxisID-1].SpeedLow[speed.Group]=speed;
			}
			break;
		case 0x48:		//电机加速度的10组参数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				AccInfo Acc;
				Acc.ReadBuffer(buff);
				m_Param[frame.AxisID-1].AccSpeed[Acc.Group]=Acc;
			}
			break;
		case 0x49:		//运动模式
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				MoveInfo Move;
				Move.ReadBuffer(buff);
				m_MoveMode[frame.AxisID-1]=Move;
			}
			break;
		case 0x50:		//运行应答
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
			}
			break;
		case 0x53:		//电机是否运行中
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				ReadBuffer(TEMP,2,buff);
				TEMP&=0x1;
				m_MotorRunning[frame.AxisID-1]=(TEMP)?TRUE:FALSE;
				if((FALSE==m_MotorRunning[frame.AxisID-1])&&(NULL!=m_pMotor[frame.AxisID-1]))
				{
					if(m_MotorListen[frame.AxisID-1]&&(NULL!=m_pMotor[frame.AxisID-1]))
					{
						//取消监听
						m_MotorListen[frame.AxisID-1]=FALSE;
					}
				}
			}
			break;
		case 0x54:		//探针减速度
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				ReadBuffer(m_LiqDec[frame.AxisID-1],4,buff);
				TEMP=m_LiqDec[frame.AxisID-1];
			}
			break;
		case 0x68:		//急停减速度
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				ReadBuffer(m_StopDec[frame.AxisID-1],4,buff);
				TEMP=m_StopDec[frame.AxisID-1];
			}
			break;
		case 0x69:		//原点限位停止通知
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=3);
				m_Notify[frame.AxisID-1].ReadBuffer(buff);
			}
			break;
		case 0x80:		//全部参数
			{
				ReadBuffer(buff);
			}
			break;
		case 0x90:		//编码器线数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=2);
				ReadBuffer(TEMP,4,buff);
				if(0==frame.AxisID)
					m_XEncodeCnt=TEMP;
				else
					m_YEncodeCnt=TEMP;
			}
			break;
		case 0x91:		//编码器超差
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=2);
				ReadBuffer(TEMP,4,buff);
			}
			break;
		case 0x92:		//电机一圈脉冲数
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=2);
				ReadBuffer(TEMP,4,buff);
			}
			break;
		case 0x93:		//编码器方向
			{
				FASSERT(frame.AxisID>0);
				FASSERT(frame.AxisID<=2);
				ReadBuffer(TEMP,4,buff);
			}
			break;
		default:
			break;
	}
	return	TEMP;
}
///////////////////////////
int CCanControl::PackageBuffer(UCHAR *buff)
{
	int AllLen=0;
	int rtnLen=0;
	int i;

	for(i=0;i<3;i++)
	{
		rtnLen=m_DriverInfo[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	rtnLen=m_InputLevel.PackageBufferForX21(buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	rtnLen=m_LimitOrgEnable.PackageBuffer(buff);
	if(rtnLen<2)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;
	for(i=0;i<2;i++)
	{
		rtnLen=m_Param[i].PackageBuffer(buff);
		if(rtnLen<240)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	rtnLen=m_Param[2].PackageBuffer(buff);
	if(rtnLen<200)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	for(i=0;i<3;i++)
	{
		rtnLen=PackageBuffer(m_LiqDec[i],4,buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}

	for(i=0;i<3;i++)
	{
		rtnLen=PackageBuffer(m_StopDec[i],4,buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}

	for(i=0;i<3;i++)
	{
		rtnLen=m_Notify[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}

	rtnLen=PackageBuffer(m_XEncodeCnt,4,buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	rtnLen=PackageBuffer(m_YEncodeCnt,4,buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;
	return AllLen;
}
int CCanControl::ReadBuffer(UCHAR *buff)
{
	int AllLen=0;
	int rtnLen=0;
	int i;

	rtnLen=ReadBuffer(m_ControlID,2,buff);
	if(rtnLen<2)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	rtnLen=ReadBuffer(m_ControlVer,4,buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	UINT TEMP;
	rtnLen=ReadBuffer(TEMP,2,buff);
	m_Baud=(Baud_TYPE)TEMP;
	if(rtnLen<2)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	for(i=0;i<3;i++)
	{
		rtnLen=m_DriverInfo[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	rtnLen=m_InputLevel.ReadBuffer(buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	rtnLen=m_LimitOrgEnable.ReadBuffer(buff);
	if(rtnLen<2)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;
	for(i=0;i<2;i++)
	{
		rtnLen=m_Param[i].ReadBuffer(buff);
		if(rtnLen<240)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	rtnLen=m_Param[2].ReadBuffer(buff);
	if(rtnLen<200)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	for(i=0;i<3;i++)
	{
		rtnLen=ReadBuffer(m_LiqDec[i],4,buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}

	for(i=0;i<3;i++)
	{
		rtnLen=ReadBuffer(m_StopDec[i],4,buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}

	for(i=0;i<3;i++)
	{
		rtnLen=m_Notify[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}

	rtnLen=ReadBuffer(m_XEncodeCnt,4,buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;

	rtnLen=ReadBuffer(m_YEncodeCnt,4,buff);
	if(rtnLen<4)
		return 0;
	buff+=rtnLen;
	AllLen+=rtnLen;
	return AllLen;
}
int CCanControl::PackageBuffer(UINT date,int len,UCHAR *buff)
{
	for(int i=0;i<len;i++)
	{
		buff[i]=date&0xff;
		date>>=8;
	}
	return len;
}
int CCanControl::ReadBuffer(UINT &date,int len,UCHAR *buff)
{
	date=0;
	UINT temp;
	for(int i=0;i<len;i++)
	{
		temp=buff[i];
		temp<<=8*i;
		date+=temp;
	}
	return len;
}
/////////////////////////////////////////////////////////////////////////////////
#ifdef _SIMULATOR_
/*
#define FINISH_ORG		1
#define FINISH_LEFT		2
#define FINISH_RIGHT		4
#define FINISH_LIQ		8
#define FINISH_STEP		16
*/
BOOL CCanControl::SimulateCallBack(BYTE AxisID)
{
	UINT ID=m_ControlID;
	ID<<=8;
	ID+=AxisID;

//	TRACE(_T("CCanControl::SimulateCallBack Add ID=%d"),ID);
	g_Can.m_CallBackLock.Enter();
	g_Can.m_WaitCallBack.Add(ID);
	g_Can.m_CallBackLock.Leave();
	return TRUE;
}

BOOL CCanControl::CallBackMotor(BYTE AxisID)
{
	NotifyInfo 	RcvNotify;
	RcvNotify.BFinishNotify=TRUE;
	//取消监听
	m_MotorListen[AxisID-1]=FALSE;
	m_PendFinishID[AxisID-1]=0;
	m_RcvNotify[AxisID-1].BFinishNotify=1;
	m_pMotor[AxisID-1]->CallBackActFinish(RcvNotify);
	return TRUE;
}
BOOL CCanControl::SimulateCallBack(UINT MASK,CCanMotor *pMotor1)
{
	NotifyInfo 	RcvNotify;
	if(MASK&FINISH_ORG)
		RcvNotify.BOrgNotify=TRUE;
	if(MASK&FINISH_LEFT)
		RcvNotify.BLeftLimitNotify=TRUE;
	if(MASK&FINISH_RIGHT)
		RcvNotify.BRightLimitNotify=TRUE;
	if(MASK&FINISH_LIQ)
		RcvNotify.BLiqidNotify=TRUE;
	if(MASK&FINISH_STEP)
		RcvNotify.BFinishNotify=TRUE;

	return TRUE;
}
#endif
