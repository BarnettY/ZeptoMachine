// CTcpCanInterface.h: interface for the CTcpCanInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
#define AFX_TCPCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_
#include "../comon.h"

#include "CanInterface.h"
#include "tcpcan.h"
#pragma comment(lib,"tcpcan.lib")


/////////////////////TCP2CAN/////////////////////////////////
class CTcpCanInterface :public  CanInterface
{
public:
	CTcpCanInterface();
	virtual ~CTcpCanInterface();

	virtual BOOL StarCan();
	virtual BOOL ResetCan();
	virtual BOOL ReceiveCan(CAN_OBJ frameinfo[50],int &Count);
	virtual BOOL SendCan(CAN_OBJ frameinfo[50],int Count);
	virtual BOOL OpenCan(Baud_TYPE baud);
	virtual BOOL CloseCan();
};
/////////////////////TCP2COM/////////////////////////////////
class CTcpComInterface:public  ComInterface
{
public:
	CTcpComInterface();
	virtual ~CTcpComInterface();

	virtual BOOL ComClose(BYTE index);
	virtual BOOL ComOpen(BYTE index);
	virtual BOOL ComSend(BYTE index,BYTE *buff,int Count);
	virtual BOOL ComReceive(BYTE index,BYTE *buff,int&Count);
};
#endif // !defined(AFX_TCPCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
