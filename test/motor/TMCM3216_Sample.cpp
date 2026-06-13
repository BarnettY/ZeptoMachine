// TMCM3216_Sample.cpp - C++98 兼容版
// 完整用法示例：多卡多轴 + 同步 / 异步控制

#include "TMCM3216Manager.h"
//#include <stdio.h>
//#include <windows.h>

// ============ 完成回调示例
void OnMotorFinished(const TMCM3216FinishInfo & info, void* pUserData)
{
    const char* axisName = (const char*)pUserData;
    printf("[CB] axis=%s finished, flag=0x%02X, value=%d\n",
           axisName ? axisName : "?", info.flag, info.value);
}

int main(int argc, char* argv[])
{
    printf("TMCM-3216 Multi-Card Multi-Axis Sample\n");

    // ========== 步骤 1：注册卡片（逻辑卡号 + CAN ID） ==========
    CTMCM3216Manager& mgr = CTMCM3216Manager::Instance();

    mgr.RegisterCard(0, 0x01);  // 卡片0: CAN ID = 1
    mgr.RegisterCard(1, 0x02);  // 卡片1: CAN ID = 2

    // ========== 步骤 2：创建电机对象 ========== ==========
    CTMCM3216BasicMotor* pX = mgr.CreateMotor(0, 0, "X-Axis");
    CTMCM3216BasicMotor* pY = mgr.CreateMotor(0, 1, "Y-Axis");
    CTMCM3216BasicMotor* pZ = mgr.CreateMotor(0, 2, "Z-Axis");
    CTMCM3216BasicMotor* pA = mgr.CreateMotor(1, 0, "A-Axis");
    CTMCM3216BasicMotor* pB = mgr.CreateMotor(1, 1, "B-Axis");

    if (!pX || !pY || !pZ || !pA || !pB) {
        printf("Failed to create motors\n");
        return -1;
    }

    // ========== 步骤 3：设置默认参数（可选） ==========
    TMCM3216MotorDefaultParam paramHigh;
    paramHigh.maxSpeed = 8000;
    paramHigh.acceleration = 2000;

    TMCM3216MotorDefaultParam paramLow;
    paramLow.maxSpeed = 5000;
    paramLow.acceleration = 1000;

    pX->SetDefaultParam(paramHigh);
    pY->SetDefaultParam(paramHigh);
    pZ->SetDefaultParam(paramHigh);
    pA->SetDefaultParam(paramLow);
    pB->SetDefaultParam(paramLow);

    // ========== 步骤 4：注册异步回调（可选） ==========
    pX->SetFinishCallback(OnMotorFinished, (void*)"X-Axis");
    pY->SetFinishCallback(OnMotorFinished, (void*)"Y-Axis");
    pA->SetFinishCallback(OnMotorFinished, (void*)"A-Axis");

    // ========== 步骤 5：初始化（应用默认参数） ==========
    printf("Initializing all motors...\n");
    mgr.InitAll();

    // ========== 步骤 6：同步运动示例 ==========
    printf("\n--- Sync motion sample --- \n");
    int rtn;
    rtn = pX->MoveAbsolute(10000, 1000);  // 同步等待 1 秒
    printf("X-Axis MoveAbsolute return=%d\n", rtn);
    // WaitTM > 0 会调用底层 DoCANWait（默认空实现，真实环境下需要对接）

    // ========== 步骤 7：异步运动示例 ==========
    printf("\n--- Async motion sample ---\n");
    pX->AsyncMoveAbs(20000);
    pY->AsyncMoveRel(5000);
    pA->AsyncMoveAbs(-8000);

    // ========== 步骤 8：模拟收到 CAN 回包（真实环境由 CAN 层触发）==========
    printf("\n--- Simulating CAN reply dispatch ---\n");

    // 构造"X-Axis完成"回包：模块地址0x01，状态0x64(100)，value=20000
    UCHAR reply1[8];
    memset(reply1, 0, 8);
    reply1[0] = 0x01;           // 模块地址 = 卡片0的CAN ID
    reply1[1] = 0x64;              // 状态码 = 成功
    reply1[2] = 0x00;              // 轴号 0
    reply1[4] = 0x00;              // value = 20000 (0x4E20)
    reply1[5] = 0x00;
    reply1[6] = 0x4E;
    reply1[7] = 0x20;
    mgr.DispatchCANFrame(0x01, reply1, 8);

    // ========== 步骤 9：多轴同步（同卡）==========
    printf("\n--- Multi-axis sync sample ---\n");
    mgr.SetSyncTarget(0, 0, 1000, 0);  // 先为卡片0的轴0写目标1000
    mgr.SetSyncTarget(0, 1, 2000, 0);  // 先为卡片0的轴1写目标2000
    mgr.SetSyncTarget(0, 2, 3000, 0);  // 先为卡片0的轴2写目标3000
    mgr.StartSyncMove(0, 0);               // 统一启动（异步）

    // ========== 步骤 10：状态显示 ==========
    printf("\n--- Status dump ---\n");
    mgr.DumpAllStatus();

    // ========== 步骤 11：状态轮询（无回调机制时使用）==========
    // mgr.PollAll();

    // ========== 步骤 12：全部停止 ==========
    printf("\n--- Stop all ---\n");
    mgr.StopAll();

    printf("\nSample finished. Cards registered: %d\n", mgr.GetCardCount());
    return 0;
}
