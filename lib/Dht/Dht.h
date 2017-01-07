#ifndef _DHT_H
#define _DHT_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#define RETRIES 5

namespace dht {
    class Dht
    {
        public:
            Dht(uint8_t pin);
            void begin(void);
            uint16_t get_humidity(void);
            int16_t get_temperature(void);

        private:
            uint8_t _pin;
            int32_t _lastreadtime;
            bool _lastresult;
            bool _read_packet(void);
            bool _read_packet_retries(int r=RETRIES);
            uint32_t _pulse_in(bool, uint32_t timeout=200);
            uint8_t _data[5];
    };
}
#endif
