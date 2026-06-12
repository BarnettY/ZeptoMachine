#pragma once
// TMCM3216BasicMotor.h   替换BasicMotor
// 参考 CBasicMotor：面向业务的电机对象，带有预设参数（速度/加速度/细分数等）、
// 业务动作（例：定位到某个工位、往返运动）、以及调试接口
#include "TMCM3216Motor.h"
// 业务动作预设参数
struct TMCM3216MotorDefaultParam
{
    UINT  maxSpeed;       // 最大速度
    UINT  acceleration;   // 加速度
    UCHAR maxCurrent;     // 最大电流 (0~255)
    UINT  microStep;      // 细分数
    int   homeOffset;     // 回零偏移
    int   softLimitPos;   // 软件正向限位
    int   softLimitNeg;   // 软件负向限位
    TMCM3216MotorDefaultParam()
        : maxSpeed(5000), acceleration(1000), maxCurrent(100),
          microStep(256), homeOffset(0), softLimitPos(1000000), softLimitNeg(-1000000) {}
};

class CTMCM3216BasicMotor : public CTMCM3216Motor
{
public:
    CTMCM3216BasicMotor(UINT cardID, UCHAR axisID, const char* name = "TMCM-Motor");
    virtual ~CTMCM3216BasicMotor();
    // ====== 名称与调试 ======
    const char* GetName() const { return m_strName; }
    void        SetName(const char* name);
    // ====== 参数 ======
    void SetDefaultParam(const TMCM3216MotorDefaultParam& param) { m_DefaultParam = param; }
    const TMCM3216MotorDefaultParam& GetDefaultParam() const { return m_DefaultParam; }
    // 应用默认参数到硬件（初始化阶段调用）
    virtual int ApplyDefaultParam();
    // ====== 业务动作 ======
    // 回到软零点
    virtual int GoHome(UINT WaitTM = 0);
    // 走到指定绝对位置（带软件限位检查）
    virtual int GoTo(int pos, UINT WaitTM = 0);
    // 按当前速度前进/后退 step 步
    virtual int Step(int step, UINT WaitTM = 0);
    // JOG：指定速度连续运动（速度模式）
    virtual int Jog(int velocity, UINT WaitTM = 0);
    virtual int Halt();   // 暂停/停止
    // ====== 调试接口 ======
    void DumpStatus();
    // ====== 重载回调：可在此记录日志/触发上层事件 ======
    virtual void OnCommandFinished(UCHAR flag, int value) override;

protected:
    char                        m_strName[64];
    TMCM3216MotorDefaultParam   m_DefaultParam;
    BOOL CheckSoftLimit(int& targetPos);    // 辅助：软件限位检查
};
