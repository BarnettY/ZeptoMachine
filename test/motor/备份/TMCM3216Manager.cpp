// TMCM3216Manager.cpp
// 多卡多轴管理器实现

#include "TMCM3216Manager.h"
#include <stdio.h>

CTMCM3216Manager& CTMCM3216Manager::Instance()
{
    static CTMCM3216Manager inst;
    return inst;
}

CTMCM3216Manager::CTMCM3216Manager()
{
    InitializeCriticalSection(&m_csLock);
}

CTMCM3216Manager::~CTMCM3216Manager()
{
    UnregisterAll();
    DeleteCriticalSection(&m_csLock);
}

BOOL CTMCM3216Manager::RegisterCard(UINT cardID, UINT canID)
{
    EnterCriticalSection(&m_csLock);
    if (m_Cards.find(cardID) != m_Cards.end())
    {
        LeaveCriticalSection(&m_csLock);
        return FALSE;
    }
    TMCMCardSlot* slot = new TMCMCardSlot;
    slot->cardID = cardID;
    slot->canID  = canID;
    slot->pControl = new CTMCM3216Control(cardID, canID);
    m_Cards[cardID] = slot;
    LeaveCriticalSection(&m_csLock);
    return TRUE;
}

void CTMCM3216Manager::UnregisterCard(UINT cardID)
{
    EnterCriticalSection(&m_csLock);
    auto it = m_Cards.find(cardID);
    if (it != m_Cards.end())
    {
        TMCMCardSlot* slot = it->second;
        if (slot)
        {
            for (int i = 0; i < TMCM3216_MAX_AXIS; ++i)
            {
                delete slot->pMotors[i];
                slot->pMotors[i] = NULL;
            }
            delete slot->pControl;
            slot->pControl = NULL;
            delete slot;
        }
        m_Cards.erase(it);
    }
    LeaveCriticalSection(&m_csLock);
}

void CTMCM3216Manager::UnregisterAll()
{
    EnterCriticalSection(&m_csLock);
    for (auto& kv : m_Cards)
    {
        TMCMCardSlot* slot = kv.second;
        if (slot)
        {
            for (int i = 0; i < TMCM3216_MAX_AXIS; ++i)
                delete slot->pMotors[i];
            delete slot->pControl;
            delete slot;
        }
    }
    m_Cards.clear();
    LeaveCriticalSection(&m_csLock);
}

CTMCM3216BasicMotor* CTMCM3216Manager::CreateMotor(UINT cardID, UCHAR axis, const char* name)
{
    if (axis >= TMCM3216_MAX_AXIS) return NULL;
    EnterCriticalSection(&m_csLock);
    auto it = m_Cards.find(cardID);
    if (it == m_Cards.end())
    {
        LeaveCriticalSection(&m_csLock);
        return NULL;
    }
    TMCMCardSlot* slot = it->second;
    if (slot->pMotors[axis] != NULL)
    {
        LeaveCriticalSection(&m_csLock);
        return slot->pMotors[axis];
    }
    CTMCM3216BasicMotor* pMotor = new CTMCM3216BasicMotor(cardID, axis, name);
    pMotor->AttachControl(slot->pControl);
    slot->pMotors[axis] = pMotor;
    LeaveCriticalSection(&m_csLock);
    return pMotor;
}

CTMCM3216Control* CTMCM3216Manager::GetControl(UINT cardID)
{
    auto it = m_Cards.find(cardID);
    return (it != m_Cards.end()) ? it->second->pControl : NULL;
}

CTMCM3216BasicMotor* CTMCM3216Manager::GetMotor(UINT cardID, UCHAR axis)
{
    if (axis >= TMCM3216_MAX_AXIS) return NULL;
    auto it = m_Cards.find(cardID);
    return (it != m_Cards.end()) ? it->second->pMotors[axis] : NULL;
}

