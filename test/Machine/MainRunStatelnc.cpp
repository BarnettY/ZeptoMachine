//////////////////////////////////////////////////////////////////////
#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MainRunState.h"
#include "Machine.h"
#include "wx/fileconf.h"
#include "../motor/BasicMotor.h"

//模拟添加程序
void CMainRunState:: AddProgramSimulator()
{
	ProgramStep step;

   m_ProgramQueue.m_Id =0;
   m_ProgramQueue.m_Type=KIT_TYPE_ONE;

   step.StepId =1 ;
   step.HolePos=1;
   step. MixTime =100;
   step. MagneticTime =120;
   step. WaitTime =100;
   step.Volume =1000;
   step.SpeedLevel=5;

   m_ProgramQueue.AddStep(step);
   step.StepId =2 ;

   step.HolePos=1;
   step. MixTime =100;
   step. MagneticTime =120;
   step. WaitTime =100;
   step.Volume =1000;
   step.SpeedLevel=5;

   m_ProgramQueue.AddStep(step);
   step.StepId =3 ;

   step.HolePos=1;
   step. MixTime =100;
   step. MagneticTime =120;
   step. WaitTime =100;
   step.Volume =1000;
   step.SpeedLevel=5;
   m_ProgramQueue.AddStep(step);
}


#define   MAX_42_CURRENT  120 //A1_2,  42最大电流
#define   MAX_42_LOCK_CURRENT  80 // A0_8
#define   MOTOR_42_CURRENT  100
#define   MOTOR_42_LOCKCURRENT  40

#define   MOTOR_57_CURRENT  160
#define   MOTOR_57_LOCKCURRENT  80

#define   MOTOR_28_CURRENT  60
#define   MOTOR_28_LOCKCURRENT  30

#define   MOTOR_SMALL_CURRENT  20
#define   MOTOR_SMALL_LOCKCURRENT  10


//电流值转换成枚举
int   GetCurrent(int  Value)
{
	int iCurrent =0;	// TODO  数组对应
	int CurrentArry[32]={0,5,10,15,20,25,30,35,40,45,50,60,70,80,90,95,100,110,115,120,125,130,135,140,145,150,155,160,170,180,190,200};
	for (int i=0;i<32;i++)
	{
       if (Value<=CurrentArry[i])
       {
		    iCurrent =i;
			break;
       }

	}
	return iCurrent;
}

//motorType  42  57  28 电机   1 -42  2-57  3-28   4- 其他小电机
void ReadCurrent(wxFileConfig *config ,wxString strsect1,CBasicMotor * m_Motor,BYTE motorType)
{

	config->SetPath(strsect1);
	wxString strsect2;
	int iMotorCurrent =0, iMotorLockCurrent=0;  // 读取不到默认电流
	switch(motorType)
	{
	case  1:
		iMotorCurrent = MOTOR_42_CURRENT;
		iMotorLockCurrent = MOTOR_42_LOCKCURRENT;
		break;
	case  2:
		iMotorCurrent = MOTOR_57_CURRENT;
		iMotorLockCurrent = MOTOR_57_LOCKCURRENT;
		break;
	case  3:
		iMotorCurrent = MOTOR_28_CURRENT;
		iMotorLockCurrent = MOTOR_28_LOCKCURRENT;
		break;
	case  4:
		iMotorCurrent = MOTOR_SMALL_CURRENT;
		iMotorLockCurrent = MOTOR_SMALL_LOCKCURRENT;
		break;
	default:
		iMotorCurrent = MOTOR_SMALL_CURRENT;
		iMotorLockCurrent = MOTOR_SMALL_LOCKCURRENT;
		break;
	}
	int TempValue =0;
	strsect2 =_T("MotorLockCurrent");
	TempValue = config->ReadLong(strsect2,iMotorLockCurrent) ;
    m_Motor->m_iLockCurrent =GetCurrent(TempValue);// 进行数值转换， 
	strsect2 =_T("MotorCurrent");
	TempValue = config->ReadLong(strsect2,iMotorCurrent) ;
	m_Motor->m_iCurrent =GetCurrent(TempValue);

}


