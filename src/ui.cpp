#include "Pcd8544.h"

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

void display_data(uint16_t pm25, uint16_t pm10, int16_t t, uint16_t h)
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

