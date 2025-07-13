#ifndef HW_MIDI_INPUT_SOURCE_H
#define HW_MIDI_INPUT_SOURCE_H

#include <MIDI.h>
#include "core/interfaces.h"

class HwMidiInputSource : public IMusicalEventSource
{
    public:
        HwMidiInputSource(PortId id, MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>& midiInstance);

        void init();
        void update();
        inline PortId getPortId() const override { return portId; }

    private:
        PortId portId;
        MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>& midi;
};

#endif