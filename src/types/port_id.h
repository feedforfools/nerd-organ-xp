#ifndef PORT_ID_H
#define PORT_ID_H

#include <cstdint>

using PortId = uint8_t;                             // Unique ID for any source/sink in the system - up to 256

// **** Predefined Port IDs ****

// Sources
constexpr PortId PORT_ID_KEYBED         = 0;
constexpr PortId PORT_ID_HW_MIDI_IN_1   = 1;
constexpr PortId PORT_ID_HW_MIDI_IN_2   = 2;
constexpr PortId PORT_ID_SWITCHES       = 10;       // Dedicated source for panel switches

// Sinks
constexpr PortId PORT_ID_HW_MIDI_OUT_1  = 100;
constexpr PortId PORT_ID_HW_MIDI_OUT_2  = 101;
constexpr PortId PORT_ID_SYSTEM_CONFIG  = 110;      // Sink for handling system-level commands

// Dynamic Ports - USB ports will be assigned dynamically starting from these bases
constexpr PortId PORT_ID_USB_BASE       = 200;
constexpr PortId PORT_ID_USB_MAX        = 255;

#endif