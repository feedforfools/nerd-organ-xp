#include "note_processor.h"

NoteProcessor::NoteProcessor(PortId portId) : portId(portId), isEnabled(true), transpose(0), midiChannel(1), highTriggerMode(false), minNote(0), maxNote(127) {}

void NoteProcessor::onMusicalEvent(const MusicalEvent& event)
{
    if (!isEnabled) return;

    // 1. Filter by note range (for splits/layers)
    if (event.type == MusicalEventType::NOTE_ON || event.type == MusicalEventType::NOTE_ON_HIGH_TRIGGER || event.type == MusicalEventType::NOTE_OFF)
    {
        if (event.data1 < minNote || event.data1 > maxNote) return; // Ignore notes out of range
    }

    // 2. Filter by trigger type
    if (highTriggerMode && event.type == MusicalEventType::NOTE_ON)
    {
        return; // Ignore NOTE_ON events if high trigger mode is enabled
    }
    if (!highTriggerMode && event.type == MusicalEventType::NOTE_ON_HIGH_TRIGGER)
    {
        return; // Ignore NOTE_ON_HIGH_TRIGGER events if velocity mode is enabled (= high trigger mode is disabled)
    }

    // 3. Process the event
    MusicalEvent processedEvent = event;
    processedEvent.sourcePortId = portId; // To make sure next component knows note has been processed => is it needed?

    // Apply transposition
    if (event.type == MusicalEventType::NOTE_ON || event.type == MusicalEventType::NOTE_ON_HIGH_TRIGGER || event.type == MusicalEventType::NOTE_OFF)
    {
        int newNote = (int)event.data1 + transpose;
        if (newNote >= 0 && newNote < 128)
        {
            processedEvent.data1 = (uint8_t)newNote;
        }
        else return; // Ignore notes transposed out of range
    }

    // 4. Enrich the event with the correct MIDI channel
    processedEvent.channel = midiChannel;

    // 5. Fire the processed event
    fireEvent(processedEvent);
}

void NoteProcessor::setParameter(ControllableParameter param, int value)
{
    switch (param)
    {
        case ControllableParameter::PROCESSOR_ENABLE:
            setEnabled(static_cast<bool>(value));
            break;
        case ControllableParameter::PROCESSOR_TRANSPOSE:
            setTranspose(value);
            break;
        case ControllableParameter::PROCESSOR_MIDI_CHANNEL:
            setMidiChannel(static_cast<uint8_t>(value));
            break;
        case ControllableParameter::PROCESSOR_HIGH_TRIGGER_MODE:
            setHighTriggerMode(static_cast<bool>(value));
            break;
        case ControllableParameter::PROCESSOR_MIN_NOTE:
            setMinNote(static_cast<uint8_t>(value));
            break;
        case ControllableParameter::PROCESSOR_MAX_NOTE:
            setMaxNote(static_cast<uint8_t>(value));
            break;
        default:
            // TODO: Handle unknown parameters?
            break;
    }
}