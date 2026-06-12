#ifndef SLIP_CODEC_H
#define SLIP_CODEC_H
#include "protocol_def.h"

class SlipCodec {
public:
    static std::vector<uint8_t> encode(const std::vector<uint8_t>& in_data);
    static std::vector<uint8_t> decode(const std::vector<uint8_t>& in_data);
};

#endif