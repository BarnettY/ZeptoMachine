// TMCM3216Control.h
// 参考 CCanControl 的架构，实现 TMCM-3216 单卡（支持3轴）的控制层
// 负责：CAN 协议打包、CAN 收发（SendAndWait/异步回调）、轴状态缓存、
//      分配子电机对象、全局参数管理（固件、地址、错误码等）

#pragma once

#include "TMCM-3216.h"
#include <windows.h>

class CTMCM3216Motor;
class CTMCM3216BasicMotor;

// TMCM-3216 每卡支持的最大轴数
#define TMCM3216_MAX_AXIS        3

// 异步回调状态标志
#define TMCM3216_FLAG_FINISH     0x01   // 运动完成
#define TMCM3216_FLAG_STOPPED    0x02   // 停止
#define TMCM3216_FLAG_LIMIT_POS  0x04   // 正向限位
#define TMCM3216_FLAG_LIMIT_NEG  0x08   // 负向限位
#define TMCM3216_FLAG_REF_FOUND  0x10   // 回零完成
#define TMCM3216_FLAG_STALL      0x20   // 失速检测
#define TMCM3216_FLAG_ERROR      0x80   // 错误

// 电机运行状态
enum TMCM3216_MOTOR_STATE
{
    MS_IDLE = 0,           // 空闲
    MS_MOVING,             // 正在运动
    MS_RUN_VEL,            // 速度模式
    MS_HOMING,             // 回零中
    MS_STOPPING,           // 停止中
    MS_ERROR               // 错误
};

// ========== CTMCM3216Control ==========
// 对应 CCanControl：一张 TMCM-3216 卡 = 一个 Control，管理最多 3 个轴电机
class CTMCM3216Control
{
public:
    CTMCM3216Control(UINT cardID, UINT canID = 1);
    virtual ~CTMCM3216Control();

    // ====== 子电机管理 ======
    void SetChildMotor(UCHAR axisIndex, CTMCM3216Motor* pMotor);
    CTMCM3216Motor* GetChildMotor(UCHAR axisIndex);

    // ====== 设备信息 ======
    UINT GetCardID()        const { return m_CardID; }
    UINT GetCANID()         const { return m_CanID; }
    UINT GetFirmwareVer()   const { return m_FirmwareVer; }
    BOOL IsCardOnline()     const { return m_bOnline; }

    // ====== 基础同步命令（调用方必须保证 CAN 发送已实现）======
    // 固件版本
    int ReadFirmware(UINT WaitTM = 500);
    // 全局错误码
    int ReadGlobalError(UINT WaitTM = 500);
    // 模块地址
    int ReadModuleAddress(UINT WaitTM = 500);
    int WriteModuleAddress(UCHAR newAddr, UINT WaitTM = 500);
    // 模块复位
    int ResetModule(UINT WaitTM = 1000);

    // ====== 轴相关：同步 ======
    // 当前位置 / 速度
    int ReadActualPos(UCHAR axis, int& pos, UINT WaitTM = 500);
    int ReadActualVel(UCHAR axis, int& vel, UINT WaitTM = 500);
    int ReadTargetPos(UCHAR axis, int& pos, UINT WaitTM = 500);

    // 轴参数：最大速度 / 加速度 / 电流 / 细分数
    int ReadMaxSpeed(UCHAR axis, UINT& speed, UINT WaitTM = 500);
    int WriteMaxSpeed(UCHAR axis, UINT speed, UINT WaitTM = 500);
    int ReadAcceleration(UCHAR axis, UINT& acc, UINT WaitTM = 500);
    int WriteAcceleration(UCHAR axis, UINT acc, UINT WaitTM = 500);
    int ReadMaxCurrent(UCHAR axis, UCHAR& current, UINT WaitTM = 500);
    int WriteMaxCurrent(UCHAR axis, UCHAR current, UINT WaitTM = 500);
    int ReadMicroStep(UCHAR axis, UINT& microstep, UINT WaitTM = 500);
    int WriteMicroStep(UCHAR axis, UINT microstep, UINT WaitTM = 500);

    // 电机运行状态（是否在运动）
    int ReadMotorRunning(UCHAR axis, BOOL& bRunning, UINT WaitTM = 500);
    int ReadHomeState(UCHAR axis, BOOL& bHome, UINT WaitTM = 500);

    // ====== 运动控制：同步 ======
    int MoveAbsolute(UCHAR axis, int pos, UINT WaitTM = 0);   // WaitTM=0 仅发命令不等待
    int MoveRelative(UCHAR axis, int step, UINT WaitTM = 0);
    int MoveVelocity(UCHAR axis, int velocity, UINT WaitTM = 0);
    int StopMotor(UCHAR axis, UINT WaitTM = 0);
    int StopAll(UINT WaitTM = 0);
    int EmergencyStop(UCHAR axis, UINT WaitTM = 0);

