// TMCM3216Control.cpp
// TMCM-3216 单卡控制层实现

#include "TMCM3216Control.h"
#include "TMCM3216Motor.h"
#include <stdio.h>
#include <string.h>

// ============== 构造/析构 ==============
CTMCM3216Control::CTMCM3216Control(UINT cardID, UINT canID)
    : m_CardID(cardID), m_CanID(canID), m_FirmwareVer(0), m_LastError(0),
      m_bOnline(FALSE), m_bDebug(FALSE), m_CommandSN(0)
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

// ============== 子电机管理 ==============
void CTMCM3216Control::SetChildMotor(UCHAR axisIndex, CTMCM3216Motor* pMotor)
{
    if (axisIndex < TMCM3216_MAX_AXIS)
        m_pMotors[axisIndex] = pMotor;
}

CTMCM3216Motor* CTMCM3216Control::GetChildMotor(UCHAR axisIndex)
{
    return (axisIndex < TMCM3216_MAX_AXIS) ? m_pMotors[axisIndex] : NULL;
}

// ============== 底层 CAN 接口（需要对接真实 CAN 驱动，这里提供默认空实现）==============
// 真实项目中请重写此函数，将 frame.id + data[8] 发到 CAN 总线
int CTMCM3216Control::DoCANSend(UINT canID, const UCHAR* data, int len)
{
    if (m_bDebug)
    {
        printf("[TMCM3216Control::DoCANSend] card=%d, CANID=%d, data=", m_CardID, canID);
        for (int i = 0; i < 8; ++i) printf("%02X ", data[i]);
        printf("\n");
    }
    return 0;   // 默认返回成功（无硬件时可用于逻辑测试）
}

int CTMCM3216Control::DoCANWait(UINT canID, UCHAR* outData, int& outLen, UINT WaitTM)
{
    // 默认实现不做真正等待（因为不知道上层 CAN 组件的 API）
    // 真实项目请在此处调用 g_Can.WaitCanRcv 之类的函数，填入 8 字节到 outData
    if (WaitTM == 0) return 0;
    return -2;   // -2 表示未实现等待
}

// ============== 同步发送主流程 ==============
int CTMCM3216Control::SendAndWait(TMCL_CAN_Frame& frame, UINT WaitTM)
{
    EnterCriticalSection(&m_csLock);

    // 1. 发 CAN 帧
    int rtn = DoCANSend(frame.id, frame.data, frame.len);
    if (rtn < 0)
    {
        m_LastError = 0xFFFF;
        LeaveCriticalSection(&m_csLock);
        return rtn;
    }

    // 2. WaitTM == 0 表示不等待（异步/仅发命令），直接返回
    if (WaitTM == 0)
    {
        LeaveCriticalSection(&m_csLock);
        return 0;
    }

    // 3. 等待响应
    UCHAR rcvBuf[8] = {0};
    int rcvLen = 0;
    rtn = DoCANWait(frame.id, rcvBuf, rcvLen, WaitTM);
    if (rtn < 0)
    {
        m_bOnline = FALSE;
        LeaveCriticalSection(&m_csLock);
        return rtn;
    }

    // 4. 解析并更新内部缓存
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
        {
            rtn = -3;
        }
    }
    else
    {
        rtn = -4;
    }

    LeaveCriticalSection(&m_csLock);
    return rtn;
}

// ============== 统一处理解析结果 ==============
void CTMCM3216Control::HandleRxResult(const TMCL_RxResult& res)
{
    // 记录错误码
    if (!res.isSuccess())
    {
        m_LastError = res.status;
    }

    // 对轴相关的回包更新缓存
    if (res.axis < TMCM3216_MAX_AXIS && res.command != 0)
    {
        // 这里只做简单的 value 缓存到位置；真实情况可根据具体命令号细分
        if (res.command == 1)  // GAP
        {
            // 由 type 区分具体读的是位置/速度/状态等
            // 简化：更新 cachedPos 的判断交给上层；此处 value 原样保留
        }
    }
}

// ============== 回调分发 ==============
void CTMCM3216Control::NotifyAxisFinished(UCHAR axis, UCHAR flag, int value)
{
    if (axis >= TMCM3216_MAX_AXIS) return;
    // 停止/完成类回调统一把状态归为 IDLE
    if (flag & (TMCM3216_FLAG_FINISH | TMCM3216_FLAG_STOPPED |
                TMCM3216_FLAG_LIMIT_POS | TMCM3216_FLAG_LIMIT_NEG))
    {
        m_State[axis] = MS_IDLE;
    }
    if (flag & TMCM3216_FLAG_ERROR)
    {
        m_State[axis] = MS_ERROR;
    }
    // 真实场景：触发子电机对象的回调
    if (m_pMotors[axis])
    {
        m_pMotors[axis]->OnCommandFinished(flag, value);
    }
}

// ============== CAN 接收回调（用于异步模式）==============
BOOL CTMCM3216Control::OnCANRecv(const TMCL_CAN_Frame& frame)
{
    return OnCANRecv(frame.data, frame.len);
}

BOOL CTMCM3216Control::OnCANRecv(const UCHAR* data, int len)
{
    if (!data || len < 8) return FALSE;

    // 判断是否为发给本卡的响应（首字节为模块地址）
    if (data[0] != (UCHAR)m_CanID && data[0] != (UCHAR)m_CardID)
    {
        // 有可能模块地址与 CAN ID 不同；这里宽松处理，交由上层校验
    }

    TMCL_RxResult res = TMCM3216_CAN_Protocol::parseFrame(data, 8);
    if (!res.valid) return FALSE;

    EnterCriticalSection(&m_csLock);
    HandleRxResult(res);
    LeaveCriticalSection(&m_csLock);

    // 如果收到完成类的响应（状态码 100=成功），则可以通知对应轴
    // 真实 TMCL 的完成状态码 = 100 (0x64)
    if (res.status == 0x64 && res.axis < TMCM3216_MAX_AXIS)
    {
        NotifyAxisFinished(res.axis, TMCM3216_FLAG_FINISH, res.value);
    }
    return TRUE;
}

