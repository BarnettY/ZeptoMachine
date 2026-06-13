// TMCM3216Motor.h
// 参考 CCanMotor：单轴抽象层，把 Control 的 axis 相关命令封装为面向对象的接口
// 负责：该轴的参数/运动/状态读写封装，回调接口（用于上层订阅完成事件）

#pragma once

#include "TMCM3216Control.h"

class CTMCM3216BasicMotor;

// 完成回调的参数结构体
struct TMCM3216FinishInfo
{
    UCHAR axis;           // 轴号 (0..2)
    UCHAR flag;           // 完成标志（见 TMCM3216_FLAG_xxx）
    int   value;          // 回包中的 value（如当前位置/错误码）
    UINT  status;         // 回包状态码
};

class CTMCM3216Motor
{
public:
    CTMCM3216Motor(UINT cardID, UCHAR axisID);
    virtual ~CTMCM3216Motor();

    // 绑定 Control（一张卡上的多个轴共享一个 Control）
    void  AttachControl(CTMCM3216Control* pCtrl);
    CTMCM3216Control* GetControl() { return m_pCtrl; }

    UINT  GetCardID()   const { return m_CardID; }
    UCHAR GetAxisID()   const { return m_AxisID; }

    // ====== 参数设置（同步）======
    virtual int SetMaxSpeed(UINT speed);
    virtual int SetAcceleration(UINT acc);
    virtual int SetMaxCurrent(UCHAR current);
    virtual int SetMicroStep(UINT microstep);

    // ====== 参数读取（同步）======
    virtual int GetActualPos(int& pos);
    virtual int GetActualVel(int& vel);
    virtual int GetTargetPos(int& pos);
    virtual int GetMaxSpeed(UINT& speed);
    virtual int GetAcceleration(UINT& acc);

    // ====== 运动控制（同步，WaitTM > 0 为阻塞等待）======
    virtual int MoveAbsolute(int pos, UINT WaitTM = 0);
    virtual int MoveRelative(int step, UINT WaitTM = 0);
    virtual int MoveVelocity(int velocity, UINT WaitTM = 0);
    virtual int Stop(UINT WaitTM = 0);
    virtual int EmergencyStop(UINT WaitTM = 0);

    // ====== 回零 ======
    virtual int Home(UINT WaitTM = 0);
    virtual int HomeSoft(UINT WaitTM = 0);

    // ====== 异步接口 ======
    virtual int AsyncMoveAbs(int pos);
    virtual int AsyncMoveRel(int step);
    virtual int AsyncMoveVel(int velocity);
    virtual int AsyncStop();
    virtual int AsyncHome();

    // ====== 状态查询 ======
    BOOL IsMoving();
    BOOL IsIdle();
    BOOL IsError();
    TMCM3216_MOTOR_STATE GetState() const { return (m_pCtrl) ? m_pCtrl->GetState(m_AxisID) : MS_ERROR; }
    int  GetCachedPos()   const { return (m_pCtrl) ? m_pCtrl->GetCachedPos(m_AxisID) : 0; }
    int  GetCachedVel()   const { return (m_pCtrl) ? m_pCtrl->GetCachedVel(m_AxisID) : 0; }

    // ====== 回调 ======
    // Control 收到该轴完成回包后会调用此函数；子类可重写实现自定义逻辑
    virtual void OnCommandFinished(UCHAR flag, int value);
    // 提供给上层业务订阅的回调接口（可选，默认空）
    typedef void (*PFN_FINISH_CALLBACK)(const TMCM3216FinishInfo& info, void* pUserData);
    void SetFinishCallback(PFN_FINISH_CALLBACK cb, void* pUser) { m_pFinishCB = cb; m_pCBUserData = pUser; }

protected:
    UINT                m_CardID;
    UCHAR               m_AxisID;
    CTMCM3216Control*   m_pCtrl;

    PFN_FINISH_CALLBACK m_pFinishCB;
    void*               m_pCBUserData;

    // 回调数据缓存
    TMCM3216FinishInfo  m_LastFinish;
};
