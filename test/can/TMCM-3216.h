#pragma once
// 为兼容 C++98：不使用 <cstdint>，自行定义固定大小的整型
typedef unsigned char      uint8_t;
typedef signed char        int8_t;
typedef unsigned short     uint16_t;
typedef signed short       int16_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;

#include <string.h>   // memcpy

// ====================== TMCL 协议指令常量 ======================
#define TMCL_GAP                 1
#define TMCL_SAP                 2
#define TMCL_STOP                3
#define TMCL_MOV_ABS             4
#define TMCL_MOV_REL             5
#define TMCL_MOV_VEL             6
#define TMCL_GIO                 6
#define TMCL_SIO                 7
#define TMCL_RFS                 13
#define TMCL_ROL                 14
#define TMCL_SLO                 15
#define TMCL_JOG                 16
#define TMCL_START               17
#define TMCL_GET_FIRMWARE        18
#define TMCL_RESET               19
#define TMCL_SGP                 20
#define TMCL_GGP                 21
#define TMCL_CALC                22
#define TMCL_BIN                 23
#define TMCL_APPLICATION         24

// 轴参数编号 (Axis Parameters)
#define AXIS_ACT_POS             1
#define AXIS_ACT_VEL             2
#define AXIS_TARGET_POS          3
#define AXIS_MAX_VEL             4
#define AXIS_MAX_ACC             5
#define AXIS_MAX_CURRENT         6
#define AXIS_RFS_STATE           7
#define AXIS_RFS_MODE            8
#define AXIS_RFS_SPEED           9
#define AXIS_RFS_ACC             10
#define AXIS_RFS_OFFSET          11
#define AXIS_SOFT_STOP           12
#define AXIS_SOFT_START          13
#define AXIS_ENCODER_RES         14
#define AXIS_ENCODER_OFFSET      15
#define AXIS_STEP_MODE           16
#define AXIS_MICROSTEP_RES       140
#define AXIS_INTERPOLATION       141
#define AXIS_RAMP_MODE           142
#define AXIS_RAMP_PARAM          143
#define AXIS_STALL_THRESHOLD     144
#define AXIS_HOLDING_CURRENT     145
#define AXIS_STANDBY_CURRENT     146
#define AXIS_STANDBY_DELAY       147
#define AXIS_PWM_FREQUENCY       148
#define AXIS_PWM_OFFSET          149
#define AXIS_CURRENT_SCALE       150
#define AXIS_ADC_OFFSET          151
#define AXIS_ADC_SCALE           152
#define AXIS_POSITION_OFFSET     153
#define AXIS_VELOCITY_OFFSET     154
#define AXIS_ACCELERATION_OFFSET 155
#define AXIS_ERROR_FLAGS         156
#define AXIS_STATUS_FLAGS        157
#define AXIS_MOTOR_TYPE          158
#define AXIS_COMM_MODE           159
#define AXIS_DRIVE_MODE          160

// 全局参数编号 (Global Parameters)
#define GLOBAL_FIRMWARE_VERSION  0
#define GLOBAL_MODULE_ADDRESS    1
#define GLOBAL_BAUDRATE          2
#define GLOBAL_CAN_ID            3
#define GLOBAL_CAN_BITRATE       4
#define GLOBAL_I2C_ADDRESS       5
#define GLOBAL_CRC_ENABLE        6
#define GLOBAL_TIMEOUT           7
#define GLOBAL_WATCHDOG          8
#define GLOBAL_BOOTLOADER        9
#define GLOBAL_ERROR_CODE        10
#define GLOBAL_STATUS            11

// 回零模式
#define RFS_START                0
#define RFS_STOP                 1
#define RFS_MODE_SINGLE_SW       2
#define RFS_MODE_DOUBLE_SW       3
#define RFS_MODE_INDEX           4
#define RFS_MODE_SOFT            5

// IO 通道编号
#define IO_DI0                   0
#define IO_DI1                   1
#define IO_DI2                   2
#define IO_DI3                   3
#define IO_DO0                   4
#define IO_DO1                   5
#define IO_DO2                   6
#define IO_DO3                   7
#define IO_AIN0                  25
#define IO_AIN1                  26
#define IO_AOUT0                 27
#define IO_AOUT1                 28

// 点动模式方向
#define JOG_FORWARD              0
#define JOG_REVERSE              1

// 斜坡模式类型
#define SLO_LINEAR               0
#define SLO_S_CURVE              1
#define SLO_TRAJECTORY           2

// 滚轮模式方向
#define ROL_FORWARD              0
#define ROL_REVERSE              1

