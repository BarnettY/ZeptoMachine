// TMCM3216Motor.cpp - C++98 兼容版
#include "TMCM3216Motor.h"
CTMCM3216Motor::CTMCM3216Motor(UINT cardID, UCHAR axisID)
    : m_CardID(cardID), m_AxisID(axisID), m_pCtrl(NULL),
      m_pFinishCB(NULL), m_pCBUserData(NULL)
{
    memset(&m_LastFinish, 0, sizeof(m_LastFinish));
    m_LastFinish.axis = axisID;
}

CTMCM3216Motor::~CTMCM3216Motor()
{
}

void CTMCM3216Motor::AttachControl(CTMCM3216Control* pCtrl)
{
    m_pCtrl = pCtrl;
    if (pCtrl) pCtrl->SetChildMotor(m_AxisID, this);
}

// ============ 参数设置 ============
int CTMCM3216Motor::SetMaxSpeed(UINT speed)
{ return m_pCtrl ? m_pCtrl->WriteMaxSpeed(m_AxisID, speed) : -1; }

int CTMCM3216Motor::SetAcceleration(UINT acc)
{ return m_pCtrl ? m_pCtrl->WriteAcceleration(m_AxisID, acc) : -1; }

int CTMCM3216Motor::SetMaxCurrent(UCHAR current)
{ return m_pCtrl ? m_pCtrl->WriteMaxCurrent(m_AxisID, current) : -1; }

int CTMCM3216Motor::SetMicroStep(UINT microstep)
{ return m_pCtrl ? m_pCtrl->WriteMicroStep(m_AxisID, microstep) : -1; }

// ============ 参数读取 ============
int CTMCM3216Motor::GetActualPos(int& pos)
{ return m_pCtrl ? m_pCtrl->ReadActualPos(m_AxisID, pos) : -1; }

int CTMCM3216Motor::GetActualVel(int& vel)
{ return m_pCtrl ? m_pCtrl->ReadActualVel(m_AxisID, vel) : -1; }

int CTMCM3216Motor::GetMaxSpeed(UINT& speed)
{ return m_pCtrl ? m_pCtrl->ReadMaxSpeed(m_AxisID, speed) : -1; }

int CTMCM3216Motor::GetAcceleration(UINT& acc)
{ return m_pCtrl ? m_pCtrl->ReadAcceleration(m_AxisID, acc) : -1; }

// ============ 运动控制 ============
int CTMCM3216Motor::MoveAbsolute(int pos, UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->MoveAbsolute(m_AxisID, pos, WaitTM) : -1; }

int CTMCM3216Motor::MoveRelative(int step, UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->MoveRelative(m_AxisID, step, WaitTM) : -1; }

int CTMCM3216Motor::MoveVelocity(int velocity, UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->MoveVelocity(m_AxisID, velocity, WaitTM) : -1; }

int CTMCM3216Motor::Stop(UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->StopMotor(m_AxisID, WaitTM) : -1; }

int CTMCM3216Motor::EmergencyStop(UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->EmergencyStop(m_AxisID, WaitTM) : -1; }

// ============ 回零 ============
int CTMCM3216Motor::Home(UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->HomeStart(m_AxisID, WaitTM) : -1; }

int CTMCM3216Motor::HomeSoft(UINT WaitTM)
{ return m_pCtrl ? m_pCtrl->HomeSoft(m_AxisID, WaitTM) : -1; }

// ============ 异步接口 ============
int CTMCM3216Motor::AsyncMoveAbs(int pos)
{ return m_pCtrl ? m_pCtrl->AsyncMoveAbs(m_AxisID, pos) : -1; }

int CTMCM3216Motor::AsyncMoveRel(int step)
{ return m_pCtrl ? m_pCtrl->AsyncMoveRel(m_AxisID, step) : -1; }

int CTMCM3216Motor::AsyncMoveVel(int velocity)
{ return m_pCtrl ? m_pCtrl->AsyncMoveVel(m_AxisID, velocity) : -1; }

int CTMCM3216Motor::AsyncStop()
{ return m_pCtrl ? m_pCtrl->AsyncStop(m_AxisID) : -1; }

int CTMCM3216Motor::AsyncHome()
{ return m_pCtrl ? m_pCtrl->AsyncHome(m_AxisID) : -1; }

// ============ 状态 ============
BOOL CTMCM3216Motor::IsMoving()
{
    if (!m_pCtrl) return FALSE;
    TMCM3216_MOTOR_STATE st = m_pCtrl->GetState(m_AxisID);
    return (st == MS_MOVING || st == MS_RUN || st == MS_HOMING);
}

BOOL CTMCM3216Motor::IsIdle()
{
    return m_pCtrl ? (m_pCtrl->GetState(m_AxisID) == MS_IDLE) : FALSE;
}

BOOL CTMCM3216Motor::IsError()
{
    return m_pCtrl ? (m_pCtrl->GetState(m_AxisID) == MS_ERROR) : TRUE;
}

// ============ 回调 ============
void CTMCM3216Motor::OnCommandFinished(UCHAR flag, int value)
{
    m_LastFinish.axis  = m_AxisID;
    m_LastFinish.flag  = flag;
    m_LastFinish.value = value;
    if (m_pFinishCB)
        m_pFinishCB(m_LastFinish, m_pCBUserData);
}
