//[fan 11-04-01] TCP_IP 连接
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "../comon.h"
#include "../trace.h"
#include "RingBuff.h"

/****************************************************************************/
/*						 环形缓存的定义								*/
/****************************************************************************/

#define READ_BUF_SIZE	(6*1024)
#define WRITE_BUF_SIZE	(6*1024)
CRingBuff::~CRingBuff()
{
	for(int i=0;i<MAXRING;i++)
	{
		if(NULL!=m_RingPtr[i])
		{
			delete []m_RingPtr[i];
			m_RingPtr[i]=NULL;
		}
	}
}
void CRingBuff::SetWriteBuff()
{
	m_Write=1;
	for(int i=0;i<MAXRING;i++)
	{
		m_RingPtr[i]=new BYTE[WRITE_BUF_SIZE+100];
	}
}
void CRingBuff::SetReadBuff()
{
	for(int i=0;i<MAXRING;i++)
	{
		m_RingPtr[i]=new BYTE[READ_BUF_SIZE+100];
	}
}
BOOL CRingBuff::ResetBuffer()
{
	m_RingMute.Enter();
	m_staroffset=0;
	m_endoffset=0;
	m_RingMute.Leave();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//	从环形缓冲中是否有数据要读
//返回 FALSE	环形缓存为空
//	   TRUE		不为空
//////////////////////////////////////////////////////////////////////////////
BOOL CRingBuff::IsReadBuff()
{
	if(m_staroffset==m_endoffset)	//环形缓冲为空
		return FALSE;
	return TRUE;
}
//接上面用，开始删除
BOOL CRingBuff::RemoveRingBuff()
{
	if(m_staroffset==m_endoffset)	//环形缓冲为空
		return FALSE;
	FASSERT(m_staroffset>=0);
	FASSERT(m_staroffset<MAXRING);
	m_RingMute.Enter();
	m_nRingPtrcount[m_staroffset]=0;
	int starafter=m_staroffset+1;
	if(starafter>=MAXRING)
		starafter=0;
	m_staroffset=starafter;
	m_RingMute.Leave();
	return TRUE;
}

//从环形缓冲中读取数据,但不删除
BOOL CRingBuff::GetRingBuffNotRemove(BYTE * &Ptr,int &count)
{
	Ptr=NULL;
	if(m_staroffset==m_endoffset)	//环形缓冲为空
		return FALSE;
	FASSERT(m_staroffset>=0);
	FASSERT(m_staroffset<MAXRING);
	m_RingMute.Enter();
	Ptr=m_RingPtr[m_staroffset];
	count=m_nRingPtrcount[m_staroffset];
	m_RingMute.Leave();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//	从环形缓冲中读取数据
// 参数	Ptr	指向缓存的指针，取出后由调用方删除
//		count	Ptr的大小
//返回 FALSE	环形缓存为空
//	   TRUE		不为空
//////////////////////////////////////////////////////////////////////////////
BOOL CRingBuff::GetRingBuff(BYTE * &Ptr,int &count)
{
	Ptr=NULL;
	if(m_staroffset==m_endoffset)	//环形缓冲为空
		return FALSE;
	m_RingMute.Enter();
	FASSERT(m_staroffset>=0);
	FASSERT(m_staroffset<MAXRING);
	Ptr=m_RingPtr[m_staroffset];
	count=m_nRingPtrcount[m_staroffset];
	m_nRingPtrcount[m_staroffset]=0;
	int starafter=m_staroffset+1;
	if(starafter>=MAXRING)
		starafter=0;
	m_staroffset=starafter;
	m_RingMute.Leave();
	//	TRACE(_T("-%x----GetRingBuff %d,m_staroffset=%d,m_endoffset=%d\n"),this,count,m_staroffset,m_endoffset);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//	从环形缓冲中存放数据
// 参数	Ptr	指向缓存的指针，环形缓冲自己拷贝一份,原来Ptr由调用方删除
//		count	Ptr的大小
//返回 FALSE	环形缓存已满
//	   TRUE		不满
//////////////////////////////////////////////////////////////////////////////
BOOL CRingBuff::PutRingBuff(const BYTE *Ptr,const int count,BOOL TRCE)
{
	//	if(TRCE)
	//		TRACE(_T("-%x-----PutRingBuff %d,m_staroffset=%d,m_endoffset=%d\n"),this,count,m_staroffset,m_endoffset);
	FASSERT(count>0);
	if(m_Write)
	{
		if(count>=WRITE_BUF_SIZE)
	{
			FASSERT(FALSE);
			TRACE(_T("CRingBuff PutRingBuff ERROR put buff>=WRITE_BUF_SIZE"));
		return FALSE;
		}
	}
	else
	{
		if(count>=READ_BUF_SIZE)
		{
			FASSERT(FALSE);
			TRACE(_T("CRingBuff PutRingBuff ERROR put buff>=READ_BUF_SIZE"));
			return FALSE;
		}
	}
	m_RingMute.Enter();
	int noffsetAfterPut=m_endoffset+1;
	if(noffsetAfterPut>=MAXRING)
		noffsetAfterPut=0;
	if(m_staroffset==noffsetAfterPut)	//环形缓冲已满
	{
		TRACE(_T("CRingBuff PutRingBuff ERROR put buff will be abandon"));
		m_RingMute.Leave();
		return FALSE;
	}
	FASSERT(m_endoffset>=0);
	FASSERT(m_endoffset<MAXRING);
	m_nRingPtrcount[m_endoffset]=count;
	if((m_Write>0)&&(count>0)&&(count<WRITE_BUF_SIZE))
		memmove(m_RingPtr[m_endoffset],Ptr,count);
	if((0==m_Write)&&(count>0)&&(count<READ_BUF_SIZE))
		memmove(m_RingPtr[m_endoffset],Ptr,count);
	m_RingPtr[m_endoffset][count]=0;//最后设置为0，方便字符串
	m_endoffset=noffsetAfterPut;
	m_RingMute.Leave();
	return TRUE;
}

