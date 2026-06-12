#ifndef GMR_API_H
#define GMR_API_H
#include "coap_client.h"
#include "protocol_def.h"
#include <string>

extern CoAPClient g_gmr_client;

bool gmr_api_init(const std::string& dev, int baud = 115200);

bool gen_put_coil_dc(double val);
bool gen_get_coil_dc(double& out_val);
bool gen_get_count(int& out_cnt);

bool gmr_start();
bool gmr_stop();
bool gmr_tare();
bool gmr_get_time(double& out_sec);

bool sensor_get_scan_period(double& out_val);
bool sensor_get_mr(double& out_val);
bool sensor_get_z(double& out_val);
bool sensor_set_excluded(bool en);

bool heater_get_count(int& out_cnt);
bool heater_set_ctl_period(double val);
bool heater_set_temp(int idx, double set_temp);
bool heater_get_temp(int idx, double& out_temp);
bool heater_set_pid(int idx, double kp, double ki, double kd);

#endif