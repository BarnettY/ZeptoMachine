#if !defined(AFX_PROTOCOL_H__F889781B_BF72_4442_8387_F3B45A725067__INCLUDED_)
#define AFX_PROTOCOL_H__F889781B_BF72_4442_8387_F3B45A725067__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CProtocol 

class CProtocol
{
// Construction
public:
	CProtocol()
	{
		empty();
	}
	void empty()
	{
		m_COM_SYN1=0X33;
		m_COM_SYN2=0X44;
		m_DST=0x01;		
		m_MsgType=0;
		m_Msglen=0;
		m_SeqID=0;
		m_ReturnID=0;
		m_MsgBuffer=NULL;
		m_DeleteMsgBuffer=FALSE;
	}
	BYTE m_COM_SYN1;		//包头
	BYTE m_COM_SYN2;		//包头
	BYTE m_DST;				//地址
	WORD m_MsgType;		//Message Type
	WORD m_Msglen;			//Message Length
	BYTE m_SeqID;			//Sequence ID
	BYTE m_ReturnID;		//Sequence ID
	BYTE * m_MsgBuffer;		//Message
	CProtocol &operator =(CProtocol &A)
	{
		m_MsgType=A.m_MsgType;
		m_Msglen=A.m_Msglen;
		m_SeqID=A.m_SeqID;
		m_ReturnID=A.m_ReturnID;
		m_MsgBuffer=A.m_MsgBuffer;
		return *this;
	}
	BOOL ParseFromBuffer(MsgBuff &msgbuf);
	BOOL PackageBuffer(MsgBuff &msgbuf);
public:
	virtual ~CProtocol();
	BOOL AddMsgIntoBuffer(MsgBuff &msgbuf,int RESN=0,BOOL DeleteMsgBuffer=TRUE);	
	BOOL ReleaseMsgBuffer()
	{
		if(m_DeleteMsgBuffer&&(NULL!=m_MsgBuffer))
		//if(m_DeleteMsgBuffer&&(NULL!=m_MsgBuffer)&&(m_Msglen>0))
		{
			delete []m_MsgBuffer;
			m_MsgBuffer=NULL;
			m_Msglen=0;
		}
		return TRUE;
	}
	BOOL m_DeleteMsgBuffer;
protected:
	BOOL ReadBuffer(WORD &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(BYTE &Date,MsgBuff &msgbuf);
	BOOL ReadBufferP(BYTE* &Date,UINT len,MsgBuff &msgbuf);
	BOOL PutBuffer(WORD Date,MsgBuff &msgbuf);
	BOOL PutBuffer(BYTE Date,MsgBuff &msgbuf);
	BOOL PutBufferP(BYTE* &Date,UINT len,MsgBuff &msgbuf);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTOCOL_H__F889781B_BF72_4442_8387_F3B45A725067__INCLUDED_)
