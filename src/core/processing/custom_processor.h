#ifndef CUSTOM_PROCESSOR_H
#define CUSTOM_PROCESSOR_H

#include "core/interfaces.h"
#include "types/controllable.h"

class CustomEventProcessor : public IMusicalEventProcessor, public IControllable
{
    public:
        CustomEventProcessor(PortId portId, MusicalEventType eventType, int midiChannel = 1, int data1 = 0, int data2 = 0);

        void onMusicalEvent(const MusicalEvent& event) override;
        inline PortId getPortId() const override { return portId; }

        void setParameter(ControllableParameter param, int value) override;
        void setEventType(MusicalEventType type, int data1, int data2);

        void setEnabled(bool enabled) { isEnabled = enabled; }
        void setMidiChannel(uint8_t channel) { midiChannel = channel; }

    private:
        PortId portId;
        bool isEnabled;
        MusicalEventType eventType; // Type of the event to process
        int data1; // Value associated with the event type
        int data2; // Second value associated with the event type
        uint8_t midiChannel;
};

#endif