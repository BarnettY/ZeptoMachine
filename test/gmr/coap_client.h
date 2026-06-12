#ifndef COAP_CLIENT_H
#define COAP_CLIENT_H
#include "protocol_def.h"
#include "uart_serial.h"
#include "slip_codec.h"
#include <string>

class CoAPClient {
private:
    UartSerial uart_;
public:
    CoAPClient() = default;

    bool init(const std::string& dev, int baud = 115200);

    bool request(CoAPMethod method,
                 const std::string& uri,
                 CoAPContentType ct,
                 const std::string& payload = "");

    bool recv_response(std::string& out_payload);
};

#endif