// UsbCanInterface.h: interface for the UsbCanInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
#define AFX_USBCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_

#include "canInterface.h"
#include "ECanVci.h"

class UsbCanInterface :public  CanInterface
{
public:
	UsbCanInterface();
	virtual ~UsbCanInterface();

	virtual BOOL StarCan();
	virtual BOOL ResetCan();
	virtual BOOL ReceiveCan(CAN_OBJ frameinfo[50],int &Count);
	virtual BOOL SendCan(CAN_OBJ frameinfo[50],int Count);
	virtual BOOL OpenCan(Baud_TYPE baud);
	virtual BOOL CloseCan();
};

#endif // !defined(AFX_USBCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
