#include "usb_midi_output_sink.h"
#include "utils/logger.h"

UsbMidiOutputSink::UsbMidiOutputSink(PortId id, MIDIDevice_BigBuffer* device)
    : portId(id), midiDevice(device) {}

void UsbMidiOutputSink::onMusicalEvent(const MusicalEvent& event)
{
    Logger::log("Sending MIDI event: " + String(int(event.type)) + " on port " + String(portId) + " with data1: " + String(event.data1) + ", data2: " + String(event.data2));
    switch (event.type)
    {   
        case MusicalEventType::NOTE_ON:
        case MusicalEventType::NOTE_ON_HIGH_TRIGGER:
            midiDevice->sendNoteOn(event.data1, event.data2, event.channel);
            break;
        case MusicalEventType::NOTE_OFF:
            midiDevice->sendNoteOff(event.data1, event.data2, event.channel);
            break;
        case MusicalEventType::CONTROL_CHANGE:
            midiDevice->sendControlChange(event.data1, event.data2, event.channel);
            break;
        case MusicalEventType::PROGRAM_CHANGE:
            midiDevice->sendProgramChange(event.data1, event.channel);
            break;
        case MusicalEventType::PITCH_BEND:
        {
            // Combine data1 and data2 into a single 14-bit value for pitch bend
            // MIDI pitch bend uses a 14-bit value, where data1 is the LSB and data2 is the MSB
            // The pitch bend value is in the range of -8192 to 8191
            int pitchValue = (event.data2 << 7) | event.data1; // Combine data1 and data2
            pitchValue -= 8192; // Adjust to -8192 to 8191 range
            midiDevice->sendPitchBend(pitchValue, event.channel);
            break;
        }
        // TODO: Implement additional MIDI message types as needed
        default:
            Logger::log("Unsupported MIDI message type for sending");
            break;
    }
}