#ifdef	VLD
#include <vld.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Part.h"
#include "../comon.h"
#include "../trace.h"
#include "wx/listimpl.cpp"
#include "Machine.h"
#include "wx/fileconf.h"
#include "../trace.h"
//////////////////////////////////////////////////////////////////////
//模块
//////////////////////////////////////////////////////////////////////
CPart::CPart(): m_UVLampEvt(TRUE),m_PauseEvent(TRUE)
{
	_tcscpy(m_strUniqueName,_T("CPart"));

	m_pXMotor= new CXMotor(1,1);; // X轴横向移动
	m_pZMotor=new CZMotor(1,2);; // Z轴升降
	m_pMagBarMotor=new CMagneticBarMotor(1,3);; // 磁棒升降


	_tcscpy(m_PartName[0],_T("XMotor"));
	_tcscpy(m_pXMotor->m_strName,m_PartName[0]);

	_tcscpy(m_PartName[1],_T("ZMotor"));
	_tcscpy(m_pZMotor->m_strName,m_PartName[1]);

	_tcscpy(m_PartName[2],_T("MagneticBarMotor"));
	_tcscpy(m_pMagBarMotor->m_strName,m_PartName[2]);

	m_ThreadExit =TRUE;
	m_SpeedLevel =1;
	m_LampStatus=0;  // 0关  1开
	m_UVLampStatus=0; //紫外灯
	m_StepHeight= 2;
	m_MixHeight= 5; //
	m_MixUpDownTimes1=2;
	m_MixUpDownTimes2=1;

	m_CompensateValue1=4;
	m_CompensateValue2=4;
	m_MixUpDownMark =1;
	m_SuckmagneticLeftStep=50;
	m_UVTime=0;
    m_XSegment =50; //水平移动分段
	m_bAlreadySuckMagnetic = FALSE;
}
CPart::~CPart()
{
	delete m_pXMotor; // X轴横向移动
	delete m_pZMotor; // Y轴升降
	delete m_pMagBarMotor; // 磁棒升降
}

