// CanCom.h: interface for the CCanCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANCOM_H__CB7E37D0_5C9C_4AA0_AC63_9631842EDF03__INCLUDED_)
#define AFX_CANCOM_H__CB7E37D0_5C9C_4AA0_AC63_9631842EDF03__INCLUDED_


#include "AxisParamInfo.h"
#include "CanInterface.h"

struct CMemBuffer
{
	CMemBuffer(UINT len)
	{
		maxlen=len;
		buffer=new BYTE[len];
	}
	~CMemBuffer()
	{
		delete []buffer;
	}
	void SetLength(UINT len)
	{
		length=len;
	}
	UINT GetLength()
	{
		return length;
	}
	UINT Read(BYTE *read,UINT &len)
	{
		len=length;
		if(length>0)
			memmove(read,buffer,length);
		return length;
	}
	void Write(BYTE *read,UINT len)
	{
		FASSERT(len+length<maxlen);
		if(len>0)
			memmove(buffer+length,read,len);
		length+=len;
	}
	UINT length;
	BYTE *buffer;
	UINT maxlen;
};
struct CMotorRcvBufferInfo
{
	CMotorRcvBufferInfo(int ID,int axis):StoreBuffer(200),RcvSemp(FALSE)
	{
		AxisID=axis;
		memset(&PreFrameInfo,0,sizeof(CAN_OBJ));
		PreFrameInfo.ID=ID;
		memset(&PreFrameIDInfo,0,sizeof(FrameIDInfo));
	}
	BOOL EmptyRcv()
	{
	/*	CanTraceLog("ResetEvent Axis=%d:",AxisID);*/

		StoreBuffer.SetLength(0);
		RcvSemp.ResetEvent();
		return TRUE;
	}
	int WaitRcvFinish(UINT WaitTM)
	{
		if(wxSEMA_NO_ERROR!=RcvSemp.WaitTimeout(WaitTM))
		{
			FASSERT(FALSE);
			return ERROR_CAN_TIMEOUT;
		}
		return 1;
	}
	BYTE *GetRcvBuffer(UINT &len)
	{
		m_Lock.Enter();
		len=StoreBuffer.GetLength();
		BYTE *buff=new BYTE[len];
		UINT rtn=StoreBuffer.Read(buff,len);
		m_Lock.Leave();
		FASSERT(rtn==len);
		return buff;
	}
	BOOL CallBackReceiveFrame(FrameIDInfo *pinfo,CAN_OBJ *pFrameInfo)
	{
		//如果是通知，单独处理
		if(0xf0==pinfo->TCW)
		{
			NotifyFrameInfo=*pFrameInfo;
			return FALSE;
		}
		BOOL diff=FALSE;
		if(pinfo->TCW!=PreFrameIDInfo.TCW)
			diff=TRUE;
		if(pinfo->BRead!=PreFrameIDInfo.BRead)
			diff=TRUE;

		//如果是条码ID号，每条都不相同，清缓存
		if(pinfo->ID>=0x50)
			diff=TRUE;

		m_Lock.Enter();
		if(diff)
		{
			PreFrameIDInfo=*pinfo;
			PreFrameInfo=*pFrameInfo;
			StoreBuffer.SetLength(0);
		}
		StoreBuffer.Write(pFrameInfo->Data,pFrameInfo->DataLen);
		m_Lock.Leave();

		//已经结束了
		if(!pinfo->BContinue)
		{
			PreFrameIDInfo.TCW=0;
			RcvSemp.SetEvent();
		}
		return TRUE;
	}

	CAN_OBJ	PreFrameInfo;
	CAN_OBJ NotifyFrameInfo;
	FrameIDInfo	PreFrameIDInfo;
	CMemBuffer	StoreBuffer;
	CxwEvent	 RcvSemp;
	wxCriticalSection m_Lock;
	UCHAR AxisID;
};
struct CtrlRcvBufferInfo
{
	CtrlRcvBufferInfo(int ID)
	{
		for(int i=0;i<4;i++)
		{
			m_pRcvBufferInfo[i]=new CMotorRcvBufferInfo(ID,i);
		}
		Resigtered=FALSE;
	}
	virtual ~CtrlRcvBufferInfo()
	{
		for(int i=0;i<4;i++)
		{
			delete m_pRcvBufferInfo[i];
		}
	}

