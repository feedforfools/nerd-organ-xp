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
                event.type = MusicalEventType::PITCH_BEND;
                event.data1 = midi.getData1();
                event.data2 = midi.getData2();
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