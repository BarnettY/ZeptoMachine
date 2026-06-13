// TMCM3216Motor.h - C++98 兼容版
// 单轴抽象层：把 Control 级别的命令封装为面向对象的接口
#pragma once
#include "../can/TMCM3216Control.h"
#include "../comon.h"

// 完成回调结构体
struct TMCM3216FinishInfo
{
    UCHAR axis;
    UCHAR flag;
    int   value;
    UINT  status;
};

// 回调函数类型
typedef void (*PFN_TMCM3216_FINISH)(const TMCM3216FinishInfo& info, void* pUserData);

class CTMCM3216Motor
{
public:
    CTMCM3216Motor(UINT cardID, UCHAR axisID);
    virtual ~CTMCM3216Motor();

    // 绑定 Control
    void AttachControl(CTMCM3216Control* pCtrl);
    CTMCM3216Control* GetControl() { return m_pCtrl; }

    UINT  GetCardID() const { return m_CardID; }
    UCHAR GetAxisID() const { return m_AxisID; }

    // ========== 参数设置 ==========
    virtual int SetMaxSpeed(UINT speed);
    virtual int SetAcceleration(UINT acc);
    virtual int SetMaxCurrent(UCHAR current);
    virtual int SetMicroStep(UINT microstep);

    // ========== 参数读取 ==========
    virtual int GetActualPos(int& pos);
    virtual int GetActualVel(int& vel);
    virtual int GetMaxSpeed(UINT& speed);
    virtual int GetAcceleration(UINT& acc);

    // ========== 运动控制 ==========
    virtual int MoveAbsolute(int pos, UINT WaitTM = 0);
    virtual int MoveRelative(int step, UINT WaitTM = 0);
    virtual int MoveVelocity(int velocity, UINT WaitTM = 0);
    virtual int Stop(UINT WaitTM = 0);
    virtual int EmergencyStop(UINT WaitTM = 0);

    // ========== 回零 ==========
    virtual int Home(UINT WaitTM = 0);
    virtual int HomeSoft(UINT WaitTM = 0);

    // ========== 异步接口 ==========
    virtual int AsyncMoveAbs(int pos);
    virtual int AsyncMoveRel(int step);
    virtual int AsyncMoveVel(int velocity);
    virtual int AsyncStop();
    virtual int AsyncHome();

    // ========== 状态查询 ==========
    BOOL IsMoving();
    BOOL IsIdle();
    BOOL IsError();

    int GetCachedPos() const { return m_pCtrl ? m_pCtrl->GetCachedPos(m_AxisID) : 0; }
    int GetCachedVel() const { return m_pCtrl ? m_pCtrl->GetCachedVel(m_AxisID) : 0; }
    TMCM3216_MOTOR_STATE GetState() const {
        return m_pCtrl ? m_pCtrl->GetState(m_AxisID) : MS_ERROR;
    }

    // ========== 回调 ==========
    // Control 收到该轴完成回包时会调用此函数；子类可重写实现自定义逻辑
    virtual void OnCommandFinished(UCHAR flag, int value);

    // 注册回调
    void SetFinishCallback(PFN_TMCM3216_FINISH cb, void* pUser) {
        m_pFinishCB = cb; m_pCBUserData = pUser;
    }

protected:
    UINT                 m_CardID;
    UCHAR                m_AxisID;
    CTMCM3216Control*    m_pCtrl;
    PFN_TMCM3216_FINISH  m_pFinishCB;
    void*                m_pCBUserData;
    TMCM3216FinishInfo   m_LastFinish;
};
