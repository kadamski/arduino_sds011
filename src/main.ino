#include <SoftwareSerial.h>
#include <FS.h>
#include "Pcd8544.h"
#include "Sds011.h"
#include "Expander.h"

#include "config.h"
#include "normal_mode.h"
#include "setup_mode.h"

#include <Wire.h>

extern struct Configuration config;

pcd8544::Pcd8544 display(13, 12, 14);
sds011::Sds011 sensor(Serial);
expander::Expander expand(0x38);

static bool set_press;

void setup()
{
    bool clear = true;
    char *banner;
    String r = ESP.getResetReason();

    expand.begin();
    Serial.begin(9600);
    SPIFFS.begin();
    display.begin();

    if (r == "Deep-Sleep Wake") {
        clear = false;
    }

    if (!load_config()) {
        banner = strdup("NO CONFIG");
    } else {
        banner = config.banner;
    }

    if (clear) {
        display.clear();
        display.setCursor(0,0);
        display.println("Hello");
        display.println(banner);
    }

    delay(1000);
    set_press = (expand.readByte() & 0b10 ) != 0b10;

    if (set_press) {
        setup_setup();
    } else {
        normal_setup();
    }
}

void loop()
{
    if (set_press) {
        setup_loop();
    } else {
        normal_loop();
    }
}
