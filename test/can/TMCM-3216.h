#pragma once
//#include <cstdint>
//#include <cstring>
#include "../comon.h"
// ====================== TMCL 协议指令常量 ======================
#define TMCL_GAP                 1       // 读取轴参数
#define TMCL_SAP                 2       // 设置轴参数
#define TMCL_STOP                3       // 停止电机
#define TMCL_MOV_ABS             4       // 绝对移动
#define TMCL_MOV_REL             5       // 相对移动
#define TMCL_MOV_VEL             6       // 速度模式移动
#define TMCL_GIO                 6       // 读取IO
#define TMCL_SIO                 7       // 设置IO
#define TMCL_RFS                 13      // 回零(参考搜索)
#define TMCL_ROL                 14      // 滚轮模式
#define TMCL_SLO                 15      // 斜坡模式
#define TMCL_JOG                 16      // 点动模式
#define TMCL_START               17      // 多轴同步启动
#define TMCL_GET_FIRMWARE        18      // 读取固件版本
#define TMCL_RESET               19      // 复位模块
#define TMCL_SGP                 20      // 设置全局参数
#define TMCL_GGP                 21      // 读取全局参数
#define TMCL_CALC                22      // 计算指令
#define TMCL_BIN                 23      // 二进制下载
#define TMCL_APPLICATION         24      // 应用指令

// 轴参数编号 (Axis Parameters)
#define AXIS_ACT_POS             1       // 当前位置
#define AXIS_ACT_VEL             2       // 当前速度
#define AXIS_TARGET_POS          3       // 目标位置
#define AXIS_MAX_VEL             4       // 最大速度
#define AXIS_MAX_ACC             5       // 加速度
#define AXIS_MAX_CURRENT         6       // 电机电流
#define AXIS_RFS_STATE           7       // 回零状态
#define AXIS_RFS_MODE            8       // 回零模式
#define AXIS_RFS_SPEED           9       // 回零速度
#define AXIS_RFS_ACC             10      // 回零加速度
#define AXIS_RFS_OFFSET          11      // 回零偏移量
#define AXIS_SOFT_STOP           12      // 软停止
#define AXIS_SOFT_START          13      // 软启动
#define AXIS_ENCODER_RES         14      // 编码器分辨率
#define AXIS_ENCODER_OFFSET      15      // 编码器偏移
#define AXIS_STEP_MODE           16      // 步进模式
#define AXIS_MICROSTEP_RES       140     // 细分数
#define AXIS_INTERPOLATION       141     // 插值因子
#define AXIS_RAMP_MODE           142     // 斜坡模式
#define AXIS_RAMP_PARAM          143     // 斜坡参数
#define AXIS_STALL_THRESHOLD     144     // 失速阈值
#define AXIS_HOLDING_CURRENT     145     // 保持电流
#define AXIS_STANDBY_CURRENT     146     // 待机电流
#define AXIS_STANDBY_DELAY       147     // 待机延迟
#define AXIS_PWM_FREQUENCY       148     // PWM频率
#define AXIS_PWM_OFFSET          149     // PWM偏移
#define AXIS_CURRENT_SCALE       150     // 电流缩放
#define AXIS_ADC_OFFSET          151     // ADC偏移
#define AXIS_ADC_SCALE           152     // ADC缩放
#define AXIS_POSITION_OFFSET     153     // 位置偏移
#define AXIS_VELOCITY_OFFSET     154     // 速度偏移
#define AXIS_ACCELERATION_OFFSET 155     // 加速度偏移
#define AXIS_ERROR_FLAGS         156     // 错误标志
#define AXIS_STATUS_FLAGS        157     // 状态标志
#define AXIS_MOTOR_TYPE          158     // 电机类型
#define AXIS_COMM_MODE           159     // 通讯模式
#define AXIS_DRIVE_MODE          160     // 驱动模式

// 全局参数编号 (Global Parameters)
#define GLOBAL_FIRMWARE_VERSION  0       // 固件版本
#define GLOBAL_MODULE_ADDRESS    1       // 模块地址
#define GLOBAL_BAUDRATE          2       // 波特率
#define GLOBAL_CAN_ID            3       // CAN ID
#define GLOBAL_CAN_BITRATE       4       // CAN比特率
#define GLOBAL_I2C_ADDRESS       5       // I2C地址
#define GLOBAL_CRC_ENABLE        6       // CRC使能
#define GLOBAL_TIMEOUT           7       // 超时时间
#define GLOBAL_WATCHDOG          8       // 看门狗
#define GLOBAL_BOOTLOADER        9       // 引导加载器
#define GLOBAL_ERROR_CODE        10      // 错误代码
#define GLOBAL_STATUS            11      // 全局状态

