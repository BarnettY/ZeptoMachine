// BarcodeCom.h: interface for the CBarcodeCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BARCODECOM_H__F9D8E6CA_2DAD_463E_A1AD_50E379A2E343__INCLUDED_)
#define AFX_BARCODECOM_H__F9D8E6CA_2DAD_463E_A1AD_50E379A2E343__INCLUDED_

#include "../comon.h"
#include "CanInterface.h"

class CPcSerialAux;
#define MAX_CNT	300
class CBarcodeCom:public CTcpAgent
{
public:
	CBarcodeCom();
	virtual ~CBarcodeCom();
	BOOL Init(BYTE Port);
	BOOL TigerBarCode();
	BOOL ReadBarCode(wxString &strcode);
	//------------CTcpAgent消息操作-------- 
	virtual	int		OnConnectMsg(UINT  port,BOOL connect){return 0; }
	virtual	int		OnSynMsg(UINT  port,int reason){return 0; }
	virtual	int		OnAckMsg(UINT  port,int  ack){ return 0;}
	virtual	int		OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount);
protected:
	BOOL CloseBarCode();
	BYTE comPort;
	CPcSerialAux	*m_pCom;
	//----------------------------------------------------------------------------
	virtual BOOL AddUnParse(const BYTE * buf,UINT len);	//添加到待解析的缓存
	BOOL EmptyAllBuffer();				//清空缓冲
	BYTE m_buffer[MAX_CNT+100];
	volatile int	m_ReadCnt;
	CxwEvent	 RcvSemp;									//表示发生的事件
};

#endif // !defined(AFX_BARCODECOM_H__F9D8E6CA_2DAD_463E_A1AD_50E379A2E343__INCLUDED_)
