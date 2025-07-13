#ifndef HW_MIDI_OUTPUT_SINK_H
#define HW_MIDI_OUTPUT_SINK_H

#include <MIDI.h>
#include "core/interfaces.h"

class HwMidiOutputSink : public IMusicalEventSink
{
    public:
        HwMidiOutputSink(PortId id, MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>& midiInstance);

        void onMusicalEvent(const MusicalEvent& event) override;      
        PortId getPortId() const override { return portId; }

    private:
        PortId portId;
        MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>& midi;
};

#endif