// ====================== 数据结构体 ======================
struct TMCL_CAN_Frame
{
    uint32_t id;
    uint8_t  data[8];
    uint8_t  len;
};

// TMCL 状态码（C++98：使用普通 enum）
enum TMCL_Status
{
    TMCL_STATUS_SUCCESS                      = 0x00,
    TMCL_STATUS_INVALID_COMMAND              = 0x01,
    TMCL_STATUS_INVALID_TYPE                 = 0x02,
    TMCL_STATUS_INVALID_AXIS                 = 0x03,
    TMCL_STATUS_INVALID_VALUE                = 0x04,
    TMCL_STATUS_EEPROM_LOCKED                = 0x05,
    TMCL_STATUS_COMMAND_NOT_AVAILABLE        = 0x06,
    TMCL_STATUS_COMMAND_LOADED               = 0x07,
    TMCL_STATUS_VARIABLE_NOT_AVAILABLE       = 0x08,
    TMCL_STATUS_BAD_CHECKSUM                 = 0x09,
    TMCL_STATUS_NO_EEPROM                    = 0x0A,
    TMCL_STATUS_WRONG_MODULE_ADDRESS         = 0x0B,
    TMCL_STATUS_MAX_EXCEEDED                 = 0x0C,
    TMCL_STATUS_MIN_EXCEEDED                 = 0x0D,
    TMCL_STATUS_INVALID_OPERATION            = 0x0E,
    TMCL_STATUS_NO_MOTOR                     = 0x0F,
    TMCL_STATUS_MOTOR_FAULT                  = 0x10,
    TMCL_STATUS_HALL_NOT_AVAILABLE           = 0x11,
    TMCL_STATUS_ENCODER_NOT_AVAILABLE        = 0x12,
    TMCL_STATUS_WRONG_PARAMETER_TYPE         = 0x13,
    TMCL_STATUS_PARAMETER_NOT_AVAILABLE      = 0x14,
    TMCL_STATUS_MOTOR_BLOCKED                = 0x15,
    TMCL_STATUS_OVER_TEMPERATURE             = 0x16,
    TMCL_STATUS_SHORT_CIRCUIT                = 0x17,
    TMCL_STATUS_UNDER_VOLTAGE                = 0x18,
    TMCL_STATUS_OVER_VOLTAGE                 = 0x19,
    TMCL_STATUS_I2C_TIMEOUT                  = 0x1A,
    TMCL_STATUS_CAN_TIMEOUT                  = 0x1B,
    TMCL_STATUS_CAN_ERROR                    = 0x1C,
    TMCL_STATUS_INVALID_CHECKSUM             = 0x1D,
    TMCL_STATUS_FLASH_ERROR                  = 0x1E,
    TMCL_STATUS_RAM_ERROR                    = 0x1F,
    TMCL_STATUS_WATCHDOG_RESET               = 0x20,
    TMCL_STATUS_NOT_INITIALIZED              = 0x21,
    TMCL_STATUS_INITIALIZATION_ERROR         = 0x22,
    TMCL_STATUS_CALIBRATION_ERROR            = 0x23,
    TMCL_STATUS_REFERENCE_NOT_FOUND          = 0x24,
    TMCL_STATUS_REFERENCE_ERROR              = 0x25,
    TMCL_STATUS_SYNCHRONIZATION_ERROR        = 0x26,
    TMCL_STATUS_OVER_CURRENT                 = 0x27,
    TMCL_STATUS_EMERGENCY_STOP               = 0x28,
    TMCL_STATUS_SOFTWARE_LIMIT_POS           = 0x29,
    TMCL_STATUS_SOFTWARE_LIMIT_NEG           = 0x2A,
    TMCL_STATUS_HARDWARE_LIMIT_POS           = 0x2B,
    TMCL_STATUS_HARDWARE_LIMIT_NEG           = 0x2C,
    TMCL_STATUS_FOLLOWING_ERROR              = 0x2D,
    TMCL_STATUS_COMMUNICATION_ERROR          = 0x2E,
    TMCL_STATUS_INVALID_BAUDRATE             = 0x2F,
    TMCL_STATUS_INVALID_CAN_ID               = 0x30,
    TMCL_STATUS_INVALID_OPERATING_MODE       = 0x31,
    TMCL_STATUS_INVALID_MICROSTEP_RESOLUTION = 0x32,
    TMCL_STATUS_INVALID_CURRENT              = 0x33,
    TMCL_STATUS_INVALID_ACCELERATION         = 0x34,
    TMCL_STATUS_INVALID_VELOCITY             = 0x35,
    TMCL_STATUS_INVALID_POSITION             = 0x36,
    TMCL_STATUS_INVALID_TRAJECTORY           = 0x37,
    TMCL_STATUS_INVALID_RAMP_MODE            = 0x38,
    TMCL_STATUS_INVALID_JOG_MODE             = 0x39,
    TMCL_STATUS_INVALID_ROLL_MODE            = 0x3A,
    TMCL_STATUS_INVALID_REFERENCE_MODE       = 0x3B,
    TMCL_STATUS_INVALID_IO_CONFIGURATION     = 0x3C,
    TMCL_STATUS_INVALID_ANALOG_INPUT         = 0x3D,
    TMCL_STATUS_INVALID_DIGITAL_INPUT        = 0x3E,
    TMCL_STATUS_INVALID_DIGITAL_OUTPUT       = 0x3F,
    TMCL_STATUS_RESERVED                     = 0x40
};

