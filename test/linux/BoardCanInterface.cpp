#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "../trace.h"
#include "../comon.h"
#include "BoardCanInterface.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BoardCanInterface::BoardCanInterface()
{
	m_hcansocket=-1;
}

BoardCanInterface::~BoardCanInterface()
{
	if(m_hcansocket>=0)
		CloseCan();
}

BOOL BoardCanInterface::ReceiveCan(CAN_OBJ frameinfo[50],int &Count)
{
	Count=0;
	if(m_hcansocket>=0)
	{
		struct can_frame fr, frdup;
		struct timeval tv;
		fd_set rset;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&rset);
		FD_SET(m_hcansocket, &rset);
		int ret = select(m_hcansocket+1, &rset, NULL, NULL, &tv);
		if (ret == 0)
		{
			Count=0;
			return FALSE;
		}
		ret = read(m_hcansocket, &frdup, sizeof(frdup));
		if (ret < sizeof(frdup))
		{
			Count=0;
			CanTraceLog("read failed");
			return FALSE;
		}
		if (frdup.can_id & CAN_ERR_FLAG) /* 检查数据帧是否错误 */
		{
			Count=0;
			handle_err_frame(frdup);
			return FALSE;
		}
		ReTranslate(frdup,frameinfo[0]);
		Count=1;
	}
	if(Count<=0)
		return FALSE;
	else
		return TRUE;
}
BOOL BoardCanInterface::SendCan(CAN_OBJ frameinfo[50],int Count)
{
	struct can_frame fr, frdup;
	if(m_hcansocket>=0)
	{
		for(int t=0;t<Count;t++)
		{
			Translate(frameinfo[t],frdup);
			int ret = write(m_hcansocket, &frdup, sizeof(frdup));	    /* 把接收到的数据帧发送出去 */
			if (ret < 0)
			{
				CanTraceLog("write failed");
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL BoardCanInterface::OpenCanEx()
{
	int ret;
	struct sockaddr_can addr;
	struct ifreq ifr;
	m_hcansocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);		/* 创建套接字	*/
	if (m_hcansocket < 0)
	{
		CanTraceLog("socket PF_CAN failed");
		return FALSE;
	}

	/* 把套接字绑定到can0接口	*/
	strcpy(ifr.ifr_name, "can0");
	ret = ioctl(m_hcansocket, SIOCGIFINDEX, &ifr);
	if (ret < 0)
	{
		CanTraceLog("ioctl failed");
		return FALSE;
	}

	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	ret = bind(m_hcansocket, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0)
	{
		CanTraceLog("bind failed");
		return FALSE;
	}
	return TRUE;
}

BOOL BoardCanInterface::OpenCan(Baud_TYPE baud)
{
	int ret;
	//配置CAN0为100K
	ret=system("ifconfig can0 down");
	if ((127==ret)||(ret<0))
	{
		CanTraceLog("ifconfig can0 down");
		return FALSE;
	}
	ret=system("echo 100000 > /sys/devices/platform/FlexCAN.0/bitrate");
	if ((127==ret)||(ret<0))
	{
		CanTraceLog("echo 100000 > /sys/devices/platform/FlexCAN.0/bitrate");
		return FALSE;
	}
	ret=system("ifconfig can0 up");
	if ((127==ret)||(ret<0))
	{
		CanTraceLog("ifconfig can0 up");
		return FALSE;
	}

	if(!OpenCanEx())
	{
		CanTraceLog("OpenCanEx failed");
		return FALSE;
	}
	wxMilliSleep(1000);
	if(!ChangeBaud())
	{
		CanTraceLog("ChangeBaud failed");
		return FALSE;
	}
	wxMilliSleep(1000);
	CloseCan();
	wxMilliSleep(1000);

	//配置CAN0为250K
	ret=system("ifconfig can0 down");
	if ((127==ret)||(ret<0))
	{
		CanTraceLog("ifconfig can0 down");
		return FALSE;
	}
	ret=system("echo 250000 > /sys/devices/platform/FlexCAN.0/bitrate");
	if ((127==ret)||(ret<0))
	{
		CanTraceLog("echo 250000 > /sys/devices/platform/FlexCAN.0/bitrate");
		return FALSE;
	}
	ret=system("ifconfig can0 up");
	if ((127==ret)||(ret<0))
	{
		CanTraceLog("ifconfig can0 up");
		return FALSE;
	}
	wxMilliSleep(1000);

	if(!OpenCanEx())
	{
		CanTraceLog("OpenCanEx failed");
		return FALSE;
	}
	return TRUE;
}
BOOL BoardCanInterface::ChangeBaud()
{
	//通知PMC改速度为250K
	CAN_OBJ frmchbaud;
	memset(&frmchbaud, 0, sizeof(CAN_OBJ));
	frmchbaud.ID = 0X01fc0003;
	frmchbaud.ExternFlag = 1;
	frmchbaud.RemoteFlag = 0;
	frmchbaud.SendType = 0;
	frmchbaud.DataLen = 2;
	frmchbaud.Data[0]=0X03;
	frmchbaud.Data[1]=0X00;

	return SendCan(&frmchbaud,1);
}

BOOL BoardCanInterface::CloseCan()
{
	if(m_hcansocket>=0)
		close(m_hcansocket);
	m_hcansocket=-1;
    return TRUE;
}
BOOL BoardCanInterface::StarCan()
{
	return TRUE;
}
BOOL BoardCanInterface::ResetCan()
{
	return TRUE;
}

void  BoardCanInterface::handle_err_frame(can_frame &info)
{
	if (info.can_id & CAN_ERR_TX_TIMEOUT)
	{
		CanTraceLog("CAN_ERR_TX_TIMEOUT");
	}
	if (info.can_id & CAN_ERR_LOSTARB)
	{
		CanTraceLog("CAN_ERR_LOSTARB");
	}
	if (info.can_id & CAN_ERR_CRTL)
	{
		CanTraceLog("CAN_ERR_CRTL");
	}
	if (info.can_id & CAN_ERR_PROT)
	{
		CanTraceLog("CAN_ERR_PROT");
	}
	if (info.can_id & CAN_ERR_TRX)
	{
		CanTraceLog("CAN_ERR_TRX");
	}
	if (info.can_id & CAN_ERR_ACK)
	{
		CanTraceLog("CAN_ERR_ACK");
	}
	if (info.can_id & CAN_ERR_BUSOFF)
	{
		CanTraceLog("CAN_ERR_BUSOFF");
	}
	if (info.can_id & CAN_ERR_BUSERROR)
	{
		CanTraceLog("CAN_ERR_BUSERROR");
	}
	if (info.can_id & CAN_ERR_RESTARTED)
	{
		CanTraceLog("CAN_ERR_RESTARTED");
	}
}
