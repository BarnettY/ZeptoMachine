#ifndef __PCSERIALAUX_H__
#define __PCSERIALAUX_H__

#include "../comon.h"

class CPcSerialAux
{
public:
	static void EmptyAllSerial();
	void AddToGobalPtr();
	//---------------------------------	
	friend class CReadThread;
	CTcpAgent*	m_pAgent;			//消息代理
	CPcSerialAux();
	~CPcSerialAux();
	BOOL OpenPort(int port,int speed=230400);
	BOOL Close( void );
	int WritePort(unsigned char *buf,int dwCharToWrite);
	BOOL m_bOpened;
protected:
	int m_serFd;
};
/////////////////////////////////////////////////////////////////////////////
class CReadThread : public wxThread
{
public:
    CReadThread();
    virtual ~CReadThread();
    virtual void* Entry();
};
#endif//__PCSERIALAUX_H__