// 接收帧解析结果
struct TMCL_RxResult
{
    bool    valid;
    uint8_t moduleAddr;
    uint8_t status;
    int32_t value;
    uint8_t command;
    uint8_t type;
    uint8_t axis;

    bool isSuccess() const { return status == (uint8_t)TMCL_STATUS_SUCCESS; }
    bool hasError()  const { return status != (uint8_t)TMCL_STATUS_SUCCESS; }
};

// 电机状态标志位
enum MotorStatusFlag
{
    MSFLAG_TARGET_REACHED      = 0x0001,
    MSFLAG_REFERENCE_FOUND     = 0x0002,
    MSFLAG_MOVING              = 0x0004,
    MSFLAG_RFS_IN_PROGRESS     = 0x0008,
    MSFLAG_STALL_DETECTED      = 0x0010,
    MSFLAG_OVER_TEMPERATURE    = 0x0020,
    MSFLAG_UNDER_VOLTAGE       = 0x0040,
    MSFLAG_OVER_VOLTAGE        = 0x0080,
    MSFLAG_SHORT_CIRCUIT       = 0x0100,
    MSFLAG_OVER_CURRENT        = 0x0200,
    MSFLAG_EEPROM_ERROR        = 0x0400,
    MSFLAG_COMM_ERROR          = 0x0800,
    MSFLAG_SOFTWARE_LIMIT_POS  = 0x1000,
    MSFLAG_SOFTWARE_LIMIT_NEG  = 0x2000,
    MSFLAG_HARDWARE_LIMIT_POS  = 0x4000,
    MSFLAG_HARDWARE_LIMIT_NEG  = 0x8000
};

// IO 状态
struct IO_Status
{
    uint8_t  digitalInputs;
    uint8_t  digitalOutputs;
    uint16_t analogInput0;
    uint16_t analogInput1;
    uint16_t analogOutput0;
    uint16_t analogOutput1;
};

// 运动状态
struct MotionStatus
{
    int32_t actualPosition;
    int32_t actualVelocity;
    int32_t targetPosition;
    int32_t targetVelocity;
    bool    isMoving;
    bool    targetReached;
};

// ====================== TMCM-3216 CAN 协议类 ======================
class TMCM3216_CAN_Protocol
{
public:
    // 底层通用打包
    static TMCL_CAN_Frame packFrame(uint8_t moduleAddr, uint8_t cmd,
                                     uint8_t type, uint8_t axis, int32_t value)
    {
        TMCL_CAN_Frame frame;
        frame.id  = 0x01;
        frame.len = 8;
        frame.data[0] = moduleAddr;
        frame.data[1] = cmd;
        frame.data[2] = type;
        frame.data[3] = axis;
        frame.data[4] = (uint8_t)((value >> 24) & 0xFF);
        frame.data[5] = (uint8_t)((value >> 16) & 0xFF);
        frame.data[6] = (uint8_t)((value >> 8)  & 0xFF);
        frame.data[7] = (uint8_t)(value & 0xFF);
        return frame;
    }

    // 接收帧解析
    static TMCL_RxResult parseFrame(const uint8_t* rxData, uint8_t dataLen)
    {
        TMCL_RxResult res;
        memset(&res, 0, sizeof(res));

        if (rxData == NULL || dataLen != 8)
            return res;

        res.moduleAddr = rxData[0];
        res.status     = rxData[1];
        res.command    = rxData[1];
        res.type       = rxData[2];
        res.axis       = rxData[3];

        res.value = (int32_t)(
            ((uint32_t)rxData[4] << 24) |
            ((uint32_t)rxData[5] << 16) |
            ((uint32_t)rxData[6] << 8)  |
            ((uint32_t)rxData[7])
        );
        res.valid = true;
        return res;
    }

