#pragma once

#include "XModem.h"
#include "CmdStatus.h"


void setup();
void loop();
bool prepBlockWrite(byte buffer[]);
ISR (SPI_STC_vect);
volatile void *memcpy_v(volatile byte* dest, const volatile byte* src, size_t n);
