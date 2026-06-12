#if !defined(AFX_GENCOMMUNICATE_H__35B012B6_2340_4D4B_9A20_E95A15F92BA0__INCLUDED_)
#define AFX_GENCOMMUNICATE_H__35B012B6_2340_4D4B_9A20_E95A15F92BA0__INCLUDED_

#include "RingBuff.h"
#include "wx/list.h"
#include "wx/socket.h"
#include "../comon.h"
/****************************************************************************/
/*				 接收判断头标识结构的定义						*/
/****************************************************************************/
//可能的头标识
struct HeadInfo
{
	HeadInfo()
	{
		m_pBegin=NULL;
		m_Len=0;
		m_Abort=0;
	}
	BYTE *m_pBegin;					//在带解析区的位置
	UINT	m_Len;						//可能的长度
	BYTE  m_Abort;					//是否废弃
};
/////////////////////////////////////////////////////////////////////////////
WX_DECLARE_LIST(HeadInfo, HeadInfoList);

/****************************************************************************/
/*				 带重发功能的SOCKET辅助类							*/
/****************************************************************************/
class CGenCommunicate
{
// Construction
public:
	friend class CParserThread;
	friend class CSendThread;
	friend class CListenThread;
	BOOL	m_TraceLog;
	CGenCommunicate();

	BOOL SetWriteReadMode(int mode)		//读写模式，可设置只读、只写
	{
		m_WRMode=mode;
		return TRUE;
	}
	virtual void SetPort(BYTE PORT)					//设置SOCKET序号
	{
		m_nPort=PORT;
	}

	BOOL IsConnect()						//是否已经连接上SOCKET
	{
		return m_Connect;
	}
	BOOL ConnectPort();

	//-----------------以下为打开关闭函数---------------
	BOOL InitCommunicate();							//初始化连接
	BOOL EmptyAllBuffer(BOOL bread,BOOL bwrite,BOOL BRestSN=FALSE);		//清空缓冲
	BOOL DealWithAck(UINT SN);		//处理ACK，已经收到ACK的不重发
	//-----------------以下为socket交互函数---------------
	int ReadSocket();
	virtual BOOL ReadDate(const BYTE * buf,int len );	//SOCKET读函数调用,提取完整包
	DWORD WriteDate(BYTE buf[],DWORD dwCharToWrite);//SOCKET写函数封装，支持缓冲
	virtual BOOL OnParseData(BYTE *pBuff,UINT  ncount)=0;			//已经解析好的数据
	virtual BOOL SendSocketDate(const BYTE * buf,int len );
	//-----------------------------------------------
public:
	virtual ~CGenCommunicate();
	virtual BOOL OnCannotRcvAck()=0;							//发送到数据没有收到ACK
protected:
	//////////////////////////////////////////////////////////////////////////////
	//	待解析的缓存操作
	//////////////////////////////////////////////////////////////////////////////
	BYTE * MemFindPackHead(const BYTE *Begin,int len);
	void EmptyHeadLst(void);									//清空未完成的头
	volatile BYTE	*m_pWaitSearchBegin;		//搜索的开始
	HeadInfoList		m_HeadLst;				//未完成的头队列

	virtual BOOL AddUnParse(const BYTE * buf,UINT len);	//添加到待解析的缓存
	BOOL InsertHeadParse(const BYTE * buf,int len);		//重新插入到待解析的缓存
	BYTE	*		m_pWaitParseBuf;			//待解析的缓存
	volatile UINT		m_UnparseCount;			//待解析的缓存大小

	CRingBuff m_WriteRingBuf;						//环形写缓冲
	CxwEvent		m_SendSemp;
	CRingBuff m_ReadRingBuf;						//环形读缓冲，解析好的包缓冲
	CxwEvent		m_ParseSemp;
	//////////////////////////////////////////////////////////////////////////////
	//	其他成员变量
	//////////////////////////////////////////////////////////////////////////////
	//判断是否是ACK一类不需要应答的指令
	BOOL IsNeedResend(const BYTE * buf,int len );
	int m_nPort;									//第几个端口
	int m_BInited;								//是否已经初始化
	int m_WRMode;								//标记模式,-1(只读)，-2(只写),-3(读写)
	volatile BOOL m_Connect;						//	SOCKET 状态

	wxSocketBase *m_psock;
	wxString	 m_remoteHost;  					//远程主机IP地址
	int     	 m_Tcpport;						  //远程主机端口
	BOOL         m_bServer;                        // 是否服务端
};

/////////////////////////////////////////////////////////////////////////////
class CListenThread : public wxThread
{
public:
	CListenThread(CGenCommunicate* pComm);
	virtual ~CListenThread();
	virtual void* Entry();
	void Listen();
	int m_port;
	wxString	 m_ip;  					//IP地址
	BOOL         m_bServer; 

protected:
	CGenCommunicate* m_pComm;
};
class CParserThread : public wxThread
{
public:
    CParserThread(CGenCommunicate* pComm);
    virtual ~CParserThread();
    virtual void* Entry();
protected:
	CGenCommunicate* m_pComm;
};
class CSendThread : public wxThread
{
public:
    CSendThread(CGenCommunicate* pComm);
    virtual ~CSendThread();
    virtual void* Entry();
	
protected:
	CGenCommunicate* m_pComm;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENCOMMUNICATE_H__35B012B6_2340_4D4B_9A20_E95A15F92BA0__INCLUDED_)
