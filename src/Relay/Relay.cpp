#include "Relay.hpp"

Relay::Relay(uint8_t p) : pin(p) {}

void Relay::begin()
{
    pinMode(pin, OUTPUT);
    off();
}

void Relay::on()
{
    digitalWrite(pin, HIGH);
    state = true;
}

void Relay::off()
{
    digitalWrite(pin, LOW);
    state = false;
}

bool Relay::isOn() const
{
    return state;
}
