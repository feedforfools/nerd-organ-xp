#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"
#include "port_id.h"


//------------------------------------------------------------------
// The universal event structure for the entire routing system
//------------------------------------------------------------------
enum class MusicalEventType
{
    NOTE_ON,
    NOTE_ON_HIGH_TRIGGER,
    NOTE_OFF,
    CONTROL_CHANGE,
    PROGRAM_CHANGE,
    PITCH_BEND,
    AFTERTOUCH,
    // TODO: check if more MIDI events are needed
};

struct MusicalEvent
{
    PortId sourcePortId;         // Source that generated the event
    MusicalEventType type;
    uint8_t channel;             // MIDI channel (1-16)
    uint8_t data1;               // First data byte (e.g., note number for NOTE_ON/NOTE_OFF, controller number for CONTROL_CHANGE)
    uint8_t data2;               // Second data byte (e.g., velocity for NOTE_ON/NOTE_OFF, control value for CONTROL_CHANGE)
};

//------------------------------------------------------------------
// Low-level event structure for the internal keyboard ONLY.
// This is translated into a MusicalEvent by the InternalKeyboardSource.
//------------------------------------------------------------------

struct KeyEvent
{
    uint8_t keyIndex;               // Physical key index on the keyboard
    event_t eventType;              // KEY_TOUCHED, KEY_PRESSED, KEY_RELEASED, KEY_TOP from types.h
    uint32_t deltaTime;             // Timer value for velocity calculation
};

class IKeyEventListener
{
    public:
        virtual ~IKeyEventListener() = default;
        virtual void onKeyEvent(const KeyEvent& event) = 0;
};

//------------------------------------------------------------------
// Low-level event structure for the internal switches ONLY.
//------------------------------------------------------------------

struct SwitchEvent
{
    uint8_t switchId;                // Unique ID for the switch
    uint8_t state;                   // Switch state (could be multi-state, e.g., three-way switch (0, 1, 2) or binary (0, 1))
};

class ISwitchEventListener
{
    public:
        virtual ~ISwitchEventListener() = default;
        virtual void onSwitchEvent(const SwitchEvent& event) = 0;
};

#endif