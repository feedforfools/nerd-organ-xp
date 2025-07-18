#include <Arduino.h>

#include "config/config.h"
#include "core/control_mapper.h"
#include "core/processing/note_processor.h"
#include "core/routing/routing_manager.h"
#include "core/sinks/hw_midi_output_sink.h"
#include "core/sources/hw_midi_input_source.h"
#include "core/sources/keybed_source.h"
#include "hardware/keyboard/keyboard_scanner.h"
#include "hardware/switches/switch_manager.h"
#include "io/usb/usb_host_manager.h"
#include "io/midi/usb_midi_service.h"
#include "utils/logger.h"

/**
 * Investigate random NOTE on events being probably generated by the keybed.
 * ControlMapper: Switch event received - ID: 0, State: 2
 * Sending MIDI event: 0 on port 100 with data1: 39, data2: 127
 * Sending MIDI event: 0 on port 100 with data1: 74, data2: 127
 * Sending MIDI event: 0 on port 100 with data1: 43, data2: 127
 * Sending MIDI event: 0 on port 100 with data1: 42, data2: 127
 * Sending MIDI event: 0 on port 100 with data1: 38, data2: 127
 * */

RoutingManager routingManager;
UsbHostManager usbHostManager;
UsbMidiService usbMidiService(routingManager);
KeyboardScanner keyboardScanner;
SwitchManager switchManager;
ControlMapper controlMapper;
KeybedSource keybedSource;
NoteProcessor noteProcessor1(150); // TODO: Create a method for generating unique port IDs
NoteProcessor noteProcessor2(151);
NoteProcessor noteProcessor3(152); // For Nerd Pico

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_IN_1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_OUT_1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI_IN_2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI_OUT_2);

HwMidiInputSource hwMidiIn1(PORT_ID_HW_MIDI_IN_1, MIDI_IN_1);
HwMidiInputSource hwMidiIn2(PORT_ID_HW_MIDI_IN_2, MIDI_IN_2);
HwMidiOutputSink hwMidiOut1(PORT_ID_HW_MIDI_OUT_1, MIDI_OUT_1);
HwMidiOutputSink hwMidiOut2(PORT_ID_HW_MIDI_OUT_2, MIDI_OUT_2);

#define NORD_MAIN_MIDI_CHANNEL 5
#define NORD_2ND_MANUAL_MIDI_CHANNEL 6

void setup()
{
   Serial.begin(9600);
   while (!Serial && millis() < 4000);
   Logger::log("\n--- Nerd Organ XP initializing ---");

   Logger::log("Initializing Core Systems (Routing, Switches, Keyboard)...");
   routingManager.init();
   switchManager.init();
   keyboardScanner.init();
   
   Logger::log("Registering components...");
   routingManager.addSource(&keybedSource);
   routingManager.addSource(&hwMidiIn1);
   routingManager.addSource(&hwMidiIn2);
   routingManager.addSink(&hwMidiOut1);
   routingManager.addSink(&hwMidiOut2);
   routingManager.addProcessor(&noteProcessor1);
   routingManager.addProcessor(&noteProcessor2);
   routingManager.addProcessor(&noteProcessor3); // For Nerd Pico

   Logger::log("Initializing USB Host System...");
   usbHostManager.init();
   usbMidiService.init();
   
   Logger::log("Connecting hardware drivers...");
   hwMidiIn1.init();
   hwMidiIn2.init();
   switchManager.registerListener(&controlMapper);
   keyboardScanner.registerListener(&keybedSource);
   usbHostManager.addListener(&usbMidiService);

   Logger::log("Creating default routes and mappings...");
   routingManager.createRoute(PORT_ID_KEYBED, noteProcessor1.getPortId());
   routingManager.createRoute(noteProcessor1.getPortId(), PORT_ID_HW_MIDI_OUT_1);
   routingManager.createRoute(PORT_ID_KEYBED, noteProcessor2.getPortId());
   routingManager.createRoute(noteProcessor2.getPortId(), PORT_ID_HW_MIDI_OUT_2);
   routingManager.createRoute(200, noteProcessor3.getPortId()); // For Nerd Pico, dynamic port ID (200 is the first USB port ID)
   routingManager.createRoute(noteProcessor3.getPortId(), PORT_ID_HW_MIDI_OUT_2); // For Nerd Pico
   // Control mappings
   controlMapper.addMapping({.switchId = THREE_WAY_SWITCH_ID, .triggerState = 0, .target = &noteProcessor1, .parameter = ControllableParameter::PROCESSOR_ENABLE, .value = 1});
   controlMapper.addMapping({.switchId = THREE_WAY_SWITCH_ID, .triggerState = 1, .target = &noteProcessor1, .parameter = ControllableParameter::PROCESSOR_ENABLE, .value = 0});
   controlMapper.addMapping({.switchId = THREE_WAY_SWITCH_ID, .triggerState = 0, .target = &noteProcessor2, .parameter = ControllableParameter::PROCESSOR_ENABLE, .value = 1});
   controlMapper.addMapping({.switchId = THREE_WAY_SWITCH_ID, .triggerState = 2, .target = &noteProcessor2, .parameter = ControllableParameter::PROCESSOR_ENABLE, .value = 0});
   controlMapper.addMapping({.switchId = TWO_WAY_SWITCH_ID, .triggerState = 0, .target = &noteProcessor1, .parameter = ControllableParameter::PROCESSOR_HIGH_TRIGGER_MODE, .value = 0});
   controlMapper.addMapping({.switchId = TWO_WAY_SWITCH_ID, .triggerState = 1, .target = &noteProcessor1, .parameter = ControllableParameter::PROCESSOR_HIGH_TRIGGER_MODE, .value = 1});
   controlMapper.addMapping({.switchId = TWO_WAY_SWITCH_ID, .triggerState = 0, .target = &noteProcessor1, .parameter = ControllableParameter::PROCESSOR_MIDI_CHANNEL, .value = NORD_MAIN_MIDI_CHANNEL});
   controlMapper.addMapping({.switchId = TWO_WAY_SWITCH_ID, .triggerState = 1, .target = &noteProcessor1, .parameter = ControllableParameter::PROCESSOR_MIDI_CHANNEL, .value = NORD_2ND_MANUAL_MIDI_CHANNEL});

   Logger::log("--- Initialization complete, starting main loop ---");
   Serial.flush();
}

void loop()
{
   keyboardScanner.scan();
   switchManager.update();
   usbHostManager.update();
   usbMidiService.update();
   hwMidiIn1.update();
   hwMidiIn2.update();
   keyboardScanner.updateTimers();
}