// 回零模式 (Reference Search Mode)
#define RFS_START                0       // 启动回零
#define RFS_STOP                 1       // 停止回零
#define RFS_MODE_SINGLE_SW       2       // 单开关回零
#define RFS_MODE_DOUBLE_SW       3       // 双开关回零
#define RFS_MODE_INDEX           4       // 索引回零
#define RFS_MODE_SOFT            5       // 软件回零

// IO 通道编号 (GIO/SIO Channel Numbers)
#define IO_DI0                   0       // 数字输入0
#define IO_DI1                   1       // 数字输入1
#define IO_DI2                   2       // 数字输入2
#define IO_DI3                   3       // 数字输入3
#define IO_DO0                   4       // 数字输出0
#define IO_DO1                   5       // 数字输出1
#define IO_DO2                   6       // 数字输出2
#define IO_DO3                   7       // 数字输出3
#define IO_AIN0                  25      // 模拟输入0
#define IO_AIN1                  26      // 模拟输入1
#define IO_AOUT0                 27      // 模拟输出0
#define IO_AOUT1                 28      // 模拟输出1

// 点动模式方向
#define JOG_FORWARD              0       // 正向点动
#define JOG_REVERSE              1       // 反向点动

// 斜坡模式类型
#define SLO_LINEAR               0       // 线性斜坡
#define SLO_S_CURVE              1       // S曲线斜坡
#define SLO_TRAJECTORY           2       // 轨迹模式

// 滚轮模式方向
#define ROL_FORWARD              0       // 正向滚轮
#define ROL_REVERSE              1       // 反向滚轮

// CAN 帧结构体
struct TMCL_CAN_Frame
{
    UINT id;         // CAN ID
    BYTE  data[8];    // 8字节数据域 
    BYTE  len;        // 固定为8
};

// TMCL 状态码枚举
enum  TMCL_Status 
{
    SUCCESS                     = 0x00,  // 执行成功
    INVALID_COMMAND             = 0x01,  // 无效指令
    INVALID_TYPE                = 0x02,  // 无效类型参数
    INVALID_AXIS                = 0x03,  // 无效轴
    INVALID_VALUE               = 0x04,  // 无效值
    EEPROM_LOCKED               = 0x05,  // EEPROM锁定
    COMMAND_NOT_AVAILABLE       = 0x06,  // 指令不可用
    COMMAND_LOADED              = 0x07,  // 指令已加载
    VARIABLE_NOT_AVAILABLE      = 0x08,  // 变量不可用
    BAD_CHECKSUM                = 0x09,  // 校验和错误
    NO_EEPROM                  = 0x0A,  // 无EEPROM
    WRONG_MODULE_ADDRESS        = 0x0B,  // 错误的模块地址
    MAX_EXCEEDED                = 0x0C,  // 超出最大值
    MIN_EXCEEDED                = 0x0D,  // 超出最小值
    INVALID_OPERATION           = 0x0E,  // 无效操作
    NO_MOTOR                   = 0x0F,  // 无电机
    MOTOR_FAULT                 = 0x10,  // 电机故障
    HALL_NOT_AVAILABLE          = 0x11,  // 霍尔传感器不可用
    ENCODER_NOT_AVAILABLE       = 0x12,  // 编码器不可用
    WRONG_PARAMETER_TYPE        = 0x13,  // 错误的参数类型
    PARAMETER_NOT_AVAILABLE     = 0x14,  // 参数不可用
    MOTOR_BLOCKED               = 0x15,  // 电机堵转
    OVER_TEMPERATURE            = 0x16,  // 过温
    SHORT_CIRCUIT               = 0x17,  // 短路
    UNDER_VOLTAGE               = 0x18,  // 欠压
    OVER_VOLTAGE                = 0x19,  // 过压
    I2C_TIMEOUT                 = 0x1A,  // I2C超时
    CAN_TIMEOUT                 = 0x1B,  // CAN超时
    CAN_ERROR                   = 0x1C,  // CAN错误
    INVALID_CHECKSUM            = 0x1D,  // 无效校验和
    FLASH_ERROR                 = 0x1E,  // Flash错误
    RAM_ERROR                   = 0x1F,  // RAM错误
    WATCHDOG_RESET              = 0x20,  // 看门狗复位
    NOT_INITIALIZED             = 0x21,  // 未初始化
    INITIALIZATION_ERROR        = 0x22,  // 初始化错误
    CALIBRATION_ERROR           = 0x23,  // 校准错误
    REFERENCE_NOT_FOUND         = 0x24,  // 参考点未找到
    REFERENCE_ERROR             = 0x25,  // 参考点错误
    SYNCHRONIZATION_ERROR       = 0x26,  // 同步错误
    OVER_CURRENT                = 0x27,  // 过流
    EMERGENCY_STOP              = 0x28,  // 急停
    SOFTWARE_LIMIT_POS          = 0x29,  // 软件正向限位
    SOFTWARE_LIMIT_NEG          = 0x2A,  // 软件负向限位
    HARDWARE_LIMIT_POS          = 0x2B,  // 硬件正向限位
    HARDWARE_LIMIT_NEG          = 0x2C,  // 硬件负向限位
    FOLLOWING_ERROR             = 0x2D,  // 跟随误差
    COMMUNICATION_ERROR         = 0x2E,  // 通讯错误
    INVALID_BAUDRATE            = 0x2F,  // 无效波特率
    INVALID_CAN_ID              = 0x30,  // 无效CAN ID
    INVALID_OPERATING_MODE      = 0x31,  // 无效操作模式
    INVALID_MICROSTEP_RESOLUTION= 0x32,  // 无效细分分辨率
    INVALID_CURRENT             = 0x33,  // 无效电流
    INVALID_ACCELERATION        = 0x34,  // 无效加速度
    INVALID_VELOCITY            = 0x35,  // 无效速度
    INVALID_POSITION            = 0x36,  // 无效位置
    INVALID_TRAJECTORY          = 0x37,  // 无效轨迹
    INVALID_RAMP_MODE           = 0x38,  // 无效斜坡模式
    INVALID_JOG_MODE            = 0x39,  // 无效点动模式
    INVALID_ROLL_MODE           = 0x3A,  // 无效滚轮模式
    INVALID_REFERENCE_MODE      = 0x3B,  // 无效参考模式
    INVALID_IO_CONFIGURATION    = 0x3C,  // 无效IO配置
    INVALID_ANALOG_INPUT        = 0x3D,  // 无效模拟输入
    INVALID_DIGITAL_INPUT       = 0x3E,  // 无效数字输入
    INVALID_DIGITAL_OUTPUT      = 0x3F,  // 无效数字输出
    RESERVED                    = 0x40   // 保留
};

