#ifndef PROTOCOL_DEF_H
#define PROTOCOL_DEF_H
#include <cstdint>
#include <string>
#include <vector>

constexpr uint8_t SLIP_END     = 0xC0;
constexpr uint8_t SLIP_ESC     = 0xDB;
constexpr uint8_t SLIP_ESC_END = 0xDC;
constexpr uint8_t SLIP_ESC_ESC = 0xDD;

enum class CoAPMethod : uint8_t {
    GET    = 1,
    PUT    = 3,
    DELETE = 4
};

enum class CoAPContentType : uint16_t {
    JSON         = 50,
    PROTOBUF     = 60,
    OCTET_STREAM = 42
};

std::string f2json(double v);
std::string i2json(int v);
std::string b2json(bool v);
double      json2f(const std::string& s);
int         json2i(const std::string& s);
bool        json2b(const std::string& s);

#endif