// GenCommunicate.cpp : implementation file
//

#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "../test.h"
#include "GenCommunicate.h"
#include "../trace.h"
#include "../comon.h"
#include "../runinfo/datadefine.h"

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(HeadInfoList);
#define BIG_BUF_SIZE	(6*1024)
#define BUFFMAXLEN		(1000)
//#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/****************************************************************************/
/*				 带重发功能的SOCKET辅助类							*/
/****************************************************************************/
CGenCommunicate::CGenCommunicate():m_ReadRingBuf(),m_WriteRingBuf(),m_ParseSemp(FALSE),m_SendSemp(FALSE)
{
	m_Connect=FALSE;
	m_UnparseCount=0;
	m_pWaitParseBuf=NULL;
	m_pWaitSearchBegin=NULL;
	m_psock=NULL;

	m_nPort=0;
	m_BInited=FALSE;
	m_WRMode=3;	//读写模式
	m_TraceLog=FALSE;
	m_HeadLst.DeleteContents(true);
}

CGenCommunicate::~CGenCommunicate()
{
	EmptyAllBuffer(TRUE,TRUE,TRUE);
	if(NULL!=m_pWaitParseBuf)
		delete []m_pWaitParseBuf;

	if(m_psock)
		delete m_psock;
}

//////////////////////////////////////////////////////////////////////////////
//						接口函数
//////////////////////////////////////////////////////////////////////////////
//处理ACK，已经收到ACK的不重发
BOOL CGenCommunicate::DealWithAck(UINT SN)
{
	return TRUE;
}
//-----------------以下为打开关闭函数---------------
//关闭连接
//初始化连接
BOOL CGenCommunicate::InitCommunicate()
{
	if(m_BInited)
		return TRUE;
	//打开客户端socket
	//建立事件句柄
	m_pWaitParseBuf=new BYTE [BIG_BUF_SIZE+100];
	memset((void *)m_pWaitParseBuf,0,BIG_BUF_SIZE+100);
	m_pWaitSearchBegin=m_pWaitParseBuf;
	if(m_WRMode&0x1)
	{
		m_ReadRingBuf.SetReadBuff();
	}
	if(m_WRMode&0x2)
	{
		m_WriteRingBuf.SetWriteBuff();
	}
	m_BInited=TRUE;
	if(m_WRMode&0x1)
		new CParserThread(this);  // 
	if(m_WRMode&0x2)
		new CSendThread(this);  // 
	return TRUE;
}

//清空缓冲
BOOL CGenCommunicate::EmptyAllBuffer(BOOL bread,BOOL bwrite,BOOL BRestSN)
{
	//	if(BRestSN)
	//		ReSetMsgSN();
	if(!m_BInited)
		return TRUE;
	if(bread&&(m_WRMode&0x1))
	{
		m_ReadRingBuf.ResetBuffer();
	}
	if(bwrite&&(m_WRMode&0x2))
	{
		m_WriteRingBuf.ResetBuffer();
	}
	return TRUE;
}