// 接收帧解析结果结构体
struct TMCL_RxResult
{
    bool          valid;          // 帧是否合法
    BYTE       moduleAddr;     // 模块地址
    BYTE       status;         // 状态码(0=正常)
    int       value;          // 解析出的32位数据
    BYTE       command;        // 原始指令码
    BYTE       type;           // 类型参数
    BYTE       axis;           // 轴号
    
    bool isSuccess() const { return status == static_cast<BYTE>(TMCL_Status::SUCCESS); }
    bool hasError() const { return status != static_cast<BYTE>(TMCL_Status::SUCCESS); }
};

// 电机状态标志位
enum  MotorStatusFlag 
{
    TARGET_REACHED      = 0x0001,    // 目标位置到达
    REFERENCE_FOUND     = 0x0002,    // 参考点找到
    MOVING              = 0x0004,    // 正在移动
    RFS_IN_PROGRESS     = 0x0008,    // 回零进行中
    STALL_DETECTED      = 0x0010,    // 检测到堵转
    OVER_TEMPERATURE    = 0x0020,    // 过温
    UNDER_VOLTAGE       = 0x0040,    // 欠压
    OVER_VOLTAGE        = 0x0080,    // 过压
    SHORT_CIRCUIT       = 0x0100,    // 短路
    OVER_CURRENT        = 0x0200,    // 过流
    EEPROM_ERROR        = 0x0400,    // EEPROM错误
    COMM_ERROR          = 0x0800,    // 通讯错误
    SOFTWARE_LIMIT_POS  = 0x1000,    // 软件正向限位
    SOFTWARE_LIMIT_NEG  = 0x2000,    // 软件负向限位
    HARDWARE_LIMIT_POS  = 0x4000,    // 硬件正向限位
    HARDWARE_LIMIT_NEG  = 0x8000,    // 硬件负向限位
};

// IO状态结构体
struct IO_Status
{
    BYTE digitalInputs;   // 数字输入状态 (bit0-3: DI0-DI3)
    BYTE digitalOutputs;  // 数字输出状态 (bit0-3: DO0-DO3)
    WORD analogInput0;   // 模拟输入0值 (0-4095)
    WORD analogInput1;   // 模拟输入1值 (0-4095)
    WORD analogOutput0;  // 模拟输出0值 (0-4095)
    WORD analogOutput1;  // 模拟输出1值 (0-4095)
};

// 运动状态结构体
struct MotionStatus
{
    int actualPosition;   // 当前位置
    int actualVelocity;   // 当前速度
    int targetPosition;   // 目标位置
    int targetVelocity;   // 目标速度
    bool isMoving;            // 是否正在移动
    bool targetReached;       // 是否到达目标位置
};

