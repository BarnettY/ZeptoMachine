// Serial.h: interface for the CTemptureSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TempSERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_)
#define AFX_TempSERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_

#ifdef  WIN32
#include "../windows/PcSerialAux.h"
#else
#include "../linux/PcSerialAux.h"
#endif

// -----------------------------温控 --未用------------------
class CTemptureSerial:public CTcpAgent
{
public:
	CTemptureSerial();
	~CTemptureSerial();
	BOOL 	Connect(int port);						//与服务器端建立连接
	BOOL Close( void )
	{
		return	m_serial.Close();
	}
	BOOL	ReadTemp(float &temp);
	BOOL    SetTemp(float temp);
	BOOL	SetOffset(float fOffset);
	BOOL	SetCHYS(float fCHYS);
	BOOL	SetPID(WORD P,WORD I,WORD D);
	BOOL	ReadPID(WORD &P,WORD &I,WORD &D);


	BOOL    SetPT100();
	BOOL    SetPonP(BYTE Value =0x01 );

    BOOL    StarHeat();

	BOOL    StopHeat();


	//------------CTcpAgent消息操作--------
	virtual	int		OnConnectMsg(UINT  port,BOOL connect);
	virtual	int		OnSynMsg(UINT  port,int reason);
	virtual	int		OnReceiveMsg(UINT  port,BYTE *pBuff,UINT  ncount);
protected:
	CxwEvent		m_TempACKSemp;							//命令接收事件
	BYTE		m_ReadBuffer[30];
	BYTE		m_ReadCnt;
	int WritePort(unsigned char *buf,int dwCharToWrite);
	CPcSerialAux m_serial;
	wxCriticalSection	m_ComeMute;
	float m_temp;
};

#endif // !defined(AFX_TempSERIAL_H__1F78EA22_307F_4862_A541_FEA74473C34F__INCLUDED_)
