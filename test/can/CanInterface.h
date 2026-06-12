// anInterface.h: interface for the CanInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANINTERFACE_H__B4F667CF_817D_489E_A4A9_4C7FCE5727C6__INCLUDED_)
#define AFX_ANINTERFACE_H__B4F667CF_817D_489E_A4A9_4C7FCE5727C6__INCLUDED_

#include "../comon.h"


//2.定义CAN信息帧的数据类型。
typedef  struct  _CAN_OBJ{
	UINT	ID;
	UINT	TimeStamp;
	BYTE	TimeFlag;
	BYTE	SendType;
	BYTE	RemoteFlag;//是否是远程帧
	BYTE	ExternFlag;//是否是扩展帧
	BYTE	DataLen;
	BYTE	Data[8];
	BYTE	Reserved[3];
}CAN_OBJ,*P_CAN_OBJ;

enum Baud_TYPE
{
	M1=0,
	K800,
	K500,
	K250,
	K125,
	K100,
	k50,
	K20,
	K10
};
class CanInterface
{
public:
	CanInterface();
	virtual ~CanInterface();

	virtual BOOL StarCan();
	virtual BOOL ResetCan();
	virtual BOOL ReceiveCan(CAN_OBJ frameinfo[50],int &Count);
	virtual BOOL SendCan(CAN_OBJ frameinfo[50],int Count);
	virtual BOOL OpenCan(Baud_TYPE baud);
	virtual BOOL CloseCan();
};
class ComInterface  
{
public:
	ComInterface();
	virtual ~ComInterface();

	virtual BOOL ComClose(BYTE index)=0;
	virtual BOOL ComOpen(BYTE index)=0;
	virtual BOOL ComSend(BYTE index,BYTE *buff,int Count)=0;
	virtual BOOL ComReceive(BYTE index,BYTE *buff,int&Count)=0;
};
 
#endif // !defined(AFX_ANINTERFACE_H__B4F667CF_817D_489E_A4A9_4C7FCE5727C6__INCLUDED_)