BOOL CGenCommunicate::ConnectPort()
{
    //
	new CListenThread(this);
	return TRUE;
}
//-----------------以下为socket交互函数---------------
int CGenCommunicate::ReadSocket()  // 读取
{
	BYTE *buf=new BYTE[BUFFMAXLEN];
	BYTE *wrbuff;
	int wrcnt=0;
	while(1)
	{
		if (!m_psock->IsConnected())
		{
			wxLogMessage("ThreadWorker: not connected",wxLOG_Error);
			delete []buf;
			return 0;
		}
		m_psock->Read(buf, BUFFMAXLEN);
		if (m_psock->Error())
		{
			wxLogMessage("ThreadWorker: Read error",wxLOG_Error);
			delete []buf;
			return 0;
		}	
		int iCount = m_psock->LastCount();
		if(iCount>0)
			ReadDate(buf,iCount);
		while(m_WriteRingBuf.IsReadBuff())
		{
			if(m_WriteRingBuf.GetRingBuff(wrbuff,wrcnt))
			{
				m_psock->Write(wrbuff,wrcnt);
				wxMilliSleep(2);
			}

		}		
		wxMilliSleep(2);
	}
	delete [] buf;
	return 0;
}
BOOL CGenCommunicate::SendSocketDate(const BYTE * buf,int len )
{
	if(m_psock&&m_psock->IsConnected())
	{
	}
	return TRUE;
}
//SOCKET读函数调用,提取完整包
BOOL CGenCommunicate::ReadDate(const BYTE * buf,int len )
{
	//	TRACE(_T("CGenCommunicate-------OnRead resv!m_nPort=%d,Len=%d"),m_nPort,len);
	if(0==(m_WRMode&0x1))
		return FALSE;
	if(len<=0)
		return FALSE;
	if(m_TraceLog)		//LOG无需解析
		return FALSE;

	//添加到待解析的缓存中
	if(!AddUnParse(buf,len))
		return FALSE;
	//从内存中开始搜索的位置
	BYTE *pBeginFind =(BYTE *) m_pWaitSearchBegin;
	//从内存中搜索的结束位置
	BYTE *PSearchEnd=(BYTE *)m_pWaitParseBuf+m_UnparseCount-3;
	//搜索所有有可能的包头
	while(pBeginFind<PSearchEnd)
	{
		//查找包括头的位置
		BYTE *pFindHead = MemFindPackHead(pBeginFind,PSearchEnd-pBeginFind);
		if(pFindHead!=NULL)
		{
			HeadInfo *addinfo=new HeadInfo;
			addinfo->m_pBegin=pFindHead;
			if(pFindHead+8<=m_pWaitParseBuf+m_UnparseCount)
			{
				BYTE chtmp=pFindHead[4];
				addinfo->m_Len=chtmp;
				addinfo->m_Len<<=8;
				chtmp=pFindHead[5];
				addinfo->m_Len+=chtmp;
				addinfo->m_Len+=8;			//这个长度还要加上2位校验码
				m_HeadLst.Append(addinfo);
			}
			else
			{
				delete addinfo;
			}
			pBeginFind=pFindHead+1;
		}
		else
		{
			m_pWaitSearchBegin=pBeginFind;
			break;
		}
	}
	//分析所有有可能的包头
	BYTE *pNextHead=NULL;
	HeadInfoList::Node* node = m_HeadLst.GetFirst();
	while (node)
	{
		HeadInfoList::Node* delnode;
		bool bdelete(false);
		HeadInfo *info = node->GetData();
		BYTE *pEnd=info->m_pBegin+info->m_Len-1;
		//如果预料的结束还没有到，分析下一个
		if(pEnd>=m_pWaitParseBuf+m_UnparseCount)
		{
			node = node->GetNext();
			continue;
		}
		else
		{
			//检验是否是有效数据
			BYTE chtemp=info->m_pBegin[info->m_Len-2];
			UINT sum=chtemp;
			sum<<=8;
			chtemp=info->m_pBegin[info->m_Len-1];
			sum+=chtemp;
			MsgBuff tempbuf(info->m_pBegin,info->m_Len);
			if(tempbuf.GetCheckSum(info->m_Len-2)==sum)
			{
				pNextHead=info->m_pBegin+info->m_Len;
				//				TRACE(_T("CGenCommunicate ReadDate m_nPort=%d Find Package Index From %d to %d"),m_nPort,info->m_pBegin-m_pWaitParseBuf,info->m_pBegin-m_pWaitParseBuf+info->m_Len);
				//作为一个包放入环形缓存内
				m_ReadRingBuf.PutRingBuff(info->m_pBegin,info->m_Len,TRUE);
				m_ParseSemp.SetEvent();
				delnode=node;
				bdelete=true;
			}
		}
		node = node->GetNext();
		//		if(bdelete)
		//			m_HeadLst.DeleteNode(delnode);
	}
	//在pNextHead之前的包全部抛弃
	if(pNextHead!=NULL)
	{
		HeadInfoList::Node* node = m_HeadLst.GetFirst();
		while (node)
		{
			HeadInfo *info = node->GetData();
			if(info->m_pBegin>=pNextHead)
			{
				info->m_pBegin=info->m_pBegin-pNextHead+m_pWaitParseBuf;
			}
			else
			{
				info->m_Abort=1;
			}
			node = node->GetNext();
		}
		node = m_HeadLst.GetFirst();
		while (node)
		{
			HeadInfo *info = node->GetData();
			HeadInfoList::Node* delnode=node;
			node = node->GetNext();
			if(info->m_Abort>0)
			{
				m_HeadLst.DeleteNode(delnode);
			}
		}
		//		TRACE(_T("CGenCommunicate ReadDate m_nPort=%d discard before Index %d"),m_nPort,pNextHead-m_pWaitParseBuf);
		//移动下次查找的头
		if(m_pWaitSearchBegin>=pNextHead)
		{
			m_pWaitSearchBegin=m_pWaitSearchBegin-pNextHead+m_pWaitParseBuf;
		}
		else
		{
			m_pWaitSearchBegin=m_pWaitParseBuf;
		}
		//移动内存
		InsertHeadParse(pNextHead,m_pWaitParseBuf+m_UnparseCount-pNextHead);
	}
	//	m_HeadLst.Clear();
	return TRUE;
}
//SOCKET写函数封装，支持缓冲
DWORD CGenCommunicate::WriteDate(BYTE buf[],DWORD dwCharToWrite)
{
	if(0==(m_WRMode&0x2))
		return 0;
	if(m_Connect)
	{
		m_WriteRingBuf.PutRingBuff(buf,dwCharToWrite);
		m_SendSemp.SetEvent();
	}
	return dwCharToWrite;
}

