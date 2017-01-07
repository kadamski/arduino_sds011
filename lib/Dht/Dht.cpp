#include "Dht.h"

using namespace dht;

Dht::Dht(uint8_t pin) : _pin(pin), _lastreadtime(-2000), _lastresult(false)
{
}

void Dht::begin(void)
{
    pinMode(_pin, INPUT_PULLUP);
}

uint16_t Dht::get_humidity(void)
{
    _read_packet_retries();
    return _data[1] + (_data[0]<<8);
}

int16_t Dht::get_temperature(void)
{
    int16_t t;
    _read_packet_retries();

    t = _data[3] + ((_data[2] & 0x7E)<<8);

    return (_data[2] & 0x80) ? -t : t;
}

uint32_t Dht::_pulse_in(bool l, uint32_t timeout)
{
    uint32_t ret = 0;

    while (digitalRead(_pin) == l) {
        if (ret++ >= timeout) {
            return 0;
        }
    }

    return ret;
}
bool Dht::_read_packet_retries(int r)
{
    for (int i = 0; i < r; i++) {
        if (_read_packet()) {
            return true;
        }
    }
    return false;
}

bool Dht::_read_packet(void)
{
    uint32_t cur = millis();
    uint_fast16_t buf[80];

    if (cur - _lastreadtime < 2000) {
        return _lastresult;
    }

    _lastreadtime = cur;

    memset(_data, 0, sizeof(_data));

    digitalWrite(_pin, HIGH);
    delay(150);

    noInterrupts();

    // start sequence
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);

    pinMode(_pin, INPUT_PULLUP);
    if (_pulse_in(LOW) == 0 || _pulse_in(HIGH) == 0) {
        Serial.println("Timeout on startup");
        _lastresult = false;
        return false;
    }

    for (int i=0; i<80; i+=2) {
        buf[i] = _pulse_in(LOW);
        buf[i+1] = _pulse_in(HIGH);
    }

    interrupts();

    for (int i=0; i<80; i+=2) {
        if (buf[i] == 0 || buf[i+1] == 0) {
            Serial.print(i/2);
            Serial.println(": timeout ");
            _lastresult = false;
            return false;
        }

        _data[i/16] <<= 1;
        if (buf[i] < buf[i+1]) {
            _data[i/16] |= 1;
        }

    }

    if (((_data[0] + _data[1] + _data[2] + _data[3]) & 0xFF) == _data[4]) {
        _lastresult = true;
    } else {
        Serial.println("CRC NOK");
        _lastresult = false;
    }

    return _lastresult;
}

