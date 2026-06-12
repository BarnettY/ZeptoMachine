#ifndef UART_SERIAL_H
#define UART_SERIAL_H
#include <string>
#include <vector>

class UartSerial {
private:
    int fd_ = -1;
public:
    UartSerial() = default;
    ~UartSerial();

    bool open(const std::string& dev, int baud = 115200);
    void close();
    bool is_opened() const;

    ssize_t write(const std::vector<uint8_t>& data);
    ssize_t read(std::vector<uint8_t>& data, int timeout_ms = 100);
};

#endif