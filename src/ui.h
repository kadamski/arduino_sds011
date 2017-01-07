#pragma once

enum wifi_status {
    WIFI_CONNECTING,
    WIFI_OK,
    WIFI_ERROR
};

void display_template(void);
void display_dust(uint16_t pm25, uint16_t pm10);
void display_temp(int16_t t, uint16_t h);
void display_status_wifi(enum wifi_status s);
