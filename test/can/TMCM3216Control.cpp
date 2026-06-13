// TMCM3216Control.cpp - C++98 兼容版
//CTMCM3216Control
#include "TMCM3216Control.h"
#include "CanCom.h"
// ============ 构造/析构 ============
CTMCM3216Control::CTMCM3216Control(UINT cardID, UINT canID):m_CardID(cardID), m_CanID(canID), m_FirmwareVer(0),m_LastError(0), m_bOnline(FALSE), m_bDebug(FALSE), m_CommandSN(0)
{
	for (int i = 0; i < TMCM3216_MAX_AXIS; ++i)
	{
		m_pMotors[i]     = NULL;
		m_CachedPos[i]    = 0;
		m_CachedVel[i]    = 0;
		m_CachedTarget[i] = 0;
		m_State[i]        = MS_IDLE;
	}
	InitializeCriticalSection(&m_csLock);

}

CTMCM3216Control::~CTMCM3216Control()
{
    DeleteCriticalSection(&m_csLock);
}

// ============ 子电机管理 ============
void CTMCM3216Control::SetChildMotor(UCHAR axisIndex, void* pMotor)
{
    if (axisIndex < TMCM3216_MAX_AXIS)
        m_pMotors[axisIndex] = pMotor;
}

void* CTMCM3216Control::GetChildMotor(UCHAR axisIndex)
{
    return (axisIndex < TMCM3216_MAX_AXIS) ? m_pMotors[axisIndex] : NULL;
}
// ============ 底层 CAN 接口（默认空实现，子类应重写对接硬件） ============
int CTMCM3216Control::DoCANSend(UINT canID, const UCHAR* data, int len)
{
    if (m_bDebug)
    {
        printf("[CAN-Send] card=%d id=0x%02X data=", m_CardID, canID);
        for (int i = 0; i < 8; ++i) printf("%02X ", data[i]);
        printf("\n");
    }
    return 0;
}

int CTMCM3216Control::DoCANWait(UINT canID, UCHAR* outData, int& outLen, UINT WaitTM)
{
    if (WaitTM == 0) return 0;
    return -2;
}
// ============ 处理解析结果 ============
void CTMCM3216Control::HandleRxResult(const TMCL_RxResult& res)
{
	if (!res.isSuccess())
		m_LastError = res.status;
}
// ============ 同步发送主流程 ============
int CTMCM3216Control::SendAndWait(TMCL_CAN_Frame& frame, UINT WaitTM)
{
    EnterCriticalSection(&m_csLock);

    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    if (rtn < 0)
    {
        m_LastError = 0xFFFF;
        LeaveCriticalSection(&m_csLock);
        return rtn;
    }

    if (WaitTM == 0)
    {
        LeaveCriticalSection(&m_csLock);
        return 0;
    }

    UCHAR rcvBuf[8] = {0};
    int rcvLen = 0;
    rtn = DoCANWait(frame.id, rcvBuf, rcvLen, WaitTM);
    if (rtn < 0)
    {
        m_bOnline = FALSE;
        LeaveCriticalSection(&m_csLock);
        return rtn;
    }

    if (rcvLen >= 8)
    {
        TMCL_RxResult res = TMCM3216_CAN_Protocol::parseFrame(rcvBuf, 8);
        if (res.valid)
        {
            HandleRxResult(res);
            if (!res.isSuccess())
            {
                m_LastError = res.status;
                rtn = -1;
            }
            else
            {
                m_bOnline = TRUE;
                rtn = 0;
            }
        }
        else
            rtn = -3;
    }
    else
        rtn = -4;

    LeaveCriticalSection(&m_csLock);
    return rtn;
}
void CTMCM3216Control::NotifyAxisFinished(UCHAR axis, UCHAR flag, int value)
{
    if (axis >= TMCM3216_MAX_AXIS) return;
    if (flag & (TMCM3216_FLAG_FINISH | TMCM3216_FLAG_STOPPED |
                TMCM3216_FLAG_LIMIT_POS | TMCM3216_FLAG_LIMIT_NEG))
        m_State[axis] = MS_IDLE;
    if (flag & TMCM3216_FLAG_ERROR)
        m_State[axis] = MS_ERROR;
}

