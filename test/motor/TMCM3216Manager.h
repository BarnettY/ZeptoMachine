// TMCM3216Manager.h
// 多卡多轴管理器：负责管理多张 TMCM-3216 卡（每张卡最多 3 个轴），
// 提供统一的初始化、枚举、以及 CAN 帧统一分发接口
// 用法：
//   g_TMCMManager.RegisterCard(0, 0x01);  // 注册卡号0，CAN ID=1
//   g_TMCMManager.CreateMotor<CTMCM3216BasicMotor>(0, 0, "X-Axis");
//   g_TMCMManager.InitAll();
//   g_TMCMManager.GoTo(0, 0, 50000);      // 卡0, 轴0, 走到50000
//   g_TMCMManager.OnCANRecv(frame);       // 收到 CAN 回包时统一分发
//
// 异步控制：
//   每张卡都可以独立地发送异步命令，回包通过 OnCANRecv 分发到对应卡的 OnCANRecv，
//   再由 Control 对象分发给对应轴 Motor 的 OnCommandFinished。
//   业务层可通过每个 Motor 的 SetFinishCallback 注册回调。

#pragma once

#include "TMCM3216Control.h"
#include "TMCM3216BasicMotor.h"
#include <map>
#include <vector>

// 每张卡的槽位：Control + 最多 TMCM3216_MAX_AXIS 个 Motor
struct TMCMCardSlot
{
    UINT cardID;
    UINT canID;
    CTMCM3216Control* pControl;
    CTMCM3216BasicMotor* pMotors[TMCM3216_MAX_AXIS];

    TMCMCardSlot() : cardID(0), canID(0), pControl(NULL)
    {
        for (int i = 0; i < TMCM3216_MAX_AXIS; ++i) pMotors[i] = NULL;
    }
};

class CTMCM3216Manager
{
public:
    static CTMCM3216Manager& Instance();

    CTMCM3216Manager();
    ~CTMCM3216Manager();

    // ====== 注册/创建 ======
    // 注册一张卡（cardID 是逻辑编号，canID 是 CAN 总线模块地址）
    BOOL RegisterCard(UINT cardID, UINT canID);
    // 注销并释放
    void UnregisterCard(UINT cardID);
    void UnregisterAll();

    // 在已注册的卡上创建一个业务电机对象
    CTMCM3216BasicMotor* CreateMotor(UINT cardID, UCHAR axis, const char* name = NULL);

    // 获取指定卡/轴
    CTMCM3216Control*    GetControl(UINT cardID);
    CTMCM3216BasicMotor* GetMotor(UINT cardID, UCHAR axis);

    // ====== 初始化 / 反初始化 ======
    // 对所有已注册卡应用默认参数（可在 Motor 上预先设置好 DefaultParam）
    int InitAll();
    // 所有轴紧急停止
    int StopAll();

    // ====== 便捷接口（同步）======
    int GoTo(UINT cardID, UCHAR axis, int pos, UINT WaitTM = 0);
    int MoveRel(UINT cardID, UCHAR axis, int step, UINT WaitTM = 0);
    int Jog(UINT cardID, UCHAR axis, int velocity, UINT WaitTM = 0);
    int StopAxis(UINT cardID, UCHAR axis, UINT WaitTM = 0);
    int Home(UINT cardID, UCHAR axis, UINT WaitTM = 0);

    // 多轴同步：在同一张卡上同时启动多个轴（需先各自设置目标位置）
    int SyncStart(UINT cardID);

    // ====== CAN 回包分发 ======
    // 上层 CAN 组件收到回包后调用此函数，Manager 会分发给对应 Card 的 Control
    BOOL DispatchCANFrame(UINT canID, const UCHAR* data, int len);
    BOOL DispatchCANFrame(const TMCL_CAN_Frame& frame);

    // 轮询所有卡状态（可选，当没有硬件事件通知时用）
    void PollAll();

    // ====== 调试 ======
    void DumpAllStatus();

    // 获取卡数
    int  GetCardCount() const { return (int)m_Cards.size(); }

private:
    std::map<UINT, TMCMCardSlot*> m_Cards;   // cardID -> slot
    CRITICAL_SECTION               m_csLock;
};

// 全局单例便捷宏
#define g_TMCMManager  CTMCM3216Manager::Instance()
