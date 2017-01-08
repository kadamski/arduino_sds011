#include "Dht.h"

using namespace dht;

Dht::Dht(uint8_t pin) : _pin(pin), _lastreadtime(-2000), _lastresult(DHT_NONE)
{
}

void Dht::begin(void)
{
    pinMode(_pin, INPUT_PULLUP);
}

int16_t Dht::get_humidity(void)
{
    if (_read_packet_retries() != DHT_OK) {
        return INT16_MIN;
    }
    return _data[1] + (_data[0]<<8);
}

int16_t Dht::get_temperature(void)
{
    int16_t t;
    if (_read_packet_retries() != DHT_OK) {
        return INT16_MIN;
    }

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

dht_status Dht::_read_packet_retries(int r)
{
    dht_status ret;

    for (int i = 0; i < r; i++) {
        if ((ret = _read_packet()) == DHT_OK) {
            return ret;
        }
    }
    return ret;
}

dht_status Dht::_read_packet(void)
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
        return _lastresult = DHT_TIMEOUT_START;
    }

    for (int i=0; i<80; i+=2) {
        buf[i] = _pulse_in(LOW);
        buf[i+1] = _pulse_in(HIGH);
    }

    interrupts();

    for (int i=0; i<80; i+=2) {
        if (buf[i] == 0 || buf[i+1] == 0) {
            return _lastresult = DHT_TIMEOUT;
        }

        _data[i/16] <<= 1;
        if (buf[i] < buf[i+1]) {
            _data[i/16] |= 1;
        }

    }

    if (((_data[0] + _data[1] + _data[2] + _data[3]) & 0xFF) == _data[4]) {
        _lastresult = DHT_OK;
    } else {
        _lastresult = DHT_BADCRC;
    }

    return _lastresult;
}

