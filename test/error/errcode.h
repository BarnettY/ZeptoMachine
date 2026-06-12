#ifndef	_ERROR_H_
#define _ERROR_H_

#include "../comon.h"
//越是硬件错误不可恢复，数值越小
#define	SUCCESS						1				//成功

//////////////////////////////////////////////////////////
//以下为警告
//////////////////////////////////////////////////////////

#define ERROR_PART_RESET			        -1        // 复位出错
#define	ERROR_FRONT_COVER_IS_OPEN		    -2		 //前舱门打开
#define	ERROR_DRAWER_NOT_IN_PLACE		    -3		 //抽屉不到位
//部件出错
#define ERROR_PART_TIMEOUT			       -100				//部件运动超时 

//--等待命令运行超时
#define ERROR_RUNPART_TIMEOUT			    -105			//等待命令运行超时

////////////////////////////////////////////////////////
///////////////	   轴操作出错	/////////////////////
////////////////////////////////////////////////////////
#define ERROR_MOVE_TIMEOUT				-110		//电机运动完成超时
#define ERROR_MOVE_HOMEERR			-111			//电机运动时收到错误的HOME信号
#define ERROR_MOVE_LEFTLIMITERR		-112			//电机运动时收到错误的左限位信号
#define ERROR_MOVE_RIGHTLIMITERR		-113			//电机运动时收到错误的右限位信号
#define ERROR_MOVE_LIQUIDERR			-114			//电机运动时收到错误的液位信号
#define ERROR_MOVE_BARCODE_END		-115			//电机运动时收到错误的条码信号   
#define ERROR_MOVE_BUSY				-116			//电机运动前上个动作未完成


#define ERROR_AFTER_NOT_HOMEERR		   -120		//电机运动完成后不是预料的HOME电平
#define ERROR_AFTER_NOT_LEFTLIMITERR	-121			//电机运动完成后不是预料的左限位信号
#define ERROR_AFTER_NOT_RIGHTLIMITERR  -122			//电机运动完成后不是预料的右限位信号
#define ERROR_AFTER_NOT_LIQUIDERR		-123			//电机运动完成后不是预料的液位信号
//#define ERROR_AFTER_NOT_B				-124			//电机运动完成后，手臂没有下降到位
#define ERROR_AFTER_ENCODEERR			-125			//电机运动完成后收到错误的编码器信号  --没有

////////////////////////////////////////////////////////
///////////////	   CAN出错		/////////////////////
////////////////////////////////////////////////////////
#define ERROR_CAN_DISCON				-130			//CAN总线断开连接
#define ERROR_CAN_UNRESIGTER			-131			//CAN控制器没有注册
#define ERROR_CAN_TIMEOUT				-132			//CAN通讯超时
#define ERROR_CAN_NOTHARDWARE		    -133			//CAN控制器没有响应


wxString InTRACE_BYTE(wxString name,BYTE DATE);
wxString InTRACE_BOOL(wxString name,BOOL DATE);
wxString InTRACE_UINT(wxString name,UINT DATE);
wxString InTRACE_int(wxString name,int DATE);
wxString InTRACE_WORD(wxString name,WORD DATE);
wxString InTRACE_DWORD(wxString name,DWORD DATE);
wxString InTRACE_ARY_DWORD(wxString name,DWORD DATE[],UINT LEN);

#define _TRACE_(T,A)		InTRACE_##T(#A,A)
#define _TRACEARY_(T,A,B)	InTRACE_ARY_##T(#A,A,B)
#endif
