#include "slip_codec.h"

std::vector<uint8_t> SlipCodec::encode(const std::vector<uint8_t>& in_data) {
    std::vector<uint8_t> out;
    out.push_back(SLIP_END);
    for (uint8_t b : in_data) {
        if (b == SLIP_END) {
            out.push_back(SLIP_ESC);
            out.push_back(SLIP_ESC_END);
        } else if (b == SLIP_ESC) {
            out.push_back(SLIP_ESC);
            out.push_back(SLIP_ESC_ESC);
        } else {
            out.push_back(b);
        }
    }
    out.push_back(SLIP_END);
    return out;
}

std::vector<uint8_t> SlipCodec::decode(const std::vector<uint8_t>& in_data) {
    std::vector<uint8_t> out;
    bool escape = false;
    for (uint8_t b : in_data) {
        if (b == SLIP_END) continue;
        if (escape) {
            if (b == SLIP_ESC_END) out.push_back(SLIP_END);
            else if (b == SLIP_ESC_ESC) out.push_back(SLIP_ESC);
            escape = false;
        } else if (b == SLIP_ESC) {
            escape = true;
        } else {
            out.push_back(b);
        }
    }
    return out;
}