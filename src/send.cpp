#include <ESP8266HTTPClient.h>
#include "send.h"
#include "config.h"

extern struct Configuration config;

int send_ts(uint16_t pm25, uint16_t pm10, int16_t t, uint16_t h)
{
    HTTPClient http;
    char buf[1024];

    if (strlen(config.ts_api_key)==0) {
        return -1;
    }

    char str_pm25[6];
    char str_pm10[6];
    char str_tmp[6];
    char str_hum[6];
    dtostrf((double)pm25/10.0, 4, 1, str_pm25);
    dtostrf((double)pm10/10.0, 4, 1, str_pm10);
    dtostrf((double)t/10.0, 4, 1, str_tmp);
    dtostrf((double)h/10.0, 4, 1, str_hum);

    snprintf(buf, sizeof(buf), "http://api.thingspeak.com/update?api_key=%s&field1=%s&field2=%s&field3=%s&field4=%s",
            config.ts_api_key, str_pm25, str_pm10, (int)str_tmp, (int)str_hum);

    http.begin(buf);

    return http.GET();
}
