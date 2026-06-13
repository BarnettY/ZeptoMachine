// CanCom.cpp: implementation of the CCanCom class.
//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "CanCom.h"
#include "CanControl.h"
#include "../trace.h"
#include "wx/listimpl.cpp"

WX_DEFINE_LIST(CANOBJList);

CCanCom g_Can;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCanCom::CCanCom():SendSemp(FALSE)
{
	m_connect=FALSE;
	int i;
	for(i=0;i<MAX_BUFF_CNT;i++)
	{
		m_CtrlRcvBuffer[i]=NULL;
	}
	for(i=0;i<MAX_ACTION_CNT;i++)
	{
		m_CanCtrl[i]=NULL;
	}
	for(i=0;i<MAX_BUFF_CNT-MAX_ACTION_CNT;i++)
	{
		m_ModuleCtrl[i]=NULL;
	}

//	m_CtrlRcvBuffer[MAX_BUFF_CNT-1]=new CtrlRcvBufferInfo(MAX_BUFF_CNT-1);
//	m_CtrlRcvBuffer[MAX_BUFF_CNT-1]->Resigtered=TRUE;
	m_pInterface=NULL;
	m_SendLst.DeleteContents(true);
	m_ParseLst.DeleteContents(true);
}

CCanCom::~CCanCom()
{
	DisConnect();
	int i;
	for(i=0;i<MAX_BUFF_CNT;i++)
	{
		if(m_CtrlRcvBuffer[i]!=NULL)
		{
			delete m_CtrlRcvBuffer[i];
			m_CtrlRcvBuffer[i]=NULL;
		}
	}
	for(i=0;i<MAX_ACTION_CNT;i++)
	{
		if(m_CanCtrl[i]!=NULL)
		{
			delete m_CanCtrl[i];
			m_CanCtrl[i]=NULL;
		}
	}
	if(NULL!=m_pInterface)
	{
		delete m_pInterface;
		m_pInterface=NULL;
	}
}
BOOL CCanCom::DisConnect()
{
	if(!m_connect) return TRUE;
	m_connect=FALSE;
	SendSemp.SetEvent();
	wxMilliSleep(500);
	return ((NULL!=m_pInterface)&&m_pInterface->CloseCan());
}
BOOL CCanCom::StarCan()
{
	if(!m_connect) return FALSE;
	return ((NULL!=m_pInterface)&&m_pInterface->StarCan());
}
BOOL CCanCom::ResetCan()
{
	if(!m_connect) return FALSE;
	return ((NULL!=m_pInterface)&&m_pInterface->ResetCan());
}

BOOL CCanCom::Connect(Baud_TYPE baud)
{
	if(m_connect) return TRUE;
	if((NULL!=m_pInterface)&&(!m_pInterface->OpenCan(K100)))
		;//FIXME:return FALSE

	m_recvcount=1;
	m_sendcnt=0;
	m_connect=TRUE;
	StarCan();
	ResetCan();
	BOOL btn=StarMonitorThread();
	return btn;
}
BOOL CCanCom::ReConnect(Baud_TYPE baud)
{
	BOOL brtn=TRUE;
	//换250k
	if(m_connect)
		brtn=(NULL!=m_pInterface)&&m_pInterface->CloseCan();
	if((NULL!=m_pInterface)&&(!m_pInterface->OpenCan(K250)))
		return FALSE;

	m_recvcount=1;
	m_sendcnt=0;
	m_connect=TRUE;
	StarCan();
	ResetCan();
//	wxMilliSleep(200);
//	ScanAllID();
	return brtn;
}

BOOL CCanCom::ResigterCtrl(UINT ID,CCanControl **pCtrl)
{
	FASSERT(ID<MAX_ACTION_CNT-1);
	if(NULL==m_CanCtrl[ID])
	{
		FASSERT(NULL==m_CtrlRcvBuffer[ID]);
		m_CtrlRcvBuffer[ID]=new CtrlRcvBufferInfo(ID);
		m_CanCtrl[ID]=new CCanControl(ID);
	}
	*pCtrl=m_CanCtrl[ID];
	return TRUE;
}
CCanControl *CCanCom::GetControl(int ID)
{
	FASSERT(ID>0);
	FASSERT(ID<MAX_ACTION_CNT-1);
	return m_CanCtrl[ID];
}
BOOL CCanCom::ResigterModule(UINT ID,CCanModule *pCtrl)
{
	FASSERT(pCtrl);
	FASSERT(ID<MAX_BUFF_CNT-1);
	FASSERT(ID>MAX_ACTION_CNT);
	FASSERT(NULL==m_ModuleCtrl[ID-MAX_ACTION_CNT]);
	if(NULL==m_ModuleCtrl[ID-MAX_ACTION_CNT])
	{
		FASSERT(NULL==m_CtrlRcvBuffer[ID]);
		m_CtrlRcvBuffer[ID]=new CtrlRcvBufferInfo(ID);
		m_ModuleCtrl[ID-MAX_ACTION_CNT]=pCtrl;
	}
	return TRUE;
}

