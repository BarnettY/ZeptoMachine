#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ErrorSet.h"
#include "errcode.h"
#include "../Machine/Machine.h"
#include "../trace.h"

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(ERROR_INFOList);

CErrorSet::CErrorSet(void)
{
	memset(m_nErrcodeAry,0,sizeof(m_nErrcodeAry));
	//	m_ErrorList.DeleteContents(true);
}

CErrorSet::~CErrorSet(void)
{
}
/****************************************************************************/
/*						仪器出错接口函数																			*/
/****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//	设置部件有错
//参数	part	只能是单个部件
//		errcode	错误代码
//		BReport	允许包括错误
//////////////////////////////////////////////////////////////////////////////
void CErrorSet::SetPartErr(BYTE part,int errcode,wxString wErrinfo,BYTE level)
{
	//如果出错，标记部件不可用
	if (errcode<0)
	{
		errcode =-errcode;
	}

	StoreErrCode(part,errcode,level,wErrinfo);
	if(level>1  )
	{
		m_pST->m_bErrorStop =TRUE; //
		m_pMainTask->TrackSwitchState(MACHINE_STOP,TRUE);
	}


}
//////////////////////////////////////////////////////////////////////////////
//得到任意部件的出错代码
//参数	part	只能是单个部件
//返回
//		0		表示无错
//		errcode	错误代码
//////////////////////////////////////////////////////////////////////////////
//部件是否有错
BOOL CErrorSet::IsPartErr(BYTE part)
{
	FASSERT(part<PART_END);
	return (m_nErrcodeAry[part]<0);
}
//有些部件是否有错
BOOL CErrorSet::IsPartsMaskErr(UINT Mask)
{
	for(BYTE i=0;i<PART_END;i++)
	{
		UINT a=1<<i;
		if(Mask&a)
		{
			if(IsPartErr(i))
				return TRUE;
		}
	}
	return FALSE;
}
int CErrorSet::GetPartErrCode(BYTE part)
{
	FASSERT(part<PART_END);
	if(IsPartErr(part))
		return m_nErrcodeAry[part];
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
//	设置部件有错
//参数	part	只能是单个部件
//////////////////////////////////////////////////////////////////////////////
void CErrorSet::ClearPartErr(BYTE part)
{
	m_nErrcodeAry[part]=0;
}
//参数	part	1<<part的MASK
void CErrorSet::ClearPartsMaskErr(UINT Mask)
{
	for(BYTE i=0;i<PART_END;i++)
	{
		int a=1<<i;
		if(Mask&a)
			ClearPartErr(i);
	}
}
/****************************************************************************/
/*						错误报告接口函数																			*/
/****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//	报告出错代码
//参数	part	ALL_PART--所有部件
//				其他
//		errcode	错误代码
//////////////////////////////////////////////////////////////////////////////
BOOL CErrorSet::StoreErrCode(int part,int errcode,BYTE LEVEL,wxString m_wErrinfo)
{

	ERROR_INFO *errInfo=new ERROR_INFO;
	errInfo->m_btMotor=(errcode>>8)&0xff;
	errInfo->m_btCode=(errcode&0xff);
	errInfo->m_btPart=part;

	TRACE(_T("CErrorSet::StoreErrCode part=%d ,Motor=%d,errcode=%d ,LEVEL=%d ,Errinfo =%s"),part,errInfo->m_btMotor ,errInfo->m_btCode, LEVEL, m_wErrinfo.mb_str(wxConvUTF8));
	errInfo->m_wErrinfo= m_wErrinfo;
	errInfo->m_bLevel =  LEVEL;
	ReportErr(*errInfo);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//	发送报告给PC
//参数	part	ALL_PART		所有部件
//				其他
//////////////////////////////////////////////////////////////////////////////
BOOL CErrorSet::ReportAllStoreErr(void)
{
	//把要处理的调试队列COPY一份,再把队列清空,需要互斥
	ERROR_INFOList Deal_LIST;
	Deal_LIST.DeleteContents(true);
	m_ListMute.Enter();
	ERROR_INFOList::Node* node = m_ErrorList.GetFirst();
	while (node)
	{
		ERROR_INFO *info = node->GetData();
		Deal_LIST.Append(info);
		node = node->GetNext();
	}
	m_ErrorList.clear();
	m_ListMute.Leave();
	//把错误列表打包
	node = Deal_LIST.GetFirst();
	while (node)
	{
		ERROR_INFO *info = node->GetData();
		ReportErr(*info);
		node = node->GetNext();
	}
	//删除错误列表
	Deal_LIST.clear();
	return TRUE;
}

BOOL CErrorSet::ReportErr(ERROR_INFO &info)
{
	CSndTestError *pErr=new CSndTestError;
	TRACE(_T("CErrorSet::ReportErr Part=%d,Code=%d,Errinfo=%s"),info.m_btPart,info.m_btCode,info.m_wErrinfo.mb_str(wxConvUTF8));
	pErr->m_btPart=info.m_btPart;
	pErr->m_btMotor= info.m_btMotor ;
	pErr->m_btCode=info.m_btCode;
	pErr->m_wErrinfo=info.m_wErrinfo;
	pErr->m_bLevel =info.m_bLevel;
	return m_pST->m_pAllTcpIp->SendMsgNotWait(pErr);

}
