#include "uart_serial.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <cstring>

UartSerial::~UartSerial() { close(); }

bool UartSerial::open(const std::string& dev, int baud) {
    fd_ = ::open(dev.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ < 0) return false;

    struct termios opt{};
    tcgetattr(fd_, &opt);
    cfsetispeed(&opt, baud);
    cfsetospeed(&opt, baud);

    opt.c_cflag |= CLOCAL | CREAD;
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~CSTOPB;
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opt.c_oflag &= ~OPOST;

    tcsetattr(fd_, TCSANOW, &opt);
    return true;
}

void UartSerial::close() {
    if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
}

bool UartSerial::is_opened() const { return fd_ >= 0; }

ssize_t UartSerial::write(const std::vector<uint8_t>& data) {
    if (!is_opened()) return -1;
    return ::write(fd_, data.data(), data.size());
}

ssize_t UartSerial::read(std::vector<uint8_t>& data, int timeout_ms) {
    if (!is_opened()) return -1;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd_, &rfds);
    timeval tv{0, timeout_ms * 1000};

    int ret = select(fd_ + 1, &rfds, nullptr, nullptr, &tv);
    if (ret <= 0) return 0;

    uint8_t buf[512];
    ssize_t n = ::read(fd_, buf, sizeof(buf));
    if (n > 0) data.assign(buf, buf + n);
    return n;
}