#include "gmr_api.h"
#include <iostream>

int main() {
    if (!gmr_api_init("/dev/ttyUSB0", 115200)) {
        std::cerr << "串口打开失败\n";
        return -1;
    }

    gmr_start();
    gen_put_coil_dc(0.55);
    heater_set_temp(1, 35.0);
    heater_set_pid(1, 2.0, 0.1, 0.05);

    double t;
    sensor_get_scan_period(t);
    std::cout << "传感器扫描周期: " << t << "s\n";

    gmr_stop();
    return 0;
}