// ====================== TMCM-3216 CAN 协议类(打包+解析) ======================
class TMCM3216_CAN_Protocol
{
public:
    // ---------------------- 底层通用打包接口 ----------------------
    static TMCL_CAN_Frame packFrame(BYTE moduleAddr, BYTE cmd, BYTE type, BYTE axis, int value)
    {
        TMCL_CAN_Frame frame{};
        frame.id  = 0x01;        // 主机发送默认CAN ID
        frame.len = 8;

        frame.data[0] = moduleAddr;
        frame.data[1] = cmd;
        frame.data[2] = type;
        frame.data[3] = axis;
        frame.data[4] = static_cast<BYTE>((value >> 24) & 0xFF);
        frame.data[5] = static_cast<BYTE>((value >> 16) & 0xFF);
        frame.data[6] = static_cast<BYTE>((value >> 8)  & 0xFF);
        frame.data[7] = static_cast<BYTE>(value & 0xFF);

        return frame;
    }

    // ---------------------- 接收帧解析接口 ----------------------
    static TMCL_RxResult parseFrame(const BYTE* rxData, BYTE dataLen)
    {
        TMCL_RxResult res{};
        res.valid = false;

        if (rxData == nullptr || dataLen != 8)
            return res;

        res.moduleAddr = rxData[0];
        res.status     = rxData[1];
        res.command    = rxData[1];
        res.type       = rxData[2];
        res.axis       = rxData[3];

        res.value = static_cast<int>(
            (static_cast<UINT>(rxData[4]) << 24) |
            (static_cast<UINT>(rxData[5]) << 16) |
            (static_cast<UINT>(rxData[6]) << 8)  |
            static_cast<UINT>(rxData[7])
        );

        res.valid = true;
        return res;
    }

    static TMCL_RxResult parseFrame(const TMCL_CAN_Frame& frame)
    {
        return parseFrame(frame.data, frame.len);
    }

    // ---------------------- 状态码解析 ----------------------
    static const char* getStatusDescription(BYTE statusCode)
    {
        switch (static_cast<TMCL_Status>(statusCode))
        {
            case TMCL_Status::SUCCESS:                     return "Success";
            case TMCL_Status::INVALID_COMMAND:             return "Invalid command";
            case TMCL_Status::INVALID_TYPE:                return "Invalid type parameter";
            case TMCL_Status::INVALID_AXIS:                return "Invalid axis";
            case TMCL_Status::INVALID_VALUE:               return "Invalid value";
            case TMCL_Status::EEPROM_LOCKED:               return "EEPROM locked";
            case TMCL_Status::COMMAND_NOT_AVAILABLE:       return "Command not available";
            case TMCL_Status::COMMAND_LOADED:              return "Command loaded";
            case TMCL_Status::VARIABLE_NOT_AVAILABLE:      return "Variable not available";
            case TMCL_Status::BAD_CHECKSUM:                return "Bad checksum";
            case TMCL_Status::NO_EEPROM:                   return "No EEPROM";
            case TMCL_Status::WRONG_MODULE_ADDRESS:        return "Wrong module address";
            case TMCL_Status::MAX_EXCEEDED:                return "Maximum exceeded";
            case TMCL_Status::MIN_EXCEEDED:                return "Minimum exceeded";
            case TMCL_Status::INVALID_OPERATION:           return "Invalid operation";
            case TMCL_Status::NO_MOTOR:                    return "No motor";
            case TMCL_Status::MOTOR_FAULT:                 return "Motor fault";
            case TMCL_Status::HALL_NOT_AVAILABLE:          return "Hall sensor not available";
            case TMCL_Status::ENCODER_NOT_AVAILABLE:       return "Encoder not available";
            case TMCL_Status::WRONG_PARAMETER_TYPE:        return "Wrong parameter type";
            case TMCL_Status::PARAMETER_NOT_AVAILABLE:     return "Parameter not available";
            case TMCL_Status::MOTOR_BLOCKED:               return "Motor blocked (stall)";
            case TMCL_Status::OVER_TEMPERATURE:            return "Over temperature";
            case TMCL_Status::SHORT_CIRCUIT:               return "Short circuit";
            case TMCL_Status::UNDER_VOLTAGE:               return "Under voltage";
            case TMCL_Status::OVER_VOLTAGE:                return "Over voltage";
            case TMCL_Status::I2C_TIMEOUT:                 return "I2C timeout";
            case TMCL_Status::CAN_TIMEOUT:                 return "CAN timeout";
            case TMCL_Status::CAN_ERROR:                   return "CAN error";
            case TMCL_Status::INVALID_CHECKSUM:            return "Invalid checksum";
            case TMCL_Status::FLASH_ERROR:                 return "Flash error";
            case TMCL_Status::RAM_ERROR:                   return "RAM error";
            case TMCL_Status::WATCHDOG_RESET:              return "Watchdog reset";
            case TMCL_Status::NOT_INITIALIZED:             return "Not initialized";
            case TMCL_Status::INITIALIZATION_ERROR:        return "Initialization error";
            case TMCL_Status::CALIBRATION_ERROR:           return "Calibration error";
            case TMCL_Status::REFERENCE_NOT_FOUND:         return "Reference not found";
            case TMCL_Status::REFERENCE_ERROR:             return "Reference error";
            case TMCL_Status::SYNCHRONIZATION_ERROR:       return "Synchronization error";
            case TMCL_Status::OVER_CURRENT:                return "Over current";
            case TMCL_Status::EMERGENCY_STOP:              return "Emergency stop";
            case TMCL_Status::SOFTWARE_LIMIT_POS:          return "Software limit positive";
            case TMCL_Status::SOFTWARE_LIMIT_NEG:          return "Software limit negative";
            case TMCL_Status::HARDWARE_LIMIT_POS:          return "Hardware limit positive";
            case TMCL_Status::HARDWARE_LIMIT_NEG:          return "Hardware limit negative";
            case TMCL_Status::FOLLOWING_ERROR:             return "Following error";
            case TMCL_Status::COMMUNICATION_ERROR:         return "Communication error";
            case TMCL_Status::INVALID_BAUDRATE:            return "Invalid baudrate";
            case TMCL_Status::INVALID_CAN_ID:              return "Invalid CAN ID";
            case TMCL_Status::INVALID_OPERATING_MODE:      return "Invalid operating mode";
            case TMCL_Status::INVALID_MICROSTEP_RESOLUTION:return "Invalid microstep resolution";
            case TMCL_Status::INVALID_CURRENT:             return "Invalid current";
            case TMCL_Status::INVALID_ACCELERATION:        return "Invalid acceleration";
            case TMCL_Status::INVALID_VELOCITY:            return "Invalid velocity";
            case TMCL_Status::INVALID_POSITION:            return "Invalid position";
            case TMCL_Status::INVALID_TRAJECTORY:          return "Invalid trajectory";
            case TMCL_Status::INVALID_RAMP_MODE:           return "Invalid ramp mode";
            case TMCL_Status::INVALID_JOG_MODE:            return "Invalid jog mode";
            case TMCL_Status::INVALID_ROLL_MODE:           return "Invalid roll mode";
            case TMCL_Status::INVALID_REFERENCE_MODE:      return "Invalid reference mode";
            case TMCL_Status::INVALID_IO_CONFIGURATION:    return "Invalid IO configuration";
            case TMCL_Status::INVALID_ANALOG_INPUT:        return "Invalid analog input";
            case TMCL_Status::INVALID_DIGITAL_INPUT:       return "Invalid digital input";
            case TMCL_Status::INVALID_DIGITAL_OUTPUT:      return "Invalid digital output";
            default:                                       return "Unknown error";
        }
    }

