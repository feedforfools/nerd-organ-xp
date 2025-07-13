#include "usb_midi_input_source.h"
#include "utils/logger.h"

UsbMidiInputSource::UsbMidiInputSource(PortId id, MIDIDevice* device) : portId(id), midiDevice(device) {}

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
                // NOTE: The MIDI.h library combines the two 7-bit data bytes
                // of a pitch bend message into a single 14-bit integer.
                // Current MusicalEvent struct uses an 8-bit data2.
                // For now, we can map this larger value down, but be aware of the loss of resolution.
                // A future improvement could be to make data1/data2 in MusicalEvent a 16-bit union.
                // Mapping [-8192, 8191] to [0, 127] for this example.
                event.type = MusicalEventType::PITCH_BEND;
                event.data2 = (uint8_t)map(midiDevice->getData2(), -8192, 8191, 0, 127);
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