void CPart::InitMotorParam()
{

	m_pMagBarMotor->m_PerMMToPluse  = MMOTOR_MM_TO_PULSE;//每mm 对应脉冲
	m_pXMotor->m_PerMMToPluse = XMOTOR_MM_TO_PULSE ;

	if (g_ST->m_MachineType==2) //样机
	{
		m_pMagBarMotor->SetDirDown(DIR_CCW) ; // 样机转向不同
		m_pMagBarMotor->m_PerMMToPluse  = MMOTOR_MM_TO_PULSE_1;//每mm 对应脉冲
		m_pXMotor->m_PerMMToPluse = XMOTOR_MM_TO_PULSE_1 ;
		m_pXMotor->SetDirDown(DIR_CCW) ; // 样机转向不同
	}


	m_pXMotor->SetCurrentParamRes(TRUE); // 设置电流
	m_pMagBarMotor->SetCurrentParamRes(TRUE);

	m_pZMotor->SetCurrentParamRes(FALSE);// 3.2A 模式。

	//--------------------------------设置传感信号-------------------
	MotorInputParam inputParam;
	inputParam.levelOrg=LevelHigh;
	inputParam.levelLeftLimit=LevelLow;
	inputParam.levelRightLimit=LevelLow;

	m_pXMotor->SetInputParam(inputParam);
	m_pXMotor->InitMotorParam();

	m_pZMotor->SetInputParam(inputParam);
	m_pZMotor->InitMotorParam();

	m_pMagBarMotor->SetInputParam(inputParam);
	m_pMagBarMotor->InitMotorParam();

	//-----------------------------X 轴 ：X 轴用的孔2位置调整-------------------------
	MotorActParam param;
	param.type=RELATIVE_POS;
	param.speedStar= m_pXMotor->m_iSpeedStart[0]; // 回零速度
	param.speedEnd = m_pXMotor->m_iSpeed[0];
	param.step =m_pXMotor->m_Offset[0] ; // 复位
	param.ACCDec=ZERO_ACC;
	m_pXMotor->SetMotorActParam(ACT_InitHome,param);

	param.type=RELATIVE_POS;
	param.speedStar= m_pXMotor->m_iSpeedStart[8];
	param.speedEnd= m_pXMotor->m_iSpeed[8];
	param.step=200;
	param.ACCDec=10000;
	m_pXMotor->SetMotorActParam(ACT_InitLeaveHome,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[1];
	param.speedEnd= m_pXMotor->m_iSpeed[1];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[0]  ;// 快速回零
	m_pXMotor->SetMotorActParam(ACT_TYPE1,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[2];//使用水平移动速度
	param.speedEnd= m_pXMotor->m_iSpeed[2];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[1] ;// 孔1中心距离孔2中心位置  ,KIT_TYPE_ONE   KIT_TYPE_THREE
	m_pXMotor->SetMotorActParam(ACT_TYPE2,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[3];
	param.speedEnd= m_pXMotor->m_iSpeed[3];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[2]  ;// 孔1间距  // 混匀速度 根据参数来
	m_pXMotor->SetMotorActParam(ACT_TYPE3,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[2];
	param.speedEnd= m_pXMotor->m_iSpeed[2];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[3]  ;// 孔2位置 ，其他相邻孔根据孔2位置计算
	m_pXMotor->SetMotorActParam(ACT_TYPE4,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[2];
	param.speedEnd= m_pXMotor->m_iSpeed[2];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[4]  ;// 圆孔间距  // 注意不能不得随意改
	m_pXMotor->SetMotorActParam(ACT_TYPE5,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[2];
	param.speedEnd= m_pXMotor->m_iSpeed[2];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[5]  ;//最后孔间距间距
	m_pXMotor->SetMotorActParam(ACT_TYPE6,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[2]; //最后孔位位置，位置计算得来，
	param.speedEnd= m_pXMotor->m_iSpeed[2];
	param.ACCDec=120000;
	param.step = m_pXMotor->m_Offset[6]  ; //位置未用
	m_pXMotor->SetMotorActParam(ACT_TYPE7,param);

	param.speedStar= m_pXMotor->m_iSpeedStart[4]; //左右移动吸磁速度
	param.speedEnd= m_pXMotor->m_iSpeed[4];

	//-----------------用到m_iSpeed[4]-----------
	//不同类型的孔中心
	param.step = m_pXMotor->m_Offset[7]  ;// 孔1中心距离孔2中心位置 KIT_TYPE_TWO
	param.step = m_pXMotor->m_Offset[8]  ;// 孔1间距(短的)KIT_TYPE_TWO

	//---------------------------Z轴 磁棒套--------------------------------------------------------

	m_pZMotor->SetMotorAccType(ACC_TIME);
	param.speedStar= m_pZMotor->m_iSpeedStart[0]; // 回零速度
	param.speedEnd = m_pZMotor->m_iSpeed[0];
	param.step =m_pZMotor->m_Offset[0] ; // 复位
	param.ACCDec=ZERO_ACC;
	param.ACCDec=300;
	m_pZMotor->SetMotorActParam(ACT_InitHome,param);

	param.type=RELATIVE_POS;
	param.speedStar= m_pZMotor->m_iSpeedStart[8];
	param.speedEnd= m_pZMotor->m_iSpeed[8];
	param.step=200;
	param.ACCDec=ZERO_ACC;
	param.ACCDec=300;
	m_pZMotor->SetMotorActParam(ACT_InitLeaveHome,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[1];
	param.speedEnd= m_pZMotor->m_iSpeed[1];
	param.ACCDec=100000;
	param.ACCDec=300;
	param.step = m_pZMotor->m_Offset[0]  ;// 快速回零
	m_pZMotor->SetMotorActParam(ACT_TYPE1,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[2];
	param.speedEnd= m_pZMotor->m_iSpeed[2];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pZMotor->m_Offset[1]  ;// 下降到底，孔1中 下降速度
	m_pZMotor->SetMotorActParam(ACT_TYPE2,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[3];
	param.speedEnd= m_pZMotor->m_iSpeed[3];
	param.ACCDec=120000;
	param.ACCDec=30;
	param.step = m_pZMotor->m_Offset[2]  ;// 快速振荡 脉冲没有使用，根据高度计算
	m_pZMotor->SetMotorActParam(ACT_TYPE3,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[4];
	param.speedEnd= m_pZMotor->m_iSpeed[4];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pZMotor->m_Offset[3]  ;// 缓慢分段下降脉冲，，根据高度计算
	m_pZMotor->SetMotorActParam(ACT_TYPE4,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[5];
	param.speedEnd= m_pZMotor->m_iSpeed[5];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pZMotor->m_Offset[4]  ;// 下降到试剂液面高度，，根据高度计算
	m_pZMotor->SetMotorActParam(ACT_TYPE5,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[6];
	param.speedEnd= m_pZMotor->m_iSpeed[6];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pZMotor->m_Offset[5]  ;// 下降到试剂液面高度，慢速
	m_pZMotor->SetMotorActParam(ACT_TYPE6,param);

	param.speedStar= m_pZMotor->m_iSpeedStart[7];
	param.speedEnd= m_pZMotor->m_iSpeed[7];
	param.ACCDec= 120000;
	param.ACCDec=300;
	param.step = m_pZMotor->m_Offset[6]  ;// 位置未用
	m_pZMotor->SetMotorActParam(ACT_TYPE7,param);

	//
//	param.step = m_pZMotor->m_Offset[7]  ;// 孔2底部

	//------------------------------磁棒 轴--------------------------------------
	m_pMagBarMotor->SetMotorAccType(ACC_TIME);
	param.speedStar= m_pMagBarMotor->m_iSpeedStart[0]; // 回零速度
	param.speedEnd = m_pMagBarMotor->m_iSpeed[0];
	param.step =m_pMagBarMotor->m_Offset[0] ; // 复位
	param.ACCDec=ZERO_ACC;
	param.ACCDec=300;
	m_pMagBarMotor->SetMotorActParam(ACT_InitHome,param);

	param.type=RELATIVE_POS;
	param.speedStar= m_pMagBarMotor->m_iSpeedStart[8];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[8];
	param.step=200;
	param.ACCDec=ZERO_ACC;
	param.ACCDec=300;
	m_pMagBarMotor->SetMotorActParam(ACT_InitLeaveHome,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[1];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[1];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pMagBarMotor->m_Offset[0]  ;// 快速回零
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE1,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[2];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[2];
	param.ACCDec=120000;

	//
	param.ACCDec=300;
	m_pMagBarMotor->m_Offset[1]= m_pMagBarMotor->m_Offset[3] +m_pZMotor->m_Offset[1] *m_pMagBarMotor->m_PerMMToPluse/ZMOTOR_MM_TO_PULSE; //(MMOTOR_MM_TO_PULSE/ZMOTOR_MM_TO_PULSE)转换关系
	param.step = m_pMagBarMotor->m_Offset[1] ;// 下降到底
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE2,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[3];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[3];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pMagBarMotor->m_Offset[2]  ;// 缓慢分段下降脉冲
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE3,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[4];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[4];
	param.ACCDec=120000;
	param.ACCDec=300;
	param.step = m_pMagBarMotor->m_Offset[3]  ;// 下降到空闲高度
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE4,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[5];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[5];
	param.ACCDec= 120000;
	param.ACCDec=300;
	param.step = m_pMagBarMotor->m_Offset[4]  ;//  运动到液面高度，根据体积计算
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE5,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[6];
	param.speedEnd= m_pMagBarMotor->m_iSpeed[6];
	param.ACCDec= 120000;
	param.ACCDec=300;
	param.step = m_pMagBarMotor->m_Offset[5]  ;// 运动到液面高度，慢速
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE6,param);

	param.speedStar= m_pMagBarMotor->m_iSpeedStart[7];// 圆孔中缓慢下降吸磁
	param.speedEnd= m_pMagBarMotor->m_iSpeed[7];
	param.ACCDec= 120000;
	param.ACCDec=300;
	param.step = m_pMagBarMotor->m_Offset[6]  ;// 圆孔中缓慢下降吸磁  ////
	m_pMagBarMotor->SetMotorActParam(ACT_TYPE7,param);

	 m_pMagBarMotor->m_SuckmagneticLeftStep  =m_SuckmagneticLeftStep;
	 m_pZMotor->m_SuckmagneticLeftStep=m_SuckmagneticLeftStep;
}
BOOL CPart::DownPartParam()
{
	InitMotorParam();
	m_pXMotor->SetMotorDefaultParam();
	m_pZMotor->SetMotorDefaultParam();
	m_pMagBarMotor->SetMotorDefaultParam();
	return TRUE;
}

int CPart::InitPart(BOOL bCheckStop)
{
	TRACE(_T("CPart:: InitPart"));
	EmptyError();

	m_pXMotor->InitMove();
	m_pZMotor->InitMove();
	m_pMagBarMotor->InitMove();
	m_PartLastError[2] =m_pMagBarMotor->InitHome();
	if (bCheckStop)
	{
		PART_ERROR_QUIT(2)
	}
	else
	{
		PART_ERROR_QUIT_1(2)
	}

	m_PartLastError[1] =m_pZMotor->InitHome();
	if (bCheckStop)
	{
		PART_ERROR_QUIT(1)
	}
	else
	{
		PART_ERROR_QUIT_1(1)
	}
	m_PartLastError[0] =m_pXMotor->InitHome();
	if (bCheckStop)
	{
		PART_ERROR_QUIT(0)
	}
	else
	{
		PART_ERROR_QUIT_1(0)
	}
	if(m_ThreadExit)
	{
		m_ThreadExit= FALSE;
		CreateUVLampTread();
	}
	m_UVLampEvt.ResetEvent(); // 紫外灯
	m_PauseEvent.SetEvent(); // 暂停一直有信号
	m_bAlreadySuckMagnetic = FALSE;
	m_SuckMagneticWay =1;
	return m_LastError;
}
#define IF_PAUSEEVENT_STOP 	if(m_ReturnValue<0) { return 0;}
//流程处理，
int	CPart:: WholeAction(ProgramStep  Info)
{
	// 区分试剂盒类型----区分位置 -区分动作。是 先混匀  在吸磁，然后等待
	BYTE Type = m_pMainTask->m_ProgramQueue.m_Type; //试剂盒类型
	DWORD now=0,last=0;
	m_SuckMagneticWay = m_pMainTask->m_ProgramQueue.m_SuckMagneticWay; //吸磁方式
	m_NumberOfSegments = m_pMainTask->m_ProgramQueue.m_NumberOfSegments ; //分段
	if (Info.SuckMagLevel==1 ) // Z 轴吸磁速度100， SuckMagLevel=1 时 ，走不准， 128细分可以
	{
        Info.SuckMagLevel =2;
	}
	TRACE(_T("CPart:: WholeAction Kit_Type=%d, Hole=%d,MixTime =%d,MagneticTime=%d,WaitTime=%d,SpeedLevel=%d,Volume=%d,SuckMagneticWay=%d,m_NumberOfSegments=%d,SuckMagLevel=%d"),Type,Info.HolePos,Info.MixTime,Info.MagneticTime,Info.WaitTime,Info.SpeedLevel,Info.Volume,m_SuckMagneticWay,m_NumberOfSegments,Info.SuckMagLevel);
	SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,0,0);
	DWORD now1=0,last1=0;
	DWORD Span1=0 ;
	////////////////////////////////////////////////////////混匀/////////////////////////////////////////////////////////////////////////
	if(Info.MixTime!=0) // 1 混匀流程
	{
		last1 =timeGetTime();
		m_LastError= MoveToHolePos(Type,Info.HolePos) ;  //横移到位
		IF_ERROR_QUIT
	    m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos(); //磁棒运动到空闲位置
		PART_ERROR_QUIT(2)
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		if (m_bAlreadySuckMagnetic) //吸磁之后进入下一步，要先提起磁棒，释放掉磁珠
		{
			m_LastError= MoveDown(Info.HolePos) ; //一起下降，
			IF_ERROR_QUIT
			m_ReturnValue = WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP
			wxMilliSleep(40);
			if(Info.HolePos ==1)
			{
				m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos();  //磁棒运动到空闲位置
				PART_ERROR_QUIT(2)
			}
			else
			{
				m_PartLastError[2] = m_pMagBarMotor->GoToHome();  //上升到Home位置
				PART_ERROR_QUIT(2)
			}
			m_ReturnValue =WaitPauseEventOrStop();
		    IF_PAUSEEVENT_STOP
			m_bAlreadySuckMagnetic = FALSE;
		}
		else
		{
			if(Info.HolePos!=1)
			{
				wxMilliSleep(40);
				m_PartLastError[2] =m_pMagBarMotor->GoToHome();  //上升到Home位置
				PART_ERROR_QUIT(2)
			}
			int MoveStep=0;
			m_PartLastError[1] =m_pZMotor->MoveDown(MoveStep,Info.HolePos); //下降到底
			PART_ERROR_QUIT(1)
		}
		m_ReturnValue = WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		now1 = timeGetTime();
		Span1 =now1 -last1;
		TRACE(_T("CPart MixTime::Span1=%d"),Span1);

		SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MIX,Info.MixTime);//通知混匀开始
		last = timeGetTime();
		DWORD SpanTime =0 ;

		if (Info.HolePos ==1)
		{
			BOOL bMoveHalf =TRUE;
			DWORD  MoveHalfHoleOneTime =m_pXMotor->GetMoveHalfHoleOneDistanceTime(Type,Info.SpeedLevel,TRUE);//计算时间
			m_ReturnValue =WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP
			while((SpanTime+MoveHalfHoleOneTime)<Info.MixTime*1000 ) //最后要移动到中心
			{
				m_LastError = MoveHoleOneMix(Type,Info.SpeedLevel,bMoveHalf,last); //左右移动混匀
				IF_ERROR_QUIT
				bMoveHalf =FALSE;
				m_ReturnValue =WaitPauseEventOrStop();
				IF_PAUSEEVENT_STOP
				if (m_ReturnValue>0)
				{
                     last+=m_ReturnValue;//暂停时间
				}
				now =timeGetTime();
				SpanTime =now-last;
#ifdef  WIN32
				TRACE(_T("CPart:: WholeAction StepId =%d,MixTime =%d ,SpanTime =%d"),Type,Info.StepId,Info.MixTime,SpanTime);
#endif
			}
			last1 =timeGetTime();
			m_PartLastError[0] = m_pXMotor->MoveHalfHoleOneDistance(Type,FALSE,TRUE,Info.SpeedLevel,WAITTIME*2); //向右移动一半
			PART_ERROR_QUIT(0)

			now1 = timeGetTime();
			Span1 =now1 -last1;
			m_ReturnValue =WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP
		}
		else  //其他孔中混匀，要计算混匀幅度
		{
			if (Info.MixPerCent<=0)
			{
				Info.MixPerCent =100;
			}
			int  Volume =  Info.Volume *Info.MixPerCent/100; //混合幅度

			m_PartLastError[1] =m_pZMotor->MoveDownFast(FALSE,Volume,0,1,Info.SpeedLevel);  //提前保存高度 和速度
			PART_ERROR_QUIT(1)

			while(SpanTime< Info.MixTime*1000)
			{
				m_PartLastError[1] =m_pZMotor->MoveDownFast(FALSE,Volume,WAITTIME,2,Info.SpeedLevel);  //根据体积算高度 速度
				PART_ERROR_QUIT(1)
				m_ReturnValue =WaitPauseEventOrStop();
				IF_PAUSEEVENT_STOP

				if (m_ReturnValue>0)
				{
					last+=m_ReturnValue; //混匀过程中，如果暂停，暂停时间不计入混匀时间
				}
				m_PartLastError[1] =m_pZMotor->MoveDownFast(TRUE,Volume,WAITTIME,2,Info.SpeedLevel);
				PART_ERROR_QUIT(1)

				m_ReturnValue =WaitPauseEventOrStop();
				IF_PAUSEEVENT_STOP
				if (m_ReturnValue>0)
				{
					last+=m_ReturnValue;
				}
				now =timeGetTime();
				SpanTime =now-last;
			}
			m_ReturnValue =WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP
		}

		m_LastError= PartHome();// Z轴和磁棒轴复位
		IF_ERROR_QUIT
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		wxMilliSleep(20);
		SndActionStarOrEndMsg(Info.StepId,ACT_STATUS_END,ACT_MIX,Info.MixTime);	//通知混匀结束
	}
	////////////////////////////////////////////////////////混匀结束/////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////吸磁开始/////////////////////////////////////////////////////////////////////////
	if (Info.MagneticTime!=0) //2 吸磁
	{
		last1 =timeGetTime();
		m_LastError= MoveToHolePos(Type,Info.HolePos) ; //水平移动到孔位
		IF_ERROR_QUIT
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos();
		PART_ERROR_QUIT(2)
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		m_LastError = MoveToLiquidTop(Type,Info.HolePos,Info.Volume,TRUE,TRUE); //下降到液位
		IF_ERROR_QUIT
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		now1 = timeGetTime();
		Span1 =now1 -last1;
		TRACE(_T("CPart MagneticTime::Span1=%d"),Span1);
		BOOL bRight =TRUE;
	 	DWORD  MoveHalfHoleOneTime =m_pXMotor->GetMoveHalfHoleOneDistanceTime(Type,Info.SuckMagLevel,FALSE);
		// -----------------------------------------------孔1 中吸磁-------------------------------------------
		if (Info.HolePos ==1)
		{
			last = timeGetTime();
			DWORD SpanTime =0 ;
			BOOL bMoveHalf =TRUE;
			float Height =GetMoveHeight(Type,Info.HolePos,Info.Volume);
			int MaxTimes = Height/m_StepHeight;  // 计算次数，高度/每次下降到高度
			int  iTimes =0;
			if (m_SuckMagneticWay == WANG_FU) //往复吸磁 次数+时间控制,先下降后上升
			{
				SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MAGNETIC,Info.MagneticTime);
				while (iTimes <= MaxTimes && (SpanTime+MoveHalfHoleOneTime) < Info.MagneticTime*1000)
				{
					m_LastError = MoveHoleOneSuckMagnetic(Type,Info.SuckMagLevel,bMoveHalf,bRight); //X 轴移动吸磁
					IF_ERROR_QUIT
                    bRight=!bRight;
					bMoveHalf = FALSE;
					m_ReturnValue =WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					m_LastError =MoveDownHeight(m_StepHeight); //孔1 下降 吸磁
					IF_ERROR_QUIT
					m_ReturnValue =WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					iTimes++;
					now =timeGetTime();
					SpanTime =now-last ;
				}
				iTimes =0;
				while (iTimes<MaxTimes && (SpanTime+MoveHalfHoleOneTime) < Info.MagneticTime*1000)
				{
					m_LastError =MoveUpSlow(m_StepHeight,Info.SuckMagLevel);
					IF_ERROR_QUIT
					m_ReturnValue =WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					m_LastError = MoveHoleOneSuckMagnetic(Type,Info.SuckMagLevel,FALSE,bRight); //X 轴移动吸磁
					IF_ERROR_QUIT
				    bRight=!bRight;
					m_ReturnValue =WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					iTimes++;
					now =timeGetTime();
					SpanTime =now-last ;
				}
			}
			else if (m_SuckMagneticWay==FENG_DUAN) //分段吸磁
			{
				SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MAGNETIC,Info.MagneticTime);
				if(m_NumberOfSegments>0) // 分段 2-9
				{
					// 分段吸磁主要是孔里运动时间
					DWORD MoveHeightTime= m_pZMotor->MoveDownStepTime(Height);
					int Time =  MoveHalfHoleOneTime * (m_NumberOfSegments*2 + 1)+MoveHeightTime*2;//左右移动+上下 TODO 分段吸磁
					int iSleepTime =0;
					if (Time<Info.MagneticTime*1000) //不停顿
					{
						iSleepTime=(Info.MagneticTime*1000-Time)/m_NumberOfSegments ;
					}
					float TempHeight = Height/m_NumberOfSegments;
					for (int i=0; i< m_NumberOfSegments;i++)
					{
						m_LastError = MoveHoleOneSuckMagnetic(Type,Info.SuckMagLevel,bMoveHalf,bRight); //X 轴移动吸磁
						IF_ERROR_QUIT
						bRight =!bRight;
						bMoveHalf = FALSE;
						if(iSleepTime>0)
						{
							wxMilliSleep(iSleepTime);  // TODO 分段吸磁  这个不合理
						}
						m_LastError =MoveDownHeight(TempHeight); // 孔1分段下降高度吸磁
						IF_ERROR_QUIT
						now =timeGetTime();
						SpanTime =now-last ;
						if (SpanTime >= Info.MagneticTime*1000) // 吸磁时间到
						{
							break;
						}
					}
				}
				else
				{
					//直接下降到底
				}
			}
			else
			{
				while (iTimes<=MaxTimes)//1号孔 悬停吸磁。
				{
					m_LastError = MoveHoleOneSuckMagnetic(Type,Info.SuckMagLevel,bMoveHalf,bRight); //X 轴移动吸磁 吸磁速度
					IF_ERROR_QUIT
					bMoveHalf = FALSE;
					bRight =!bRight;
					m_ReturnValue = WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					m_LastError =MoveDownHeight(m_StepHeight); //孔1下降 --下降到底部不能继续下降,内部判断。
					IF_ERROR_QUIT
					m_ReturnValue = WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					iTimes++;
				}
				m_ReturnValue = WaitPauseEventOrStop();
				IF_PAUSEEVENT_STOP;
				last =timeGetTime();
				SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MAGNETIC,Info.MagneticTime); //通知吸磁开始.吸磁，下降到底部计算时间

				while((SpanTime+MoveHalfHoleOneTime)< Info.MagneticTime*1000)
				{
					m_LastError = MoveHoleOneSuckMagnetic(Type,Info.SuckMagLevel,FALSE,bRight);//X 轴移动吸磁 //
					IF_ERROR_QUIT
					bRight =!bRight;
					m_ReturnValue = WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					now =timeGetTime();
					SpanTime =now-last ;
				}
			}
			m_LastError = MoveHalfHoleOneSuckMagnetic(Type,Info.SuckMagLevel,bRight);//X 轴移动到中间 //
			IF_ERROR_QUIT
			m_ReturnValue =WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP
			m_LastError = MoveToLiquidTop(Type,Info.HolePos,Info.Volume,FALSE,FALSE);	//缓慢上升到液面
			IF_ERROR_QUIT
			m_ReturnValue =WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP
		}
		else//----------------------------------------------其他孔中吸磁-------------------------------------------
		{
			last = timeGetTime();
			DWORD SpanTime =0 ;
			float Height =GetMoveHeight(Type,Info.HolePos,Info.Volume);
			if (m_SuckMagneticWay==WANG_FU) //往复吸磁
			{
				SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MAGNETIC,Info.MagneticTime);
				while(SpanTime< Info.MagneticTime*1000)//1.反复的上升 下降
				{
					m_LastError = MoveDownSlow(FALSE,0,Info.SuckMagLevel); // 缓慢下降吸磁
					IF_ERROR_QUIT
					m_ReturnValue = WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					m_LastError =MoveUpSlow(Height,Info.SuckMagLevel);// 上升到液面
					IF_ERROR_QUIT
					m_ReturnValue = WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP;
					now =timeGetTime();
					SpanTime =now-last ;
				}
			}
			else if (m_SuckMagneticWay==FENG_DUAN) //分段吸磁
			{
				SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MAGNETIC,Info.MagneticTime);
				//1 一段段下降，然后是中间要进行停顿。 ，如果是时间小于当前的下降时间，中间不停顿 ，计算往复时间，  高度/时间
				int Time = (Height*(ZMOTOR_MM_TO_PULSE)/m_pZMotor->m_iSpeed[7])*1000*2;  //2 因为上升和下降
				int iSleepTime =0;
				if (Time<Info.MagneticTime*1000) //不停顿
				{
					iSleepTime=(Info.MagneticTime*1000-Time)/(m_NumberOfSegments*2)  ; //2 因为上升和下降
				}
				if (m_NumberOfSegments>0) //分段吸磁
				{
					float SegmentsHeight =Height/m_NumberOfSegments;
					for (int i=0; i< m_NumberOfSegments;i++)
					{
						m_LastError = MoveDownSlow(TRUE,SegmentsHeight,Info.SuckMagLevel);// 分段缓慢下降吸磁	分段下降
						IF_ERROR_QUIT
						m_ReturnValue =WaitPauseEventOrStop();
						IF_PAUSEEVENT_STOP
						if(iSleepTime>0)
						{
							wxMilliSleep(iSleepTime);
						}
					}
					for (int i=0; i< m_NumberOfSegments;i++)	// 上升到液面 ，分段上升
					{
						m_LastError =MoveUpSlow(SegmentsHeight,Info.SuckMagLevel);//缓慢上升吸磁
						IF_ERROR_QUIT
						m_ReturnValue =WaitPauseEventOrStop();
						IF_PAUSEEVENT_STOP
						if(iSleepTime>0)
						{
							wxMilliSleep(iSleepTime);
						}
					}
				}
			}
			else //悬停吸磁。
			{
				m_LastError = MoveDownSlow(FALSE,0,Info.SuckMagLevel);
				IF_ERROR_QUIT
				m_ReturnValue =WaitPauseEventOrStop();
				IF_PAUSEEVENT_STOP
				SndActionStarOrEndMsg(Info.StepId, ACT_STATUS_START,ACT_MAGNETIC,Info.MagneticTime);
			    last =timeGetTime();
				while(SpanTime< Info.MagneticTime*1000) // 缓慢下降，然后到底部进行吸磁 ，停顿吸磁时间
				{
					wxMilliSleep(200);	//等待吸磁时间结束
					m_ReturnValue =WaitPauseEventOrStop();
					IF_PAUSEEVENT_STOP
					now =timeGetTime();
					SpanTime =now-last ;
				}
				m_LastError =MoveUpSlow(Height,Info.SuckMagLevel);// 缓慢上升到液面 ,上升比下降多50脉冲
				IF_ERROR_QUIT
			}
		}
		m_bAlreadySuckMagnetic=TRUE;
		SndActionStarOrEndMsg(Info.StepId,ACT_STATUS_END,ACT_MAGNETIC,Info.MagneticTime); 	//通知吸磁结束
		m_LastError= PartHome();
		IF_ERROR_QUIT
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
		wxMilliSleep(50);
	}
	////////////////////////////////////////////////////////吸磁结束/////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////等待/////////////////////////////////////////////////////////////////////////////
	if (Info.WaitTime!=0) // 3 等待 不分试剂盒类型，直接等待
	{
		SndActionStarOrEndMsg(Info.StepId,ACT_STATUS_START,ACT_WAIT,Info.WaitTime);	//通知等待开始
		last = timeGetTime();
		while(TRUE)
		{
			wxMilliSleep(100);
			now =timeGetTime();
			DWORD SpanTM=now-last;
			if (SpanTM >= Info.WaitTime*1000)
			{
				break;
			}
			if (g_ST->m_bErrorStop || (g_MachineState!=MACHINE_TEST && g_MachineState!=MACHINE_PAUSE)) //
			{
				SndActionStarOrEndMsg(Info.StepId,ACT_STATUS_END,ACT_WAIT,Info.WaitTime);		//通知等待结束
				return 0;//
			}
		}
		SndActionStarOrEndMsg(Info.StepId,ACT_STATUS_END,ACT_WAIT,Info.WaitTime);	//通知等待结束
	}

	m_ReturnValue =WaitPauseEventOrStop();
	IF_PAUSEEVENT_STOP
	SndActionStarOrEndMsg(Info.StepId,ACT_STATUS_END,0,0);
	return 0;
}

void CPart::SetPauseEvent()
{
	m_PauseEvent.SetEvent();
}
void CPart:: ResetPauseEvent()
{
	m_PauseEvent.ResetEvent();
}
int CPart::WaitPauseEventOrStop()
{
	DWORD  PauseStart = timeGetTime();
	while(TRUE)
	{
		if(wxSEMA_NO_ERROR!=m_PauseEvent.WaitTimeout(1000))	//没有信号停止
		{
			continue;
		}
		else
		{
			break;
		}
		if (g_ST->m_bErrorStop || g_MachineState==MACHINE_STOP )
		{
			return -1;//
		}
	}
	if (g_ST->m_bErrorStop || g_MachineState==MACHINE_STOP )// 如果停止收到命令
	{
		return -1;//
	}
	DWORD PauseSpanTime = timeGetTime()-PauseStart; //间隔时间
	if(PauseSpanTime<1000) PauseSpanTime =0;

	return PauseSpanTime;
}
// 获取试剂盒最后的孔位置
int  CPart:: GetLastHolePos(BYTE Type)
{
	int Pos =LAST_HOLE_POS_TYPE_2_3; //类型2和3 的最后孔
	if (Type== KIT_TYPE_ONE)
	{
		Pos = LAST_HOLE_POS_TYPE_1;
	}
	else if(Type== KIT_TYPE_TWO || Type== KIT_TYPE_THREE)
	{
		Pos = LAST_HOLE_POS_TYPE_2_3;
	}
	return Pos;
}
// 打开日光灯
void CPart:: OpenOrCloseLamp(BYTE Open)
{
	if (m_LampStatus!=Open)
	{
		m_pXMotor->WriteOutput(1,Open);
		m_LampStatus =Open;
	}
}
//打开紫外灯
void  CPart::OpenOrCloseUVLamp(BYTE Open,int Time)
{
	if (Open!=m_UVLampStatus)
	{
		m_UVLampStatus =Open;
		m_UVTime =Time*60*1000; // 传出过的是分钟
		m_pXMotor->WriteOutput(0,Open);
		if (m_UVLampStatus==1)
		{
			m_UVLampEvt.SetEvent(); // 时间完成，自动关闭
		}
	}
	if (Open==0) //关灯直接回复
	{
		CSndOpenCloseUVLamp *pMsg1 =new CSndOpenCloseUVLamp;
		g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);	//回复指令
	}
}
void CPart:: GetLampAndCoverStaues(BYTE &Lamp,BYTE &UVLamp,BYTE &FrontCover,BYTE &Drawer)
{
	Lamp =m_LampStatus ;
	UVLamp =m_UVLampStatus ;
	BOOL BVaild=FALSE;
	if (m_CheckFrontCoverMark ==1)
	{
		m_pZMotor->GetMotorLiquid(BVaild);
		FrontCover=(BVaild)?0:1;
	}
	else
	{
        FrontCover =0 ;// 0 是舱门关闭
	}
	wxMilliSleep(20);
	m_pZMotor->GetMotorRightLimit(BVaild);
	Drawer=(BVaild)?0:1;

#ifdef _SIMULATOR_ 
   FrontCover=0;
   Drawer=0;
#endif


	m_FrontCoverStatus =FrontCover; //更新状态标记
	m_DrawerStatus =Drawer;
}

// 发送灯和舱门的状态
void  CPart:: SendLampAndCoverStaues()
{
	DWORD now=timeGetTime();
	DWORD SpanTM=now-m_StateQueryTime;
	if(SpanTM >1000) // 间隔太短，读取频繁容易出问题
	{
		BYTE FrontCoverStatus;
		BYTE DrawerStatus ;
		BOOL BVaild=FALSE;
		if(m_CheckFrontCoverMark==1)
		{
			m_pZMotor->GetMotorLiquid(BVaild);
			FrontCoverStatus=(BVaild)?0:1;
		}
        else
		{
	        FrontCoverStatus =0;
		}
		wxMilliSleep(20);
		m_pZMotor->GetMotorRightLimit(BVaild);
		DrawerStatus=(BVaild)?0:1;
		m_StateQueryTime=timeGetTime();
		if (FrontCoverStatus!=m_FrontCoverStatus || DrawerStatus!=m_DrawerStatus )
		{
			m_FrontCoverStatus =FrontCoverStatus;
			m_DrawerStatus =DrawerStatus;
			CSndQueryLampAndCoverStatus *pMsg =new CSndQueryLampAndCoverStatus ;
			pMsg->m_Lamp =m_LampStatus ;
			pMsg->m_UVLamp =m_UVLampStatus;
			pMsg->m_FrontCover =m_FrontCoverStatus;
			pMsg->m_Drawer=  m_DrawerStatus;
			g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg); //回复设置成功;
		}
	}
}
//电机调试
int	CPart::DebugPartMotor(BYTE index,BYTE act,int param)
{
	switch(index)
	{
	case 0:
		m_pXMotor->DebugMotor(act,param);
		break;
	case 1:
		m_pZMotor->DebugMotor(act,param);
		break;
	case 2:
		m_pMagBarMotor->DebugMotor(act,param);
		break;
	default:
		FASSERT(FALSE);
		break;
	}
	return 0;
}

//创建紫外灯线程
BOOL CPart::CreateUVLampTread()
{
	new CUVLampThread(this);
	return TRUE;
}

//
UINT CPart::UVLamp()
{
	while(1)
	{
		if(wxSEMA_NO_ERROR!=m_UVLampEvt.WaitTimeout(WAIT_FOREVER))//等待开始事件
			continue;
		DWORD SpanTime =0;
		m_UVLampEvt.ResetEvent();
		while(m_UVLampStatus && m_UVTime>=SpanTime )
		{
			wxMilliSleep(500);
			SpanTime+=1000;
		}
		m_pXMotor->WriteOutput(0,0);//时间到关灯
		m_UVTime =0;
		m_UVLampStatus =0;
		CSndOpenCloseUVLamp *pMsg =new CSndOpenCloseUVLamp;
		g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg);	//回复指令
	}
	m_ThreadExit= TRUE;
	return 1;
}

// 修改 同步上升，磁棒上升到空闲位置。
int CPart::PartHome()
{
	DWORD Time = timeGetTime();
	TRACE(_T("CPart::PartHome"));
	m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos(0);
	PART_ERROR_QUIT(2)
	m_PartLastError[1] =m_pZMotor->GoToHome1(); // 为同步修改，直接回零
	PART_ERROR_QUIT(1)
	m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
	PART_ERROR_QUIT(2)
    Time =timeGetTime()-Time;
	TRACE(_T("CPart::PartHome End SpanTime =%d"),Time);
	return m_LastError;
}

//孔1中混匀，X轴左右移动， Z轴上下移动。
int CPart:: MoveHoleOneMix(BYTE Type, int SpeedLevel, BOOL bHalfMove,DWORD &LastTime)
{
	TRACE(_T("CPart::MoveHoleOneMix Type=%d,SpeedLevel=%d,bHalfMove=%d"),Type,SpeedLevel,bHalfMove);
	if (bHalfMove)
	{
		m_PartLastError[0] =m_pXMotor->MoveHalfHoleOneDistance(Type,TRUE,TRUE,SpeedLevel,WAITTIME);
		PART_ERROR_QUIT(0)
		bHalfMove =FALSE;
	}
	m_ReturnValue =WaitPauseEventOrStop();
	IF_PAUSEEVENT_STOP
	if (m_ReturnValue>0)
	{
		LastTime+=m_ReturnValue;
	}
	DWORD  MoveHoleOneTime =m_pXMotor->GetMoveHalfHoleOneDistanceTime(Type,SpeedLevel,TRUE)*2;  //水平移动时间
	float Height =m_MixHeight; // 上下振幅
	int   UpDownSpeedLevel=10;
	int   iMixUpDownTimes =0,iCompensateValue=0;

	if (Type==KIT_TYPE_TWO)
	{
		iMixUpDownTimes = m_MixUpDownTimes2;
		iCompensateValue =m_CompensateValue2;
	}
	else
	{
	   iMixUpDownTimes = m_MixUpDownTimes1;
       iCompensateValue =m_CompensateValue1;
	}
	//算速度--水平移动一次，上下m_MixUpDownTimes  1000是毫秒转秒 , 2 是往返， ZMOTOR_MM_TO_PULSE 100 是毫米转脉冲，STEP_1MM ，10 是Level 转换。
	int TempLevel = iCompensateValue*(Height*2*ZMOTOR_MM_TO_PULSE)*(iMixUpDownTimes*1000)*10/(MoveHoleOneTime*m_pZMotor->m_iSpeed[3]);

	m_PartLastError[1] =m_pZMotor->MoveUpDownFast(TRUE,Height,0,1,TempLevel);  //
	PART_ERROR_QUIT(1)
	m_PartLastError[0] =m_pXMotor->MoveHoleOneDistance(Type,TRUE,TRUE,SpeedLevel,0); //水平运动
	PART_ERROR_QUIT(0)
	if (m_MixUpDownMark == 1)
	{
		for(int i=0;i<iMixUpDownTimes;i++)
		{
			m_PartLastError[1] =m_pZMotor->MoveUpDownFast(FALSE,Height,WAITTIME,2,TempLevel);  //
			PART_ERROR_QUIT(1)
			m_PartLastError[1] =m_pZMotor->MoveUpDownFast(TRUE,Height,WAITTIME,2,TempLevel);  //
			PART_ERROR_QUIT(1)
		}
	}
	m_PartLastError[0] =m_pXMotor->WaitStepFinish(WAITTIME);
	PART_ERROR_QUIT(0)
	m_ReturnValue =WaitPauseEventOrStop();
	IF_PAUSEEVENT_STOP
	if (m_ReturnValue>0)
	{
		LastTime+=m_ReturnValue;
	}
	m_PartLastError[0] =m_pXMotor->MoveHoleOneDistance(Type,FALSE,TRUE,SpeedLevel,0);
	PART_ERROR_QUIT(0)

	if (m_MixUpDownMark==1)
	{
		for(int i=0;i<iMixUpDownTimes;i++)
		{
			m_PartLastError[1] =m_pZMotor->MoveUpDownFast(FALSE,Height,WAITTIME,2,TempLevel);  //
			PART_ERROR_QUIT(1)
			m_PartLastError[1] =m_pZMotor->MoveUpDownFast(TRUE,Height,WAITTIME,2,TempLevel);  //
			PART_ERROR_QUIT(1)
		}
	}

	m_PartLastError[0] =m_pXMotor->WaitStepFinish(WAITTIME);
	PART_ERROR_QUIT(0)
	m_ReturnValue =WaitPauseEventOrStop();
	IF_PAUSEEVENT_STOP
	if (m_ReturnValue>0)
	{
		LastTime+=m_ReturnValue;
	}
	return m_LastError;
}

/*
   孔1中吸磁
   Type 试剂盒类型  SpeedLevel 速度   bHalfMove 标记是否移动一半   bRight 标记左右移动

*/
int CPart:: MoveHoleOneSuckMagnetic(BYTE Type, int SpeedLevel, BOOL bHalfMove,BOOL bRight)
{
	TRACE(_T("CPart::MoveHoleOneSuckMagnetic Type=%d,SpeedLevel=%d,bHalfMove=%d,bRight=%d"),Type,SpeedLevel,bHalfMove,bRight);
	m_PartLastError[0] =m_pXMotor->MoveToHoleOneBySegment(Type,TRUE,FALSE,SpeedLevel,m_XSegment,WAITTIME,1); //先设置参数
	PART_ERROR_QUIT(0)
	if (bHalfMove)
	{
		for (int i=0 ;i< m_XSegment/2 ;i++)
		{
			m_PartLastError[0] =m_pXMotor->MoveToHoleOneBySegment(Type,FALSE,FALSE,SpeedLevel,m_XSegment,WAITTIME,2);
			PART_ERROR_QUIT(0)
			m_ReturnValue =WaitPauseEventOrStop();
			IF_PAUSEEVENT_STOP  //STOP 退出，暂停等待
		}
		bHalfMove =FALSE;
	}
	m_ReturnValue =WaitPauseEventOrStop();
	IF_PAUSEEVENT_STOP  //STOP 退出，暂停等待
	for (int i=0 ;i< m_XSegment ;i++)
	{
		m_PartLastError[0] =m_pXMotor->MoveToHoleOneBySegment(Type,bRight,FALSE,SpeedLevel,m_XSegment,WAITTIME,2);
		PART_ERROR_QUIT(0)
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP  //STOP 退出，暂停等待
	}
	m_ReturnValue =WaitPauseEventOrStop();
	IF_PAUSEEVENT_STOP  //STOP 退出，暂停等待
    return m_LastError;
}

int CPart:: MoveHalfHoleOneSuckMagnetic(BYTE Type, int SpeedLevel,BOOL bRight) //运动一半孔1吸磁
{
	TRACE(_T("CPart::MoveHoleOneSuckMagnetic Type=%d,SpeedLevel=%d,bRight=%d"),Type,SpeedLevel,bRight);
	m_PartLastError[0] =m_pXMotor->MoveToHoleOneBySegment(Type,bRight,FALSE,SpeedLevel,m_XSegment,WAITTIME,1); //设置参数
	PART_ERROR_QUIT(0)
	for (int i=0 ;i< m_XSegment/2 ;i++)
	{
		m_PartLastError[0] =m_pXMotor->MoveToHoleOneBySegment(Type,bRight,FALSE,SpeedLevel,m_XSegment,WAITTIME,2);
		PART_ERROR_QUIT(0)
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP  //STOP 退出，暂停等待
	}
	return m_LastError;
}

// X轴水平移动孔位置
int   CPart:: MoveToHolePos(BYTE Type,BYTE HolePos)
{
	TRACE(_T("CPart::MoveToHolePos Type=%d,HolePos=%d"),Type,HolePos);
	if (HolePos == 1)
	{
		m_PartLastError[0] =m_pXMotor->MoveToHoleOneCenter(Type);
		PART_ERROR_QUIT(0)
		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP
	}
	else //其他孔
	{
		if(Type == KIT_TYPE_ONE &&  HolePos == KIT_TYPE_ONE_SPECIAL_POS) // 10ml 新增特殊位置，放置2ml EP管
		{
			m_PartLastError[0] =m_pXMotor->MoveToSpecialHolePos();
			PART_ERROR_QUIT(0)
		}
		else
		{
			int  LastPos =GetLastHolePos(Type);	//  最后孔的位置不同
			if(HolePos>LastPos)  HolePos = LastPos ; //// 其他孔 不能超过最大孔位置,防止撞。
			if (LastPos == HolePos )
			{
				m_PartLastError[0] =m_pXMotor->MoveToLastHolePos(Type);
				PART_ERROR_QUIT(0)
			}
			else // 其他孔 不能超过最大孔位置
			{
				m_PartLastError[0] =m_pXMotor->MoveToHolePos(Type,HolePos);
				PART_ERROR_QUIT(0)
			}
		}

		m_ReturnValue =WaitPauseEventOrStop();
		IF_PAUSEEVENT_STOP  //STOP 退出，暂停等待
	}
	return m_LastError;
}
//下降Z轴先 ,上升Z轴后。
int CPart:: MoveToLiquidTop(BYTE Type,BYTE HolePos,int Volume,BOOL bFast , BOOL bZMotorFirst)
{
	TRACE(_T("CPart::MoveToLiquidTop Type=%d,HolePos=%d,Volume=%d,bFast=%d,bZMotorFirst=%d"),Type,HolePos,Volume,bFast,bZMotorFirst);
	UINT WaitTM =0;BYTE Mark =1;
	int MoveStep1=0,MoveStep2=0;
	m_PartLastError[2] =m_pMagBarMotor->MoveToLiquidTop(MoveStep1,Type,HolePos,Volume,bFast,WaitTM,Mark); //保存参数
    PART_ERROR_QUIT(2)
	m_PartLastError[1] =m_pZMotor->MoveToLiquidTop(MoveStep2,Type,HolePos,Volume,bFast,WaitTM,Mark);  //保存参数
	PART_ERROR_QUIT(1)
	if (MoveStep1!=0 && MoveStep2 !=0) // step=0 ,不动作
	{
		Mark=2;
		if (bZMotorFirst)
		{
			m_PartLastError[1] =m_pZMotor->MoveToLiquidTop(MoveStep2,Type,HolePos,Volume,bFast,WaitTM,Mark);
			PART_ERROR_QUIT(1)
			m_PartLastError[2] =m_pMagBarMotor->MoveToLiquidTop(MoveStep1,Type,HolePos,Volume,bFast,WaitTM,Mark);
			PART_ERROR_QUIT(2)
		}
		else
		{
			m_PartLastError[2] =m_pMagBarMotor->MoveToLiquidTop(MoveStep1,Type,HolePos,Volume,bFast,WaitTM,Mark);
			PART_ERROR_QUIT(2)
			m_PartLastError[1] =m_pZMotor->MoveToLiquidTop(MoveStep2,Type,HolePos,Volume,bFast,WaitTM,Mark);
			PART_ERROR_QUIT(1)
		}
		m_PartLastError[1] =m_pZMotor->WaitStepFinish();
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
		PART_ERROR_QUIT(2)
	}
	return m_LastError;
}
//孔1 吸磁下降
int CPart:: MoveDownHeight(float Height)
{
	TRACE(_T("CPart::MoveDownHeight Height=%f"),Height);
    UINT WaitTM =0;BYTE Mark =1;
	int MoveStep1=0,MoveStep2=0;
	m_PartLastError[1] =m_pZMotor->MoveDownStep(MoveStep1,Height,WaitTM,Mark);
	PART_ERROR_QUIT(1)
	m_PartLastError[2] =m_pMagBarMotor->MoveDownStep(MoveStep2,Height,WaitTM,Mark);
	PART_ERROR_QUIT(2)
    if (MoveStep1!=0 && MoveStep2 !=0) // step=0 ,不动作
    {

		Mark =2; // 动作
		m_PartLastError[1] =m_pZMotor->MoveDownStep(MoveStep1,Height,WaitTM,Mark);
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->MoveDownStep(MoveStep2,Height,WaitTM,Mark);
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->WaitStepFinish(); //速度固定
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
		PART_ERROR_QUIT(2)
    }
    TRACE(_T("CPart::MoveDownHeight MoveStep1=%d,MoveStep2=%d"),MoveStep1,MoveStep2);
	return m_LastError;
}

////下降到底  HolePos 孔位
int CPart:: MoveDown(BYTE HolePos)
{
	TRACE(_T("CPart::MoveDown HolePos=%d"),HolePos);
	UINT WaitTM =0;BYTE Mark =1;
	int MoveStep1=0,MoveStep2=0;
	m_PartLastError[1] =m_pZMotor->MoveDown(MoveStep1,HolePos,WaitTM,Mark); // Mark =1 ,先保存参数
	PART_ERROR_QUIT(1)
	m_PartLastError[2] =m_pMagBarMotor->MoveDown(MoveStep2,HolePos,WaitTM,Mark);
	PART_ERROR_QUIT(2)

    if (MoveStep1!=0 && MoveStep2 !=0)
	{
		Mark =2;
		m_PartLastError[1] =m_pZMotor->MoveDown(MoveStep1,HolePos,WaitTM,2);
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->MoveDown(MoveStep2,HolePos,WaitTM,2);
		PART_ERROR_QUIT(2)

		m_PartLastError[1] =m_pZMotor->WaitStepFinish(); //等待下降到位
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
		PART_ERROR_QUIT(2)
	}

	TRACE(_T("CPart::MoveDown HolePos=%d End,MoveStep1=%d,MoveStep2 =%d"),HolePos,MoveStep1,MoveStep2);
	return m_LastError;
}
//缓慢下降吸磁
int CPart:: MoveDownSlow(BOOL bSegment, float Height,BYTE suckLevel)
{
	TRACE(_T("CPart::MoveDownSlow  bSegment=%d, Height=%f,suckLevel=%d"),bSegment,Height,suckLevel);

	UINT WaitTM =0;BYTE Mark =1;
	int MoveStep1=0,MoveStep2=0;

	m_PartLastError[1] =m_pZMotor->MoveDownSlow(MoveStep1,bSegment, Height,WaitTM,Mark,suckLevel);  //设置参数
	PART_ERROR_QUIT(1)
	m_PartLastError[2] =m_pMagBarMotor->MoveDownSlow(MoveStep2,bSegment, Height,WaitTM,Mark,suckLevel); //设置参数
	PART_ERROR_QUIT(2)

    if (MoveStep1!=0 && MoveStep2 !=0)
    {
		Mark =2;
		m_PartLastError[1] =m_pZMotor->MoveDownSlow(MoveStep1,bSegment, Height,WaitTM,Mark,suckLevel);  //缓慢下降吸磁
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->MoveDownSlow(MoveStep2,bSegment, Height,WaitTM,Mark,suckLevel); //缓慢下降吸磁
		PART_ERROR_QUIT(2)

		if(!bSegment) //不分段，最大高度
		{
			Height = 43;//TODO 试剂盒圆孔深度 ，用来算时间
		}
		UINT WaitTime = 1.5* Height*(ZMOTOR_MM_TO_PULSE)*10*1000/(m_pZMotor->m_iSpeed[7]*suckLevel);//1.5是系数，  (ZMOTOR_MM_TO_PULSE) 100 是毫米转脉冲 ，10 是level ，1000是秒转毫秒

		if(WaitTime<20*1000)  WaitTime=20*1000;

		m_PartLastError[1] =m_pZMotor->WaitStepFinish(WaitTime); //等待下降到位
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish(WaitTime);
		PART_ERROR_QUIT(2)
    }
	TRACE(_T("CPart::MoveDownSlow  End,MoveStep1=%d,MoveStep2 =%d"),MoveStep1,MoveStep2);
	return m_LastError;
}
// 缓慢上升
int  CPart::MoveUpSlow(float Height, int SpeedLevel)
{
	TRACE(_T("CPart::MoveUpSlow  Height=%f,SpeedLevel=%d"),Height,SpeedLevel);
	UINT WaitTM =0;BYTE Mark =1;
	int MoveStep1=0,MoveStep2=0;

	m_PartLastError[1] =m_pZMotor->MoveUpSlow(MoveStep1,Height,WaitTM,Mark,SpeedLevel);  //设置参数
	PART_ERROR_QUIT(1)
	m_PartLastError[2] =m_pMagBarMotor->MoveUpSlow(MoveStep2,Height,WaitTM,Mark,SpeedLevel); //设置参数
	PART_ERROR_QUIT(2)
	if (MoveStep1!=0 && MoveStep2 !=0)
	{
		Mark =2;
		m_PartLastError[1] =m_pZMotor->MoveUpSlow(MoveStep1,Height,WaitTM,Mark,SpeedLevel);  //缓慢上升
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->MoveUpSlow(MoveStep2,Height,WaitTM,Mark,SpeedLevel); //缓慢上升
		PART_ERROR_QUIT(2)
		UINT WaitTime = 1.5* Height*(ZMOTOR_MM_TO_PULSE)*10*1000/(m_pZMotor->m_iSpeed[7]*SpeedLevel);	//1.5是系数， ZMOTOR_MM_TO_PULSE 100 是毫米转脉冲 ，10 是level
		if(WaitTime<20*1000)  WaitTime = 20*1000;
		m_PartLastError[1] =m_pZMotor->WaitStepFinish(WaitTime); //等待到位
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish(WaitTime);
		PART_ERROR_QUIT(2)
	}
    TRACE(_T("CPart::MoveUpSlow  End,MoveStep1=%d,MoveStep2 =%d"),MoveStep1,MoveStep2);
	return m_LastError;
}

//获取高度
float CPart:: GetMoveHeight(BYTE Type,BYTE HolePos,int Volume)
{
	float Height1 =m_pZMotor->GetHeightByVolumeInHolePos(Type, HolePos,Volume);

	float Height2 =m_pMagBarMotor->GetHeightByVolumeInHolePos(Type,HolePos,Volume); //

	float Height =(Height1<=Height2)? Height1 :Height2 ; //单位mm

	TRACE(_T("CPart:: GetMoveHeight Height =%f,Type=%d,HolePos=%d,Volume=%d"),Height,Type,HolePos,Volume);
	return Height;
}

int CPart::PartDebug(UCHAR ACTION)
{
	int Step=0;
	switch(ACTION)
	{
	case 0://
		EmptyError();
		m_pXMotor->InitMove();
		m_pZMotor->InitMove();
		m_pMagBarMotor->InitMove();
		m_PartLastError[2] =m_pMagBarMotor->InitHome();
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->InitHome();
		PART_ERROR_QUIT(1)
		m_PartLastError[0] =m_pXMotor->InitHome();
		PART_ERROR_QUIT(0)
		break;
	case 1:
		//混匀
		m_PartLastError[0] =m_pXMotor->MoveToHoleOneCenter(KIT_TYPE_ONE);
		PART_ERROR_QUIT(0)
		m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos();
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->MoveDown(Step,1,0);
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->MoveDown(Step,1,0);
		PART_ERROR_QUIT(2)
		//等待下降到位
		m_PartLastError[1] =m_pZMotor->WaitStepFinish();
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
		PART_ERROR_QUIT(2)

		for (int i=0; i<10 ;i++)
		{
			if (i==0)
			{
				m_PartLastError[0] =m_pXMotor->MoveHalfHoleOneDistance(KIT_TYPE_ONE,TRUE,TRUE,5,WAITTIME*3);
				PART_ERROR_QUIT(0)
			}

			m_PartLastError[0] =m_pXMotor->MoveHoleOneDistance(KIT_TYPE_ONE,TRUE,TRUE,5,WAITTIME*6);
			PART_ERROR_QUIT(0)
			m_PartLastError[0] =m_pXMotor->MoveHoleOneDistance(KIT_TYPE_ONE,FALSE,TRUE,5,WAITTIME*6);
			PART_ERROR_QUIT(0)
		}

		break;
	case 2: //缓慢下降
		//
		m_PartLastError[0] =m_pXMotor->MoveToHoleOneCenter(KIT_TYPE_ONE);
		PART_ERROR_QUIT(0)
		m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos();
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->MoveToLiquidTop(Step,KIT_TYPE_ONE,1,10000,TRUE,0);
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->MoveToLiquidTop(Step,KIT_TYPE_ONE,1,10000,TRUE,0);
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->WaitStepFinish();
		PART_ERROR_QUIT(1)
		m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
		PART_ERROR_QUIT(2)

		for (int i=0; i<5 ;i++)
		{
			m_PartLastError[1] =m_pZMotor->MoveDownStep(Step,2,0);
			PART_ERROR_QUIT(1)
			m_PartLastError[2] =m_pMagBarMotor->MoveDownStep(Step,2,0);
			PART_ERROR_QUIT(2)
			m_PartLastError[1] =m_pZMotor->WaitStepFinish();
			PART_ERROR_QUIT(1)
			m_PartLastError[2] =m_pMagBarMotor->WaitStepFinish();
			PART_ERROR_QUIT(2)

			if (i==0)
			{
				m_PartLastError[0] =m_pXMotor->MoveHalfHoleOneDistance(KIT_TYPE_ONE,TRUE,FALSE,5,WAITTIME*3);
				PART_ERROR_QUIT(0)
			}

			m_PartLastError[0] =m_pXMotor->MoveHoleOneDistance(KIT_TYPE_ONE,TRUE,FALSE,5,WAITTIME*6);
			PART_ERROR_QUIT(0)
			m_PartLastError[0] =m_pXMotor->MoveHoleOneDistance(KIT_TYPE_ONE,FALSE,FALSE,5,WAITTIME*6);
			PART_ERROR_QUIT(0)
		}
		break;

	case 3: //其他孔振荡
		//
		m_PartLastError[0] =m_pXMotor->MoveToLastHolePos(KIT_TYPE_ONE);
		PART_ERROR_QUIT(0)
		m_PartLastError[2] =m_pMagBarMotor->MoveIdlePos();
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->MoveDown(Step,2);
		PART_ERROR_QUIT(1)

		for (int i=0; i<10 ;i++)
		{
			m_PartLastError[1] =m_pZMotor->MoveDownFast(FALSE,1000);
			PART_ERROR_QUIT(1)
			m_PartLastError[1] =m_pZMotor->MoveDownFast(TRUE,1000);
			PART_ERROR_QUIT(1)
		}

		break;
	default:
		break;
	}

	if (ACTION>0 && ACTION<=3)
	{
		m_PartLastError[2] =m_pMagBarMotor->GoToHome();
		PART_ERROR_QUIT(2)
		m_PartLastError[1] =m_pZMotor->GoToHome();
		PART_ERROR_QUIT(1)
		m_PartLastError[0] =m_pXMotor->GoToHome();
		PART_ERROR_QUIT(0)
	}
	return 0;
}
/*
	BYTE m_Mark ; //  1动作开始 2 动作结束
	BYTE m_ActType ;//  1 混匀  2  吸磁   3 等待
	int  m_Time; // 时长;秒
*/
void CPart:: SndActionStarOrEndMsg(BYTE StepID,BYTE Mark,BYTE ActType ,int  Time)
{
	TRACE(_T("CPart:: SndActionStarOrEndMsg StepID=%d,Mark=%d,ActType=%d,Time=%d"),StepID,Mark,ActType,Time);
	CSndActionStarOrEnd *pMsg = new CSndActionStarOrEnd;
	pMsg->m_StepId =StepID;
	pMsg->m_Mark =Mark ;
	pMsg->m_ActType = ActType;
	pMsg->m_Time = Time;
	g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg); //发送动作更新。
}
//设置吸磁方式
void  CPart::SetSuckMagneticWay(BYTE  SuckMagneticWay)
{
	TRACE(_T("CPart:: SetSuckMagneticWay SuckMagneticWay=%d"),SuckMagneticWay);
	m_SuckMagneticWay =SuckMagneticWay;
}
// 设置运动参数
void   CPart::SetMotionParameters(BYTE HorizontalSpeed,int HorizontalMixSpeed,BYTE HorizontalSuckMagneticSpeed,BYTE UpDownSpeed,int UpDownMixSpeed, BYTE SuckMagneticSpeed )
{
	m_pXMotor->m_iSpeed[2]=HorizontalSpeed*m_pXMotor->m_PerMMToPluse ;  //  水平移动速度
    m_pXMotor->m_iSpeed[3] = HorizontalMixSpeed*m_pXMotor->m_PerMMToPluse;
    m_pXMotor->m_iSpeed[4] =HorizontalSuckMagneticSpeed*m_pXMotor->m_PerMMToPluse;

    m_pZMotor->m_iSpeed[2] =UpDownSpeed*ZMOTOR_MM_TO_PULSE ; //磁棒套升降速度mm/s
	m_pZMotor->m_iSpeed[3] =UpDownMixSpeed*ZMOTOR_MM_TO_PULSE; //上下混匀速度
    m_pZMotor->m_iSpeed[7]=  SuckMagneticSpeed*ZMOTOR_MM_TO_PULSE; // 磁棒套吸磁速度

    m_pMagBarMotor->m_iSpeed[2] =UpDownSpeed*m_pMagBarMotor->m_PerMMToPluse ;//磁棒升降速度mm/s
    m_pMagBarMotor->m_iSpeed[7] =SuckMagneticSpeed*m_pMagBarMotor->m_PerMMToPluse ;// 吸磁速度

	SaveSpeedParameters();//然后保存
}
// 说明： 磁棒套和磁棒使用相同速度，所有只有6个速度
void   CPart::GetMotionParameters(BYTE &HorizontalSpeed,int &HorizontalMixSpeed,BYTE &HorizontalSuckMagneticSpeed,BYTE &UpDownSpeed,int  &UpDownMixSpeed, BYTE &SuckMagneticSpeed)
{
	HorizontalSpeed = m_pXMotor->m_iSpeed[2]/m_pXMotor->m_PerMMToPluse ;  //  水平移动速度
	HorizontalMixSpeed  =  m_pXMotor->m_iSpeed[3]/m_pXMotor->m_PerMMToPluse ;
	HorizontalSuckMagneticSpeed = m_pXMotor->m_iSpeed[4]/m_pXMotor->m_PerMMToPluse;//

	UpDownSpeed =m_pZMotor->m_iSpeed[2]/ZMOTOR_MM_TO_PULSE ; //磁棒升降速度mm/s
	UpDownMixSpeed =m_pZMotor->m_iSpeed[3]/ZMOTOR_MM_TO_PULSE; //上下混匀速度
	SuckMagneticSpeed= m_pZMotor->m_iSpeed[7]/ZMOTOR_MM_TO_PULSE; // 吸磁速度
}

void CPart::SetFactoryDefaultSpeed()
{
	m_pXMotor->m_iSpeed[2]=m_FactoryHorizontalSpeed*m_pXMotor->m_PerMMToPluse ;  //  水平移动速度
	m_pXMotor->m_iSpeed[3] = m_FactoryHorizontalMixSpeed*m_pXMotor->m_PerMMToPluse;
	m_pXMotor->m_iSpeed[4] =m_FactoryHorizontalSuckMagneticSpeed*m_pXMotor->m_PerMMToPluse;

	m_pZMotor->m_iSpeed[2] =m_FactoryUpDownSpeed*ZMOTOR_MM_TO_PULSE ; //磁棒升降速度mm/s
	m_pZMotor->m_iSpeed[3] =m_FactoryUpDownMixSpeed*ZMOTOR_MM_TO_PULSE; //上下混匀速度
	m_pZMotor->m_iSpeed[7]=  m_FactorySuckMagneticSpeed*ZMOTOR_MM_TO_PULSE; // 吸磁速度

	m_pMagBarMotor->m_iSpeed[2] =m_FactoryUpDownSpeed*m_pMagBarMotor->m_PerMMToPluse ;
	m_pMagBarMotor->m_iSpeed[7] =m_FactorySuckMagneticSpeed*m_pMagBarMotor->m_PerMMToPluse ;

   // 发送参数
	CSndMotionParameters *pMsg1 =new CSndMotionParameters;
	GetMotionParameters(pMsg1->m_HorizontalSpeed,pMsg1->m_HorizontalMixSpeed,pMsg1->m_HorizontalSuckMagneticSpeed,pMsg1->m_UpDownSpeed,pMsg1->m_UpDownMixSpeed,pMsg1->m_SuckMagneticSpeed);
	g_ST->m_pAllTcpIp->SendMsgNotWait(pMsg1);	//回复指令

	SaveSpeedParameters();
}
int CPart:: StopMotor()
{
	TRACE(_T("CPart:: StopMotor"));
	m_pMagBarMotor->Stop();
	m_pZMotor->Stop();
	m_pXMotor->Stop();
	return m_LastError;
}

void CPart:: SaveSpeedParameters()
{
#ifdef  WIN32
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("MotorSpeed.ini"), wxT(""),wxCONFIG_USE_RELATIVE_PATH);
#else
	wxFileConfig *config = new wxFileConfig(wxT(""), wxT(""), wxT("/opt/MotorSpeed.ini"), wxT(""),wxCONFIG_USE_GLOBAL_FILE);
#endif
	wxString strsect1,strsect2;
	//X轴
	strsect1 =_T("/Motor1");  //
	config->SetPath(strsect1);
	strsect2 =_T("MotorSpeed2");
	config->Write(strsect2, m_pXMotor->m_iSpeed[2]);
	strsect2 =_T("MotorSpeed3");
	config->Write(strsect2, m_pXMotor->m_iSpeed[3]);
	strsect2 =_T("MotorSpeed4");
	config->Write(strsect2, m_pXMotor->m_iSpeed[4] );

    //磁棒套轴
	strsect1 =_T("/Motor2");  //
	config->SetPath(strsect1);
	strsect2 =_T("MotorSpeed2");
	config->Write(strsect2, m_pZMotor->m_iSpeed[2] );
	strsect2 =_T("MotorSpeed3");
	config->Write(strsect2,m_pZMotor->m_iSpeed[3] );
	strsect2 =_T("MotorSpeed7");
	config->Write(strsect2, m_pZMotor->m_iSpeed[7] );

	//磁棒轴

	strsect1 =_T("/Motor3");  //
	config->SetPath(strsect1);
	strsect2 =_T("MotorSpeed2");
	config->Write(strsect2, m_pMagBarMotor->m_iSpeed[2]);

	strsect2 =_T("MotorSpeed7");
	config->Write(strsect2, m_pMagBarMotor->m_iSpeed[7]);
	delete config;
	//重新加载参数
    DownPartParam();
}

/////////////////////////////////紫外灯线程///////////////////////////////////////
CUVLampThread::CUVLampThread(CPart* pfather)
{
	m_pfather=pfather;
	Create();
	SetPriority(100);
	Run();
}
void* CUVLampThread::Entry()
{
	m_pfather->UVLamp();
	return NULL;
}
CUVLampThread::~CUVLampThread()
{
}