int CTMCM3216Manager::InitAll()
{
    int fail = 0;
    EnterCriticalSection(&m_csLock);
    for (auto& kv : m_Cards)
    {
        TMCMCardSlot* slot = kv.second;
        if (!slot || !slot->pControl) { ++fail; continue; }
        slot->pControl->ReadFirmware(500);
        for (int i = 0; i < TMCM3216_MAX_AXIS; ++i)
        {
            if (slot->pMotors[i])
            {
                if (slot->pMotors[i]->ApplyDefaultParam() < 0) ++fail;
            }
        }
    }
    LeaveCriticalSection(&m_csLock);
    return fail == 0 ? 0 : -1;
}

int CTMCM3216Manager::StopAll()
{
    int rtn = 0;
    EnterCriticalSection(&m_csLock);
    for (auto& kv : m_Cards)
    {
        if (kv.second && kv.second->pControl)
        {
            if (kv.second->pControl->StopAll() < 0) rtn = -1;
        }
    }
    LeaveCriticalSection(&m_csLock);
    return rtn;
}

int CTMCM3216Manager::GoTo(UINT cardID, UCHAR axis, int pos, UINT WaitTM)
{
    CTMCM3216BasicMotor* m = GetMotor(cardID, axis);
    return m ? m->GoTo(pos, WaitTM) : -1;
}

int CTMCM3216Manager::MoveRel(UINT cardID, UCHAR axis, int step, UINT WaitTM)
{
    CTMCM3216BasicMotor* m = GetMotor(cardID, axis);
    return m ? m->Step(step, WaitTM) : -1;
}

int CTMCM3216Manager::Jog(UINT cardID, UCHAR axis, int velocity, UINT WaitTM)
{
    CTMCM3216BasicMotor* m = GetMotor(cardID, axis);
    return m ? m->Jog(velocity, WaitTM) : -1;
}

int CTMCM3216Manager::StopAxis(UINT cardID, UCHAR axis, UINT WaitTM)
{
    CTMCM3216BasicMotor* m = GetMotor(cardID, axis);
    return m ? m->Stop(WaitTM) : -1;
}

int CTMCM3216Manager::Home(UINT cardID, UCHAR axis, UINT WaitTM)
{
    CTMCM3216BasicMotor* m = GetMotor(cardID, axis);
    return m ? m->Home(WaitTM) : -1;
}

int CTMCM3216Manager::SyncStart(UINT cardID)
{
    CTMCM3216Control* ctrl = GetControl(cardID);
    return ctrl ? ctrl->StartSyncMove() : -1;
}

BOOL CTMCM3216Manager::DispatchCANFrame(UINT canID, const UCHAR* data, int len)
{
    if (!data || len < 8) return FALSE;
    // 遍历所有卡，匹配 canID 或 cardID
    EnterCriticalSection(&m_csLock);
    BOOL handled = FALSE;
    for (auto& kv : m_Cards)
    {
        TMCMCardSlot* slot = kv.second;
        if (slot && slot->pControl &&
            (slot->canID == canID || slot->cardID == canID))
        {
            if (slot->pControl->OnCANRecv(data, len)) handled = TRUE;
        }
    }
    LeaveCriticalSection(&m_csLock);
    return handled;
}

BOOL CTMCM3216Manager::DispatchCANFrame(const TMCL_CAN_Frame& frame)
{
    return DispatchCANFrame(frame.id, frame.data, frame.len);
}

void CTMCM3216Manager::PollAll()
{
    EnterCriticalSection(&m_csLock);
    for (auto& kv : m_Cards)
    {
        if (kv.second && kv.second->pControl)
            kv.second->pControl->PollStatus();
    }
    LeaveCriticalSection(&m_csLock);
}

void CTMCM3216Manager::DumpAllStatus()
{
    printf("===== TMCM3216Manager::DumpAllStatus cards=%d =====\n", (int)m_Cards.size());
    for (auto& kv : m_Cards)
    {
        TMCMCardSlot* slot = kv.second;
        printf("  Card %d (CAN ID=%d)\n", slot->cardID, slot->canID);
        for (int i = 0; i < TMCM3216_MAX_AXIS; ++i)
        {
            if (slot->pMotors[i])
            {
                slot->pMotors[i]->DumpStatus();
            }
        }
    }
}