BOOL CCanCom::ScanAllID()
{
	int itrn=0;
	UCHAR buff[8]={0};
	for(int i=0;i<MAX_BUFF_CNT;i++)// MAX_BUFF_CNT 128
	{
		if(NULL!=m_CtrlRcvBuffer[i])
		{
			FrameIDInfo frame(i,0,0x01,TRUE);
			UINT ID=frame.PackageFrameID();
			if(i<MAX_ACTION_CNT)  //MAX_ACTION_CNT  80
			{
				itrn=SendSingleFrame(ID,NULL,0,TRUE);
			}
			else
			{
				//电路板第一位是SN，必带
				itrn=SendSingleFrame(ID,buff,1,TRUE);
			}
		}
	}
	return TRUE;
}
BOOL CCanCom::ChangeBaud()
{
	int itrn=0;
	FrameIDInfo frame(0x7f,0,0x03,FALSE);
	UCHAR buff[8]={0};
	buff[0]=0x03;
	buff[1]=0x00;
	UINT ID=frame.PackageFrameID();
	itrn=SendSingleFrame(ID,buff,2,TRUE);
	return TRUE;
}

//#define BARCODE_BEGIN_CTRLID	0X50

int CCanCom::SendSingleFrame(DWORD frameID,UCHAR *buffer,int len,BOOL init)
{
//	CanTraceLog("SendSingleFrame");
	if(!m_connect)
		return ERROR_CAN_DISCON;
	CAN_OBJ *frameinfo=new CAN_OBJ;
	frameinfo->TimeFlag=0;
	frameinfo->DataLen=len;
	if(len>0)
		memmove(&frameinfo->Data,buffer,len);
	frameinfo->RemoteFlag=0;
	frameinfo->ExternFlag=1;
	frameinfo->ID=frameID;
	frameinfo->SendType=0;
	FrameIDInfo info=FrameIDInfo::ParseFrameID(frameinfo->ID);
	if((NULL==m_CtrlRcvBuffer[info.ID])&&(0x7f!=info.ID))// 判断是否注册
		return ERROR_CAN_UNRESIGTER;
	if(!init&&(m_CtrlRcvBuffer[info.ID])&&(!m_CtrlRcvBuffer[info.ID]->Resigtered))//是否注册成功
	{
		return ERROR_CAN_NOTHARDWARE;
	}

//	CanTraceLog(_T("SendSingleFrame"));
	m_SendLock.Enter();
	m_SendLst.Append(frameinfo);
	m_SendLock.Leave();
	SendSemp.SetEvent();
	return TRUE;
}
int CCanCom::EmptyRcv(FrameIDInfo info,BOOL UseChannel)
{
	if(NULL==m_CtrlRcvBuffer[info.ID])
		return ERROR_CAN_UNRESIGTER;
	if(UseChannel)
		return m_CtrlRcvBuffer[info.ID]->EmptyRcv(info.Channel);
	else
		return m_CtrlRcvBuffer[info.ID]->EmptyRcv(info.AxisID);
}

int CCanCom::WaitCanRcv(FrameIDInfo info,UINT WaitTM,BOOL UseChannel)
{
	if(NULL==m_CtrlRcvBuffer[info.ID])
		return ERROR_CAN_UNRESIGTER;
	if(UseChannel)
		return m_CtrlRcvBuffer[info.ID]->WaitRcvFinish(WaitTM,info.Channel);
	else
		return m_CtrlRcvBuffer[info.ID]->WaitRcvFinish(WaitTM,info.AxisID);
}
UCHAR *CCanCom::GetCanRcvBuffer(FrameIDInfo info,UINT &len,int &errcode,BOOL UseChannel)
{
	len=0;
	errcode=0;
	if(NULL==m_CtrlRcvBuffer[info.ID])
	{
		errcode=ERROR_CAN_UNRESIGTER;
		return NULL;
	}
	if(UseChannel)
		return m_CtrlRcvBuffer[info.ID]->GetRcvBuffer(len,info.Channel);
	else
		return m_CtrlRcvBuffer[info.ID]->GetRcvBuffer(len,info.AxisID);
}


