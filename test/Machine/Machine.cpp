#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "Machine.h"
#include "wx/fileconf.h"
#include "../trace.h"

MachineStateType   g_MachineState; 
CWorkThread   *m_pWorkThread;				//控制各自部件的线程
CErrorSet	  *m_pErrSet;			//仪器的出错信息
CPart   *m_pPart;
CMainRunState	 *m_pMainTask;	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMachine::CMachine():m_DealPCcmdSemp(FALSE)
{
	m_pMainTask=new CMainRunState;
	m_pPart  = new CPart();
	m_pAllTcpIp=new CAllTcpIp;
	m_pAllTcpIp->SetFather(this);
	m_pMainTask->SetFather(this);
	m_pErrSet=new CErrorSet;
	m_pErrSet->SetFather(this);
	m_pWorkThread=new CWorkThread;
	m_pWorkThread->SetFather(this);
	m_bErrorStop =FALSE;
}

CMachine::~CMachine(void)
{
	delete m_pPart;
	delete  m_pMainTask;
	delete	m_pAllTcpIp;
}

BOOL CMachine::Initial()
{
	BOOL brtn=PartInitParam();
	brtn&=m_pWorkThread->InitWork();
	return brtn;
}

BOOL CMachine::StartRun()
{
	return m_pMainTask->WorkProc();
}
BOOL CMachine::PartInitParam()
{

#ifdef WIN32
	m_pAllTcpIp->InitSocket(TRUE, _T("127.0.0.1"),7000);
#else
	m_pAllTcpIp->InitSocket(TRUE,strIP,m_Port);  // 192.168.2.136
#endif

	return TRUE ;
}

void CMachine:: Connect()
{
#ifndef	_NO_CONNECT_PC_
	m_pAllTcpIp->Connect(1);
#endif
  // 开启串口
#ifndef WIN32
	m_pAllTcpIp->Connect(3); // 开启串口 
#endif
}

BOOL CMachine::PartDownParam()
{
	Connect();// 连接网络
	m_pPart->DownPartParam();// 电机设置参数
	return m_pMainTask->InitRuningState(); //初始化状态
}
int CMachine:: Reset()
{
	m_bErrorStop =FALSE;
    return m_pMainTask->Reset();
}

// 加载系统参数
BOOL  CMachine:: LoadSystemParam()
{
#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("set.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/set.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	wxString strsect1,strsect2;
	config->SetPath(_T("/Local"));
	strIP=config->Read(_T("IP"),_T("192.168.1.136"));
	m_Port =  config->Read(_T("PORT"),7000);// 端口
	m_LogPort =  config->Read(_T("LOGPROT "),9005);//日志端口
	m_MachineType = config->Read(_T("MACHINETYPE"),1);

	config->SetPath(_T("/RunParameter"));	//出厂参数运行参数

	m_pPart-> m_FactoryHorizontalSpeed = config->ReadDouble(_T("HORIZONTAL_SPEED"),10);//水平移动速度
    m_pPart->m_FactoryHorizontalMixSpeed = config->ReadDouble(_T("HORIZONTAL_MIX_SPEED"),10);//水平混匀速度
    m_pPart->m_FactoryHorizontalSuckMagneticSpeed = config->ReadDouble(_T("HORIZONTAL_SUCK_SPEED"),1);//水平吸磁速度
    m_pPart->m_FactoryUpDownSpeed =   config->ReadDouble(_T("UPDOWN_SPEED"),10);//升降速度

    m_pPart->m_FactoryUpDownMixSpeed =   config->ReadDouble(_T("UPDOWN_MIX_SPEED"),10); // 上下混匀速度
    m_pPart->m_FactorySuckMagneticSpeed =   config->ReadDouble(_T("SUCKMAGNETIC_SPEED"),1); //出厂吸磁速度
	m_pPart->m_StepHeight =  config->Read(_T("STEP_HEIGHT"),2); //;每次下降高度

	m_pPart->m_SuckmagneticLeftStep =  config->Read(_T("SUCKMAGNETIC_LEFT_STEP"),50); //底部上抬脉冲 ,吸磁离底部间隙
	m_pPart->m_XSegment =  config->Read(_T("XSEGMENT"),50); //水平分段次数
	m_pPart->m_MixHeight =  config->Read(_T("MIX_HEIGHT"),5);  //混匀时，上下幅度
	m_pPart->m_MixUpDownTimes1 =  config->Read(_T("MIX_UPDOWN_TIMES1"),2);//混匀上下次数1 10Ml
	m_pPart->m_MixUpDownTimes2 =  config->Read(_T("MIX_UPDOWN_TIMES2"),1); //混匀上下次数2 5ml
	m_pPart->m_CompensateValue1 =  config->Read(_T("COMPENSATE_VALUE1"),4);//混匀上下速度补偿参数
	m_pPart->m_CompensateValue2 =  config->Read(_T("COMPENSATE_VALUE2"),4);//混匀上下速度补偿参数
	m_pPart->m_MixUpDownMark = config->Read(_T("MIX_UPDOWN_MARK"),1); //混匀是否上下标记
	m_pPart->m_CheckFrontCoverMark = config->Read(_T("FRONT_COVER_MARK"),1); //前舱门是否检查标记

    delete config;
	return TRUE;
}

// 保存IP和端口

BOOL  CMachine:: SaveSystemParam()
{

#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("set.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/set.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	config->SetPath(wxT("/Local"));
	config->Write(wxT("IP"), strIP);
	config->Write(wxT("PORT"), m_Port);
	config->Write(wxT("LOGPROT"), m_LogPort);
	delete config;
	return TRUE ;
}

int  CMachine::PartReset()
{
	GLOBAL(m_pErrSet)->ClearPartsMaskErr(ALL_PART); //清空报错
	ProgramStep	EMPTY;
	GLOBAL(m_pWorkThread)->JudgeIdelAndSendPartCmdWaitRun(PART_ARM,CMD_RESET,EMPTY);
	GLOBAL(m_pWorkThread)->WaitAllPartFinish(60*1000);
	return !m_pErrSet->IsPartsMaskErr(RESET_OK_MASK);

}
