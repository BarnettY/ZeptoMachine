// AxisParamInfo.cpp: implementation of the CAxisParamInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "AxisParamInfo.h"
#include "../trace.h"


//////////////////////////////////////////////////////////////////////
// class PosInfo
//////////////////////////////////////////////////////////////////////
BOOL PosInfo::VerifyData()
{
	//FASSERT(POS<=0XFFFFFF);
	if(POS>0X7FFFFF) return FALSE;
	FASSERT(Group<=9);
	if(Group>9) return FALSE;
	return TRUE;	
}
int PosInfo::PackageBuffer(UCHAR *buff)
{
	if(!VerifyData()) return FALSE;
	
	UINT itemp=POS;
	UCHAR chtemp=itemp&0xff;
	buff[0]=chtemp;
	chtemp=(itemp>>8)&0xff;
	buff[1]=chtemp;
	chtemp=(itemp>>16)&0xff;
	buff[2]=chtemp;
	
	chtemp=Group;
	//BIT28加写保护位
	if(BLock)
		chtemp|=0x10;
	buff[3]=chtemp;
	return 4;	
}
int PosInfo::ReadBuffer(UCHAR *buff)
{
	BLock=FALSE;
	POS=0;

	UINT TEMP=0;
	//BIT23 符号位
	if(buff[2]&0x80)
	{
		TEMP=0XFF;
	}
	TEMP<<=8;
	TEMP+=buff[2];
	TEMP<<=8;
	TEMP+=buff[1];
	TEMP<<=8;
	TEMP+=buff[0];
	POS=TEMP;

	UCHAR chtemp=buff[3];
	UCHAR Group1=chtemp&0x0f;
	FASSERT(Group1==Group);
	if(chtemp&0x10)
		BLock=TRUE;
	if(!VerifyData()) return FALSE;
	if(Group1!=Group)
		return 0;
	return 4;	
}

//////////////////////////////////////////////////////////////////////
// class SpeedInfo
//////////////////////////////////////////////////////////////////////
BOOL SpeedInfo::VerifyData()
{
	FASSERT(Speed<=0XFFFFFF);
	if(Speed>0XFFFFFF) return FALSE;
	FASSERT(Group<=9);
	if(Group>9) return FALSE;
	return TRUE;	
}
int SpeedInfo::PackageBuffer(UCHAR *buff)
{
	if(!VerifyData()) return FALSE;
	UINT itemp=Speed;
	UCHAR chtemp=itemp&0xff;
	buff[0]=chtemp;
	chtemp=(itemp>>8)&0xff;
	buff[1]=chtemp;
	chtemp=(itemp>>16)&0xff;
	buff[2]=chtemp;
	chtemp=Group;
	//BIT28加写保护位
	if(BLock)
		chtemp|=0x10;
	buff[3]=chtemp;
	return 4;	
}
int SpeedInfo::ReadBuffer(UCHAR *buff)
{
	BLock=FALSE;
	Speed=0;

	int itemp=0;	
	itemp=buff[0];
	Speed=itemp;
	itemp=buff[1];
	Speed+=(itemp<<8);
	itemp=buff[2];
	Speed+=(itemp<<16);
	
	UCHAR chtemp=buff[3];
	UCHAR Group1=chtemp&0x0f;
	FASSERT(Group1==Group);
	if(chtemp&0x10)
		BLock=TRUE;
	if(!VerifyData()) return FALSE;
	if(Group1!=Group)
		return 0;
	return 4;	
}