//////////////////////////////////////////////////////////////////////
// CanTraceLog and Thread
//////////////////////////////////////////////////////////////////////
void CCanCom::TraceFrame(CAN_OBJ *pFrame,BOOL BRcv)
{
	if(BRcv)
	{
		m_recvcount++;
		if(0==m_recvcount)
			m_recvcount=1;
		//CanTraceLog(_T("     RCV TOTAL %d"),m_recvcount);
	}
	wxString str,tmpstr;
	wxString str1;
	if(BRcv)
		str=_T("RCV CAN:");
	else
		str=_T("Send CAN:");
	FrameIDInfo info=FrameIDInfo::ParseFrameID(pFrame->ID);
	tmpstr.Printf(_T("%s ID:0X%0.2x Axis:%d Is Finish=%s %s TCW:0X%0.2x      "),
		info.BRcv?_T("Rcv from"):_T("Send to"),info.ID,info.AxisID,
		info.BContinue?_T("FALSE"):_T("TRUE"),info.BRead?_T("Read"):_T("Write"),info.TCW);

	str+=tmpstr;
	str+=_T("FRAME:");
	if(pFrame->RemoteFlag==0)
		tmpstr=_T("DATA ");
	else
		tmpstr=_T("FRA ");
	str+=tmpstr;
	str+=_T("TYPE:");
	if(pFrame->ExternFlag==0)
		tmpstr=_T("NORMAL ");
	else
		tmpstr=_T("EXTERN ");
	str+=tmpstr;
	if(pFrame->RemoteFlag==0)
	{
		if(BRcv)
			str1=_T("RCV DATA:");
		else
			str1=_T("SEND DATA:");
		tmpstr.Printf(_T("FrameID=0X%08x ,DataLen=%d,Data="),pFrame->ID,pFrame->DataLen);
		str1+=tmpstr;

		if(pFrame->DataLen>8)
			pFrame->DataLen=8;
		for(int j=0;j<pFrame->DataLen;j++)
		{
			tmpstr.Printf(_T("0X%02x "),pFrame->Data[j]);
			str1+=tmpstr;
		}
		str+=str1;
	}
	//CanTraceLog(str.mb_str());
}

BOOL CCanCom::DealSend()
{
	BOOL Empty=FALSE;
	m_SendLock.Enter();
	Empty=m_SendLst.IsEmpty();
	m_SendLock.Leave();
	while(!Empty)
	{
//		CanTraceLog("DealSend");
		CAN_OBJ frameinfo;
		m_SendLock.Enter();
		CANOBJList::Node* node = m_SendLst.GetFirst();
		CAN_OBJ*date = node->GetData();
		frameinfo=*date;
		m_SendLst.DeleteNode(node);
		FrameIDInfo info=FrameIDInfo::ParseFrameID(frameinfo.ID);
		Empty=m_SendLst.IsEmpty();
		m_SendLock.Leave();
//		CanTraceLog("readySend");
		TraceFrame(&frameinfo,FALSE);
		if((NULL!=m_pInterface)&&(m_pInterface->SendCan(&frameinfo,1)))
		{
			m_sendcnt += 1;
//			CanTraceLog(_T("WRITE OK"));
		}
		else
		{
//			CanTraceLog(_T("WRITE FAILED"));
		}
	//	wxMilliSleep(1);
	}
	return TRUE;
}
BOOL CCanCom::DealPendFinish()
{
	for(int i=0;i<MAX_ACTION_CNT;i++)
	{
		if(m_CanCtrl[i]!=NULL)
		{
			m_CanCtrl[i]->DealPend(m_recvcount);
		}
	}
	return TRUE;
}
BOOL CCanCom::RarseFrameAry(CAN_OBJ frameinfo[],UINT count)
{
	for(int t=0;t<count;t++)
	{
		FrameIDInfo info=FrameIDInfo::ParseFrameID(frameinfo[t].ID);
		TraceFrame(&frameinfo[t],TRUE); // 日志
		DealPendFinish();
		if(NULL!= m_CtrlRcvBuffer[info.ID])// 缓存
		{
			if(0X1==info.TCW)
				m_CtrlRcvBuffer[info.ID]->Resigtered=TRUE;  // ID注册回复

			if(info.ID<MAX_ACTION_CNT)		//运动部件 0x3f
			{
				BOOL BNotNotify=m_CtrlRcvBuffer[info.ID]->CallBackReceiveFrame(&info,&frameinfo[t],info.AxisID); //处理
				if(!BNotNotify&&(NULL!=m_CanCtrl[info.ID]))
				{
					m_CanCtrl[info.ID]->ParseNotifyBuffer(info,m_CtrlRcvBuffer[info.ID]->GetNotifyFrameInfo(info.AxisID).Data,
						m_CtrlRcvBuffer[info.ID]->GetNotifyFrameInfo(info.AxisID).DataLen);
				}
			}
			else			//-----------电路板  未用
			{
				BYTE INDEX=info.ID-MAX_ACTION_CNT;
				if(NULL==m_ModuleCtrl[INDEX])
					continue;
				m_CtrlRcvBuffer[info.ID]->CallBackReceiveFrame(&info,&frameinfo[t],info.Channel);  // info.ID = 0 ; 轴号 0 
			}
		}
	//	wxMilliSleep(1);
	}
	return TRUE;
}

