#include "custom_processor.h"
#include "utils/logger.h"

CustomEventProcessor::CustomEventProcessor(PortId portId, MusicalEventType eventType, int midiChannel, int data1, int data2)
    : portId(portId), isEnabled(true), eventType(eventType), data1(data1), data2(data2), midiChannel(midiChannel) {}

void CustomEventProcessor::onMusicalEvent(const MusicalEvent& event)
{
    if (!isEnabled) return;

    if (event.type == eventType && data1 == event.data1)
    {
        MusicalEvent processedEvent = event;
        processedEvent.sourcePortId = portId;
        processedEvent.channel = midiChannel;

        fireEvent(processedEvent);
    }
}

void CustomEventProcessor::setParameter(ControllableParameter param, int value)
{
    switch (param)
    {
        case ControllableParameter::PROCESSOR_ENABLE:
            setEnabled(static_cast<bool>(value));
            break;
        default:
            Logger::log("CustomEventProcessor: Unsupported parameter for setting: " + String(int(param)));
            break;
    }
}

void CustomEventProcessor::setEventType(MusicalEventType type, int data1, int data2)
{
    eventType = type;
    this->data1 = data1;
    this->data2 = data2;
}

