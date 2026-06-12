#ifndef __PCSERIALAUX_H__
#define __PCSERIALAUX_H__

#include "../comon.h"

#define FC_DTRDSR		0x01
#define FC_RTSCTS		0x02
#define FC_XONXOFF		0x04

class CPcSerialAux
{
public:
	friend class CReadThread;
	CTcpAgent*	m_pAgent;			//消息代理
	CPcSerialAux();
	~CPcSerialAux();
	BOOL OpenPort(int port,int speed=115200);
	BOOL Close( void );
	int WritePort(unsigned char *buf,int dwCharToWrite);
	BOOL m_bOpened;
protected:
	int SendData(unsigned char *buffer, int size );
	int ReadData( void *, int );
	int ReadDataWaiting( void );
	
	BOOL Open( int nPort = 2, int nBaud = 9600 );
	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
};
/////////////////////////////////////////////////////////////////////////////
class CReadThread : public wxThread  
{
public:
    CReadThread(CPcSerialAux* pComm);
    virtual ~CReadThread();
    virtual void* Entry();
protected:
	CPcSerialAux* m_pserial;
};

#endif//__PCSERIALAUX_H__
