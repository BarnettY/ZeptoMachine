#if !defined(_RINGBUFFER_H_)
#define _RINGBUFFER_H_

#include "wx/thread.h"
#include "../comon.h"

/****************************************************************************/
/*						 环形缓冲的定义								*/
/****************************************************************************/
#define MAXRING		40
class CRingBuff
{
public:
	CRingBuff()
	{
		m_staroffset=0;
		m_endoffset=0;
		m_Write=0;
		for(int i=0;i<MAXRING;i++)
		{
			m_RingPtr[i]=NULL;
			m_nRingPtrcount[i]=0;
		}
	}
	virtual ~CRingBuff();
	//释放内存
	BOOL RelaseBuff(int index)
	{
		wxASSERT(index>=0);
		wxASSERT(index<MAXRING);
		if(m_RingPtr[index])
		{
//			delete []m_RingPtr[index];
//			m_RingPtr[index]=NULL;
			m_nRingPtrcount[index]=0;
		}
		return TRUE;
	}
	void SetWriteBuff();
	void SetReadBuff();
	BOOL IsReadBuff();
	//---------------------------------读取接口函数
	BOOL ResetBuffer();
	BOOL GetRingBuff(BYTE * &Ptr,int &count);				//从环形缓冲中读取数据
	BOOL GetRingBuffNotRemove(BYTE * &Ptr,int &count);	//从环形缓冲中读取数据
	BOOL RemoveRingBuff();			//接上面用，开始删除
	BOOL PutRingBuff(const BYTE *Ptr,const int count,BOOL TRCE=TRUE);					//从环形缓冲中存放数据
protected:
	BYTE *m_RingPtr[MAXRING+5];							//环形缓冲的指针
	int m_nRingPtrcount[MAXRING+5];								//环形缓冲的大小
	BYTE		m_Write;									//读写标记
	volatile int m_staroffset;									//环形缓冲的头标
	volatile int m_endoffset;									//环形缓冲的尾标
	wxCriticalSection m_RingMute;								//读写锁
};


#endif
