#ifndef	_DATA_DEFINE_H_
#define _DATA_DEFINE_H_
#include "../comon.h"

/****************************************************************************/
/*						 命令缓冲的定义										*/
/****************************************************************************/

#define MAX_BUFFLEN		6*1024


struct MsgBuff
{
 	friend class CBaseMsg;
 	friend class CProtocol;
	MsgBuff(BYTE *buf,int count)
	{
		m_MsgBuf=buf;
		m_MsgCount=count;
		ResetPoint();
		m_DeleteBuf =FALSE;
	}
	MsgBuff()
	{
		m_MsgBuf=NULL;
		m_MsgCount=0;
		ResetPoint();
			m_DeleteBuf =FALSE;
	}
	UINT GetBufferCount()
	{
		return m_MsgCount;
	}
	const BYTE *GetBuffer()
	{
		return m_MsgBuf;
	}
	BOOL NewBuffer()
	{
		m_MsgBuf=new BYTE[MAX_BUFFLEN];
		m_MsgCount=0;
		ResetPoint();
		m_DeleteBuf =TRUE;
		return TRUE;
	}
	BOOL ReleaseBuffer()
	{
		ResetPoint();
		m_MsgCount=0;
		if(NULL!=m_MsgBuf &&m_DeleteBuf)
			delete [] m_MsgBuf;
		m_MsgBuf=NULL;
		return TRUE;
	}
	//-----------------------
	BOOL PointNotExceed()
	{
		return (m_MsgPoint<=m_MsgCount);
	}
	UINT GetPoint()
	{
		return m_MsgPoint;
	}
	BOOL ResetPoint()
	{
		m_MsgPoint=0;
		return TRUE;
	}
	BOOL SubmitPoint()
	{
		m_MsgCount=m_MsgPoint;
		return TRUE;
	}
	WORD GetCheckSum(UINT ncount)
	{
		if(m_MsgCount<ncount)
			return 0;
		WORD sum=0;
		for(UINT i=0;i<ncount;i++)
			sum+=m_MsgBuf[i];
		return sum;
	}

protected:
 	BYTE *m_MsgBuf;		//智能指针
	UINT m_MsgCount;		//指针数据大小
	UINT m_MsgPoint;		//指针读取点
	BOOL m_DeleteBuf;


	//-----------------------------
	//分割字符串
	int StrSpilt(wxString &strInBuff,wxString &strrtn,const wxString strsplit,BOOL BDelete=TRUE)
	{
		int ifind=0;
		strrtn=_T("");
		int length=strInBuff.Length();
		if(length<=0)
			return -1;
		ifind=strInBuff.Find(strsplit);
		if(ifind<0)//最后一个，全推进去
		{
			strrtn=strInBuff;
			if(BDelete)
				strInBuff=_T("");
			return 0;
		}
		strrtn=strInBuff.Left(ifind);
		if(BDelete)
		{
			int right=length-ifind-strsplit.Length();
			if(right>0)
				strInBuff=strInBuff.Right(right);
			else
				strInBuff=_T("");
		}

		return ifind+1;
	}
};
#endif

