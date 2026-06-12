// BarcodeCom.cpp: implementation of the CBarcodeCom class.
//
//////////////////////////////////////////////////////////////////////

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "BarcodeCom.h"
#include "../linux/PcSerialAux.h"

#include "../trace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBarcodeCom::CBarcodeCom():RcvSemp(TRUE)
{
	comPort=0xff;
	m_pCom=NULL;
}

CBarcodeCom::~CBarcodeCom()
{
	if((NULL!=m_pCom)&&(0xff!=comPort))
	{
		m_pCom->Close();
		delete m_pCom;
	}
}
BOOL CBarcodeCom::Init(BYTE Port)
{
	comPort=Port;
	m_pCom=new CPcSerialAux;
	m_pCom->m_pAgent=this;
	return m_pCom->OpenPort(Port,9600);
	return TRUE;
}
BOOL CBarcodeCom::TigerBarCode()
{
	EmptyAllBuffer();
	BYTE cmm[]={0x1b,0x5a,0x0d};
	if(NULL!=m_pCom)
		return m_pCom->WritePort(cmm,sizeof(cmm));
	else
		return FALSE;
}
BOOL CBarcodeCom::CloseBarCode()
{
	BYTE cmm[]={0x1b,0x59,0x0d};
	if(NULL!=m_pCom)
		return m_pCom->WritePort(cmm,sizeof(cmm));
	else
		return FALSE;
}
BOOL CBarcodeCom::ReadBarCode(wxString &strcode)
{
	strcode="";
	if(NULL!=m_pCom)
	{
		wxMilliSleep(2000);
		if(wxSEMA_NO_ERROR==RcvSemp.WaitTimeout(500))
		{
			int ncount=m_ReadCnt;
			TCHAR 	*Buffer=new 	TCHAR[ncount+30];
			memset(Buffer,0,ncount+30);
			//有时候收到奇怪的字符
			int i=0;
			for(i=0;i<ncount;i++)
			{
				Buffer[i]=m_buffer[i];
				if('\r'==m_buffer[i])
					break;
			}
			if((i>0)&&(i<ncount))
			{
				TRACE(_T("CBarcodeCom::ReadBarCode count=%d,name=%s"),i,Buffer);
				strcode=wxString(Buffer);
				delete []Buffer;
				return TRUE;
			}
			else
			{
				delete []Buffer;
				CloseBarCode();
				return FALSE;
			}
		}
		else
		{
			CloseBarCode();
			return FALSE;
		}
	}
	else
		return FALSE;
}

//------------消息操作--------
int	CBarcodeCom::OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount)
{
	return AddUnParse(pBuff,ncount);
}
wxString GetBinString(const BYTE *buff,UINT len)
{
	wxString strtemp,strrtn;
	for(int i=0;i<len;i++)
	{
		if(0!=i)
			strrtn+=" ";
		strtemp.Printf("%0.2X",buff[i]);
		strrtn+=strtemp;
	}
	return strrtn;
}
//添加到待解析的缓存
BOOL CBarcodeCom::AddUnParse(const BYTE * buf,UINT len)
{
	TRACE(_T("CBarcodeCom AddUnParse!m_nPort=%d m_UnparseCount=%d,Len=%d"),comPort,m_ReadCnt,len);
	wxString readbufstr=GetBinString(buf,len);
	const char *buff =readbufstr.mb_str();//wxStrdup(strline.mb_str());
	TRACE(buff);

	if(len+m_ReadCnt>=MAX_CNT)
	{
		EmptyAllBuffer();
		TRACE(_T("CBarcodeCom AddUnParse m_nPort=%d discard !Len=%d"),comPort,m_ReadCnt);
		return FALSE;
	}
	if(len>0)
		memmove((void *)(m_buffer+m_ReadCnt),buf,len);

	m_ReadCnt+=len;

	//查找\r\n,看是否结束接收
	if(wxSEMA_NO_ERROR==RcvSemp.WaitTimeout(0))
		return TRUE;

	BYTE *pFindHead = (BYTE *)memchr(m_buffer,'\r',m_ReadCnt);
	if(pFindHead!=NULL)
	{
		RcvSemp.SetEvent();
	}
	return TRUE;
}
//清空缓冲
BOOL  CBarcodeCom::EmptyAllBuffer()
{
	RcvSemp.ResetEvent();
	m_ReadCnt=0;
	return TRUE;
}
