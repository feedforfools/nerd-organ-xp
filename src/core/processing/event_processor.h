#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

#include "core/interfaces.h"
#include "types/controllable.h"

class EventProcessor : public IMusicalEventProcessor, public IControllable
{
    public:
        EventProcessor(PortId portId);

        void onMusicalEvent(const MusicalEvent& event) override;
        inline PortId getPortId() const override { return portId; }
        
        void setParameter(ControllableParameter param, int value) override;

        void setEnabled(bool enabled);
        void setTranspose(int semitones) { transpose = semitones; }
        void setMidiChannel(uint8_t channel) { midiChannel = channel; }
        void setHighTriggerMode(bool mode) { highTriggerMode = mode; }
        void setMinNote(uint8_t note) { minNote = note; }
        void setMaxNote(uint8_t note) { maxNote = note; }

    private:
        PortId portId;
        bool isEnabled;
        int transpose;
        uint8_t midiChannel;
        bool highTriggerMode;
        uint8_t minNote;
        uint8_t maxNote;
};

#endif