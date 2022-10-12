#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t from;
    uint16_t to;
} symbol_t;

uint8_t* write_output(uint8_t* utf, const uint8_t* start, const uint8_t* end, const symbol_t* decoder);

#endif // !DECODE_H