#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include "Pcd8544.h"
#include "Sds011.h"
#include "config.h"

#define WIFI_AP_SSID "ESPdust"
#define WIFI_AP_PASS "dustdust"

extern pcd8544::Pcd8544 display;
extern struct Configuration config;
extern sds011::Sds011 sensor;

ESP8266WebServer server(80);

static void on_root(void)
{
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
        server.send(200, "text/plain", "No index.html!");
        return;
    }

    String s = file.readString();
    s.replace("{wifi_staip}", WiFi.localIP().toString());
    s.replace("{wifi_stassid}", config.wifi_ssid);
    s.replace("{wifi_stapass}", config.wifi_pass);
    s.replace("{wifi_apip}", WiFi.softAPIP().toString().c_str());
    s.replace("{ts_api_key}", config.ts_api_key);
    s.replace("{banner}", config.banner);
    s.replace("{sleep_time}", String(config.sleep_time));

    server.send(200, "text/html", s.c_str());
}

static void on_form(void)
{
    if (server.method() != HTTP_POST) {
        server.send(500, "text/plain", "Only POST supported");
    }

    if (!server.hasArg("banner") || !server.hasArg("wifi_stassid") ||
        !server.hasArg("wifi_stapass") || !server.hasArg("ts_api_key")) {
        server.send(500, "text/plain", "Bad form");
    }

    String val = server.arg("banner");
    config.banner = strdup(val.c_str());

    val = server.arg("wifi_stassid");
    config.wifi_ssid = strdup(val.c_str());

    val = server.arg("wifi_stapass");
    config.wifi_pass = strdup(val.c_str());

    val = server.arg("ts_api_key");
    config.ts_api_key = strdup(val.c_str());

    val = server.arg("sleep_time");
    if (atoi(val.c_str()) > 19) {
        config.sleep_time = atoi(val.c_str());
    }

    save_config();

    server.sendHeader("Location", "/");
    server.send(301, "text/html", "Moved Permanently");
}

static void on_update_post(void)
{
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        WiFiUDP::stopAll();
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        Update.begin(maxSketchSpace);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        Update.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        Update.end(true);
    }
    yield();
}

void setup_setup(void)
{
    sensor.set_sleep(true);

    display.clear();
    display.setCursor(0, 0);
    display.println("WIFI");

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
    display.println(WiFi.softAPIP().toString().c_str());

    WiFi.begin(config.wifi_ssid, config.wifi_pass);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        display.println("Wifi Connection Failed!");
    }

    display.println(WiFi.localIP().toString().c_str());

    server.on("/", on_root);
    server.on("/form", on_form);

    server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, on_update_post);

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        display.setCursor(0, 5);
        display.println(String(progress / (total / 100)).c_str());
    });
    ArduinoOTA.begin();

    server.begin();

    SPIFFS.begin();
}

void setup_loop(void)
{
    ArduinoOTA.handle();
    server.handleClient();
    delay(1000);
}
