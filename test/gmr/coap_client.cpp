#include "coap_client.h"
#include <cstdlib>
#include <vector>

bool CoAPClient::init(const std::string& dev, int baud) {
    return uart_.open(dev, baud);
}

bool CoAPClient::request(CoAPMethod method,
                         const std::string& uri,
                         CoAPContentType ct,
                         const std::string& payload) {
    if (!uart_.is_opened()) return false;

    std::vector<uint8_t> raw;
    uint16_t msg_id = rand() % 65535;

    raw.push_back(0x40);
    raw.push_back(static_cast<uint8_t>(method));
    raw.push_back(msg_id >> 8);
    raw.push_back(msg_id & 0xFF);

    raw.insert(raw.end(), uri.begin(), uri.end());
    raw.push_back(0xFF);
    raw.insert(raw.end(), payload.begin(), payload.end());

    auto slip_pkt = SlipCodec::encode(raw);
    return uart_.write(slip_pkt) > 0;
}

bool CoAPClient::recv_response(std::string& out_payload) {
    std::vector<uint8_t> recv;
    if (uart_.read(recv, 100) <= 0) return false;

    auto raw = SlipCodec::decode(recv);
    if (raw.size() < 4) return false;

    size_t sep = 4;
    while (sep < raw.size() && raw[sep] != 0xFF) sep++;
    if (sep >= raw.size()) return false;

    out_payload.assign(raw.begin() + sep + 1, raw.end());
    return true;
}