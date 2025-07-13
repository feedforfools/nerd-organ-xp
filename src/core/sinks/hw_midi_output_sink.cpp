#include "hw_midi_output_sink.h"
#include "utils/logger.h"

HwMidiOutputSink::HwMidiOutputSink(PortId id, MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>& midiInstance)
    : portId(id), midi(midiInstance) {}

void HwMidiOutputSink::onMusicalEvent(const MusicalEvent& event)
{
    Logger::log("Sending MIDI event: " + String(int(event.type)) + " on port " + String(portId) + " with data1: " + String(event.data1) + ", data2: " + String(event.data2));
    switch (event.type)
    {   
        case MusicalEventType::NOTE_ON:
        case MusicalEventType::NOTE_ON_HIGH_TRIGGER:
            midi.sendNoteOn(event.data1, event.data2, event.channel);
            break;
        case MusicalEventType::NOTE_OFF:
            midi.sendNoteOff(event.data1, event.data2, event.channel);
            break;
        case MusicalEventType::CONTROL_CHANGE:
            midi.sendControlChange(event.data1, event.data2, event.channel);
            break;
        case MusicalEventType::PROGRAM_CHANGE:
            midi.sendProgramChange(event.data1, event.channel);
            break;
        case MusicalEventType::PITCH_BEND:
            midi.sendPitchBend(event.data2, event.channel);
            break;
        // TODO: Implement additional MIDI message types as needed
        default:
            Logger::log("Unsupported MIDI message type for sending");
            break;
    }
}