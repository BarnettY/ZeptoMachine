// TMCM3216Manager.h - C++98 兼容版
// 多卡多轴管理器：统一注册、创建电机、分发 CAN 回包
#pragma once

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../can/TMCM3216Control.h"
#include "TMCM3216BasicMotor.h"
#include <wx/hashmap.h>

struct TMCM3216CardSlot
{
    UINT cardID;
    UINT canID;
    CTMCM3216Control*    pControl;
    CTMCM3216BasicMotor* pMotors[TMCM3216_MAX_AXIS];

    TMCM3216CardSlot() : cardID(0), canID(0), pControl(NULL) {
        for (int i = 0; i < TMCM3216_MAX_AXIS; ++i) pMotors[i] = NULL;
    }
};

class CTMCM3216Manager
{
public:
    static CTMCM3216Manager& Instance();

    CTMCM3216Manager();
    ~CTMCM3216Manager();

    // ========== 卡片注册 / 注销 ==========
    BOOL RegisterCard(UINT cardID, UINT canID);
    void UnregisterCard(UINT cardID);
    void UnregisterAll();

    // ========== 电机创建 ==========
    CTMCM3216BasicMotor* CreateMotor(UINT cardID, UCHAR axis, const char* name = NULL);

    // ========== 获取 Control / Motor ==========
    CTMCM3216Control*    GetControl(UINT cardID);
    CTMCM3216BasicMotor* GetMotor(UINT cardID, UCHAR axis);

    // ========== 统一初始化 ==========
    int InitAll();
    int StopAll();

    // ========== 便捷运动接口 ==========
    int GoTo(UINT cardID, UCHAR axis, int pos, UINT WaitTM = 0);
    int MoveRel(UINT cardID, UCHAR axis, int step, UINT WaitTM = 0);
    int Jog(UINT cardID, UCHAR axis, int velocity, UINT WaitTM = 0);
    int StopAxis(UINT cardID, UCHAR axis, UINT WaitTM = 0);
    int Home(UINT cardID, UCHAR axis, UINT WaitTM = 0);

    // ========== 多轴同步（同卡） ==========
    int SetSyncTarget(UINT cardID, UCHAR axis, int pos, UINT WaitTM = 0);
    int StartSyncMove(UINT cardID, UINT WaitTM = 0);

    // ========== CAN 回包分发 ==========
    // 上层 CAN 层收到帧后调用：按 canID 匹配卡片再交给其 Control.OnCANRecv
    BOOL DispatchCANFrame(UINT canID, const UCHAR* data, int len);
    BOOL DispatchCANFrame(const TMCL_CAN_Frame& frame);

    // ========== 轮询 ==========
    void PollAll();
    void DumpAllStatus();
    int GetCardCount() const { return (int)m_Cards.size(); }
private: 
	wxHashMap<UINT, TMCM3216CardSlot*> m_Cards;
    CRITICAL_SECTION m_csLock;
};
