#include "usb_midi_input_source.h"
#include "utils/logger.h"

UsbMidiInputSource::UsbMidiInputSource(PortId id, MIDIDevice_BigBuffer* device) : portId(id), midiDevice(device) {}

void UsbMidiInputSource::update()
{
    if (midiDevice->read())
    {
        MusicalEvent event;
        event.sourcePortId = portId;
        event.channel = midiDevice->getChannel();
        event.data1 = midiDevice->getData1();
        event.data2 = midiDevice->getData2();

        bool isValidEvent = true;

        switch (midiDevice->getType())
        {
            case MIDIDeviceBase::MidiType::NoteOn:
                event.type = MusicalEventType::NOTE_ON;
                break;
            case MIDIDeviceBase::MidiType::NoteOff:
                event.type = MusicalEventType::NOTE_OFF;
                break;
            case MIDIDeviceBase::MidiType::ControlChange:
                event.type = MusicalEventType::CONTROL_CHANGE;
                break;
            case MIDIDeviceBase::MidiType::ProgramChange:
                event.type = MusicalEventType::PROGRAM_CHANGE;
                break;
            case MIDIDeviceBase::MidiType::PitchBend:
                event.type = MusicalEventType::PITCH_BEND;
                event.data1 = midiDevice->getData1();
                event.data2 = midiDevice->getData2();
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