// Serial.h: interface for the CBarCodeSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BarCodeSERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_)
#define AFX_BarCodeSERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_

#ifdef  WIN32
#include "../windows/PcSerialAux.h"
#else
#include "../linux/PcSerialAux.h"
#endif

//----------------------------扫描---------------未用------------------
class CBarCodeSerial:public CTcpAgent
{
public:
	CBarCodeSerial();
	~CBarCodeSerial();
	BOOL 	OpenPort(int port,int speed);						
	BOOL Close( void )
	{
		return	m_serial.Close();
	}
	BOOL	StartRead();
	BOOL	StopRead();
	BOOL	ReadBarCode(wxString& strBarCode,int iWaitTime=1000);
	//------------CTcpAgent消息操作--------
	virtual	int		OnConnectMsg(UINT  port,BOOL connect);
	virtual	int		OnSynMsg(UINT  port,int reason);
	virtual	int		OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount);
protected:
	CxwEvent		m_CodeACKSemp;							//命令接收事件
	BYTE		m_ReadBuffer[30];
	BYTE		m_ReadCnt;
	int WritePort(unsigned char *buf,int dwCharToWrite);
	CPcSerialAux m_serial;
	BOOL     m_Start;
};

#endif // !defined(AFX_BarCodeSERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_)
