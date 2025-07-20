#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// **** Hardware configuration ****
// Keybed configuration
#define NUM_KEYS                61                          // number of keys in the keyboard used
#define NUM_KEYS_PER_BANK       8                           // number of keys in each bank
#define NUM_BANKS               8                           // number of group of keys (for Fatar 61 they're arranged in 8 banks)
#define NUM_KEYZONES            2
#define NUM_CONTACTS            2                           // number of contacts per key (top and bottom)
#define NUM_BIT_MPLX            3
#define NUM_DEBOUNCE_CYCLES     3                           // number of cycles of the keybed scanning for debouncing each key
#define LEFT                    0                           // alias for left keyzone
#define RIGHT                   1                           // alias for right keyzone
#define TOP                     0                           // alias for top contact of the key
#define BOTTOM                  1                           // alias for bottom contact of the key

// MIDI ports configuration
#define NUM_HW_MIDI_OUT_PORTS   2
#define NUM_HW_MIDI_IN_PORTS    2                           // number of hardware MIDI in/out ports (couples)

// Other (switches)
#define SWS_TIMER               200                         // switches scanning period in milliseconds
#define NUM_SWITCHES            2                           // number of switches for mode selector (3-way and 2-way)
#define THREE_WAY_SWITCH_ID     0
#define TWO_WAY_SWITCH_ID       1
#define NUM_SWITCH_PINS         3                           // number of pins for the mode selector switches

// Pins configuration
const byte SWITCH_PINS[NUM_SWITCH_PINS] = {2, 3, 4};        // Pins for the mode selector switches (First two for 3-way and last for 2-way)
const byte MIDI_IN_PINS[NUM_HW_MIDI_IN_PORTS] = {0, 7};     // RX1, RX2 pins respectively: for reading MIDI signals
const byte MIDI_OUT_PINS[NUM_HW_MIDI_OUT_PORTS] = {1, 8};   // TX1, TX2 pins respectively: for writing MIDI signals
const byte USB_POWER_PIN = 32;                              // Pin to power the MIC2545A (EN pin) for the USB host
const byte USB_STATUS_PIN = 30;                             // Pin to read the status of the MIC2545A (FLG pin) for the USB host

// MIDI configuration
#define MIDI_CHANNEL_OMNI       0                           // MIDI channel for omni mode (0 means all channels)
#define HIGH_TRIG_VELOCITY      1
#define MIDI_STARTING_NOTE      36                          // MIDI note number for the first key (C2)

// USB configuration
#define NUM_USB_MIDI_DEVICES    4
#define NUM_USB_STORAGE_DEVICES 4 
#define TOTAL_USB_DRIVERS (NUM_USB_MIDI_DEVICES + NUM_USB_STORAGE_DEVICES) // Total number of USB driver instances the manager will check

// Custom device properties
#define JD08_VENDOR_ID          0x0582
#define JD08_PRODUCT_ID         0x028C
#define NERD_PICO_VENDOR_ID     0x16C0
#define NERD_PICO_PRODUCT_ID    0x0485

#endif