// ============== 轮询状态 ==============
void CTMCM3216Control::PollStatus()
{
    for (UCHAR axis = 0; axis < TMCM3216_MAX_AXIS; ++axis)
    {
        BOOL bRunning = FALSE;
        if (ReadMotorRunning(axis, bRunning, 200) == 0)
        {
            if (!bRunning && m_State[axis] != MS_IDLE)
            {
                NotifyAxisFinished(axis, TMCM3216_FLAG_FINISH, m_CachedPos[axis]);
            }
        }
    }
}

// ============== 同步命令：设备信息 ==============
int CTMCM3216Control::ReadFirmware(UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getFirmwareVersion((UCHAR)m_CanID);
    int rtn = SendAndWait(frame, WaitTM);
    return rtn;
}

int CTMCM3216Control::ReadGlobalError(UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getErrorCode((UCHAR)m_CanID);
    int rtn = SendAndWait(frame, WaitTM);
    return rtn;
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

// ============== 轴相关同步读 ==============
int CTMCM3216Control::ReadActualPos(UCHAR axis, int& pos, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getPosition((UCHAR)m_CanID, axis);
    int rtn = SendAndWait(frame, WaitTM);
    if (rtn == 0 && WaitTM > 0)
    {
        // value 由 HandleRxResult 更新；这里简化：直接使用返回的 value
        pos = (int)frame.data[4]; // 占位，真实实现应从解析结果取
        // 推荐做法：把 SendAndWait 的 value 写回到 pos
        m_CachedPos[axis] = pos;
    }
    return rtn;
}

int CTMCM3216Control::ReadActualVel(UCHAR axis, int& vel, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getActualSpeed((UCHAR)m_CanID, axis);
    int rtn = SendAndWait(frame, WaitTM);
    if (rtn == 0 && WaitTM > 0)
    {
        m_CachedVel[axis] = vel;
    }
    return rtn;
}

int CTMCM3216Control::ReadTargetPos(UCHAR axis, int& pos, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame;  // 没有现成的 getTargetPos，用通用 packFrame
    frame = TMCM3216_CAN_Protocol::packFrame((UCHAR)m_CanID, TMCL_GAP, 3, axis, 0);
    return SendAndWait(frame, WaitTM);
}

// ============== 轴参数读/写 ==============
int CTMCM3216Control::ReadMaxSpeed(UCHAR axis, UINT& speed, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getMaxSpeed((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteMaxSpeed(UCHAR axis, UINT speed, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setMaxSpeed((UCHAR)m_CanID, axis, (int)speed);
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
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setAcceleration((UCHAR)m_CanID, axis, (int)acc);
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
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setMicrostep((UCHAR)m_CanID, axis, (WORD)microstep);
    return SendAndWait(frame, WaitTM);
}

// ============== 运行状态 ==============
int CTMCM3216Control::ReadMotorRunning(UCHAR axis, BOOL& bRunning, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    // 读取轴状态标志 (axis parameter 157 status flags)
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::packFrame((UCHAR)m_CanID, TMCL_GAP, 157, axis, 0);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadHomeState(UCHAR axis, BOOL& bHome, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getHomeState((UCHAR)m_CanID, axis);
    return SendAndWait(frame, WaitTM);
}

// ============== 运动控制 ==============
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
    m_State[axis] = MS_RUN_VEL;
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

// ============== 回零 ==============
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

// ============== IO ==============
int CTMCM3216Control::ReadDigitalInput(UCHAR ch, BOOL& bHigh, UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::getDI0((UCHAR)m_CanID);
    // 使用 type=ch 对应具体通道，用 packFrame 直接构造
    frame = TMCM3216_CAN_Protocol::packFrame((UCHAR)m_CanID, TMCL_GIO, ch, 0, 0);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::WriteDigitalOutput(UCHAR ch, BOOL bHigh, UINT WaitTM)
{
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setDO0((UCHAR)m_CanID, bHigh ? TRUE : FALSE);
    frame = TMCM3216_CAN_Protocol::packFrame((UCHAR)m_CanID, TMCL_SIO, ch, 0, bHigh ? 1 : 0);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::ReadAnalogInput(UCHAR ch, UINT& value, UINT WaitTM)
{
    // 模拟输入 type 从 25 开始
    UCHAR type = 25 + ch;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::packFrame((UCHAR)m_CanID, TMCL_GIO, type, 0, 0);
    return SendAndWait(frame, WaitTM);
}

// ============== 多轴同步 ==============
int CTMCM3216Control::SetSyncTargetPos(UCHAR axis, int pos, UINT WaitTM)
{
    if (axis >= TMCM3216_MAX_AXIS) return -1;
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::setSyncTargetPos((UCHAR)m_CanID, axis, pos);
    return SendAndWait(frame, WaitTM);
}

int CTMCM3216Control::StartSyncMove(UINT WaitTM)
{
    for (UCHAR i = 0; i < TMCM3216_MAX_AXIS; ++i)
    {
        if (m_pMotors[i] != NULL) m_State[i] = MS_MOVING;
    }
    TMCL_CAN_Frame frame = TMCM3216_CAN_Protocol::startSyncMove((UCHAR)m_CanID);
    return SendAndWait(frame, WaitTM);
}

// ============== 异步接口 ==============
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
    m_State[axis] = MS_RUN_VEL;
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
