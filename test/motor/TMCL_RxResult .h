#pragma once

#include "TMCM-3216.h"

void receiveExample()
{
    uint8_t rxData[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xD0};
    
    TMCL_RxResult res = TMCM3216_CAN_Protocol::parseFrame(rxData, 8);
    
    if (res.valid)
    {
        uint8_t addr = res.moduleAddr;
        uint8_t sta  = res.status;
        int32_t data = res.value;
    }
}