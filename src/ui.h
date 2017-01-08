#pragma once

enum wifi_status {
    WIFI_CONNECTING,
    WIFI_OK,
    WIFI_ERROR
};

enum sensor_status {
    SENSOR_OK,
    SENSOR_ERROR
};

enum send_status {
    SEND_START,
    SEND_OK,
    SEND_ERROR
};

void display_template(void);
void display_dust(uint16_t pm25, uint16_t pm10);
void display_temp(int16_t t, int16_t h);
void display_status_wifi(enum wifi_status s);
void display_status_sensor(enum sensor_status s);
void display_status_send(enum send_status s);
