// TMCM3216BasicMotor.cpp - C++98 兼容版
#include "TMCM3216BasicMotor.h"
#include "../comon.h"

CTMCM3216BasicMotor::CTMCM3216BasicMotor(UINT cardID, UCHAR axisID, const char* name)
    : CTMCM3216Motor(cardID, axisID)
{
    memset(m_strName, 0, sizeof(m_strName));
    strncpy(m_strName, name ? name : "TMCM-Motor", sizeof(m_strName) - 1);
}

CTMCM3216BasicMotor::~CTMCM3216BasicMotor()
{
}

void CTMCM3216BasicMotor::SetName(const char* name)
{
    if (!name) return;
    memset(m_strName, 0, sizeof(m_strName));
    strncpy(m_strName, name, sizeof(m_strName) - 1);
}

BOOL CTMCM3216BasicMotor::CheckSoftLimit(int& targetPos)
{
    if (targetPos > m_DefaultParam.softLimitPos) { targetPos = m_DefaultParam.softLimitPos; return FALSE; }
    if (targetPos < m_DefaultParam.softLimitNeg) { targetPos = m_DefaultParam.softLimitNeg; return FALSE; }
    return TRUE;
}

int CTMCM3216BasicMotor::ApplyDefaultParam()
{
    if (!m_pCtrl) return -1;
    int r1 = SetMaxSpeed(m_DefaultParam.maxSpeed);
    int r2 = SetAcceleration(m_DefaultParam.acceleration);
    int r3 = SetMaxCurrent(m_DefaultParam.maxCurrent);
    int r4 = SetMicroStep(m_DefaultParam.microStep);
    if (r1 < 0 || r2 < 0 || r3 < 0 || r4 < 0) return -1;
    return 0;
}

int CTMCM3216BasicMotor::GoHome(UINT WaitTM)
{
    return Home(WaitTM);
}

int CTMCM3216BasicMotor::GoTo(int pos, UINT WaitTM)
{
    CheckSoftLimit(pos);
    return MoveAbsolute(pos, WaitTM);
}

int CTMCM3216BasicMotor::Step(int step, UINT WaitTM)
{
    return MoveRelative(step, WaitTM);
}

int CTMCM3216BasicMotor::Jog(int velocity, UINT WaitTM)
{
    return MoveVelocity(velocity, WaitTM);
}

int CTMCM3216BasicMotor::Halt()
{
    return Stop();
}

void CTMCM3216BasicMotor::DumpStatus()
{
    int pos = 0, vel = 0;
    GetActualPos(pos);
    GetActualVel(vel);
    printf("[%s] card=%u axis=%u state=%d pos=%d vel=%d\n",
           m_strName, m_CardID, m_AxisID, GetState(), pos, vel);
}

void CTMCM3216BasicMotor::OnCommandFinished(UCHAR flag, int value)
{
    CTMCM3216Motor::OnCommandFinished(flag, value);
    if (flag & TMCM3216_FLAG_ERROR) {
        printf("[%s] Error finished, flag=0x%02X value=%d\n", m_strName, flag, value);
    } else if (flag & TMCM3216_FLAG_FINISH) {
        printf("[%s] Command finished OK, value=%d\n", m_strName, value);
    }
}
