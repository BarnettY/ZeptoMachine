#include "TMCM-3216.h"

void exampleUsage()
{
    BYTE moduleAddr = 1;
    BYTE axis = 0;

    // 运动控制示例
    TMCL_CAN_Frame frame1 = TMCM3216_CAN_Protocol::moveAbsolute(moduleAddr, axis, 10000);
    TMCL_CAN_Frame frame2 = TMCM3216_CAN_Protocol::moveRelative(moduleAddr, axis, 500);
    TMCL_CAN_Frame frame3 = TMCM3216_CAN_Protocol::moveVelocity(moduleAddr, axis, 1000);
    TMCL_CAN_Frame frame4 = TMCM3216_CAN_Protocol::stopMotor(moduleAddr, axis);
    TMCL_CAN_Frame frame5 = TMCM3216_CAN_Protocol::emergencyStop(moduleAddr, axis);

    // 点动模式示例
    TMCL_CAN_Frame frame6 = TMCM3216_CAN_Protocol::jogForward(moduleAddr, axis, 500);
    TMCL_CAN_Frame frame7 = TMCM3216_CAN_Protocol::jogReverse(moduleAddr, axis, 500);

    // 滚轮模式示例
    TMCL_CAN_Frame frame8 = TMCM3216_CAN_Protocol::rollForward(moduleAddr, axis, 1000);
    TMCL_CAN_Frame frame9 = TMCM3216_CAN_Protocol::rollReverse(moduleAddr, axis, 1000);

    // 参数设置示例
    TMCL_CAN_Frame frame10 = TMCM3216_CAN_Protocol::setMaxSpeed(moduleAddr, axis, 2000);
    TMCL_CAN_Frame frame11 = TMCM3216_CAN_Protocol::setAcceleration(moduleAddr, axis, 500);
    TMCL_CAN_Frame frame12 = TMCM3216_CAN_Protocol::setCurrent(moduleAddr, axis, 50);
    TMCL_CAN_Frame frame13 = TMCM3216_CAN_Protocol::setMicrostep(moduleAddr, axis, 256);
    TMCL_CAN_Frame frame14 = TMCM3216_CAN_Protocol::setHoldingCurrent(moduleAddr, axis, 30);
    TMCL_CAN_Frame frame15 = TMCM3216_CAN_Protocol::setStandbyDelay(moduleAddr, axis, 1000);

    // 参数读取示例
    TMCL_CAN_Frame frame16 = TMCM3216_CAN_Protocol::getPosition(moduleAddr, axis);
    TMCL_CAN_Frame frame17 = TMCM3216_CAN_Protocol::getActualSpeed(moduleAddr, axis);
    TMCL_CAN_Frame frame18 = TMCM3216_CAN_Protocol::getCurrent(moduleAddr, axis);
    TMCL_CAN_Frame frame19 = TMCM3216_CAN_Protocol::getErrorFlags(moduleAddr, axis);
    TMCL_CAN_Frame frame20 = TMCM3216_CAN_Protocol::getStatusFlags(moduleAddr, axis);

    // 回零控制示例
    TMCL_CAN_Frame frame21 = TMCM3216_CAN_Protocol::homeStart(moduleAddr, axis);
    TMCL_CAN_Frame frame22 = TMCM3216_CAN_Protocol::homeStop(moduleAddr, axis);
    TMCL_CAN_Frame frame23 = TMCM3216_CAN_Protocol::homeSingleSwitch(moduleAddr, axis);
    TMCL_CAN_Frame frame24 = TMCM3216_CAN_Protocol::getHomeState(moduleAddr, axis);

    // IO控制示例
    TMCL_CAN_Frame frame25 = TMCM3216_CAN_Protocol::getAIN0(moduleAddr);
    TMCL_CAN_Frame frame26 = TMCM3216_CAN_Protocol::getDI0(moduleAddr);
    TMCL_CAN_Frame frame27 = TMCM3216_CAN_Protocol::setDO0(moduleAddr, true);
    TMCL_CAN_Frame frame28 = TMCM3216_CAN_Protocol::setAOUT0(moduleAddr, 2048);

    // 全局参数示例
    TMCL_CAN_Frame frame29 = TMCM3216_CAN_Protocol::getFirmwareVersion(moduleAddr);
    TMCL_CAN_Frame frame30 = TMCM3216_CAN_Protocol::getErrorCode(moduleAddr);
    TMCL_CAN_Frame frame31 = TMCM3216_CAN_Protocol::resetModule(moduleAddr);

    // 多轴同步示例
    TMCM3216_CAN_Protocol::setSyncTargetPos(moduleAddr, 0, 10000);
    TMCM3216_CAN_Protocol::setSyncTargetPos(moduleAddr, 1, 5000);
    TMCL_CAN_Frame syncFrame = TMCM3216_CAN_Protocol::startSyncMove(moduleAddr);
}

void parseExample()
{
    BYTE rxData[8] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0xD0};
    TMCL_RxResult res = TMCM3216_CAN_Protocol::parseFrame(rxData, 8);

    if (res.valid)
    {
        BYTE addr = res.moduleAddr;
        BYTE sta = res.status;
        int data = res.value;

        if (res.isSuccess())
        {
            const char* desc = TMCM3216_CAN_Protocol::getStatusDescription(res);
            
            int position = res.value;
            
            WORD uint16Val = TMCM3216_CAN_Protocol::toUint16(res);
            int16_t int16Val = TMCM3216_CAN_Protocol::toInt16(res);
            BYTE uint8Val = TMCM3216_CAN_Protocol::toUint8(res);
            bool boolVal = TMCM3216_CAN_Protocol::toBool(res);
        }
        else
        {
            const char* errorDesc = TMCM3216_CAN_Protocol::getStatusDescription(res);
        }
    }

    BYTE statusRxData[8] = {0x01, 0x00, 0x39, 0x00, 0x00, 0x00, 0x00, 0x05};
    TMCL_RxResult statusRes = TMCM3216_CAN_Protocol::parseFrame(statusRxData, 8);
    
    if (statusRes.valid && statusRes.isSuccess())
    {
        UINT statusFlags = static_cast<UINT>(statusRes.value);
        
        bool isMoving = TMCM3216_CAN_Protocol::isMotorMoving(statusFlags);
        bool targetReached = TMCM3216_CAN_Protocol::isTargetReached(statusFlags);
        bool stallDetected = TMCM3216_CAN_Protocol::isStallDetected(statusFlags);
        bool overTemp = TMCM3216_CAN_Protocol::hasOverTemperature(statusFlags);
        bool limitTriggered = TMCM3216_CAN_Protocol::hasLimitTriggered(statusFlags);
    }

    BYTE ainRxData[8] = {0x01, 0x00, 0x19, 0x00, 0x00, 0x00, 0x07, 0xFF};
    TMCL_RxResult ainRes = TMCM3216_CAN_Protocol::parseFrame(ainRxData, 8);
    
    if (ainRes.valid && ainRes.isSuccess())
    {
        float voltage = TMCM3216_CAN_Protocol::parseAINVoltage(ainRes, 3.3f);
        WORD rawValue = TMCM3216_CAN_Protocol::toUint16(ainRes);
    }

    BYTE fwRxData[8] = {0x01, 0x00, 0x12, 0x00, 0x01, 0x02, 0x00, 0x64};
    TMCL_RxResult fwRes = TMCM3216_CAN_Protocol::parseFrame(fwRxData, 8);
    
    if (fwRes.valid && fwRes.isSuccess())
    {
        BYTE major, minor;
        WORD build;
        TMCM3216_CAN_Protocol::parseFirmwareVersion(fwRes, major, minor, build);
    }
}