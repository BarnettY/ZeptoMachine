//
//////////////////////////////////////////////////////////////////////

// ------------------待修改-------------------
#if !defined(AFX_GeneralPart_H__FD1D81C9_7DF4_4D47_A6F3_4F8C7ED39525__INCLUDED_)
#define AFX_GeneralPart_H__FD1D81C9_7DF4_4D47_A6F3_4F8C7ED39525__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../comon.h"
//////////////////////////////////////////////////////////////////////
// class GenerPart
//////////////////////////////////////////////////////////////////////
//零件，可能含有多个电机
class GenerPart
{
public:
	GenerPart();
	virtual ~GenerPart(){};			//FIXBUG:内存泄漏 

	void EmptyError()
	{
		m_LastError=0;
		memset(m_PartLastError,0,sizeof(m_PartLastError));
	}
protected:
	virtual void IsPartErr(UCHAR index);	//出错处理
public:

	virtual BOOL DownPartParam();//下载运动参数
protected:	
	virtual void InitMotorParam()=0;						//预备设置各动作参数组
	TCHAR	m_MotorName[6][20];				//部件包含的电机名称
	TCHAR	m_strName[15];
	TCHAR	m_strUniqueName[20];

	int		m_LastError;					//部件最后出错信息
	int		m_PartLastError[7];			//部件包含的电机最后出错信息
	TCHAR	m_PartName[7][20];			//部件包含的电机名称

};
#define IF_ERROR_QUIT  	if(m_LastError<0) { return m_LastError;}
#define PART_ERROR_QUIT(A)  IsPartErr(A);	{IF_EMER_STOP; IF_ERROR_QUIT} 
#define PART_ERROR_QUIT_1(A)  IsPartErr(A);	{ IF_ERROR_QUIT} 
#endif // !defined(AFX_GeneralPart_H__FD1D81C9_7DF4_4D47_A6F3_4F8C7ED39525__INCLUDED_)