	BOOL EmptyRcv(UCHAR AxisID)
	{
		FASSERT(AxisID<4);
		return m_pRcvBufferInfo[AxisID]->EmptyRcv();
	}
	int WaitRcvFinish(UINT WaitTM,UCHAR AxisID)
	{
		FASSERT(AxisID<4);
		return m_pRcvBufferInfo[AxisID]->WaitRcvFinish(WaitTM);
	}
	UCHAR *GetRcvBuffer(UINT &len,UCHAR AxisID)
	{
		FASSERT(AxisID<4);
		UCHAR* buff=m_pRcvBufferInfo[AxisID]->GetRcvBuffer(len);
	/*	wxString strtemp1;
		wxString strtemp;
		strtemp.Printf("GetRcvBuffer Axis=%d:",AxisID);
		for(UINT t=0;t<len;t++)
		{
			strtemp1.Printf("%0.2x ",buff[t]);
			strtemp+=strtemp1;
		}
		CanTraceLog(strtemp.mb_str(wxConvUTF8));*/

		return buff;
	}
	BOOL CallBackReceiveFrame(FrameIDInfo *pinfo,CAN_OBJ *pFrameInfo,UCHAR AxisID)
	{
		FASSERT(AxisID<4);
		return m_pRcvBufferInfo[AxisID]->CallBackReceiveFrame(pinfo,pFrameInfo);
	}
	CAN_OBJ GetNotifyFrameInfo(UCHAR AxisID)
	{
		FASSERT(AxisID<4);
		return m_pRcvBufferInfo[AxisID]->NotifyFrameInfo;
	}
	CMotorRcvBufferInfo *m_pRcvBufferInfo[4];		//一个控制器对应轴号0-3
	BOOL		Resigtered;
};
//////////////////////////////////////////////////////////////////////
// class CCanCom
//////////////////////////////////////////////////////////////////////
#define MAX_BUFF_CNT	0X80
#define MAX_ACTION_CNT	0X50        //64 个

class CCanControl;
class CCanModule;
WX_DECLARE_LIST(CAN_OBJ, CANOBJList);
class CCanCom
{
public:
 	friend class CanReceiveThread;
 	friend class CanSendThread;
 	friend class CanParserThread;
#ifdef _SIMULATOR_
	wxCriticalSection m_CallBackLock;
	wxArrayInt m_WaitCallBack;
#endif
	BOOL m_connect;
	//发送接收计数
	DWORD m_sendcnt;
	volatile WORD m_recvcount;

	CCanCom();
	virtual ~CCanCom();
	BOOL ResigterCtrl(UINT ID,CCanControl **pCtrl);
	BOOL ResigterModule(UINT ID,CCanModule *pCtrl);// 未用
	CCanControl *GetControl(int CtrlIndex);
	void	  SetCanInterface(CanInterface *pInterface)
	{
		m_pInterface=pInterface;
	}

	BOOL StarCan();
	BOOL ResetCan();
	BOOL Connect(Baud_TYPE baud);
	BOOL ReConnect(Baud_TYPE baud);
	BOOL DisConnect();
	int SendSingleFrame(DWORD frameID,UCHAR *buffer,int len,BOOL init=FALSE);
	int EmptyRcv(FrameIDInfo info,BOOL UseChannel=FALSE);
	int WaitCanRcv(FrameIDInfo info,UINT WaitTM,BOOL UseChannel=FALSE);
	UCHAR *GetCanRcvBuffer(FrameIDInfo info,UINT &len,int &errcode,BOOL UseChannel=FALSE);
	BOOL ScanAllID();
protected:
	BOOL RarseFrameAry(CAN_OBJ info[],UINT count);
	BOOL ChangeBaud();
	void TraceFrame(CAN_OBJ *pFrame,BOOL BRcv);
	BOOL DealSend();
	BOOL DealPendFinish();
#ifdef _SIMULATOR_
	BOOL DealRarse();
#endif
	CtrlRcvBufferInfo *m_CtrlRcvBuffer[MAX_BUFF_CNT];		//每个ID号一个专门的缓存
	CCanControl *m_CanCtrl[MAX_ACTION_CNT];
	CCanModule *m_ModuleCtrl[MAX_BUFF_CNT-MAX_ACTION_CNT];	//每个ID对应一个电路板      未用
	wxCriticalSection m_SendLock;
	CANOBJList	m_SendLst;
	CANOBJList	m_ParseLst;

	//消息线程
	BOOL StarMonitorThread();							//启动监控线程
	UINT ReceiveMain();									//专门接收
	CxwEvent	 SendSemp;									//表示发生的事件
	CanInterface *m_pInterface;
};
extern CCanCom g_Can;
////////////////////////////////////////////////////////////////////////////////
class CanReceiveThread : public wxThread
{
public:
    CanReceiveThread(CCanCom *pcan);
    virtual ~CanReceiveThread();
    virtual void* Entry();
protected:
	CCanCom	*m_pcan;
};
class CanSendThread : public wxThread
{
public:
    CanSendThread(CCanCom *pcan);
    virtual ~CanSendThread();
    virtual void* Entry();
protected:
	CCanCom	*m_pcan;
};
#endif // !defined(AFX_CANCOM_H__CB7E37D0_5C9C_4AA0_AC63_9631842EDF03__INCLUDED_)
