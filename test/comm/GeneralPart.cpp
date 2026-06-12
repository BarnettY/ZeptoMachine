//
//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "GeneralPart.h"
//////////////////////////////////////////////////////////////////////
// class GenerPart
//////////////////////////////////////////////////////////////////////
GenerPart::GenerPart()
{
}

//---------------------动作---------------------
//下载运动参数
BOOL GenerPart::DownPartParam()
{
	InitMotorParam();
	return TRUE;
}

//出错处理
void GenerPart::IsPartErr(UCHAR index)
{
    if(0==strcmp((const char *)m_PartName[index],_T("")))
	{
#ifdef  WIN32
		FASSERT(FALSE);
#endif

	}
    int Error=- m_PartLastError[index] ;
	if(m_PartLastError[index]<0)
	{
#ifdef  WIN32
       FASSERT(FALSE);
#endif
		m_LastError=(Error&0xff)+((index+1)<<8);
		m_LastError=-m_LastError;
	}
}

