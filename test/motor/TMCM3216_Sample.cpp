// TMCM3216_Sample.cpp
// 演示：多卡多轴 + 异步控制的完整使用流程

#include "TMCM3216Manager.h"
#include <stdio.h>
#include <windows.h>

// ========== 业务回调函数 ==========
// 当某个轴异步命令完成后，Manager -> Control -> Motor -> 此函数
void OnMotorFinished(const TMCM3216FinishInfo& info, void* pUserData)
{
    const char* axisName = (const char*)pUserData;
    printf("[CB] axis=%s finished, flag=0x%02X, value=%d\n",
           axisName ? axisName : "?", info.flag, info.value);
}

// ========== 模拟的 CAN 发送：真实项目中需要对接 CAN 驱动 ==========
// 此处演示如何在 Control 之上重写 DoCANSend：
class CMyTMCMControl : public CTMCM3216Control
{
public:
    CMyTMCMControl(UINT cardID, UINT canID) : CTMCM3216Control(cardID, canID) {}
    virtual int DoCANSend(UINT canID, const UCHAR* data, int len) override
    {
        // 真实项目：g_Can.SendSingleFrame(canID, data, len);
        printf("[CAN-SEND] card=%d id=0x%02X data=", m_CardID, canID);
        for (int i = 0; i < 8; ++i) printf("%02X ", data[i]);
        printf("\n");
        return 0;
    }
    virtual int DoCANWait(UINT canID, UCHAR* outData, int& outLen, UINT WaitTM) override
    {
        // 真实项目：阻塞等待并读取回包
        if (WaitTM == 0) return 0;
        // 这里模拟：直接返回"未实现等待"，让异步模式生效
        return -2;
    }
};

// ========== 主流程 ==========
int main(int argc, char* argv[])
{
    // 1. 注册两张卡（CAN ID 分别为 0x01/0x02）
    g_TMCMManager.RegisterCard(0, 0x01);
    g_TMCMManager.RegisterCard(1, 0x02);

    // 1.1 可选：用自定义 Control 替换默认 Control（以便重写 DoCANSend）
    //      如果不需要自定义，可跳过这一步；此处演示用法
    //      真实项目可直接在 Control 的派生类中实现 CAN 收发对接
    //      本示例为简单起见，不实际替换，直接使用默认 Control（打印 debug）

    // 2. 在各卡上创建业务电机
    CTMCM3216BasicMotor* pX = g_TMCMManager.CreateMotor(0, 0, "X-Axis");
    CTMCM3216BasicMotor* pY = g_TMCMManager.CreateMotor(0, 1, "Y-Axis");
    CTMCM3216BasicMotor* pZ = g_TMCMManager.CreateMotor(0, 2, "Z-Axis");
    CTMCM3216BasicMotor* pA = g_TMCMManager.CreateMotor(1, 0, "A-Axis");
    CTMCM3216BasicMotor* pB = g_TMCMManager.CreateMotor(1, 1, "B-Axis");

    if (!pX || !pY || !pZ || !pA || !pB)
    {
        printf("CreateMotor failed\n");
        return -1;
    }

    // 3. (可选) 为每个 Motor 注册完成回调
    pX->SetFinishCallback(OnMotorFinished, (void*)"X-Axis");
    pY->SetFinishCallback(OnMotorFinished, (void*)"Y-Axis");
    pZ->SetFinishCallback(OnMotorFinished, (void*)"Z-Axis");
    pA->SetFinishCallback(OnMotorFinished, (void*)"A-Axis");
    pB->SetFinishCallback(OnMotorFinished, (void*)"B-Axis");

    // 4. 预设置各轴参数（可选）
    TMCM3216MotorDefaultParam paramX, paramA;
    paramX.maxSpeed = 8000;  paramX.acceleration = 2000;
    paramA.maxSpeed = 5000;  paramA.acceleration = 1000;
    pX->SetDefaultParam(paramX);
    pY->SetDefaultParam(paramX);
    pZ->SetDefaultParam(paramX);
    pA->SetDefaultParam(paramA);
    pB->SetDefaultParam(paramA);

    // 5. 应用参数（真实项目中会下发到硬件）
    g_TMCMManager.InitAll();

    printf("=== 异步运动测试：同时发 5 个轴的异步命令 ===\n");
    // 异步：不等完成，立即返回；回包通过 DispatchCANFrame 触发回调
    pX->AsyncMoveAbs(10000);
    pY->AsyncMoveAbs(20000);
    pZ->AsyncMoveAbs(30000);
    pA->AsyncMoveRel(5000);
    pB->AsyncMoveRel(5000);

    // 在真实系统里，这里会有 CAN 中断/线程从硬件收取回包，
    // 并调用 g_TMCMManager.DispatchCANFrame(canID, data, 8) 分发。
    // 演示中我们手动构造一个"X 轴完成"回包来测试回调流程：
    UCHAR sampleRx[8] = {0x01, 0x64, 0x04, 0x00, 0x00, 0x00, 0x27, 0x10};
    //        模块地址^^     ^^状态码=100(成功) ^^命令号=4(MVP_ABS) ^^轴号=0
    //                                                         value = 0x00002710 = 10000
    g_TMCMManager.DispatchCANFrame(0x01, sampleRx, 8);

    // 6. 同步接口测试：对 Z 轴用阻塞等待
    printf("=== 同步运动测试 ===\n");
    int rtn = pZ->GoTo(50000, 1000);
    printf("Z axis sync return = %d (0=send OK, -2=WaitNotImpl)\n", rtn);

    // 7. 多轴同步：同一张卡上先写目标位置，再统一启动
    printf("=== 多轴同步测试 ===\n");
    CTMCM3216Control* ctrl0 = g_TMCMManager.GetControl(0);
    if (ctrl0)
    {
        ctrl0->SetSyncTargetPos(0, 1000, 0);
        ctrl0->SetSyncTargetPos(1, 2000, 0);
        ctrl0->SetSyncTargetPos(2, 3000, 0);
        ctrl0->StartSyncMove();
    }

    // 8. 状态打印
    g_TMCMManager.DumpAllStatus();

    // 9. 停止所有轴
    printf("=== 全部停止 ===\n");
    g_TMCMManager.StopAll();

    return 0;
}