//////////////////////////////////////////////////////////////////////
// class AccInfo
//////////////////////////////////////////////////////////////////////
BOOL AccInfo::VerifyData()
{
	FASSERT(Acc<=0XFFFFFF);
	if(Acc>0XFFFFFF) return FALSE;
	FASSERT(Group<=9);
	if(Group>9) return FALSE;
	return TRUE;	
}
int AccInfo::PackageBuffer(UCHAR *buff)
{
	if(!VerifyData()) return FALSE;
	int itemp=Acc;
	UCHAR chtemp=itemp&0xff;
	buff[0]=chtemp;
	chtemp=(itemp>>8)&0xff;
	buff[1]=chtemp;
	chtemp=(itemp>>16)&0xff;
	buff[2]=chtemp;
	chtemp=Group;
	//BIT28加写保护位
	if(BLock)
		chtemp|=0x10;
	buff[3]=chtemp;
	return 4;	
}
int AccInfo::ReadBuffer(UCHAR *buff)
{
	BLock=FALSE;
	Acc=0;

	int itemp=0;	
	itemp=buff[0];
	Acc=itemp;
	itemp=buff[1];
	Acc+=(itemp<<8);
	itemp=buff[2];
	Acc+=(itemp<<16);
	
	UCHAR chtemp=buff[3];
	UCHAR Group1=chtemp&0x0f;
	FASSERT(Group1==Group);
	if(chtemp&0x10)
		BLock=TRUE;
	if(!VerifyData()) return FALSE;
	if(Group1!=Group)
		return 0;
	return 4;	
}

