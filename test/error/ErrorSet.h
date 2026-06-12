#pragma once
#include "errcode.h"

////////////////////////////////////////////////////////
///////////////	    仪器出错代码	/////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
//

struct ERROR_INFO
{
	BYTE m_btPart;
	BYTE m_btMotor;
	int  m_btCode;
	wxString m_wErrinfo ;
	BYTE m_bLevel;
};
WX_DECLARE_LIST(ERROR_INFO, ERROR_INFOList);


class CMachine;
class CErrorSet
{
public:
	CErrorSet(void);
	~CErrorSet(void);

	void SetFather(CMachine *pfather)
	{
		m_pST=pfather;
	}
	BOOL IsPartErr(BYTE part);									//部件是否有错
	BOOL IsPartsMaskErr(UINT Mask);							//有些部件是否有错
	void SetPartErr(BYTE part,int errcode,wxString wErrinfo=_T(""),BYTE level =2);		//设置部件错误
	int  GetPartErrCode(BYTE part);							//得到任意部件的出错代码
	void ClearPartErr(BYTE part);								//清除部件错误
	void ClearPartsMaskErr(UINT Mask);						//清除部件错误
	//-------------------错误报告接口函数---------
	BOOL ReportAllStoreErr(void);								//发送报告给PC
	BOOL ReportErr(ERROR_INFO &info);							//发送报告给PC
	BOOL StoreErrCode(int part,int errcode,BYTE LEVEL=0, wxString m_wErrinfo=_T("")); //报告出错代码
protected:	
	CMachine *m_pST;
	int  m_nErrcodeAry[PART_END];							//每个部件的出错代码,负值表示出错
	//-------------------
	ERROR_INFOList m_ErrorList;			//错误报告的队列
	wxCriticalSection m_ListMute;
};
