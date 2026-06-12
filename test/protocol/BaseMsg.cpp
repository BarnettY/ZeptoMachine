#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "BaseMsg.h"

/////////////////////////////////////////////////////////////////////////////
// CBaseMsg
BOOL CBaseMsg::ReadBuffer(DWORD &Date,MsgBuff &msgbuf)
{
	DWORD temp=0;
	if(msgbuf.m_MsgPoint+4>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(int &Date,MsgBuff &msgbuf)
{
	int temp=0;
	if(msgbuf.m_MsgPoint+4>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}

BOOL CBaseMsg::ReadBuffer(WORD &Date,MsgBuff &msgbuf)
{
	WORD temp=0;
	if(msgbuf.m_MsgPoint+2>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}

BOOL CBaseMsg::ReadBuffer(short &Date,MsgBuff &msgbuf)
{
	short temp=0;
	if(msgbuf.m_MsgPoint+2>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp<<=8;
	temp+=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}

BOOL CBaseMsg::ReadBuffer(BYTE &Date,MsgBuff &msgbuf)
{
	BYTE temp=0;
	if(msgbuf.m_MsgPoint+1>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	temp=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=temp;
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(float &Date,MsgBuff &msgbuf)
{
	union _CONVERTFLOAT2BYTE{
		float fData;
		BYTE bData[4];
	}Convert;
	Convert.fData=0.0f;
	if(msgbuf.m_MsgPoint+4>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}

	Convert.bData[0]=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Convert.bData[1]=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Convert.bData[2]=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Convert.bData[3]=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	Date=Convert.fData;
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(wxString &Date,UINT len,MsgBuff &msgbuf)
{
	if(len<=0)
		return FALSE;
	if(msgbuf.m_MsgPoint+len>msgbuf.m_MsgCount)
	{
		FASSERT(FALSE);
		return FALSE;
	}

	char *temp=new char [len+10];
	for(UINT t=0;t<len;t++)
		temp[t]=msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++];
	temp[len]=0;
	Date=wxString(temp);
	delete []temp;
	return FALSE;
}
//-------------------------------------------------------------------
BOOL CBaseMsg::ReadBuffer(DWORD Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!ReadBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(int Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!ReadBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(WORD Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!ReadBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(short Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!ReadBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(BYTE Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!ReadBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::ReadBuffer(float Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!ReadBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
//-------------------------------------------------------------------
BOOL CBaseMsg::PutBuffer(int Date,MsgBuff &msgbuf)
{
	int temp=Date;
	if(msgbuf.m_MsgPoint+4>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp>>24)&0xFF;
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp>>16)&0xFF;
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp>>8)&0xFF;
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp)&0xFF;
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(DWORD Date,MsgBuff &msgbuf)
{
	DWORD temp=Date;
	if(msgbuf.m_MsgPoint+4>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(BYTE)((temp>>24)&0xFF);
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(BYTE)((temp>>16)&0xFF);
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(BYTE)((temp>>8)&0xFF);
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(BYTE)((temp)&0xFF);

	return TRUE;
}
BOOL CBaseMsg::PutBuffer(WORD Date,MsgBuff &msgbuf)
{
	WORD temp=Date;
	if(msgbuf.m_MsgPoint+2>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp>>8)&0xFF;
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp)&0xFF;
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(short Date,MsgBuff &msgbuf)
{
	short temp=Date;
	if(msgbuf.m_MsgPoint+2>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp>>8)&0xFF;
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp)&0xFF;
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(BYTE Date,MsgBuff &msgbuf)
{
	BYTE temp=Date;
	if(msgbuf.m_MsgPoint+1>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=(temp)&0xFF;
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(float Date,MsgBuff &msgbuf)
{
	union _CONVERTFLOAT2BYTE{
		float fData;
		BYTE bData[4];
	}Convert;
	Convert.fData=Date;

	if(msgbuf.m_MsgPoint+4>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=Convert.bData[0];
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=Convert.bData[1];
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=Convert.bData[2];
	msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=Convert.bData[3];
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(wxString Date,UINT len,MsgBuff &msgbuf)
{
	if(len<=0)
		return FALSE;
	if(msgbuf.m_MsgPoint+len>MAX_BUFFLEN)
	{
		FASSERT(FALSE);
		return FALSE;
	}
	UINT count=Date.Length();
	const char *temp =Date.mb_str();
	//char *temp =wxStrdup(Date.mb_str());

	if(count<len)
	{
		UINT t;
		for(t=0;t<count;t++)
			msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=temp[t];
		for(t=count;t<len;t++)
			msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=0;
	}
	else
	{
		for(UINT t=0;t<len;t++)
			msgbuf.m_MsgBuf[msgbuf.m_MsgPoint++]=temp[t];
	}
	//delete temp;

	return FALSE;
}
//-------------------------------------------------------------------
BOOL CBaseMsg::PutBuffer(DWORD Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!PutBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(int Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!PutBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(WORD Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!PutBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
BOOL CBaseMsg::PutBuffer(short Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!PutBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}

BOOL CBaseMsg::PutBuffer(BYTE Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!PutBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}

BOOL CBaseMsg::PutBuffer(float Date[],UINT len,MsgBuff &msgbuf)
{
	for(UINT t=0;t<len;t++)
	{
		if(!PutBuffer(Date[t],msgbuf))
			return FALSE;
	}
	return TRUE;
}
//
//BOOL CBaseMsg::ReadBuffer(PosParamInfo &info,MsgBuff &msgbuf)
//{
//
//	ReadBuffer(info.m_PartSampleInPos,msgbuf);
//	ReadBuffer(info.m_PartSampleCollect1Pos,msgbuf);
//	ReadBuffer(info.m_PartSampleCollect2Pos,msgbuf);
//	//for (int i= 0; i<MAX_PartSampleMove ; i ++)
//	//{
// //     	ReadBuffer(info.m_PartSampleMovePos[i],msgbuf);
//	//}
//
//	return TRUE;
//}
//BOOL CBaseMsg::ReadBuffer(PartSampleInPos &pos,MsgBuff &msgbuf)
//{
//
//	ReadBuffer(pos.PM_MAX_POS,msgbuf);
//
//	ReadBuffer(pos.PM_STEP,msgbuf);
//    ReadBuffer(pos.RLM_MAX_POS,msgbuf);
//
////	ReadBuffer(pos.RLM_TUBESPACE,msgbuf);
//	ReadBuffer(pos.RLM_FIRSTSPACE,msgbuf);
//	ReadBuffer(pos.RLM_SECONDSPACE,msgbuf);
//
//  return TRUE;
//}
//BOOL CBaseMsg:: ReadBuffer(PartSampleCollect1Pos &pos,MsgBuff &msgbuf)
//{
//
//	ReadBuffer(pos.PM_MAX_POS,msgbuf);
//	ReadBuffer(pos.CTM_MAX_POS,msgbuf);
//	return TRUE;
//}
//BOOL CBaseMsg:: ReadBuffer(PartSampleCollect2Pos &pos,MsgBuff &msgbuf)
//{
//	ReadBuffer(pos.PM_MAX_POS,msgbuf);
//	ReadBuffer(pos.CTM_MAX_POS,msgbuf);
//	ReadBuffer(pos.BM_MAX_POS,msgbuf);
//	return TRUE;
//}
//BOOL CBaseMsg::ReadBuffer(PartSampleMovePos &pos,MsgBuff &msgbuf)
//{
//
//	ReadBuffer(pos.TM_MAX_POS,msgbuf); 
//    ReadBuffer(pos.TM_LIMIT_POS,msgbuf); 
//	ReadBuffer(pos.PIM_MAX_POS,msgbuf);
//	ReadBuffer(pos.PIM_NOR_POS,msgbuf);
//	ReadBuffer(pos.UDM_MAX_POS,msgbuf);
//	ReadBuffer(pos.UDM_UP_POS,msgbuf);
//	ReadBuffer(pos.LM_MAX_POS,msgbuf);
//	ReadBuffer(pos.POM_MAX_POS,msgbuf);
//    ReadBuffer(pos.POM_NOR_POS,msgbuf);
//    ReadBuffer(pos.POM_NOR_POS1,msgbuf);
//    ReadBuffer(pos.POM_BACK_POS,msgbuf);
//	ReadBuffer(pos.UDL_DOWN_POS1,msgbuf);
//	ReadBuffer(pos.UDL_DOWN_POS,msgbuf);
//	return TRUE;
//}
//
//
//
//BOOL CBaseMsg::PutBuffer(PosParamInfo &info,MsgBuff &msgbuf)
//{
//	PutBuffer(info.m_PartSampleInPos,msgbuf);
//	PutBuffer(info.m_PartSampleCollect1Pos,msgbuf);
//	PutBuffer(info.m_PartSampleCollect2Pos,msgbuf);
//	//for (int i= 0; i<MAX_PartSampleMove ; i++)
//	//{
//	//	PutBuffer(info.m_PartSampleMovePos[i],msgbuf);
//	//}
//
//	return TRUE;
//}
//
//
//BOOL CBaseMsg::PutBuffer(PartSampleInPos &pos,MsgBuff &msgbuf)
//{
//    PutBuffer(pos.PM_MAX_POS,msgbuf);
//	PutBuffer(pos.PM_STEP,msgbuf);
//	PutBuffer(pos.RLM_MAX_POS,msgbuf);
//	//PutBuffer(pos.RLM_TUBESPACE,msgbuf);
//	PutBuffer(pos.RLM_FIRSTSPACE,msgbuf);
//	PutBuffer(pos.RLM_SECONDSPACE,msgbuf);
//
//	return TRUE;
//}
//
//BOOL CBaseMsg:: PutBuffer(PartSampleCollect1Pos &pos,MsgBuff &msgbuf)
//{
//	PutBuffer(pos.PM_MAX_POS,msgbuf);
//	PutBuffer(pos.CTM_MAX_POS,msgbuf);
//	return TRUE;
//}
//
//BOOL CBaseMsg::PutBuffer(PartSampleCollect2Pos &pos,MsgBuff &msgbuf)
//{
//	PutBuffer(pos.PM_MAX_POS,msgbuf);
//	PutBuffer(pos.CTM_MAX_POS,msgbuf);
//	PutBuffer(pos.BM_MAX_POS,msgbuf);
//	return TRUE;
//}
//
//BOOL  CBaseMsg::PutBuffer(PartSampleMovePos &pos,MsgBuff &msgbuf)
//{
//	PutBuffer(pos.TM_MAX_POS,msgbuf);
//    PutBuffer(pos.TM_LIMIT_POS,msgbuf); 
//	PutBuffer(pos.PIM_MAX_POS,msgbuf);
//	PutBuffer(pos.PIM_NOR_POS,msgbuf);
//	PutBuffer(pos.UDM_MAX_POS,msgbuf);
//	PutBuffer(pos.UDM_UP_POS,msgbuf);
//	PutBuffer(pos.LM_MAX_POS,msgbuf);
//	PutBuffer(pos.POM_MAX_POS,msgbuf);
//   	PutBuffer(pos.POM_NOR_POS,msgbuf);
//	PutBuffer(pos.POM_NOR_POS1,msgbuf);
//  	PutBuffer(pos.POM_BACK_POS,msgbuf);
//	PutBuffer(pos.UDL_DOWN_POS1,msgbuf);
//	PutBuffer(pos.UDL_DOWN_POS,msgbuf);
//	return TRUE;
//}
//
//
//
//BOOL CBaseMsg::ReadBuffer(trayInfo &tInfo,MsgBuff &msgbuf)
//{
//	ReadBuffer(tInfo.strName,20,msgbuf); 
//
//	for (int i =0; i<TRAY_CNT ;i++ )
//	{
//		ReadBuffer(tInfo.m_SampleID[i],20,msgbuf); 
//	}
//	ReadBuffer(tInfo.m_MachineId,msgbuf); 
//	
//	return TRUE;
//
//}
//
//
//BOOL CBaseMsg::PutBuffer(trayInfo &tInfo,MsgBuff &msgbuf)
//{
//	PutBuffer(tInfo.strName,20,msgbuf); 
//
//	for (int i =0; i<TRAY_CNT ;i++ )
//	{
//		PutBuffer(tInfo.m_SampleID[i],20,msgbuf); 
//	}
//	PutBuffer(tInfo.m_MachineId,msgbuf); 
//	return TRUE;
//
//}

//
//
//BOOL  CBaseMsg::ReadBuffer(SystemParamInfo &Info,MsgBuff &msgbuf)
//{
//
//	//BOOL m_bCollect2Install;// 回收区2 是否安装
//	//BOOL m_bSampleMoveInstall[8];//检测 轨道区2 是否安装
//	//BOOL m_bSampleMoveEnable[8]; //检测 轨道区2 是否启用
//    ReadBuffer(Info.m_bCollect2Install,msgbuf); 
//	for (int i =0; i<8 ;i++ )
//	{
//		ReadBuffer(Info.m_bSampleMoveInstall[i],msgbuf); 
//		ReadBuffer(Info.m_bSampleMoveEnable[i],msgbuf); 
//
//	}
//    return TRUE;
//
//
//}
//BOOL CBaseMsg:: PutBuffer(SystemParamInfo &Info,MsgBuff &msgbuf)
//{
//
//	PutBuffer(Info.m_bCollect2Install,msgbuf); 
//	for (int i =0; i<8 ;i++ )
//	{
//		PutBuffer(Info.m_bSampleMoveInstall[i],msgbuf); 
//		PutBuffer(Info.m_bSampleMoveEnable[i],msgbuf); 
//
//	}
//	return TRUE;
//
//}