    // 回零（参考搜索）
    int HomeStart(UCHAR axis, UINT WaitTM = 0);
    int HomeSoft(UCHAR axis, UINT WaitTM = 0);

    // ====== IO：同步 ======
    int ReadDigitalInput(UCHAR ch, BOOL& bHigh, UINT WaitTM = 500);
    int WriteDigitalOutput(UCHAR ch, BOOL bHigh, UINT WaitTM = 500);
    int ReadAnalogInput(UCHAR ch, UINT& value, UINT WaitTM = 500);

    // ====== 多轴同步运动 ======
    // 先用 SetSyncTargetPos 准备各轴目标位置，再调用 StartSyncMove 统一启动
    int SetSyncTargetPos(UCHAR axis, int pos, UINT WaitTM = 0);
    int StartSyncMove(UINT WaitTM = 0);

    // ====== 异步接口：发送命令后立即返回，通过回调通知 ======
    // 异步读取位置
    int AsyncReadPos(UCHAR axis);
    // 异步运动（发送后立即返回，完成后通过 NotifyAxisFinished 回调）
    int AsyncMoveAbs(UCHAR axis, int pos);
    int AsyncMoveRel(UCHAR axis, int step);
    int AsyncMoveVel(UCHAR axis, int velocity);
    int AsyncStop(UCHAR axis);
    int AsyncHome(UCHAR axis);

    // ====== CAN 接收回调：上层 CAN 层收到该卡的响应帧后调用 ======
    // 返回 TRUE 表示此帧已处理
    BOOL OnCANRecv(const TMCL_CAN_Frame& frame);
    BOOL OnCANRecv(const UCHAR* data, int len);

    // ====== 状态轮询（若上层没有主动回调，则定时调用此函数拉状态）======
    void PollStatus();

    // ====== 缓存读取 ======
    int  GetCachedPos(UCHAR axis)   const { return (axis < TMCM3216_MAX_AXIS) ? m_CachedPos[axis] : 0; }
    int  GetCachedVel(UCHAR axis)   const { return (axis < TMCM3216_MAX_AXIS) ? m_CachedVel[axis] : 0; }
    int  GetCachedTarget(UCHAR axis)const { return (axis < TMCM3216_MAX_AXIS) ? m_CachedTarget[axis] : 0; }
    TMCM3216_MOTOR_STATE GetState(UCHAR axis) const { return (axis < TMCM3216_MAX_AXIS) ? m_State[axis] : MS_ERROR; }
    UINT GetLastError() const { return m_LastError; }

    // ====== 调试 ======
    void SetDebugMode(BOOL bDebug) { m_bDebug = bDebug; }

protected:
    // ====== 底层：打包 + 发送 + 等待（同步接口的核心）======
    virtual int SendAndWait(TMCL_CAN_Frame& frame, UINT WaitTM);

    // 真实的 CAN 发送函数（需要上层/子类实现或连接到已有 CAN 组件）
    // 默认实现只做打包并返回 0，供无硬件环境调试
    virtual int DoCANSend(UINT canID, const UCHAR* data, int len);
    // 真实的 CAN 接收等待（同步接口内部使用）
    virtual int DoCANWait(UINT canID, UCHAR* outData, int& outLen, UINT WaitTM);

    // 处理接收到的解析结果（同步/异步公用）
    void HandleRxResult(const TMCL_RxResult& res);

    // 异步命令回调分发到对应轴
    void NotifyAxisFinished(UCHAR axis, UCHAR flag, int value = 0);

    // ====== 成员变量 ======
    UINT                m_CardID;           // 逻辑卡号（用于上层多卡区分）
    UINT                m_CanID;            // CAN 总线 ID
    UINT                m_FirmwareVer;      // 固件版本
    UINT                m_LastError;        // 最近一次错误码
    BOOL                m_bOnline;          // 设备是否在线
    BOOL                m_bDebug;           // 调试模式

    CTMCM3216Motor*    m_pMotors[TMCM3216_MAX_AXIS];  // 子电机对象
    // 状态缓存
    int                 m_CachedPos[TMCM3216_MAX_AXIS];
    int                 m_CachedVel[TMCM3216_MAX_AXIS];
    int                 m_CachedTarget[TMCM3216_MAX_AXIS];
    TMCM3216_MOTOR_STATE m_State[TMCM3216_MAX_AXIS];

    // 命令序号（用于异步匹配，简单递增即可）
    UCHAR               m_CommandSN;

    CRITICAL_SECTION    m_csLock;          // 并发锁（对应原工程的 AxisMute0）
};