//判断是否是ACK一类不需要应答的指令
BOOL CGenCommunicate::IsNeedResend(const BYTE * buf,int len )
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//						待解析的缓存操作
//////////////////////////////////////////////////////////////////////////////
//清空未完成的头
void CGenCommunicate::EmptyHeadLst(void)
{
	m_HeadLst.Clear();
}
//添加到待解析的缓存
BOOL CGenCommunicate::AddUnParse(const BYTE * buf,UINT len)
{
	if(0==(m_WRMode&0x1))
		return FALSE;
	//	TRACE(_T("CGenCommunicate AddUnParse!m_nPort=%d m_UnparseCount=%d,Len=%d"),m_nPort,m_UnparseCount,len);

	if(len+m_UnparseCount>=BIG_BUF_SIZE)
	{
		EmptyHeadLst();
		//		TRACE(_T("CGenCommunicate AddUnParse m_nPort=%d discard !Len=%d"),m_nPort,m_UnparseCount);
		m_UnparseCount=0;
		m_pWaitSearchBegin=m_pWaitParseBuf;
		return FALSE;
	}
	if(len>0)
		memmove((void *)(m_pWaitParseBuf+m_UnparseCount),buf,len);

	m_UnparseCount+=len;
	return TRUE;
}
//重新插入到待解析的缓存
BOOL CGenCommunicate::InsertHeadParse(const BYTE * buf,int len)
{
	if(0==len)
	{
		m_UnparseCount=0;
		return TRUE;
	}
	//	TRACE(_T("CGenCommunicate InsertHeadParse!m_nPort=%d m_UnparseCount=%d,Len=%d"),m_nPort,m_UnparseCount,len);
	if(len>=BIG_BUF_SIZE)
	{
		m_UnparseCount=0;
		return FALSE;
	}
	if(len>0)
		memmove(m_pWaitParseBuf,buf,len);
	m_UnparseCount=len;
	return TRUE;
}
//内存查找
BYTE * CGenCommunicate::MemFindPackHead(const BYTE *Begin,int len)
{
	const BYTE *pBeginFind =Begin;
	//查找可能的包括头的位置
	BYTE *pFindHead=(BYTE *)memchr(pBeginFind,0x33,len);
	if((pFindHead!=NULL)&&(0x44==*(pFindHead+1))&&(0x10==*(pFindHead+2)))	//找到了
		return pFindHead;
	return NULL;
}
//////////////////////////////////////////////////////////////////////////////
//			启动读写监控线程
//////////////////////////////////////////////////////////////////////////////
//启动读写监护线程
CParserThread::CParserThread(CGenCommunicate* pComm)
{
	m_pComm=pComm;
	Create();
	Run();
}
void* CParserThread::Entry() 
{
	while(1)
	{
		m_pComm->m_ParseSemp.WaitTimeout(1000);
		//已经接受到数据事件
		BYTE *buff;
		int ncount=0;
		while(m_pComm->m_ReadRingBuf.IsReadBuff())
		{
			if(m_pComm->m_ReadRingBuf.GetRingBuff(buff,ncount))
			{
				m_pComm->OnParseData(buff,ncount);
			}
			//			wxMilliSleep(1);
		}
	}
	return NULL;
}
CParserThread::~CParserThread()
{
}