BOOL  CMainRunState:: ReadCurrentIni() // 读取电流
{
// 电流改掉 ，直接电流大小*100
#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("MotorCurrent.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/MotorCurrent.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	wxString strsect1,strsect2;

	strsect1 =_T("/Motor1");  //
	ReadCurrent(config , strsect1, m_pPart->m_pXMotor,1);

	strsect1 =_T("/Motor2");  //
	ReadCurrent(config , strsect1, m_pPart->m_pZMotor,2);

	strsect1 =_T("/Motor3");  //
	ReadCurrent(config , strsect1, m_pPart->m_pMagBarMotor,3);
	delete config;
	return TRUE;

}


#define   MOTOR_SPEED_HOME  2000  // 默认初始化
#define   MOTOR_SPEED_HOME_START  1000  // 默认初始化
#define   MOTOR_SPEED_START  1000  //
#define   MOTOR_SPEED_END  2000  //


void ReadSpeed(wxFileConfig *config ,wxString strsect1,CBasicMotor * m_Motor,BYTE num)
{
	config->SetPath(strsect1);
	wxString strsect2;
    int size =sizeof(m_Motor->m_iSpeed) / sizeof(m_Motor->m_iSpeed[0]);
    if(size>num)
	  size =num;
	for (int i=0  ; i<size ;i++ )
	{
		strsect2.Printf(_T("MotorSpeed%d"),i);
		m_Motor->m_iSpeed[i]  = config->ReadLong(strsect2,MOTOR_SPEED_END) ;
		strsect2.Printf(_T("MotorSpeedStart%d"),i);
		m_Motor->m_iSpeedStart[i] = config->ReadLong(strsect2,MOTOR_SPEED_START) ;
	}

}
void ReadStep(wxFileConfig *config ,wxString strsect1,CBasicMotor * m_Motor,BYTE num)
{
	config->SetPath(strsect1);
	wxString strsect2;
	int size=sizeof(m_Motor->m_Offset) / sizeof(m_Motor->m_Offset[0]);
	if(size >num)
		size =num;
	for (int i=0  ; i<size ;i++ )
	{
		strsect2.Printf(_T("Step%d"),i);
		m_Motor->m_Offset[i] = config->ReadLong(strsect2,0) ;
		//TRACE(_T("ReadStep::MotorName =%s,i=%d,Step=%d"),m_Motor->m_strName,i,m_Motor->m_Offset[i] );
	}
}

BOOL CMainRunState::ReadIniToMotor()
{
#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("MotorParameter.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/MotorParameter.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	//读取全部电机参数，并保存
	wxString strsect1,strsect2;
	int StepCount = 10;
	strsect1 =_T("/Motor1");  // 进样区
	ReadStep(config , strsect1, m_pPart->m_pXMotor,StepCount);

	strsect1 =_T("/Motor2");  //
	ReadStep(config , strsect1, m_pPart->m_pZMotor,StepCount);

	strsect1 =_T("/Motor3");  //
	ReadStep(config , strsect1, m_pPart->m_pMagBarMotor,StepCount);

	delete config;
	return TRUE;  //
}

void ReadDiv(wxFileConfig *config ,wxString strsect1,CBasicMotor * m_Motor,BYTE defVal)
{
	config->SetPath(strsect1);
	wxString strsect2;
	strsect2 =_T("SubDiv");
	m_Motor->m_subDiv = config->ReadLong(strsect2,defVal) ;

}
BOOL CMainRunState:: ReadSubDiv()
{

#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("MotorSubDiv.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/MotorSubDiv.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	wxString strsect1,strsect2;
	// 设置细分数
	BYTE  bSubDiv = 4;

	strsect1 =_T("/Motor1");  // 进样区
	ReadDiv(config , strsect1, m_pPart->m_pXMotor,bSubDiv);
	strsect1 =_T("/Motor2");  //
	ReadDiv(config , strsect1, m_pPart->m_pZMotor,bSubDiv);
	strsect1 =_T("/Motor3");  //
	ReadDiv(config , strsect1, m_pPart->m_pMagBarMotor,bSubDiv);


	delete config;
	return TRUE;
}

