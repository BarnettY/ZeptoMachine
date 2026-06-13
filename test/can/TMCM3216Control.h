// TMCM3216Control.h - C++98 兼容版
// TMCM-3216 单卡（最多3轴）控制层：CAN 协议打包 + 收发 + 状态缓存

#pragma once

#include "TMCM-3216.h"
#include "CanInterface.h"

#define TMCM3216_MAX_AXIS        3

#define TMCM3216_FLAG_FINISH     0x01
#define TMCM3216_FLAG_STOPPED    0x02
#define TMCM3216_FLAG_LIMIT_POS  0x04
#define TMCM3216_FLAG_LIMIT_NEG  0x08
#define TMCM3216_FLAG_REF_FOUND  0x10
#define TMCM3216_FLAG_STALL      0x20
#define TMCM3216_FLAG_ERROR      0x80

enum TMCM3216_MOTOR_STATE
{
    MS_IDLE   = 0,
    MS_MOVING = 1,
    MS_RUN    = 2,
    MS_HOMING = 3,
    MS_STOPPING = 4,
    MS_ERROR  = 5
};

class CTMCM3216Control
{
public:
    CTMCM3216Control(UINT cardID, UINT canID = 1);
    virtual ~CTMCM3216Control();

    void SetChildMotor(UCHAR axisIndex, void* pMotor);
    void* GetChildMotor(UCHAR axisIndex);

    UINT GetCardID()        const { return m_CardID; }
    UINT GetCANID()         const { return m_CanID; }
    BOOL IsCardOnline()     const { return m_bOnline; }

    // ========== 同步命令 ==========
    int ReadFirmware(UINT WaitTM = 500);
    int ReadGlobalError(UINT WaitTM = 500);
    int ReadModuleAddress(UINT WaitTM = 500);
    int WriteModuleAddress(UCHAR newAddr, UINT WaitTM = 500);
    int ResetModule(UINT WaitTM = 1000);
	int ReadActualPos(UCHAR axis, int& pos, UINT WaitTM = 500);
    int ReadActualVel(UCHAR axis, int& vel, UINT WaitTM = 500);
    int ReadMaxSpeed(UCHAR axis, UINT& speed, UINT WaitTM = 500);
    int WriteMaxSpeed(UCHAR axis, UINT speed, UINT WaitTM = 500);
    int ReadAcceleration(UCHAR axis, UINT& acc, UINT WaitTM = 500);
    int WriteAcceleration(UCHAR axis, UINT acc, UINT WaitTM = 500);
    int ReadMaxCurrent(UCHAR axis, UCHAR& current, UINT WaitTM = 500);
    int WriteMaxCurrent(UCHAR axis, UCHAR current, UINT WaitTM = 500);
    int ReadMicroStep(UCHAR axis, UINT& microstep, UINT WaitTM = 500);
    int WriteMicroStep(UCHAR axis, UINT microstep, UINT WaitTM = 500);
	int ReadMotorRunning(UCHAR axis,BOOL& bRunning,UINT WaitTM = 500);
    int ReadHomeState(UCHAR axis, BOOL& bHome, UINT WaitTM = 500);

    // 运动控制
    int MoveAbsolute(UCHAR axis, int pos, UINT WaitTM = 0);
    int MoveRelative(UCHAR axis, int step, UINT WaitTM = 0);
    int MoveVelocity(UCHAR axis, int velocity, UINT WaitTM = 0);
    int StopMotor(UCHAR axis, UINT WaitTM = 0);
    int StopAll(UINT WaitTM = 0);
    int EmergencyStop(UCHAR axis, UINT WaitTM = 0);

    // 回零
    int HomeStart(UCHAR axis, UINT WaitTM = 0);
    int HomeSoft(UCHAR axis, UINT WaitTM = 0);

    // IO
    int ReadDigitalInput(UCHAR ch, BOOL& bHigh, UINT WaitTM = 500);
    int WriteDigitalOutput(UCHAR ch, BOOL bHigh, UINT WaitTM = 500);
    int ReadAnalogInput(UCHAR ch, UINT& value, UINT WaitTM = 500);

    // 多轴同步
    int SetSyncTargetPos(UCHAR axis, int pos, UINT WaitTM = 0);
    int StartSyncMove(UINT WaitTM = 0);

    // ========== 异步接口 ==========
    int AsyncMoveAbs(UCHAR axis, int pos);
    int AsyncMoveRel(UCHAR axis, int step);
    int AsyncMoveVel(UCHAR axis, int velocity);
    int AsyncStop(UCHAR axis);
    int AsyncHome(UCHAR axis);
    int AsyncReadPos(UCHAR axis);

    // ========== CAN 接收回调：上层收到此卡的响应帧时调用 ==========
    BOOL OnCANRecv(const TMCL_CAN_Frame& frame);
    BOOL OnCANRecv(const UCHAR* data, int len);

    // ========== 状态轮询（当没有主动回调机制时使用） ==========
    void PollStatus();

    // ========== 缓存读取 ==========
    int GetCachedPos(UCHAR axis) const   { return (axis < TMCM3216_MAX_AXIS) ? m_CachedPos[axis] : 0; }
    int GetCachedVel(UCHAR axis) const   { return (axis < TMCM3216_MAX_AXIS) ? m_CachedVel[axis] : 0; }
    int GetCachedTarget(UCHAR axis) const { return (axis < TMCM3216_MAX_AXIS) ? m_CachedTarget[axis] : 0; }
    TMCM3216_MOTOR_STATE GetState(UCHAR axis) const {
        return (axis < TMCM3216_MAX_AXIS) ? m_State[axis] : MS_ERROR;
    }
    UINT GetLastError() const { return m_LastError; }

    // ========== 调试 ==========
    void SetDebugMode(BOOL bDebug) { m_bDebug = bDebug; }

protected:
    // ========== 底层打包 + 发送 + 等待 ==========
    virtual int SendAndWait(TMCL_CAN_Frame& frame, UINT WaitTM);

    // 真实 CAN 发送函数（由子类实现对接硬件，默认空实现）
    virtual int DoCANSend(UINT canID, const UCHAR* data, int len);
    // 真实 CAN 接收等待（同步接口内部使用）
    virtual int DoCANWait(UINT canID, UCHAR* outData, int& outLen, UINT WaitTM);

    // 处理接收到的解析结果（同步/异步公用)
    void HandleRxResult(const TMCL_RxResult& res);
	void NotifyAxisFinished(UCHAR axis, UCHAR flag, int value = 0);
    // ========== 成员变量 ==========
    UINT                m_CardID;
    UINT                m_CanID;
    UINT                m_FirmwareVer;
    UINT                m_LastError;
    BOOL                m_bOnline;
    BOOL                m_bDebug;
    void*               m_pMotors[TMCM3216_MAX_AXIS];  // 子电机对象（void* 以避免循环依赖）

    int                 m_CachedPos[TMCM3216_MAX_AXIS];
    int                 m_CachedVel[TMCM3216_MAX_AXIS];
    int                 m_CachedTarget[TMCM3216_MAX_AXIS];
    TMCM3216_MOTOR_STATE m_State[TMCM3216_MAX_AXIS];
    UCHAR               m_CommandSN;
    CRITICAL_SECTION    m_csLock;
};
