#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "Sds011.h"
#include "Pcd8544.h"
#include "Expander.h"
#include "Dht.h"
#include "config.h"
#include "send.h"

extern pcd8544::Pcd8544 display;
extern expander::Expander expand;
extern struct Configuration config;
extern sds011::Sds011 sensor;

dht::Dht dht22(14);
Ticker timer1;
Ticker timer2;

static const int PM25_NORM=25;
static const int PM10_NORM=40;
static const int SAMPLES=10;

static String val_to_str(uint16_t v)
{
    String r;

    r = String(v/10);
    if (v < 1000 && v%10) {
        r += String(".") + String(v%10);
    }

    for (int i = 4 - r.length(); i > 0; i--) {
        r = String(" ") + r;
    }

    return r;
}

static void display_data(uint16_t pm25, uint16_t pm10, int16_t t, uint16_t h)
{
    display.clear();
    display.setCursor(0, 0);

    display.println("    2.5   10");

    display.print("ug ");
    display.print(val_to_str(pm25).c_str());

    display.setCursor(8*7, 1);
    display.println(val_to_str(pm10).c_str());

    display.print("%  ");
    display.print(val_to_str((10*pm25/PM25_NORM)*10).c_str());
    display.setCursor(8*7, 2);
    display.print(val_to_str((10*pm10/PM10_NORM)*10).c_str());

    display.setCursor(0, 3);
    display.print("t:  ");
    display.print(val_to_str(t).c_str());
    display.print("C");
    display.setCursor(0, 4);
    display.print("h:  ");
    display.print(val_to_str(h).c_str());
    display.println("%");
}

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

    WiFi.begin(config.wifi_ssid, config.wifi_pass);

    sensor.set_sleep(false);
    delay(1000);
    ok = sensor.query_data_auto(&pm25, &pm10, SAMPLES);
    sensor.set_sleep(true);

    int16_t t = dht22.get_temperature();
    uint16_t h = dht22.get_humidity();

    if (ok) {
        display_data(pm25, pm10, t, h);
        display.print(".");
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            display.println("Wifi Connection Failed!");
        }
        display.print(".");
        display.println(String(send_ts(pm25, pm10, t, h)).c_str());
    } else {
        display.clear();
        display.setCursor(0, 0);
        display.println("NO SENSOR!");
    }

    WiFi.disconnect();
    delay(100);
    WiFi.forceSleepBegin(); // Use WiFi.forceSleepWake() to enable wifi

    delay(3000);
    ESP.deepSleep(1000*1000*config.sleep_time, WAKE_RF_DEFAULT);
}