BOOL  CMainRunState:: ReadSpeedIni()
{

#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("MotorSpeed.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/MotorSpeed.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	//读取全部电机参数，并保存

	wxString strsect1,strsect2;
	int SpeedCount = 10;
	strsect1 =_T("/Motor1");  // 进样区
	ReadSpeed(config , strsect1, m_pPart->m_pXMotor,SpeedCount);
	strsect1 =_T("/Motor2");  //
	ReadSpeed(config , strsect1, m_pPart->m_pZMotor,SpeedCount);
	strsect1 =_T("/Motor3");  //
	ReadSpeed(config , strsect1, m_pPart->m_pMagBarMotor,SpeedCount);
	delete config;
	return TRUE;

}


void  SaveParameter(wxFileConfig *config ,wxString strsect1,CBasicMotor * m_Motor,BYTE Num)
{
	config->SetPath(strsect1);
	wxString strsect2;
	for (int i=0 ; i<Num ;i++ )
	{
		strsect2.Printf(_T("Step%d"),i);
		config->Write(strsect2, m_Motor->m_Offset[i]);
	}
}

//保存位置参数
BOOL CMainRunState::SaveParamToIni()
{

#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("MotorParameter.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/MotorParameter.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif

	wxString strsect1,strsect2;
	strsect1 =_T("/Motor1");  // 进样区
	int  StepCount =10;
	SaveParameter(config , strsect1, m_pPart->m_pXMotor,StepCount);
	strsect1 =_T("/Motor2");
	SaveParameter(config , strsect1, m_pPart->m_pZMotor,StepCount);
	strsect1 =_T("/Motor3");  //
	SaveParameter(config , strsect1, m_pPart->m_pMagBarMotor,StepCount);

	delete config;
	return TRUE ;
}

// 读取电机参数
BOOL CMainRunState:: ReadMotorParameter()
{

	ReadCurrentIni(); //电机电流参数
	ReadSubDiv(); //细分
	ReadSpeedIni(); //速度
	ReadIniToMotor(); //电机位置参数
 	return TRUE ;
}
// 读取IO
void CMainRunState:: ReadIO(BYTE input[ALL_IO_COUNT])
{

	 memset(input,0,ALL_IO_COUNT);
	 BOOL BVaild[ALL_IO_COUNT]={FALSE};
     m_pPart->m_pZMotor->GetMotorRightLimit(BVaild[0]);
	 wxMilliSleep(10);
     m_pPart->m_pZMotor->GetMotorLiquid(BVaild[1]); 
	 for(int i=0;i<ALL_IO_COUNT;i++)
	 {
		if(BVaild[i])
		{
			input[i]=1;
		}
#ifdef _SIMULATOR_
        input[i]=1;
#endif
	}
}


UINT CMainRunState::ExecPartMotorDebug(BYTE Part,BYTE MotorId,BYTE act,int param)
{
	if(0xff==MotorId)
	{
		DebugPart(Part,act,param);		// 模块调试
	}
	else
	{
		DebugPartMotor(Part,MotorId,act,param);		// 单个电机调试
	}
	return 1;
}
// 调试模块
BOOL CMainRunState::DebugPart(UCHAR Part,UCHAR ACTION,int Param)
{
	switch(Part)
	{
	case  PART_ARM:
		m_pPart->PartDebug(ACTION);
		break;

	default:
		break;
	}
	return  0;
}

//调试马达电机
int	CMainRunState::DebugPartMotor(UCHAR Part,BYTE index,BYTE act,int Param)
{
	switch(Part)
	{
	case  PART_ARM:
		m_pPart->DebugPartMotor(index,act,Param);
		break;
	default:
		break;
	}
	return 0;
}