    static const char* getStatusDescription(const TMCL_RxResult& result)
    {
        return getStatusDescription(result.status);
    }

    // ---------------------- 状态标志位解析 ----------------------
    static bool checkStatusFlag(UINT statusValue, MotorStatusFlag flag)
    {
        return (statusValue & static_cast<UINT>(flag)) != 0;
    }

    static bool isMotorMoving(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::MOVING);
    }

    static bool isTargetReached(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::TARGET_REACHED);
    }

    static bool isReferenceFound(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::REFERENCE_FOUND);
    }

    static bool isRFSInProgress(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::RFS_IN_PROGRESS);
    }

    static bool isStallDetected(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::STALL_DETECTED);
    }

    static bool hasOverTemperature(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::OVER_TEMPERATURE);
    }

    static bool hasOverCurrent(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::OVER_CURRENT);
    }

    static bool hasShortCircuit(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::SHORT_CIRCUIT);
    }

    static bool hasLimitTriggered(UINT statusValue)
    {
        return checkStatusFlag(statusValue, MotorStatusFlag::SOFTWARE_LIMIT_POS) ||
               checkStatusFlag(statusValue, MotorStatusFlag::SOFTWARE_LIMIT_NEG) ||
               checkStatusFlag(statusValue, MotorStatusFlag::HARDWARE_LIMIT_POS) ||
               checkStatusFlag(statusValue, MotorStatusFlag::HARDWARE_LIMIT_NEG);
    }

    // ---------------------- 数据类型转换 ----------------------
    static WORD toUint16(const TMCL_RxResult& result)
    {
        return static_cast<WORD>(result.value & 0xFFFF);
    }

    static int16_t toInt16(const TMCL_RxResult& result)
    {
        return static_cast<int16_t>(result.value & 0xFFFF);
    }

    static BYTE toUint8(const TMCL_RxResult& result)
    {
        return static_cast<BYTE>(result.value & 0xFF);
    }

    static int8_t toInt8(const TMCL_RxResult& result)
    {
        return static_cast<int8_t>(result.value & 0xFF);
    }

    static float toFloat(const TMCL_RxResult& result)
    {
        float f;
        memcpy(&f, &result.value, sizeof(float));
        return f;
    }

    static bool toBool(const TMCL_RxResult& result)
    {
        return result.value != 0;
    }

    // ---------------------- 模拟量转换 ----------------------
    static float analogToVoltage(WORD analogValue, float refVoltage = 3.3f)
    {
        return (static_cast<float>(analogValue) / 4095.0f) * refVoltage;
    }

    static float analogToCurrent(WORD analogValue, float maxCurrent)
    {
        return (static_cast<float>(analogValue) / 4095.0f) * maxCurrent;
    }

    static float parseAINVoltage(const TMCL_RxResult& result, float refVoltage = 3.3f)
    {
        return analogToVoltage(toUint16(result), refVoltage);
    }

    // ---------------------- 固件版本解析 ----------------------
    static void parseFirmwareVersion(const TMCL_RxResult& result, BYTE& major, BYTE& minor, WORD& build)
    {
        UINT version = static_cast<UINT>(result.value);
        major = static_cast<BYTE>((version >> 24) & 0xFF);
        minor = static_cast<BYTE>((version >> 16) & 0xFF);
        build = static_cast<WORD>(version & 0xFFFF);
    }

    // ====================== 功能接口：参数设置 (Axis Parameters) ======================
    static TMCL_CAN_Frame setMaxSpeed(BYTE addr, BYTE axis, int speed)
    {
        return packFrame(addr, TMCL_SAP, AXIS_MAX_VEL, axis, speed);
    }

    static TMCL_CAN_Frame setAcceleration(BYTE addr, BYTE axis, int accel)
    {
        return packFrame(addr, TMCL_SAP, AXIS_MAX_ACC, axis, accel);
    }

    static TMCL_CAN_Frame setCurrent(BYTE addr, BYTE axis, BYTE current)
    {
        return packFrame(addr, TMCL_SAP, AXIS_MAX_CURRENT, axis, static_cast<int>(current));
    }

    static TMCL_CAN_Frame setMicrostep(BYTE addr, BYTE axis, WORD microStep)
    {
        return packFrame(addr, TMCL_SAP, AXIS_MICROSTEP_RES, axis, static_cast<int>(microStep));
    }

    static TMCL_CAN_Frame setHoldingCurrent(BYTE addr, BYTE axis, BYTE current)
    {
        return packFrame(addr, TMCL_SAP, AXIS_HOLDING_CURRENT, axis, static_cast<int>(current));
    }

    static TMCL_CAN_Frame setStandbyCurrent(BYTE addr, BYTE axis, BYTE current)
    {
        return packFrame(addr, TMCL_SAP, AXIS_STANDBY_CURRENT, axis, static_cast<int>(current));
    }

    static TMCL_CAN_Frame setStandbyDelay(BYTE addr, BYTE axis, UINT delayMs)
    {
        return packFrame(addr, TMCL_SAP, AXIS_STANDBY_DELAY, axis, delayMs);
    }

    static TMCL_CAN_Frame setStallThreshold(BYTE addr, BYTE axis, WORD threshold)
    {
        return packFrame(addr, TMCL_SAP, AXIS_STALL_THRESHOLD, axis, static_cast<int>(threshold));
    }

    static TMCL_CAN_Frame setSoftStop(BYTE addr, BYTE axis, bool enable)
    {
        return packFrame(addr, TMCL_SAP, AXIS_SOFT_STOP, axis, enable ? 1 : 0);
    }

    static TMCL_CAN_Frame setSoftStart(BYTE addr, BYTE axis, bool enable)
    {
        return packFrame(addr, TMCL_SAP, AXIS_SOFT_START, axis, enable ? 1 : 0);
    }

    static TMCL_CAN_Frame setRFSpeed(BYTE addr, BYTE axis, int speed)
    {
        return packFrame(addr, TMCL_SAP, AXIS_RFS_SPEED, axis, speed);
    }

    static TMCL_CAN_Frame setRFAcceleration(BYTE addr, BYTE axis, int accel)
    {
        return packFrame(addr, TMCL_SAP, AXIS_RFS_ACC, axis, accel);
    }

    static TMCL_CAN_Frame setRFOffset(BYTE addr, BYTE axis, int offset)
    {
        return packFrame(addr, TMCL_SAP, AXIS_RFS_OFFSET, axis, offset);
    }

    static TMCL_CAN_Frame setRFScanMode(BYTE addr, BYTE axis, BYTE mode)
    {
        return packFrame(addr, TMCL_SAP, AXIS_RFS_MODE, axis, static_cast<int>(mode));
    }

    static TMCL_CAN_Frame setTargetPosition(BYTE addr, BYTE axis, int pos)
    {
        return packFrame(addr, TMCL_SAP, AXIS_TARGET_POS, axis, pos);
    }

    // ====================== 功能接口：参数读取 (Axis Parameters) ======================
    static TMCL_CAN_Frame getPosition(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_ACT_POS, axis, 0);
    }

    static TMCL_CAN_Frame getActualSpeed(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_ACT_VEL, axis, 0);
    }

    static TMCL_CAN_Frame getMaxSpeed(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_MAX_VEL, axis, 0);
    }

    static TMCL_CAN_Frame getAcceleration(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_MAX_ACC, axis, 0);
    }

    static TMCL_CAN_Frame getCurrent(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_MAX_CURRENT, axis, 0);
    }

    static TMCL_CAN_Frame getMicrostep(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_MICROSTEP_RES, axis, 0);
    }

    static TMCL_CAN_Frame getTargetPosition(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_TARGET_POS, axis, 0);
    }

    static TMCL_CAN_Frame getHoldingCurrent(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_HOLDING_CURRENT, axis, 0);
    }

    static TMCL_CAN_Frame getStandbyCurrent(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_STANDBY_CURRENT, axis, 0);
    }

    static TMCL_CAN_Frame getStandbyDelay(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_STANDBY_DELAY, axis, 0);
    }

    static TMCL_CAN_Frame getStallThreshold(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_STALL_THRESHOLD, axis, 0);
    }

    static TMCL_CAN_Frame getErrorFlags(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_ERROR_FLAGS, axis, 0);
    }

    static TMCL_CAN_Frame getStatusFlags(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_STATUS_FLAGS, axis, 0);
    }

    // ====================== 功能接口：运动控制 (Motion Control) ======================
    static TMCL_CAN_Frame moveAbsolute(BYTE addr, BYTE axis, int pos)
    {
        return packFrame(addr, TMCL_MOV_ABS, 0, axis, pos);
    }

    static TMCL_CAN_Frame moveRelative(BYTE addr, BYTE axis, int step)
    {
        return packFrame(addr, TMCL_MOV_REL, 0, axis, step);
    }

    static TMCL_CAN_Frame moveVelocity(BYTE addr, BYTE axis, int velocity)
    {
        return packFrame(addr, TMCL_MOV_VEL, 0, axis, velocity);
    }

    static TMCL_CAN_Frame stopMotor(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_STOP, 0, axis, 0);
    }

    static TMCL_CAN_Frame emergencyStop(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_STOP, 0, axis, 1);
    }

    static TMCL_CAN_Frame jogForward(BYTE addr, BYTE axis, int speed)
    {
        return packFrame(addr, TMCL_JOG, JOG_FORWARD, axis, speed);
    }

    static TMCL_CAN_Frame jogReverse(BYTE addr, BYTE axis, int speed)
    {
        return packFrame(addr, TMCL_JOG, JOG_REVERSE, axis, speed);
    }

    static TMCL_CAN_Frame rollForward(BYTE addr, BYTE axis, int speed)
    {
        return packFrame(addr, TMCL_ROL, ROL_FORWARD, axis, speed);
    }

    static TMCL_CAN_Frame rollReverse(BYTE addr, BYTE axis, int speed)
    {
        return packFrame(addr, TMCL_ROL, ROL_REVERSE, axis, speed);
    }

    static TMCL_CAN_Frame setRampLinear(BYTE addr, BYTE axis, int duration)
    {
        return packFrame(addr, TMCL_SLO, SLO_LINEAR, axis, duration);
    }

    static TMCL_CAN_Frame setRampSCurve(BYTE addr, BYTE axis, int duration)
    {
        return packFrame(addr, TMCL_SLO, SLO_S_CURVE, axis, duration);
    }

    static TMCL_CAN_Frame setRampTrajectory(BYTE addr, BYTE axis, int param)
    {
        return packFrame(addr, TMCL_SLO, SLO_TRAJECTORY, axis, param);
    }

    // ====================== 功能接口：多轴同步 (Multi-Axis Sync) ======================
    static TMCL_CAN_Frame setSyncTargetPos(BYTE addr, BYTE axis, int pos)
    {
        return moveAbsolute(addr, axis, pos);
    }

    static TMCL_CAN_Frame startSyncMove(BYTE addr)
    {
        return packFrame(addr, TMCL_START, 0, 0, 0);
    }

    // ====================== 功能接口：回零控制 (Reference Search) ======================
    static TMCL_CAN_Frame homeStart(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_RFS, RFS_START, axis, 0);
    }

    static TMCL_CAN_Frame homeStop(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_RFS, RFS_STOP, axis, 0);
    }

    static TMCL_CAN_Frame getHomeState(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_GAP, AXIS_RFS_STATE, axis, 0);
    }

    static TMCL_CAN_Frame homeSingleSwitch(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_RFS, RFS_MODE_SINGLE_SW, axis, 0);
    }

    static TMCL_CAN_Frame homeDoubleSwitch(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_RFS, RFS_MODE_DOUBLE_SW, axis, 0);
    }

    static TMCL_CAN_Frame homeIndex(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_RFS, RFS_MODE_INDEX, axis, 0);
    }

    static TMCL_CAN_Frame homeSoft(BYTE addr, BYTE axis)
    {
        return packFrame(addr, TMCL_RFS, RFS_MODE_SOFT, axis, 0);
    }

    // 给 Control 使用：通用打包（已存在 packFrame，这里再暴露一下作为显式静态接口）
    static TMCL_CAN_Frame packFrameEx(BYTE moduleAddr, BYTE cmd, BYTE type, BYTE axis, int value)
    {
        return packFrame(moduleAddr, cmd, type, axis, value);
    }

    // ====================== 功能接口：IO 控制 (Digital/Analog IO) ======================
    static TMCL_CAN_Frame getAIN0(BYTE addr)
    {
        return packFrame(addr, TMCL_GIO, IO_AIN0, 0, 0);
    }

    static TMCL_CAN_Frame getAIN1(BYTE addr)
    {
        return packFrame(addr, TMCL_GIO, IO_AIN1, 0, 0);
    }

    static TMCL_CAN_Frame getDI0(BYTE addr)
    {
        return packFrame(addr, TMCL_GIO, IO_DI0, 0, 0);
    }

    static TMCL_CAN_Frame getDI1(BYTE addr)
    {
        return packFrame(addr, TMCL_GIO, IO_DI1, 0, 0);
    }

    static TMCL_CAN_Frame getDI2(BYTE addr)
    {
        return packFrame(addr, TMCL_GIO, IO_DI2, 0, 0);
    }

    static TMCL_CAN_Frame getDI3(BYTE addr)
    {
        return packFrame(addr, TMCL_GIO, IO_DI3, 0, 0);
    }

    static TMCL_CAN_Frame setDO0(BYTE addr, bool state)
    {
        return packFrame(addr, TMCL_SIO, IO_DO0, 0, state ? 1 : 0);
    }

    static TMCL_CAN_Frame setDO1(BYTE addr, bool state)
    {
        return packFrame(addr, TMCL_SIO, IO_DO1, 0, state ? 1 : 0);
    }

    static TMCL_CAN_Frame setDO2(BYTE addr, bool state)
    {
        return packFrame(addr, TMCL_SIO, IO_DO2, 0, state ? 1 : 0);
    }

    static TMCL_CAN_Frame setDO3(BYTE addr, bool state)
    {
        return packFrame(addr, TMCL_SIO, IO_DO3, 0, state ? 1 : 0);
    }

    static TMCL_CAN_Frame setAOUT0(BYTE addr, WORD value)
    {
        return packFrame(addr, TMCL_SIO, IO_AOUT0, 0, static_cast<int>(value));
    }

    static TMCL_CAN_Frame setAOUT1(BYTE addr, WORD value)
    {
        return packFrame(addr, TMCL_SIO, IO_AOUT1, 0, static_cast<int>(value));
    }

    // ====================== 功能接口：全局参数 (Global Parameters) ======================
    static TMCL_CAN_Frame getFirmwareVersion(BYTE addr)
    {
        return packFrame(addr, TMCL_GGP, GLOBAL_FIRMWARE_VERSION, 0, 0);
    }

    static TMCL_CAN_Frame getModuleAddress(BYTE addr)
    {
        return packFrame(addr, TMCL_GGP, GLOBAL_MODULE_ADDRESS, 0, 0);
    }

    static TMCL_CAN_Frame getCANID(BYTE addr)
    {
        return packFrame(addr, TMCL_GGP, GLOBAL_CAN_ID, 0, 0);
    }

    static TMCL_CAN_Frame getErrorCode(BYTE addr)
    {
        return packFrame(addr, TMCL_GGP, GLOBAL_ERROR_CODE, 0, 0);
    }

    static TMCL_CAN_Frame getGlobalStatus(BYTE addr)
    {
        return packFrame(addr, TMCL_GGP, GLOBAL_STATUS, 0, 0);
    }

    static TMCL_CAN_Frame setModuleAddress(BYTE addr, BYTE newAddr)
    {
        return packFrame(addr, TMCL_SGP, GLOBAL_MODULE_ADDRESS, 0, static_cast<int>(newAddr));
    }

    static TMCL_CAN_Frame setCANID(BYTE addr, UINT canId)
    {
        return packFrame(addr, TMCL_SGP, GLOBAL_CAN_ID, 0, canId);
    }

    static TMCL_CAN_Frame setCANBitrate(BYTE addr, UINT bitrate)
    {
        return packFrame(addr, TMCL_SGP, GLOBAL_CAN_BITRATE, 0, bitrate);
    }

    // ====================== 功能接口：模块控制 (Module Control) ======================
    static TMCL_CAN_Frame resetModule(BYTE addr)
    {
        return packFrame(addr, TMCL_RESET, 0, 0, 0);
    }

    static TMCL_CAN_Frame getFirmwareInfo(BYTE addr)
    {
        return packFrame(addr, TMCL_GET_FIRMWARE, 0, 0, 0);
    }
};