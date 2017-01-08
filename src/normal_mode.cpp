#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "Sds011.h"
#include "Pcd8544.h"
#include "Expander.h"
#include "Dht.h"
#include "config.h"
#include "send.h"
#include "ui.h"

extern expander::Expander expand;
extern struct Configuration config;
extern sds011::Sds011 sensor;

dht::Dht dht22(14);
Ticker timer1;
Ticker timer2;

static const int SAMPLES=10;

static void turnOff(void)
{
    expand.digitalWrite(0, HIGH);
}

static void turnOn(void)
{
    uint8_t b = expand.readByte();

    if ((b & 0b10) != 0b10) {
        timer2.once_ms(5000, turnOff);
        expand.digitalWrite(0, LOW);
    }
}

static void iter()
{
    timer1.once_ms(30, turnOn);
}

void normal_setup(void)
{
    dht22.begin();
    expand.attachInterrupt(iter);

    WiFi.mode(WIFI_STA);

    sensor.set_sleep(false);
    sensor.set_mode(sds011::QUERY);
    sensor.set_sleep(true);
}

void normal_loop(void)
{
    int pm25, pm10;
    bool ok;

    int16_t t = dht22.get_temperature();
    uint16_t h = dht22.get_humidity();

    display_template();
    display_temp(t, h);

    WiFi.begin(config.wifi_ssid, config.wifi_pass);
    display_status_wifi(WIFI_CONNECTING);

    sensor.set_sleep(false);
    delay(1000);
    ok = sensor.query_data_auto(&pm25, &pm10, SAMPLES);
    sensor.set_sleep(true);

    if (ok) {
        display_dust(pm25, pm10);
        display_status_sensor(SENSOR_OK);
    } else {
        display_status_sensor(SENSOR_ERROR);
        pm25 = pm10 = 0;
    }

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        display_status_wifi(WIFI_ERROR);
    } else {
        display_status_wifi(WIFI_OK);
        display_status_send(SEND_START);
        int res = send_ts(pm25, pm10, t, h);
        if (res == 200) {
            display_status_send(SEND_OK);
        } else {
            display_status_send(SEND_ERROR);
        }
    }

    WiFi.disconnect();
    delay(100);
    WiFi.forceSleepBegin(); // Use WiFi.forceSleepWake() to enable wifi

    delay(3000);
    ESP.deepSleep(1000*1000*config.sleep_time, WAKE_RF_DEFAULT);
}