#ifdef _SIMULATOR_
BOOL CCanCom::DealRarse()
{
	BOOL Empty=FALSE;
	Empty=m_ParseLst.IsEmpty();
	m_CallBackLock.Enter();
	int ncount=m_WaitCallBack.GetCount();
	for(int t=0;t<ncount;t++)
	{
		UINT id=m_WaitCallBack.Item(t);
		BYTE ID=(id>>8)&0XFF;
		BYTE Axis=id&0XFF;
		m_CanCtrl[ID]->CallBackMotor(Axis);
	}
	m_WaitCallBack.Clear();
	m_CallBackLock.Leave();
	return TRUE;
}
#endif

//启动马达监控线程
BOOL CCanCom::StarMonitorThread()
{
	SendSemp.WaitTimeout(0);

	new CanReceiveThread(this);
	new CanSendThread(this);
//	new CanParserThread(this);
	return TRUE;
}

UINT  CCanCom::ReceiveMain()
{
	CAN_OBJ frameinfo[50];
	int i=0;
	int len=1;
	wxString str,tmpstr;
//	CanTraceLog(_T("ReceiveCan begin"));
	m_pInterface->ReceiveCan(frameinfo,len); // 接收 数据 
//	CanTraceLog(_T("ReceiveCan end"));
	if(len>0)
	{
		RarseFrameAry(frameinfo,len);
	}
	else
	{
		m_recvcount++;
		if(0==m_recvcount)
			m_recvcount=1;
		DealPendFinish();
	}
#ifdef _SIMULATOR_
	DealRarse();
#endif
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
CanReceiveThread::CanReceiveThread(CCanCom *pcan)
{
	m_pcan=pcan;
	Create();
	SetPriority(100);
	Run();
}
CanReceiveThread::~CanReceiveThread()
{
}

void* CanReceiveThread::Entry()
{
	while(m_pcan->m_connect)
	{
		if(NULL==m_pcan->m_pInterface)
			break;
		m_pcan->ReceiveMain();
		wxMilliSleep(0);
	}
	return NULL;
}


CanSendThread::CanSendThread(CCanCom *pcan)
{
	m_pcan=pcan;
	Create();
	SetPriority(100);
	Run();
}
CanSendThread::~CanSendThread()
{
}

void* CanSendThread::Entry()
{
#ifdef  WIN32
	m_pcan->ChangeBaud();
#endif
	int a=0;
	while(m_pcan->m_connect)
	{
		a=m_pcan->SendSemp.WaitTimeout(1000);
//		CanTraceLog(_T("CanSendThread WaitTimeout=%d"),a);
		if(!m_pcan->m_connect)
			break;
		if(NULL==m_pcan->m_pInterface)
			break;
		m_pcan->DealSend();
	}
	return NULL;
}