// ============ CAN 接收回调（用于异步模式） ============
BOOL CTMCM3216Control::OnCANRecv(const TMCL_CAN_Frame& frame)
{
    return OnCANRecv(frame.data, frame.len);
}

BOOL CTMCM3216Control::OnCANRecv(const UCHAR* data, int len)
{
    if (!data || len < 8) return FALSE;

    if (data[0] != (UCHAR)m_CanID && data[0] != (UCHAR)m_CardID)
        return FALSE;

    TMCL_RxResult res = TMCM3216_CAN_Protocol::parseFrame(data, 8);
    if (!res.valid) return FALSE;

    EnterCriticalSection(&m_csLock);
    HandleRxResult(res);
    LeaveCriticalSection(&m_csLock);

    if (res.isSuccess() && res.axis < TMCM3216_MAX_AXIS)
        NotifyAxisFinished(res.axis, TMCM3216_FLAG_FINISH, res.value);

    return TRUE;
}

// ============ 轮询状态 ============
void CTMCM3216Control::PollStatus()
{
    for (UCHAR axis = 0; axis < TMCM3216_MAX_AXIS; ++axis)
    {
        BOOL bRunning = FALSE;
        if (ReadMotorRunning(axis, bRunning, 200) == 0)
        {
            if (!bRunning && m_State[axis] != MS_IDLE)
                NotifyAxisFinished(axis, TMCM3216_FLAG_FINISH, m_CachedPos[axis]);
        }
    }
}

