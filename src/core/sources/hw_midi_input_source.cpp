#include "hw_midi_input_source.h"
#include "utils/logger.h"

HwMidiInputSource::HwMidiInputSource(PortId id, MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>& midiInstance) : portId(id), midi(midiInstance) {}

void HwMidiInputSource::init()
{
    midi.begin(MIDI_CHANNEL_OMNI);
    Logger::log("HwMidiInputSource on port " + String(portId) + " initialized");
}

void HwMidiInputSource::update()
{
    if (midi.read())
    {
        MusicalEvent event;
        event.sourcePortId = portId;
        event.channel = midi.getChannel();
        event.data1 = midi.getData1();
        event.data2 = midi.getData2();

        bool isValidEvent = true;

        switch (midi.getType())
        {
            case midi::NoteOn:
                event.type = MusicalEventType::NOTE_ON;
                break;
            case midi::NoteOff:
                event.type = MusicalEventType::NOTE_OFF;
                break;
            case midi::ControlChange:
                event.type = MusicalEventType::CONTROL_CHANGE;
                break;
            case midi::ProgramChange:
                event.type = MusicalEventType::PROGRAM_CHANGE;
                break;
            case midi::PitchBend:
                // NOTE: The MIDI.h library combines the two 7-bit data bytes
                // of a pitch bend message into a single 14-bit integer.
                // Current MusicalEvent struct uses an 8-bit data2.
                // For now, we can map this larger value down, but be aware of the loss of resolution.
                // A future improvement could be to make data1/data2 in MusicalEvent a 16-bit union.
                // Mapping [-8192, 8191] to [0, 127] for this example.
                event.type = MusicalEventType::PITCH_BEND;
                event.data2 = (uint8_t)map(midi.getData2(), -8192, 8191, 0, 127);
                break;
            default:
                isValidEvent = false; // Unsupported MIDI message type
                break;
        }

        if (isValidEvent)
        {
            fireEvent(event);
        }
    }
}