//////////////////////////////////////////////////////////////////////
// class AxisParamInfo
//////////////////////////////////////////////////////////////////////
int AxisParamInfo::PackageBuffer(UCHAR *buff)
{
	int AllLen=0;
	int rtnLen=0;
	int i;
	
	for(i=0;i<9;i++)
	{
		rtnLen=AbsPos[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	for(i=0;i<9;i++)
	{
		rtnLen=RelaPos[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}	
	if(HaveEncode)
	{
		for(i=0;i<9;i++)
		{
			rtnLen=EncodePos[i].PackageBuffer(buff);
			if(rtnLen<4)
				return 0;
			buff+=rtnLen;
			AllLen+=rtnLen;
		}
	}
	for(i=0;i<9;i++)
	{
		rtnLen=SpeedHigh[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	for(i=0;i<9;i++)
	{
		rtnLen=SpeedLow[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}	
	for(i=0;i<9;i++)
	{
		rtnLen=AccSpeed[i].PackageBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}	
	return AllLen;
}
int AxisParamInfo::ReadBuffer(UCHAR *buff)
{
	int AllLen=0;
	int rtnLen=0;
	int i;
	
	for(i=0;i<9;i++)
	{
		rtnLen=AbsPos[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	for(i=0;i<9;i++)
	{
		rtnLen=RelaPos[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}	
	if(HaveEncode)
	{
		for(i=0;i<9;i++)
		{
			rtnLen=EncodePos[i].ReadBuffer(buff);
			if(rtnLen<4)
				return 0;
			buff+=rtnLen;
			AllLen+=rtnLen;
		}
	}
	for(i=0;i<9;i++)
	{
		rtnLen=SpeedHigh[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}
	for(i=0;i<9;i++)
	{
		rtnLen=SpeedLow[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}	
	for(i=0;i<9;i++)
	{
		rtnLen=AccSpeed[i].ReadBuffer(buff);
		if(rtnLen<4)
			return 0;
		buff+=rtnLen;
		AllLen+=rtnLen;
	}	
	return AllLen;
}
//////////////////////////////////////////////////////////////////////
// class AxisDriverInfo
//////////////////////////////////////////////////////////////////////
int AxisDriverInfo::PackageBuffer(UCHAR *buff)
{
	buff[0]=((UCHAR)Current)&0x1F;
	if(ModeA1_6)
		buff[0]|=0x20;
	buff[1]=(UCHAR)SubDiv;
	buff[2]=(UCHAR)LockMotor;
	return 4;	
}
int AxisDriverInfo::ReadBuffer(UCHAR *buff)
{
	UCHAR chtemp=buff[0];
	Current=(Current_TYPE)(chtemp&0X1F);
	ModeA1_6=(chtemp&0X20)?Enable:Disable;
	chtemp=buff[1];
	SubDiv=(SubDiv_TYPE)(chtemp&0XF);
	chtemp=buff[2]&0X1;
	LockMotor=chtemp?TRUE:FALSE;
	return 4;	
}
//////////////////////////////////////////////////////////////////////
// class LimitOrgLevel
//////////////////////////////////////////////////////////////////////
void LimitOrgLevel::SetAxisLimitOrgLevel(UCHAR index,Level_TYPE org,Level_TYPE leftlimit,Level_TYPE rightlimit,Level_TYPE liq)
{
	FASSERT(index>0);
	FASSERT(index<4);
	if(1==index)
	{
		LimitLevel[0]=org;
		LimitLevel[1]=leftlimit;
		LimitLevel[2]=rightlimit;
		LimitLevel[3]=liq;
	}
	else if(2==index)
	{
		LimitLevel[4]=org;
		LimitLevel[5]=leftlimit;
		LimitLevel[6]=rightlimit;
		LimitLevel[7]=liq;
	}	
	else if(3==index)
	{
		LimitLevel[8]=org;
		LimitLevel[9]=leftlimit;
		LimitLevel[10]=rightlimit;
		LimitLevel[11]=liq;
	}
	else
		FASSERT(FALSE);
}

//状态值	Bit11	Bit10	Bit9	Bit8		Bit7	Bit6		Bit5	Bit4		Bit3	Bit2		Bit1		Bit0
//功能	zLiq		保留	z+	zOrg		yLiq	保留	y+	yOrg		xLiq	保留	x+		XOrg	

int LimitOrgLevel::PackageBufferForX25(UCHAR *buff)
{
	UINT itemp=0;
	for(int i=11;i>=0;i--)
	{
		itemp<<=1;			
		if(LimitLevel[i])
			itemp+=1;
	}

	buff[0]=itemp&0xff;
	buff[1]=(itemp>>8)&0xff;
	return 2;
}

int LimitOrgLevel::ReadBuffer(UCHAR *buff)
{
	UINT itemp=buff[1];
	itemp<<=8;
	itemp+=buff[0];
	for(int i=0;i<12;i++)
	{
		LimitLevel[i]=(itemp&0x1)?LevelHigh:LevelLow;
		itemp>>=1;			
	}
	return 4;
}

//////////////////////////////////////////////////////////////////////
// class InputValidLevel
//////////////////////////////////////////////////////////////////////
void InputValidLevel::SetAxisInput(UCHAR index,Level_TYPE org,Level_TYPE left,Level_TYPE right,Level_TYPE liq)
{
	FASSERT(index>0);
	FASSERT(index<4);
	if(1==index)
	{
		InputLevel[11]=org;
		InputLevel[10]=left;
		InputLevel[9]=right;
		InputLevel[2]=liq;
	}
	else if(2==index)
	{
		InputLevel[8]=org;
		InputLevel[7]=left;
		InputLevel[6]=right;
		InputLevel[2]=liq;
	}	
	else if(3==index)
	{
		InputLevel[5]=org;
		InputLevel[4]=left;
		InputLevel[3]=right;
		InputLevel[2]=liq;
	}
	else
		FASSERT(FALSE);
}
//寄存器	Bit11	Bit10	Bit9	Bit8	Bit7	Bit6	Bit5	Bit4	Bit3	Bit2	Bit1	Bit0
//输入口	IN12		IN11		IN10	IN9	IN8	IN7	IN6	IN5	IN4	IN3	IN2	IN1
//定义	Xorg		X-		X+	Yorg	Y-	Y+	Zorg	Z-	Z+	LIQ	IN2	IN1
int InputValidLevel::PackageBufferForX21(UCHAR *buff)
{
	UINT itemp=0;
	for(int i=11;i>=0;i--)
	{
		if(LevelHigh==InputLevel[i])
		{
			itemp+=1;
		}
		if(i>0)
			itemp<<=1;			
	}
	buff[0]=itemp&0xff;
	buff[1]=(itemp>>8)&0xff;
	return 4;
}

int InputValidLevel::ReadBuffer(UCHAR *buff)
{
	UINT itemp=buff[1];
	itemp<<=8;
	itemp+=buff[0];
	for(int i=0;i<12;i++)
	{
		InputLevel[i]=(itemp&0x1)?LevelHigh:LevelLow;
		itemp>>=1;			
	}
	return 4;
}

//////////////////////////////////////////////////////////////////////
// class InputInterEnableInfo
//////////////////////////////////////////////////////////////////////
void InputInterEnableInfo::SetAxisInput(UCHAR index,Enable_TYPE org,Enable_TYPE left,Enable_TYPE right,Enable_TYPE liq)
{
	FASSERT(index>0);
	FASSERT(index<4);
	if(1==index)
	{
		InputEnable[11]=org;
		InputEnable[10]=left;
		InputEnable[9]=right;
		InputEnable[2]=liq;
	}
	else if(2==index)
	{
		InputEnable[8]=org;
		InputEnable[7]=left;
		InputEnable[6]=right;
		InputEnable[2]=liq;
	}	
	else if(3==index)
	{
		InputEnable[5]=org;
		InputEnable[4]=left;
		InputEnable[3]=right;
		InputEnable[2]=liq;
	}
	else
		FASSERT(FALSE);
}

//从IO口映射到轴信息
BOOL InputInterEnableInfo::GetAxisSignal(UCHAR &SN,NotifyInfo &tmpNotify)
{
	SN=0;
	if(InputEnable[0])
	{
		SN=1;		//8SWITCH使用
		tmpNotify.BFivePosNotify=TRUE;
		return TRUE;
	}
	
	if(InputEnable[2])
	{
		SN=0;		//RRV使用
		tmpNotify.BLiqidNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[3])
	{
		SN=2;
		tmpNotify.BRightLimitNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[4])
	{
		SN=2;
		tmpNotify.BLeftLimitNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[5])
	{
		SN=2;
		tmpNotify.BOrgNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[6])
	{
		SN=1;
		tmpNotify.BRightLimitNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[7])
	{
		SN=1;
		tmpNotify.BLeftLimitNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[8])
	{
		SN=1;
		tmpNotify.BOrgNotify=TRUE;
		return TRUE;
	}	
	if(InputEnable[9])
	{
		SN=0;
		tmpNotify.BRightLimitNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[10])
	{
		SN=0;
		tmpNotify.BLeftLimitNotify=TRUE;
		return TRUE;
	}
	if(InputEnable[11])
	{
		SN=0;
		tmpNotify.BOrgNotify=TRUE;
		return TRUE;
	}
	return FALSE;
}

//寄存器	Bit11	Bit10	Bit9	Bit8	Bit7	Bit6	Bit5	Bit4	Bit3	Bit2	Bit1	Bit0
//输入口	IN12		IN11		IN10	IN9	IN8	IN7	IN6	IN5	IN4	IN3	IN2	IN1
//定义	Xorg		X-		X+	Yorg	Y-	Y+	Zorg	Z-	Z+	LIQ	IN2	IN1
int InputInterEnableInfo::PackageBuffer(UCHAR *buff)
{
	UINT itemp=0;
	for(int i=11;i>=0;i--)
	{
		if(Enable==InputEnable[i])
		{
			itemp+=1;
		}
		if(i>0)
			itemp<<=1;			
	}
	buff[0]=itemp&0xff;
	buff[1]=(itemp>>8)&0xff;
	return 4;
}

int InputInterEnableInfo::ReadBuffer(UCHAR *buff)
{
	UINT itemp=buff[1];
	itemp<<=8;
	itemp+=buff[0];
	for(int i=0;i<12;i++)
	{
		InputEnable[i]=(itemp&0x1)?Enable:Disable;
		itemp>>=1;			
	}
	return 4;
}
//////////////////////////////////////////////////////////////////////
// class LimitOrgEnableInfo
//////////////////////////////////////////////////////////////////////
void LimitOrgEnableInfo::SetAxisInput(UCHAR index,Enable_TYPE org,Enable_TYPE limit,Enable_TYPE liq)
{
	FASSERT(index>0);
	FASSERT(index<4);
	InputEnable[4*(index-1)+0]=org;
	InputEnable[4*(index-1)+1]=limit;
	InputEnable[4*(index-1)+3]=liq;
}

int LimitOrgEnableInfo::PackageBuffer(UCHAR *buff)
{
	UINT itemp=0;
	for(int i=12;i>=0;i--)
	{
		if(Enable==InputEnable[i])
		{
			itemp+=1;
		}
		if(i>0)
			itemp<<=1;			
	}
	buff[0]=itemp&0xff;
	buff[1]=(itemp>>8)&0xff;
	return 2;
}
int LimitOrgEnableInfo::ReadBuffer(UCHAR *buff)
{
	UINT itemp=buff[1];
	itemp<<=8;
	itemp+=buff[0];
	for(int i=0;i<13;i++)
	{
		InputEnable[i]=(itemp&0x1)?Enable:Disable;
		itemp>>=1;			
	}
	return 2;
}
//////////////////////////////////////////////////////////////////////
// class OrgActionInfo
//////////////////////////////////////////////////////////////////////
void OrgActionInfo::SetAxisOrgAction(UCHAR index,Enable_STOP_NOW stop,Enable_ORG_RESETABSPOS resetABS,Enable_STOP_NOW limit)
{
	FASSERT(index>0);
	FASSERT(index<4);
	OrgAction[index-1]=stop;
	OrgResetAbs[index-1]=resetABS;
	LimitDec[index-1]=limit;
}

int OrgActionInfo::PackageBuffer(UCHAR *buff)
{
	UINT itemp=0;
	for(int i=0;i<3;i++)
	{
		if(StopDec==OrgAction[i])
		{
			itemp+=1<<(i*4);
		}
		if(OrgResetAbsPos==OrgResetAbs[i])
		{
			itemp+=1<<(i*4+1);
		}
		if(StopDec==LimitDec[i])
		{
			itemp+=1<<(i*4+2);
		}		
	}
	buff[0]=itemp&0xff;
	buff[1]=(itemp>>8)&0xff;
	return 2;
}
int OrgActionInfo::ReadBuffer(UCHAR *buff)
{
	int i;
	for(i=0;i<3;i++)
	{
		OrgAction[i]=StopNow;
		OrgResetAbs[i]=OrgNotResetAbsPos;
		LimitDec[i]=StopNow;
	}			
	UINT itemp=buff[1];
	itemp<<=8;
	itemp+=buff[0];
	UINT mask=0;
	for(i=0;i<3;i++)
	{
		mask=1<<(i*4);
		if(itemp&mask)
		{
			OrgAction[i]=StopDec;
		}
		mask=1<<(i*4+1);
		if(itemp&mask)
		{
			OrgResetAbs[i]=OrgResetAbsPos;
		}	
		mask=1<<(i*4+2);
		if(itemp&mask)
		{
			LimitDec[i]=StopDec;
		}	
	}
	return 2;
}

//////////////////////////////////////////////////////////////////////
// class NotifyInfo
//////////////////////////////////////////////////////////////////////
int NotifyInfo::PackageBuffer(UCHAR *buff)
{
	UINT itemp=0;
	if(ACC_TIME==AccType)
		itemp|=0x1;
	if(BOrgNotify)
		itemp|=(0x1<<2);
	if(BLeftLimitNotify)
		itemp|=(0x1<<3);
	if(BRightLimitNotify)
		itemp|=(0x1<<4);
	if(BLiqidNotify)
		itemp|=(0x1<<5);
	if(BFinishNotify)
		itemp|=(0x1<<6);
	if(BEncodeLimitNotify)
		itemp|=(0x1<<7);
	if(BEncodeZNotify)
		itemp|=(0x1<<8);
	if(BFivePosNotify)
		itemp|=(0x1<<9);
	buff[0]=itemp&0xff;
	buff[1]=(itemp>>8)&0xff;
	
	return 4;
}
int NotifyInfo::ReadBuffer(UCHAR *buff)
{
	AccType=ACC_SPEED;
	BOrgNotify=FALSE;
	BLeftLimitNotify=FALSE;
	BRightLimitNotify=FALSE;
	BLiqidNotify=FALSE;
	BFinishNotify=FALSE;
	BEncodeLimitNotify=FALSE;
	UINT itemp=buff[1];
	itemp<<=8;
	itemp+=buff[0];
	
	if(itemp&0X1)
		AccType=ACC_TIME;
	if(itemp&(0X1<<2))
		BOrgNotify=TRUE;
	if(itemp&(0X1<<3))
		BLeftLimitNotify=TRUE;
	if(itemp&(0X1<<4))
		BRightLimitNotify=TRUE;
	if(itemp&(0X1<<5))
		BLiqidNotify=TRUE;
	if(itemp&(0X1<<6))
		BFinishNotify=TRUE;
	if(itemp&(0X1<<7))
		BEncodeLimitNotify=TRUE;
	if(itemp&(0X1<<8))
		BEncodeZNotify=TRUE;
	if(itemp&(0X1<<9))
		BFivePosNotify=TRUE;
	return 4;
}
//////////////////////////////////////////////////////////////////////
// class LockCurrentInfo
//////////////////////////////////////////////////////////////////////
int LockCurrentInfo::PackageBuffer(UCHAR *buff)
{
	UCHAR itemp=0;
	if(BEnalbe)
		itemp|=0x1;
	itemp&=0x3F;
	itemp+=(LockCurrent<<1);
	buff[0]=itemp;
	return 2;
}
int LockCurrentInfo::ReadBuffer(UCHAR *buff)
{
	LockCurrent=A0;
	BEnalbe=FALSE;

	UCHAR itemp=buff[0];
	if(itemp&0X1)
		BEnalbe=TRUE;
	LockCurrent=(Current_TYPE)((itemp>>1)&0x1f);
	return 2;
}
//////////////////////////////////////////////////////////////////////
// class FrameIDInfo
//////////////////////////////////////////////////////////////////////
UINT FrameIDInfo::PackageFrameID()
{
	UINT temp=0,temp1;
	temp=TCW;
	temp1=BRead?1:0;
	temp1<<=8;
	temp+=temp1;
	temp1=BContinue?1:0;
	temp1<<=9;
	temp+=temp1;
	temp1=AxisID;
	temp1<<=10;
	temp+=temp1;
	temp1=ID;
	temp1<<=18;
	temp+=temp1;
	temp1=BRcv?1:0;
	temp1<<=27;
	temp+=temp1;
	return temp;	
}
FrameIDInfo FrameIDInfo::ParseFrameID(UINT ID)
{
	UINT temp=0;
	FrameIDInfo info;
	info.TCW=ID&0xff;
	info.BRead=(ID&0x100)?TRUE:FALSE;
	temp=1<<9;
	info.BContinue=(ID&temp)?TRUE:FALSE;
	temp=ID>>10;
	info.AxisID=temp&0xf;
	temp=ID>>18;
	info.ID=temp&0x7f;
	temp=ID>>27;
	info.BRcv=(temp&0x1)?TRUE:FALSE;
	return info;	
}

//////////////////////////////////////////////////////////////////////
// class MoveInfo
//////////////////////////////////////////////////////////////////////
int MoveInfo::PackageBuffer(UCHAR *buff)
{
	UCHAR itemp=0;
	if(MOVE_CONTINUE==Mode1)
		itemp|=0x1;
	UCHAR itemp1=(Group1<<1);
	itemp+=itemp1;
	itemp1=((UCHAR)POS1<<5);
	itemp+=itemp1;	
	if(DIR_CCW==DIR1)
		itemp|=0x80;
	
	buff[0]=itemp;
	itemp=0;
	if(BContinueMove)
		itemp|=0x80;
	buff[1]=itemp;

	itemp=0;
	if(MOVE_CONTINUE==Mode2)
		itemp|=0x1;
	itemp1=(Group2<<1);
	itemp+=itemp1;
	itemp1=((UCHAR)POS2<<5);
	itemp+=itemp1;	
	if(DIR_CCW==DIR2)
		itemp|=0x80;
	buff[2]=itemp;
	return 4;
}
int MoveInfo::ReadBuffer(UCHAR *buff)
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

	UCHAR itemp=buff[0];
	Mode1=(itemp&0x1)?MOVE_CONTINUE:MOVE_RT;
	itemp>>=1;
	Group1=itemp&0xf;
	itemp>>=4;
	POS1=(POS_TYPE)(itemp&0x3);
	DIR1=(buff[0]&0x80)?DIR_CCW:DIR_CW;
	BContinueMove=(buff[1]&0x80)?TRUE:FALSE;

	itemp=buff[2];
	Mode2=(itemp&0x1)?MOVE_CONTINUE:MOVE_RT;
	itemp>>=1;
	Group2=itemp&0xf;
	itemp>>=4;
	POS2=(POS_TYPE)(itemp&0x3);
	DIR2=(buff[2]&0x80)?DIR_CCW:DIR_CW;
	return 4;
}

