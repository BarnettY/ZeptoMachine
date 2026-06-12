// BoardCanInterface.h: interface for the BoardCanInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOARDCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
#define AFX_BOARDCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_

#include "../can/CanInterface.h"
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

class BoardCanInterface :public  CanInterface
{
public:
	BoardCanInterface();
	virtual ~BoardCanInterface();

	virtual BOOL StarCan();
	virtual BOOL ResetCan();
	virtual BOOL ReceiveCan(CAN_OBJ frameinfo[50],int &Count);
	virtual BOOL SendCan(CAN_OBJ frameinfo[50],int Count);
	virtual BOOL OpenCan(Baud_TYPE baud);
	virtual BOOL CloseCan();
protected:
	BOOL OpenCanEx();
	BOOL ChangeBaud();
	int	m_hcansocket;
	void  Translate(CAN_OBJ &frame,can_frame &info)
	{
		info.can_id=frame.ID;
		if(frame.ExternFlag>0)
			info.can_id|=CAN_EFF_FLAG;
		if(frame.RemoteFlag>0)
			info.can_id|=CAN_RTR_FLAG;
		info.can_dlc=frame.DataLen;
		memcpy(info.data,frame.Data,8);	
	}		
	void  ReTranslate(can_frame &info,CAN_OBJ &frame)
	{
		frame.ID=info.can_id&0xfffffff;
		if (info.can_id & CAN_EFF_FLAG)
			frame.ExternFlag=1;
		if (info.can_id & CAN_RTR_FLAG)
			frame.RemoteFlag=1;
		frame.DataLen=info.can_dlc;
		memcpy(frame.Data,info.data,8);	
	}		
	void  handle_err_frame(can_frame &info);
};

#endif // !defined(AFX_BOARDCANINTERFACE_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
