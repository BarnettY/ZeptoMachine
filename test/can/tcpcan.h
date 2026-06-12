#if !defined(AFX_TCPCAN_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_)
#define AFX_TCPCAN_H__C761F9F0_F269_40BD_8DA4_873DB5339A4E__INCLUDED_



#define TCPDllAPI __declspec(dllimport)

extern "C" TCPDllAPI BOOL TcpOpenCan(BYTE index);
extern "C" TCPDllAPI BOOL TcpCloseCan(BYTE index);
extern "C" TCPDllAPI BOOL TcpResetCan(BYTE index);
extern "C" TCPDllAPI BOOL TcpReceiveCan(BYTE index,CAN_OBJ frameinfo[50],int &Count);
extern "C" TCPDllAPI BOOL TcpSendCan(BYTE index,CAN_OBJ frameinfo[50],int Count);
extern "C" TCPDllAPI BOOL TcpComSend(BYTE index,BYTE *buff,int Count);
extern "C" TCPDllAPI BOOL TcpComReceive(BYTE index,BYTE *buff,int&Count);

#endif