// ============ 固件/模块信息 ============
int CTMCM3216Control::ReadFirmware(UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getFirmwareVersion((UCHAR)m_CanID);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadGlobalError(UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getErrorCode((UCHAR)m_CanID);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadModuleAddress(UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getModuleAddress((UCHAR)m_CanID);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteModuleAddress(UCHAR newAddr, UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setModuleAddress((UCHAR)m_CanID, newAddr);
    int rtn = SendAndWait(frame, WaitTM);
    if (rtn == 0) m_CanID = newAddr;
    return rtn;
}

int CTMCM3216Control::ResetModule(UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::resetModule((UCHAR)m_CanID);
    return SendAndWait(frame, WaitTM);
}
int CTMCM3216Control::ReadActualPos(UCHAR axis, int& pos, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getPosition((UCHAR)m_CanID, axis);
    int rtn = SendAndWait(frame, WaitTM);
    if (rtn == 0 && WaitTM > 0)
        m_CachedPos[axis] = pos;
    return rtn;
}

int CTMCM3216Control::ReadActualVel(UCHAR axis, int& vel, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getActualSpeed((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadMaxSpeed(UCHAR axis, UINT& speed, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getMaxSpeed((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteMaxSpeed(UCHAR axis, UINT speed, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setMaxSpeed((UCHAR)m_CanID, axis, (int32_t)speed);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadAcceleration(UCHAR axis, UINT& acc, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getAcceleration((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteAcceleration(UCHAR axis, UINT acc, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setAcceleration((UCHAR)m_CanID, axis, (int32_t)acc);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadMaxCurrent(UCHAR axis, UCHAR& current, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getCurrent((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteMaxCurrent(UCHAR axis, UCHAR current, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setCurrent((UCHAR)m_CanID, axis, current);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadMicroStep(UCHAR axis, UINT& microstep, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getMicrostep((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteMicroStep(UCHAR axis, UINT microstep, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setMicrostep((UCHAR)m_CanID, axis, (uint16_t)microstep);
    return SendAndWait(frame, WaitTM);
}

// ============ 状态 ============
int CTMCM3216Control::ReadMotorRunning(UCHAR axis, BOOL& bRunning, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getStatusFlags((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadHomeState(UCHAR axis, BOOL& bHome, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getHomeState((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

// ============ 运动控制 ============
int CTMCM3216Control::MoveAbsolute(UCHAR axis, int pos, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_MOVING;
    m_CachedTarget[axis] = pos;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::moveAbsolute((UCHAR)m_CanID, axis, pos);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::MoveRelative(UCHAR axis, int step, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_MOVING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::moveRelative((UCHAR)m_CanID, axis, step);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::MoveVelocity(UCHAR axis, int velocity, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_RUN;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::moveVelocity((UCHAR)m_CanID, axis, velocity);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::StopMotor(UCHAR axis, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_STOPPING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::stopMotor((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::StopAll(UINT WaitTM)
{
    int rtn = 0;
    for (UCHAR axis = 0; axis < TMCM3216_MAX_AXIS; ++axis)
    {
        if (m_pMotors[axis] != NULL || m_State[axis] != MS_IDLE)
        {
            int r = StopMotor(axis, WaitTM);
            if (r < 0) rtn = r;
        }
    }
    return rtn;
}

int CTMCM3216Control::EmergencyStop(UCHAR axis, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_STOPPING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::emergencyStop((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

// ============ 回零 ============
int CTMCM3216Control::HomeStart(UCHAR axis, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_HOMING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::homeStart((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::HomeSoft(UCHAR axis, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_HOMING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::homeSoft((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

// ============ IO ============
int CTMCM3216Control::ReadDigitalInput(UCHAR ch, BOOL& bHigh, UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::packFrameEx(
        (UCHAR)m_CanID, TMCL_GIO, ch, 0, 0);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteDigitalOutput(UCHAR ch, BOOL bHigh, UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::packFrameEx(
        (UCHAR)m_CanID, TMCL_SIO, ch, 0, bHigh ? 1 : 0);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadAnalogInput(UCHAR ch, UINT& value, UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::packFrameEx(
        (UCHAR)m_CanID, TMCL_GIO, (UCHAR)(25 + ch), 0, 0);
    return SendAndWait(frame, WaitTM);
}

// ============ 多轴同步 ============
int CTMCM3216Control::SetSyncTargetPos(UCHAR axis, int pos, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setSyncTargetPos((UCHAR)m_CanID, axis, pos);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::StartSyncMove(UINT WaitTM)
{
    for (UCHAR i = 0; i < TMCM3216_MAX_AXIS; ++i)
        if (m_pMotors[i] != NULL) m_State[i] = MS_MOVING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::startSyncMove((UCHAR)m_CanID);
    return SendAndWait(frame, WaitTM);
}

// ============ 异步接口 ============
int CTMCM3216Control::AsyncReadPos(UCHAR axis)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getPosition((UCHAR)m_CanID, axis);
    EnterCriticalSection(&m_csLock);
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    LeaveCriticalSection(&m_csLock);
    return rtn;
}

int CTMCM3216Control::AsyncMoveAbs(UCHAR axis, int pos)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_MOVING;
    m_CachedTarget[axis] = pos;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::moveAbsolute((UCHAR)m_CanID, axis, pos);
    EnterCriticalSection(&m_csLock);
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    LeaveCriticalSection(&m_csLock);
    return rtn;
}

int CTMCM3216Control::AsyncMoveRel(UCHAR axis, int step)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_MOVING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::moveRelative((UCHAR)m_CanID, axis, step);
    EnterCriticalSection(&m_csLock);
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    LeaveCriticalSection(&m_csLock);
    return rtn;
}

int CTMCM3216Control::AsyncMoveVel(UCHAR axis, int velocity)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_RUN;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::moveVelocity((UCHAR)m_CanID, axis, velocity);
    EnterCriticalSection(&m_csLock);
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    LeaveCriticalSection(&m_csLock);
    return rtn;
}

int CTMCM3216Control::AsyncStop(UCHAR axis)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_STOPPING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::stopMotor((UCHAR)m_CanID, axis);
    EnterCriticalSection(&m_csLock);
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    LeaveCriticalSection(&m_csLock);
    return rtn;
}

int CTMCM3216Control::AsyncHome(UCHAR axis)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    m_State[axis] = MS_HOMING;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::homeStart((UCHAR)m_CanID, axis);
    EnterCriticalSection(&m_csLock);
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    LeaveCriticalSection(&m_csLock);
    return rtn;
}
