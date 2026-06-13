// TMCM3216BasicMotor.h - C++98 兼容版
// 业务电机对象：带预设参数、软件限位、动作日志
#pragma once
#include "TMCM3216Motor.h"

struct TMCM3216MotorDefaultParam
{
    UINT  maxSpeed;
    UINT  acceleration;
    UCHAR maxCurrent;
    UINT  microStep;
    int   homeOffset;
    int   softLimitPos;
    int   softLimitNeg;

    TMCM3216MotorDefaultParam()
        : maxSpeed(5000), acceleration(1000), maxCurrent(100),
          microStep(256), homeOffset(0),
          softLimitPos(1000000), softLimitNeg(-1000000) {}
};

class CTMCM3216BasicMotor : public CTMCM3216Motor
{
public:
    CTMCM3216BasicMotor(UINT cardID, UCHAR axisID, const char* name = "TMCM-Motor");
    virtual ~CTMCM3216BasicMotor();

    const char* GetName() const { return m_strName; }
    void        SetName(const char* name);

    void SetDefaultParam(const TMCM3216MotorDefaultParam& param) { m_DefaultParam = param; }
    const TMCM3216MotorDefaultParam& GetDefaultParam() const { return m_DefaultParam; }

    // 应用默认参数到硬件（初始化阶段调用）
    virtual int ApplyDefaultParam();

    // ========== 业务动作 ==========
    virtual int GoHome(UINT WaitTM = 0);
    virtual int GoTo(int pos, UINT WaitTM = 0);
    virtual int Step(int step, UINT WaitTM = 0);
    virtual int Jog(int velocity, UINT WaitTM = 0);
    virtual int Halt();

    // ========== 调试 ==========
    void DumpStatus();

    // ========== 重载回调 ==========
    virtual void OnCommandFinished(UCHAR flag, int value);

protected:
    char                      m_strName[64];
    TMCM3216MotorDefaultParam m_DefaultParam;

    // 辅助：软件限位检查
    BOOL CheckSoftLimit(int& targetPos);
};
