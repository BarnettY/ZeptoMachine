#include "gmr_api.h"

CoAPClient g_gmr_client;

bool gmr_api_init(const std::string& dev, int baud) {
    return g_gmr_client.init(dev, baud);
}

bool gen_put_coil_dc(double val) {
    return g_gmr_client.request(CoAPMethod::PUT, "/gen/coil_dc", CoAPContentType::JSON, f2json(val));
}
bool gen_get_coil_dc(double& out_val) {
    std::string resp;
    if (!g_gmr_client.request(CoAPMethod::GET, "/gen/coil_dc", CoAPContentType::JSON, "")) return false;
    if (!g_gmr_client.recv_response(resp)) return false;
    out_val = json2f(resp);
    return true;
}
bool gen_get_count(int& out_cnt) {
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, "/gen/count", CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_cnt = json2i(resp);
    return true;
}

bool gmr_start() {
    return g_gmr_client.request(CoAPMethod::PUT, "/gmr", CoAPContentType::OCTET_STREAM, "start");
}
bool gmr_stop() {
    return g_gmr_client.request(CoAPMethod::PUT, "/gmr", CoAPContentType::OCTET_STREAM, "stop");
}
bool gmr_tare() {
    return g_gmr_client.request(CoAPMethod::PUT, "/gmr", CoAPContentType::OCTET_STREAM, "tare");
}
bool gmr_get_time(double& out_sec) {
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, "/gmr/time", CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_sec = json2f(resp);
    return true;
}

bool sensor_get_scan_period(double& out_val) {
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, "/gmr/sensors/scan_period", CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_val = json2f(resp);
    return true;
}
bool sensor_get_mr(double& out_val) {
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, "/gmr/sensors/mr", CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_val = json2f(resp);
    return true;
}
bool sensor_get_z(double& out_val) {
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, "/gmr/sensors/z", CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_val = json2f(resp);
    return true;
}
bool sensor_set_excluded(bool en) {
    return g_gmr_client.request(CoAPMethod::PUT, "/gmr/sensors/excluded", CoAPContentType::JSON, b2json(en));
}

bool heater_get_count(int& out_cnt) {
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, "/heater/count", CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_cnt = json2i(resp);
    return true;
}
bool heater_set_ctl_period(double val) {
    return g_gmr_client.request(CoAPMethod::PUT, "/heater/ctl_period", CoAPContentType::JSON, f2json(val));
}
bool heater_set_temp(int idx, double set_temp) {
    std::string uri = "/heater/h" + std::to_string(idx) + "/setpoint";
    return g_gmr_client.request(CoAPMethod::PUT, uri, CoAPContentType::JSON, f2json(set_temp));
}
bool heater_get_temp(int idx, double& out_temp) {
    std::string uri = "/heater/h" + std::to_string(idx) + "/temp";
    std::string resp;
    g_gmr_client.request(CoAPMethod::GET, uri, CoAPContentType::JSON, "");
    if (!g_gmr_client.recv_response(resp)) return false;
    out_temp = json2f(resp);
    return true;
}
bool heater_set_pid(int idx, double kp, double ki, double kd) {
    std::string uriKp = "/heater/h" + std::to_string(idx) + "/kp";
    std::string uriKi = "/heater/h" + std::to_string(idx) + "/ki";
    std::string uriKd = "/heater/h" + std::to_string(idx) + "/kd";
    g_gmr_client.request(CoAPMethod::PUT, uriKp, CoAPContentType::JSON, f2json(kp));
    g_gmr_client.request(CoAPMethod::PUT, uriKi, CoAPContentType::JSON, f2json(ki));
    g_gmr_client.request(CoAPMethod::PUT, uriKd, CoAPContentType::JSON, f2json(kd));
    return true;
}