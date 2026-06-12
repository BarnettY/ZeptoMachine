#if !defined(AFX_BASEMSG_H__F889781B_BF72_4442_8387_F3B45A725067__INCLUDED_)
#define AFX_BASEMSG_H__F889781B_BF72_4442_8387_F3B45A725067__INCLUDED_

#include "../runinfo/datadefine.h"
#include "CommonMsg.h"
/////////////////////////////////////////////////////////////////////////////
// CBaseMsg
class TreeGridInterfer
{
public:
	virtual BOOL AddRow(wxString strname,wxString value,wxString hint)=0;
	void ResetTree()
	{
		m_nLevel=0;
	}
	void IntoTree()
	{
		m_nLevel++;
	}
	void OutTree()
	{
		m_nLevel--;
	}
protected:
	int m_nLevel;
};

class CBaseMsg
{
public:
	CBaseMsg()
	{
		m_MessageType=MSG_NONE;
	}

	MSG_TYPE m_MessageType;
	BOOL ReadBuffer(DWORD &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(int &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(WORD &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(short &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(BYTE &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(float &Date,MsgBuff &msgbuf);
	BOOL ReadBuffer(wxString &Date,UINT len,MsgBuff &msgbuf);
	//-------------------------------------------------------------------
	BOOL ReadBuffer(DWORD Date[],UINT len,MsgBuff &msgbuf);
	BOOL ReadBuffer(int Date[],UINT len,MsgBuff &msgbuf);
	BOOL ReadBuffer(WORD Date[],UINT len,MsgBuff &msgbuf);
	BOOL ReadBuffer(short Date[],UINT len,MsgBuff &msgbuf);
	BOOL ReadBuffer(BYTE Date[],UINT len,MsgBuff &msgbuf);
	BOOL ReadBuffer(float Date[],UINT len,MsgBuff &msgbuf);
	//------------------------------------------------------------------

	BOOL PutBuffer(int Date,MsgBuff &msgbuf);
	BOOL PutBuffer(DWORD Date,MsgBuff &msgbuf);
	BOOL PutBuffer(WORD Date,MsgBuff &msgbuf);
	BOOL PutBuffer(short Date,MsgBuff &msgbuf);
	BOOL PutBuffer(BYTE Date,MsgBuff &msgbuf);
	BOOL PutBuffer(float Date,MsgBuff &msgbuf);
	BOOL PutBuffer(wxString Date,UINT len,MsgBuff &msgbuf);
	//-------------------------------------------------------------------
	BOOL PutBuffer(DWORD Date[],UINT len,MsgBuff &msgbuf);
	BOOL PutBuffer(int Date[],UINT len,MsgBuff &msgbuf);
	BOOL PutBuffer(WORD Date[],UINT len,MsgBuff &msgbuf);
	BOOL PutBuffer(short Date[],UINT len,MsgBuff &msgbuf);
	BOOL PutBuffer(BYTE Date[],UINT len,MsgBuff &msgbuf);
	BOOL PutBuffer(float Date[],UINT len,MsgBuff &msgbuf);
	//------------------------------------------------------------------
	virtual BOOL ParseFromBuffer(MsgBuff &msgbuf)
	{
		return FALSE;
	}
	virtual BOOL PackageBuffer(MsgBuff &msgbuf)
	{
		return FALSE;
	}
};

#define	NO_ParseFromBuffer(A) BOOL A::ParseFromBuffer(MsgBuff &msgbuf)	{\
							msgbuf.ResetPoint();	\
							return msgbuf.PointNotExceed();}
#define	NO_PackageBuffer(A) BOOL A::PackageBuffer(MsgBuff &msgbuf)	{\
							FASSERT(FALSE);	\
							return TRUE;}
#define	NO_PrintToTreeGrid(A) BOOL A::PrintToTreeGrid(TreeGridInterfer *pInterfer)	{\
							wxString strNote="";		\
							return TRUE;}


#define	SdNO_ParseFromBuffer(A)		BOOL A::ParseFromBuffer(MsgBuff &msgbuf)	{\
							msgbuf.ResetPoint();	\
							return msgbuf.PointNotExceed();}


#define	SdNO_PackageBuffer(A)		BOOL A::PackageBuffer(MsgBuff &msgbuf)	{\
							msgbuf.NewBuffer();	\
							msgbuf.SubmitPoint();	\
							return TRUE;}
#define	SdNO_PrintToTreeGrid(A)	BOOL A::PrintToTreeGrid(TreeGridInterfer *pInterfer)	{\
							wxString strNote="";		\
							return TRUE;}

#endif
