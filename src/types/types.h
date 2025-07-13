#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

// Possible key states
typedef enum {
   KEY_IS_UP,
   KEY_IS_DOWN,
   KEY_IS_GOING_UP,
   KEY_IS_GOING_DOWN,
} state_t;

// Possible key events
typedef enum {
   KEY_TOUCHED,                     // Key is touched (break contact in short, still not to bottom)
   KEY_PRESSED,                     // Key reached bottom (make contact in short, fully pressed)
   KEY_RELEASED,                    // Key is released (make contact not short, break still short, still not to top)
   KEY_TOP,                         // Key reached top (no more shorts, no more touched)
} event_t;

typedef struct {
   bool played;
   state_t state:4;
   uint32_t t;
} key_fatar_t;

typedef struct {
   byte pin;
   byte bit;
} teensy_port_reg;

typedef struct {
   byte breaks;                     // byte of shorts in break contacts (top) in current bank
   byte makes;                      // byte of shorts in make contacts (bottom) in current bank
} bank_t;

#endif