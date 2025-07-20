#include "event_processor.h"
#include "utils/logger.h"

EventProcessor::EventProcessor(PortId portId) : portId(portId), isEnabled(true), transpose(0), midiChannel(1), highTriggerMode(false), minNote(0), maxNote(127) {}

void EventProcessor::onMusicalEvent(const MusicalEvent& event)
{
    if (!isEnabled) return;

    MusicalEvent processedEvent = event;
    processedEvent.sourcePortId = portId; // To make sure next component knows note has been processed => TODO: is it needed?
    processedEvent.channel = midiChannel;

    switch (event.type)
    {
        case MusicalEventType::NOTE_ON:
        case MusicalEventType::NOTE_ON_HIGH_TRIGGER:
        case MusicalEventType::NOTE_OFF:
        {
            // Filter by note range
            if (event.data1 < minNote || event.data1 > maxNote) return;

            // Filter by trigger type
            if ((highTriggerMode && event.type == MusicalEventType::NOTE_ON) ||             // Ignore NOTE_ON events if high trigger mode is enabled
                (!highTriggerMode && event.type == MusicalEventType::NOTE_ON_HIGH_TRIGGER)) // Ignore NOTE_ON_HIGH_TRIGGER events if velocity mode is enabled (= high trigger mode is disabled)
            {
                return;
            }

            // Apply transposition
            int newNote = (int)event.data1 + transpose;
            if (newNote >= 0 && newNote < 128)
            {
                processedEvent.data1 = (uint8_t)newNote;
            }
            else return; // Ignore notes transposed out of range
            break;
        }
        case MusicalEventType::CONTROL_CHANGE:
        case MusicalEventType::PROGRAM_CHANGE:
        case MusicalEventType::PITCH_BEND:
        case MusicalEventType::AFTERTOUCH:
            // Pass through other types of events without filtering
            break;
        default:
            Logger::log("EventProcessor: Unsupported MIDI message type for processing: " + String(int(event.type)));
            // Unsupported MIDI message type, do not process
            return;
    }

    fireEvent(processedEvent);
}

void EventProcessor::setParameter(ControllableParameter param, int value)
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