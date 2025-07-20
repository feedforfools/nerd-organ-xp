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
constexpr PortId PORT_ID_HW_MIDI_OUT_1  = 101;
constexpr PortId PORT_ID_HW_MIDI_OUT_2  = 102;
constexpr PortId PORT_ID_SYSTEM_CONFIG  = 110;      // Sink for handling system-level commands

// Processors
constexpr PortId PORT_ID_NORD_PROCESSOR = 150;
constexpr PortId PORT_ID_JD08_PROCESSOR = 151;

// Custom device port IDs
constexpr PortId PORT_ID_JD08           = 170;      // Custom device (e.g. JD-08) port ID
constexpr PortId PORT_ID_NERD_PICO      = 171;      // Custom device (e.g. Nerd Pico) port ID

// Dynamic Ports - USB ports will be assigned dynamically starting from this base
constexpr PortId PORT_ID_USB_BASE       = 200;
constexpr PortId PORT_ID_USB_MAX        = 255;

#endif