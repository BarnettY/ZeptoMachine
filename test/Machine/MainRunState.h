// MainRunState.h: interface for the CMainRunState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINRUNSTATE_H__0E4ACEA9_64B2_4FF9_A2BF_DB62D10A3AA8__INCLUDED_)
#define AFX_MAINRUNSTATE_H__0E4ACEA9_64B2_4FF9_A2BF_DB62D10A3AA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../comm/GeneralPart.h"
#include "../protocol/BaseMsg.h"
#include "../runinfo/DebugPcCmdLst.h"
#include "../runinfo/ItemList.h"


class CMachine ;
class CMainRunState
{
public:
	friend class CMachine;
	CMainRunState();
	virtual ~CMainRunState();
	virtual BOOL InitRuningState();
	void SetFather(CMachine *pfather)
	{
		m_pST=pfather;
	}
	//---------------------------------仪器命令接口函数
	BOOL AddDebugCmd(CBaseMsg* pcmd);
	BOOL EmptyPCDebugCmd(void);						//删除所有的PC调试命令
	BOOL DealWithMsg(MSG_TYPE type,CBaseMsg *pnew);		//处理来的消息
	BOOL CanDealCmd(MSG_TYPE type,CBaseMsg *pnew);		//是否能接收该指

	//  ----------------电机参数 ------------------- 
	//读取速度参数
	BOOL ReadIniToMotor();// 读取位置参数
	BOOL ReadSpeedIni();  // 读取速度
	BOOL ReadCurrentIni(); // 读取电流 
	BOOL ReadSubDiv(); // 读取细分数
	BOOL ReadMotorParameter();

	BOOL SaveSpeedIni();  // 保存速度
	BOOL SaveParamToIni(); // 保存参数
	BOOL SaveCurrentIni(); // 保存电流  

protected:
	CDebugPcCmdLst m_DEBUG_CMD_LIST;	//调试命令队列
	//---------------------------------仪器外部命令队列及其接口函数
	virtual UINT WorkProc(void);				//仪器的工作线程
	UINT RunDebugProc(void);					//调试流程

	UINT ExecPCDebugCmd(CBaseMsg* pcmd);				//处理PC传来的调试命令
	UINT ExecPartMotorDebug(BYTE Part,BYTE MotorId,BYTE act,int param);		//马达或部件调试

	BOOL DebugPart(UCHAR Part,UCHAR ACTION,int Param);   // 调试模块
	int	 DebugPartMotor(UCHAR Part,BYTE index,BYTE act,int Param);	//调试马达电机
	UINT RunIdleProc(void);								//空闲流程
	UINT RunTestProc(void);							//测试流程
	UINT RunStopProc(void);		                  //停止流程
	int  RunAllReset();                            	//复位
	BOOL Reset();
	//---------------------------------仪器变量------------------------------
	CxwEvent m_GoEvt;
	BOOL  m_Exit;  
	CMachine  *m_pST;
public: 

	UINT  SatePartRun();					//部件线程主函数
	void  TrackSwitchState(MachineStateType mode,BOOL BSend=TRUE); //切换状态
	void  SndMachineState();  // 发送仪器状态
	void  ReadIO(BYTE input[ALL_IO_COUNT]); //读取IO
	void  OpenCloseIO(BYTE IO, BYTE State); //打开IO  
	void  EmptyProgramStep(); // 清空程序步骤

	BOOL  CheckDrawerInPlace(); //检测抽屉是否到位
    BOOL  CheckFrontCover();   // 检测上盖状态
	BOOL  RcvProgramStep(CReciveProgramStep * pMsg);
   	DWORD GetCompensateTime(int StepRunIndex); //获取运行补偿时间
	void  AddProgramSimulator(); //模拟生成程序步骤
	void  ResetReportErr() ;// 复位报错标记
	ProgramStep GetProgramNextStepByStep(int step); //获取下一步反应步骤信息
public: 
	ProgramQueue  m_ProgramQueue;  //程序队列
	BYTE  m_StepRunIndex;  // 记录当前运行的步骤。  // 当前运行完成或者停止置0
	BOOL   m_bReportFrontCoverErr ; // 前盖板打开报过 不在重复
	BOOL   m_bReportDrawerErr ;   //抽屉没有到位
	BOOL   m_bNeedReset ; //  标记是否需要初始化
	BOOL   m_bNeedResetEvent; // 
	BOOL   m_bSendLeftTime ; //发送剩余时间
    DWORD  m_LastCheckDrawTime ; // 上次检查抽屉时间
	DWORD  m_LastCheckCoverTime; // 上次检查舱门时间

    BOOL   m_bNeedInit ; //标记是否需要初始化。
	UINT   m_RunStep;  //运行步数

};

#endif // !defined(AFX_MAINRUNSTATE_H__0E4ACEA9_64B2_4FF9_A2BF_DB62D10A3AA8__INCLUDED_)
