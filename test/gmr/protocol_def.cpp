#include "protocol_def.h"
#include <sstream>

std::string f2json(double v) {
    std::ostringstream oss;
    oss.precision(6);
    oss << v;
    return oss.str();
}

std::string i2json(int v) {
    return std::to_string(v);
}

std::string b2json(bool v) {
    return v ? "true" : "false";
}

double json2f(const std::string& s) {
    return std::stod(s);
}

int json2i(const std::string& s) {
    return std::stoi(s);
}

bool json2b(const std::string& s) {
    return s == "true" || s == "1";
}