    static TMCL_RxResult parseFrame(const TMCL_CAN_Frame& frame)
    {
        return parseFrame(frame.data, frame.len);
    }

    // 状态码描述
    static const char* getStatusDescription(uint8_t statusCode)
    {
        switch ((TMCL_Status)statusCode)
        {
            case TMCL_STATUS_SUCCESS:                     return "Success";
            case TMCL_STATUS_INVALID_COMMAND:             return "Invalid command";
            case TMCL_STATUS_INVALID_TYPE:                return "Invalid type parameter";
            case TMCL_STATUS_INVALID_AXIS:                return "Invalid axis";
            case TMCL_STATUS_INVALID_VALUE:               return "Invalid value";
            case TMCL_STATUS_EEPROM_LOCKED:               return "EEPROM locked";
            case TMCL_STATUS_COMMAND_NOT_AVAILABLE:       return "Command not available";
            case TMCL_STATUS_COMMAND_LOADED:              return "Command loaded";
            case TMCL_STATUS_VARIABLE_NOT_AVAILABLE:      return "Variable not available";
            case TMCL_STATUS_BAD_CHECKSUM:                return "Bad checksum";
            case TMCL_STATUS_NO_EEPROM:                   return "No EEPROM";
            case TMCL_STATUS_WRONG_MODULE_ADDRESS:        return "Wrong module address";
            case TMCL_STATUS_MAX_EXCEEDED:                return "Maximum exceeded";
            case TMCL_STATUS_MIN_EXCEEDED:                return "Minimum exceeded";
            case TMCL_STATUS_INVALID_OPERATION:           return "Invalid operation";
            case TMCL_STATUS_NO_MOTOR:                    return "No motor";
            case TMCL_STATUS_MOTOR_FAULT:                 return "Motor fault";
            case TMCL_STATUS_HALL_NOT_AVAILABLE:          return "Hall sensor not available";
            case TMCL_STATUS_ENCODER_NOT_AVAILABLE:       return "Encoder not available";
            case TMCL_STATUS_WRONG_PARAMETER_TYPE:        return "Wrong parameter type";
            case TMCL_STATUS_PARAMETER_NOT_AVAILABLE:     return "Parameter not available";
            case TMCL_STATUS_MOTOR_BLOCKED:               return "Motor blocked (stall)";
            case TMCL_STATUS_OVER_TEMPERATURE:            return "Over temperature";
            case TMCL_STATUS_SHORT_CIRCUIT:               return "Short circuit";
            case TMCL_STATUS_UNDER_VOLTAGE:               return "Under voltage";
            case TMCL_STATUS_OVER_VOLTAGE:                return "Over voltage";
            case TMCL_STATUS_I2C_TIMEOUT:                 return "I2C timeout";
            case TMCL_STATUS_CAN_TIMEOUT:                 return "CAN timeout";
            case TMCL_STATUS_CAN_ERROR:                   return "CAN error";
            case TMCL_STATUS_INVALID_CHECKSUM:            return "Invalid checksum";
            case TMCL_STATUS_FLASH_ERROR:                 return "Flash error";
            case TMCL_STATUS_RAM_ERROR:                   return "RAM error";
            case TMCL_STATUS_WATCHDOG_RESET:              return "Watchdog reset";
            case TMCL_STATUS_NOT_INITIALIZED:             return "Not initialized";
            case TMCL_STATUS_INITIALIZATION_ERROR:        return "Initialization error";
            case TMCL_STATUS_CALIBRATION_ERROR:           return "Calibration error";
            case TMCL_STATUS_REFERENCE_NOT_FOUND:         return "Reference not found";
            case TMCL_STATUS_REFERENCE_ERROR:             return "Reference error";
            case TMCL_STATUS_SYNCHRONIZATION_ERROR:       return "Synchronization error";
            case TMCL_STATUS_OVER_CURRENT:                return "Over current";
            case TMCL_STATUS_EMERGENCY_STOP:              return "Emergency stop";
            case TMCL_STATUS_SOFTWARE_LIMIT_POS:          return "Software limit positive";
            case TMCL_STATUS_SOFTWARE_LIMIT_NEG:          return "Software limit negative";
            case TMCL_STATUS_HARDWARE_LIMIT_POS:          return "Hardware limit positive";
            case TMCL_STATUS_HARDWARE_LIMIT_NEG:          return "Hardware limit negative";
            case TMCL_STATUS_FOLLOWING_ERROR:             return "Following error";
            case TMCL_STATUS_COMMUNICATION_ERROR:         return "Communication error";
            case TMCL_STATUS_INVALID_BAUDRATE:            return "Invalid baudrate";
            case TMCL_STATUS_INVALID_CAN_ID:              return "Invalid CAN ID";
            case TMCL_STATUS_INVALID_OPERATING_MODE:      return "Invalid operating mode";
            case TMCL_STATUS_INVALID_MICROSTEP_RESOLUTION:return "Invalid microstep resolution";
            case TMCL_STATUS_INVALID_CURRENT:             return "Invalid current";
            case TMCL_STATUS_INVALID_ACCELERATION:        return "Invalid acceleration";
            case TMCL_STATUS_INVALID_VELOCITY:            return "Invalid velocity";
            case TMCL_STATUS_INVALID_POSITION:            return "Invalid position";
            case TMCL_STATUS_INVALID_TRAJECTORY:          return "Invalid trajectory";
            case TMCL_STATUS_INVALID_RAMP_MODE:           return "Invalid ramp mode";
            case TMCL_STATUS_INVALID_JOG_MODE:            return "Invalid jog mode";
            case TMCL_STATUS_INVALID_ROLL_MODE:           return "Invalid roll mode";
            case TMCL_STATUS_INVALID_REFERENCE_MODE:      return "Invalid reference mode";
            case TMCL_STATUS_INVALID_IO_CONFIGURATION:    return "Invalid IO configuration";
            case TMCL_STATUS_INVALID_ANALOG_INPUT:        return "Invalid analog input";
            case TMCL_STATUS_INVALID_DIGITAL_INPUT:       return "Invalid digital input";
            case TMCL_STATUS_INVALID_DIGITAL_OUTPUT:      return "Invalid digital output";
            default:                                       return "Unknown error";
        }
    }

