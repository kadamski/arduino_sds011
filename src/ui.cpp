#include "Pcd8544.h"
#include "Dht.h"
#include "ui.h"

static const int PM25_NORM=25;
static const int PM10_NORM=40;

extern pcd8544::Pcd8544 display;

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

void display_template(void)
{
    display.clear();
    display.setCursor(0, 0);
    display.println("    2.5   10");
    display.print("ug ");
    display.setCursor(0, 2);
    display.print("%  ");

    display.setCursor(0, 3);
    display.print("t:  ");
    display.setCursor(0, 4);
    display.print("h:  ");

    display.setCursor(0, 5);
}

void display_dust(uint16_t pm25, uint16_t pm10)
{
    display.setCursor(3*7, 1);

    display.print(val_to_str(pm25).c_str());

    display.setCursor(8*7, 1);
    display.println(val_to_str(pm10).c_str());

    display.setCursor(3*7, 2);
    display.print(val_to_str((10*pm25/PM25_NORM)*10).c_str());
    display.setCursor(8*7, 2);
    display.print(val_to_str((10*pm10/PM10_NORM)*10).c_str());

    display.setCursor(0, 5);
}

void display_temp(int16_t t, int16_t h)
{
    display.setCursor(4*7, 3);
    if (t == INT16_MIN) {
        display.print("E");
    } else {
        display.print(val_to_str(t).c_str());
    }
    display.print("C");

    display.setCursor(4*7, 4);
    if (h == INT16_MIN) {
        display.print("E");
    } else {
        display.print(val_to_str(h).c_str());
    }
    display.println("%");

    display.setCursor(0, 5);
}

void display_status_wifi(enum wifi_status s)
{
    display.setCursor(0, 5);

    display.print("W");
    switch(s) {
        case WIFI_CONNECTING:
            display.print("?");
            break;
        case WIFI_ERROR:
            display.print("!");
            break;
        case WIFI_OK:
            display.print(".");
            break;
        default:
            display.print(" ");
            break;
    }
}

void display_status_sensor(enum sensor_status s)
{
    display.setCursor(2*7, 5);

    display.print("D");
    switch(s) {
        case SENSOR_ERROR:
            display.print("!");
            break;
        case SENSOR_OK:
            display.print(".");
            break;
        default:
            display.print(" ");
            break;
    }
}

void display_status_send(enum send_status s)
{
    display.setCursor(4*7, 5);

    display.print("U");
    switch(s) {
        case SEND_START:
            display.print("?");
            break;
        case SEND_ERROR:
            display.print("!");
            break;
        case SEND_OK:
            display.print(".");
            break;
        default:
            display.print(" ");
            break;
    }
}

void display_status_dht(enum dht::dht_status s)
{
    display.setCursor(6*7, 5);

    display.print("H");
    switch(s) {
        case dht::DHT_OK:
            display.print(".");
            break;
        case dht::DHT_BADCRC:
            display.print("C");
            break;
        case dht::DHT_TIMEOUT_START:
            display.print("!");
            break;
        case dht::DHT_TIMEOUT:
            display.print("T");
            break;
        default:
            display.print(" ");
            break;
    }
}
