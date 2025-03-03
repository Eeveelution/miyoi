#include "psyqo/fixed-point.hh"
#include "psyqo/gte-kernels.hh"
#include "psyqo/gte-registers.hh"
#include "psyqo/primitives/common.hh"
#include "psyqo/primitives/control.hh"
#include "psyqo/primitives/sprites.hh"
#include "psyqo/vector.hh"

// array size is 152
static const uint8_t HITBOX[]  = {
    0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x8e, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 
    0x08, 0x00, 0x08, 0x00, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 
    0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x39, 0x67, 0xde, 0x7b, 0xde, 0x7b, 0x39, 0x67, 
    0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x39, 0x67, 0xff, 0x5e, 0xbd, 0x18, 0xdd, 0x1c, 0xff, 0x5e, 
    0x18, 0x63, 0x10, 0x42, 0x10, 0x42, 0xde, 0x7b, 0xbd, 0x18, 0x7d, 0x10, 0x7d, 0x10, 0xbd, 0x18, 
    0xde, 0x7b, 0x10, 0x42, 0x10, 0x42, 0xde, 0x7b, 0xbd, 0x18, 0x7d, 0x10, 0x7d, 0x10, 0xbd, 0x18, 
    0xde, 0x7b, 0x10, 0x42, 0x10, 0x42, 0x39, 0x67, 0xff, 0x5e, 0xbd, 0x18, 0xdd, 0x1c, 0xff, 0x5e, 
    0x18, 0x63, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x39, 0x67, 0xde, 0x7b, 0xde, 0x7b, 0x39, 0x67, 
    0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 0x10, 0x42, 
    0x10, 0x42, 0x10, 0x42, 0x00, 0x00, 0x00, 0x00
  };