#pragma once

#include "TMCM-3216.h"

void receiveExample()
{
    BYTE rxData[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xD0};
    
    TMCL_RxResult res = TMCM3216_CAN_Protocol::parseFrame(rxData, 8);
    
    if (res.valid)
    {
        BYTE addr = res.moduleAddr;
        BYTE sta  = res.status;
        int data = res.value;
    }
}