CSendThread::CSendThread(CGenCommunicate* pComm)
{
	m_pComm=pComm;
	Create();
	Run();
}
void* CSendThread::Entry()
{
	BYTE *buf=new BYTE[BUFFMAXLEN];
	BYTE *wrbuff;
	int wrcnt=0;
	while(1)
	{
		if(!m_pComm->m_psock)
		{
			m_pComm->m_WriteRingBuf.ResetBuffer();
			wxMilliSleep(1000);
			continue;
		}
		if (!m_pComm->m_psock->IsConnected())
		{
			m_pComm->m_WriteRingBuf.ResetBuffer();
			m_pComm->m_psock->Destroy();
			m_pComm->m_psock=NULL;
			wxMilliSleep(1000);
			continue;
		}
		m_pComm->m_psock->Read(buf, BUFFMAXLEN);
		if (m_pComm->m_psock->Error())
		{
			m_pComm->m_WriteRingBuf.ResetBuffer();
			wxMilliSleep(1000);
			continue;
		}	
		int iCount = m_pComm->m_psock->LastCount();
		if(iCount>0)
			m_pComm->ReadDate(buf,iCount);
		while(m_pComm->m_WriteRingBuf.IsReadBuff())
		{
			if(m_pComm->m_WriteRingBuf.GetRingBuff(wrbuff,wrcnt))
			{
				m_pComm->m_psock->Write(wrbuff,wrcnt);
				wxMilliSleep(2);
			}

		}		
		wxMilliSleep(2);
	}
	delete []buf;
}
CSendThread::~CSendThread()
{
}


CListenThread::CListenThread(CGenCommunicate* pComm)
{
	m_pComm=pComm;
	Create();
	Run();
}

void* CListenThread::Entry()
{
	Listen();
	return NULL;
}
typedef wxIPV4address IPaddress;
void CListenThread::Listen()
{
	if (m_pComm->m_bServer)
	{

		wxIPV4address addrReal;
		addrReal.Service(m_pComm->m_Tcpport);
		// Create the socket
	   if ( !addrReal.Hostname(m_pComm->m_remoteHost))
	   {
		   TRACE("ERROR: couldn't get the address we bound to");
	   }
	   else
	   {
		   wxString strtmp=addrReal.IPAddress();
		   TRACE("Server listening at %s:%u",strtmp.mb_str(wxConvUTF8), addrReal.Service());
	   }

		wxSocketServer* m_server = new wxSocketServer(addrReal);
		// We use IsOk() here to see if the server is really listening
		if (! m_server->IsOk())
		{
			wxLogMessage("Could not listen at the specified port !");
			return ;
		}

		m_server->Notify(false);
		m_server->SetFlags(wxSOCKET_WAITALL|wxSOCKET_BLOCK);
		while(1)
		{
			wxString s = _("OnSocketEvent: ");
			wxSocketBase *sock;
			sock = m_server->Accept(true);
			if (!sock)
			{
				wxMilliSleep(1000);
				continue;
			}

			IPaddress addr;
			if ( !sock->GetPeer(addrReal) )
			{
				wxMilliSleep(1000);
				continue;
			}

			sock->Notify(false);
			sock->SetFlags(wxSOCKET_NOWAIT);
			m_pComm->m_WriteRingBuf.ResetBuffer();
			m_pComm->m_psock=sock;
			m_pComm->m_Connect=TRUE;
			//		m_pComm->ReadSocket();
			//		m_pComm->m_psock->Destroy();
			//		m_pComm->m_psock=NULL;
		}

	}
	else // 客户端
	{

		wxSocketClient  *m_pclient =new wxSocketClient();	// socket客户端
		wxIPV4address addr;
		addr.Hostname(m_pComm->m_remoteHost);
		addr.Service(m_pComm->m_Tcpport);
		m_pclient->Notify(false);
		wxIPV4address local;
		  
		while(1)
		{
			if (!m_pclient->IsConnected())
			{
				
				m_pclient->Connect(addr,true);// 等待接收事件
				if (m_pclient->IsConnected())
				{
					m_pComm->m_WriteRingBuf.ResetBuffer();
					m_pComm->m_psock=m_pclient;
					m_pComm->m_Connect=TRUE;
				}
			}
			else
			{
				wxMilliSleep(1000);
			}

		}
   
	}
	return ;
}
CListenThread::~CListenThread()
{
}