    static const char* getStatusDescription(const TMCL_RxResult& result)
    {
        return getStatusDescription(result.status);
    }

    // 状态标志位解析
    static bool checkStatusFlag(uint32_t statusValue, MotorStatusFlag flag)
    {
        return (statusValue & (uint32_t)flag) != 0;
    }

    static bool isMotorMoving(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_MOVING);
    }

    static bool isTargetReached(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_TARGET_REACHED);
    }

    static bool isReferenceFound(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_REFERENCE_FOUND);
    }

    static bool isRFSInProgress(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_RFS_IN_PROGRESS);
    }

    static bool isStallDetected(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_STALL_DETECTED);
    }

    static bool hasOverTemperature(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_OVER_TEMPERATURE);
    }

    static bool hasOverCurrent(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_OVER_CURRENT);
    }

    static bool hasShortCircuit(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_SHORT_CIRCUIT);
    }

    static bool hasLimitTriggered(uint32_t statusValue)
    {
        return checkStatusFlag(statusValue, MSFLAG_SOFTWARE_LIMIT_POS) ||
               checkStatusFlag(statusValue, MSFLAG_SOFTWARE_LIMIT_NEG) ||
               checkStatusFlag(statusValue, MSFLAG_HARDWARE_LIMIT_POS) ||
               checkStatusFlag(statusValue, MSFLAG_HARDWARE_LIMIT_NEG);
    }

    // 数据类型转换
    static uint16_t toUint16(const TMCL_RxResult& result)
    { return (uint16_t)(result.value & 0xFFFF); }

    static int16_t toInt16(const TMCL_RxResult& result)
    { return (int16_t)(result.value & 0xFFFF); }

    static uint8_t toUint8(const TMCL_RxResult& result)
    { return (uint8_t)(result.value & 0xFF); }

    static int8_t toInt8(const TMCL_RxResult& result)
    { return (int8_t)(result.value & 0xFF); }

    static float toFloat(const TMCL_RxResult& result)
    {
        float f;
        memcpy(&f, &result.value, sizeof(float));
        return f;
    }

    static bool toBool(const TMCL_RxResult& result)
    { return result.value != 0; }

    // 模拟量转换
    static float analogToVoltage(uint16_t analogValue, float refVoltage = 3.3f)
    {
        return ((float)analogValue / 4095.0f) * refVoltage;
    }

    static float analogToCurrent(uint16_t analogValue, float maxCurrent)
    {
        return ((float)analogValue / 4095.0f) * maxCurrent;
    }

    static float parseAINVoltage(const TMCL_RxResult& result, float refVoltage = 3.3f)
    {
        return analogToVoltage(toUint16(result), refVoltage);
    }

    // 固件版本解析
    static void parseFirmwareVersion(const TMCL_RxResult& result,
                                      uint8_t& major, uint8_t& minor, uint16_t& build)
    {
        uint32_t version = (uint32_t)result.value;
        major = (uint8_t)((version >> 24) & 0xFF);
        minor = (uint8_t)((version >> 16) & 0xFF);
        build = (uint16_t)(version & 0xFFFF);
    }

    // ========== 功能接口：参数设置 ==========
    static TMCL_CAN_Frame setMaxSpeed(uint8_t addr, uint8_t axis, int32_t speed)
    { return packFrame(addr, TMCL_SAP, AXIS_MAX_VEL, axis, speed); }

    static TMCL_CAN_Frame setAcceleration(uint8_t addr, uint8_t axis, int32_t accel)
    { return packFrame(addr, TMCL_SAP, AXIS_MAX_ACC, axis, accel); }

    static TMCL_CAN_Frame setCurrent(uint8_t addr, uint8_t axis, uint8_t current)
    { return packFrame(addr, TMCL_SAP, AXIS_MAX_CURRENT, axis, (int32_t)current); }

    static TMCL_CAN_Frame setMicrostep(uint8_t addr, uint8_t axis, uint16_t microStep)
    { return packFrame(addr, TMCL_SAP, AXIS_MICROSTEP_RES, axis, (int32_t)microStep); }

    static TMCL_CAN_Frame setHoldingCurrent(uint8_t addr, uint8_t axis, uint8_t current)
    { return packFrame(addr, TMCL_SAP, AXIS_HOLDING_CURRENT, axis, (int32_t)current); }

    static TMCL_CAN_Frame setStandbyCurrent(uint8_t addr, uint8_t axis, uint8_t current)
    { return packFrame(addr, TMCL_SAP, AXIS_STANDBY_CURRENT, axis, (int32_t)current); }

    static TMCL_CAN_Frame setStandbyDelay(uint8_t addr, uint8_t axis, uint32_t delayMs)
    { return packFrame(addr, TMCL_SAP, AXIS_STANDBY_DELAY, axis, (int32_t)delayMs); }

    static TMCL_CAN_Frame setStallThreshold(uint8_t addr, uint8_t axis, uint16_t threshold)
    { return packFrame(addr, TMCL_SAP, AXIS_STALL_THRESHOLD, axis, (int32_t)threshold); }

    static TMCL_CAN_Frame setSoftStop(uint8_t addr, uint8_t axis, bool enable)
    { return packFrame(addr, TMCL_SAP, AXIS_SOFT_STOP, axis, enable ? 1 : 0); }

    static TMCL_CAN_Frame setSoftStart(uint8_t addr, uint8_t axis, bool enable)
    { return packFrame(addr, TMCL_SAP, AXIS_SOFT_START, axis, enable ? 1 : 0); }

    static TMCL_CAN_Frame setRFSpeed(uint8_t addr, uint8_t axis, int32_t speed)
    { return packFrame(addr, TMCL_SAP, AXIS_RFS_SPEED, axis, speed); }

    static TMCL_CAN_Frame setRFAcceleration(uint8_t addr, uint8_t axis, int32_t accel)
    { return packFrame(addr, TMCL_SAP, AXIS_RFS_ACC, axis, accel); }

    static TMCL_CAN_Frame setRFOffset(uint8_t addr, uint8_t axis, int32_t offset)
    { return packFrame(addr, TMCL_SAP, AXIS_RFS_OFFSET, axis, offset); }

    static TMCL_CAN_Frame setRFScanMode(uint8_t addr, uint8_t axis, uint8_t mode)
    { return packFrame(addr, TMCL_SAP, AXIS_RFS_MODE, axis, (int32_t)mode); }

    static TMCL_CAN_Frame setTargetPosition(uint8_t addr, uint8_t axis, int32_t pos)
    { return packFrame(addr, TMCL_SAP, AXIS_TARGET_POS, axis, pos); }

    // ========== 功能接口：参数读取 ==========
    static TMCL_CAN_Frame getPosition(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_ACT_POS, axis, 0); }

    static TMCL_CAN_Frame getActualSpeed(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_ACT_VEL, axis, 0); }

    static TMCL_CAN_Frame getMaxSpeed(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_MAX_VEL, axis, 0); }

    static TMCL_CAN_Frame getAcceleration(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_MAX_ACC, axis, 0); }

    static TMCL_CAN_Frame getCurrent(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_MAX_CURRENT, axis, 0); }

    static TMCL_CAN_Frame getMicrostep(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_MICROSTEP_RES, axis, 0); }

    static TMCL_CAN_Frame getTargetPosition(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_TARGET_POS, axis, 0); }

    static TMCL_CAN_Frame getHoldingCurrent(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_HOLDING_CURRENT, axis, 0); }

    static TMCL_CAN_Frame getStandbyCurrent(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_STANDBY_CURRENT, axis, 0); }

    static TMCL_CAN_Frame getStandbyDelay(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_STANDBY_DELAY, axis, 0); }

    static TMCL_CAN_Frame getStallThreshold(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_STALL_THRESHOLD, axis, 0); }

    static TMCL_CAN_Frame getErrorFlags(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_ERROR_FLAGS, axis, 0); }

    static TMCL_CAN_Frame getStatusFlags(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_STATUS_FLAGS, axis, 0); }

    // ========== 功能接口：运动控制 ==========
    static TMCL_CAN_Frame moveAbsolute(uint8_t addr, uint8_t axis, int32_t pos)
    { return packFrame(addr, TMCL_MOV_ABS, 0, axis, pos); }

    static TMCL_CAN_Frame moveRelative(uint8_t addr, uint8_t axis, int32_t step)
    { return packFrame(addr, TMCL_MOV_REL, 0, axis, step); }

    static TMCL_CAN_Frame moveVelocity(uint8_t addr, uint8_t axis, int32_t velocity)
    { return packFrame(addr, TMCL_MOV_VEL, 0, axis, velocity); }

    static TMCL_CAN_Frame stopMotor(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_STOP, 0, axis, 0); }

    static TMCL_CAN_Frame emergencyStop(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_STOP, 0, axis, 1); }

    static TMCL_CAN_Frame jogForward(uint8_t addr, uint8_t axis, int32_t speed)
    { return packFrame(addr, TMCL_JOG, JOG_FORWARD, axis, speed); }

    static TMCL_CAN_Frame jogReverse(uint8_t addr, uint8_t axis, int32_t speed)
    { return packFrame(addr, TMCL_JOG, JOG_REVERSE, axis, speed); }

    static TMCL_CAN_Frame rollForward(uint8_t addr, uint8_t axis, int32_t speed)
    { return packFrame(addr, TMCL_ROL, ROL_FORWARD, axis, speed); }

    static TMCL_CAN_Frame rollReverse(uint8_t addr, uint8_t axis, int32_t speed)
    { return packFrame(addr, TMCL_ROL, ROL_REVERSE, axis, speed); }

    static TMCL_CAN_Frame setRampLinear(uint8_t addr, uint8_t axis, int32_t duration)
    { return packFrame(addr, TMCL_SLO, SLO_LINEAR, axis, duration); }

    static TMCL_CAN_Frame setRampSCurve(uint8_t addr, uint8_t axis, int32_t duration)
    { return packFrame(addr, TMCL_SLO, SLO_S_CURVE, axis, duration); }

    static TMCL_CAN_Frame setRampTrajectory(uint8_t addr, uint8_t axis, int32_t param)
    { return packFrame(addr, TMCL_SLO, SLO_TRAJECTORY, axis, param); }

    // ========== 功能接口：多轴同步 ==========
    static TMCL_CAN_Frame setSyncTargetPos(uint8_t addr, uint8_t axis, int32_t pos)
    { return moveAbsolute(addr, axis, pos); }

    static TMCL_CAN_Frame startSyncMove(uint8_t addr)
    { return packFrame(addr, TMCL_START, 0, 0, 0); }

    // ========== 功能接口：回零控制 ==========
    static TMCL_CAN_Frame homeStart(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_RFS, RFS_START, axis, 0); }

    static TMCL_CAN_Frame homeStop(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_RFS, RFS_STOP, axis, 0); }

    static TMCL_CAN_Frame getHomeState(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_GAP, AXIS_RFS_STATE, axis, 0); }

    static TMCL_CAN_Frame homeSingleSwitch(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_RFS, RFS_MODE_SINGLE_SW, axis, 0); }

    static TMCL_CAN_Frame homeDoubleSwitch(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_RFS, RFS_MODE_DOUBLE_SW, axis, 0); }

    static TMCL_CAN_Frame homeIndex(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_RFS, RFS_MODE_INDEX, axis, 0); }

    static TMCL_CAN_Frame homeSoft(uint8_t addr, uint8_t axis)
    { return packFrame(addr, TMCL_RFS, RFS_MODE_SOFT, axis, 0); }

    // 通用打包（供外部使用）
    static TMCL_CAN_Frame packFrameEx(uint8_t moduleAddr, uint8_t cmd,
                                       uint8_t type, uint8_t axis, int32_t value)
    { return packFrame(moduleAddr, cmd, type, axis, value); }

    // ========== 功能接口：IO 控制 ==========
    static TMCL_CAN_Frame getAIN0(uint8_t addr)
    { return packFrame(addr, TMCL_GIO, IO_AIN0, 0, 0); }

    static TMCL_CAN_Frame getAIN1(uint8_t addr)
    { return packFrame(addr, TMCL_GIO, IO_AIN1, 0, 0); }

    static TMCL_CAN_Frame getDI0(uint8_t addr)
    { return packFrame(addr, TMCL_GIO, IO_DI0, 0, 0); }

    static TMCL_CAN_Frame getDI1(uint8_t addr)
    { return packFrame(addr, TMCL_GIO, IO_DI1, 0, 0); }

    static TMCL_CAN_Frame getDI2(uint8_t addr)
    { return packFrame(addr, TMCL_GIO, IO_DI2, 0, 0); }

    static TMCL_CAN_Frame getDI3(uint8_t addr)
    { return packFrame(addr, TMCL_GIO, IO_DI3, 0, 0); }

    static TMCL_CAN_Frame setDO0(uint8_t addr, bool state)
    { return packFrame(addr, TMCL_SIO, IO_DO0, 0, state ? 1 : 0); }

    static TMCL_CAN_Frame setDO1(uint8_t addr, bool state)
    { return packFrame(addr, TMCL_SIO, IO_DO1, 0, state ? 1 : 0); }

    static TMCL_CAN_Frame setDO2(uint8_t addr, bool state)
    { return packFrame(addr, TMCL_SIO, IO_DO2, 0, state ? 1 : 0); }

    static TMCL_CAN_Frame setDO3(uint8_t addr, bool state)
    { return packFrame(addr, TMCL_SIO, IO_DO3, 0, state ? 1 : 0); }

    static TMCL_CAN_Frame setAOUT0(uint8_t addr, uint16_t value)
    { return packFrame(addr, TMCL_SIO, IO_AOUT0, 0, (int32_t)value); }

    static TMCL_CAN_Frame setAOUT1(uint8_t addr, uint16_t value)
    { return packFrame(addr, TMCL_SIO, IO_AOUT1, 0, (int32_t)value); }

    // ========== 功能接口：全局参数 ==========
    static TMCL_CAN_Frame getFirmwareVersion(uint8_t addr)
    { return packFrame(addr, TMCL_GGP, GLOBAL_FIRMWARE_VERSION, 0, 0); }

    static TMCL_CAN_Frame getModuleAddress(uint8_t addr)
    { return packFrame(addr, TMCL_GGP, GLOBAL_MODULE_ADDRESS, 0, 0); }

    static TMCL_CAN_Frame getCANID(uint8_t addr)
    { return packFrame(addr, TMCL_GGP, GLOBAL_CAN_ID, 0, 0); }

    static TMCL_CAN_Frame getErrorCode(uint8_t addr)
    { return packFrame(addr, TMCL_GGP, GLOBAL_ERROR_CODE, 0, 0); }

    static TMCL_CAN_Frame getGlobalStatus(uint8_t addr)
    { return packFrame(addr, TMCL_GGP, GLOBAL_STATUS, 0, 0); }

    static TMCL_CAN_Frame setModuleAddress(uint8_t addr, uint8_t newAddr)
    { return packFrame(addr, TMCL_SGP, GLOBAL_MODULE_ADDRESS, 0, (int32_t)newAddr); }

    static TMCL_CAN_Frame setCANID(uint8_t addr, uint32_t canId)
    { return packFrame(addr, TMCL_SGP, GLOBAL_CAN_ID, 0, (int32_t)canId); }

    static TMCL_CAN_Frame setCANBitrate(uint8_t addr, uint32_t bitrate)
    { return packFrame(addr, TMCL_SGP, GLOBAL_CAN_BITRATE, 0, (int32_t)bitrate); }

    // ========== 功能接口：模块控制 ==========
    static TMCL_CAN_Frame resetModule(uint8_t addr)
    { return packFrame(addr, TMCL_RESET, 0, 0, 0); }

    static TMCL_CAN_Frame getFirmwareInfo(uint8_t addr)
    { return packFrame(addr, TMCL_GET_FIRMWARE, 0, 0, 0); }
};
