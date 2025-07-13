#ifndef USB_MIDI_OUTPUT_SINK_H
#define USB_MIDI_OUTPUT_SINK_H

#include "core/interfaces.h"
#include "io/usb/usb_host_manager.h"

class UsbMidiOutputSink : public IMusicalEventSink
{
    public:
        UsbMidiOutputSink(PortId id, MIDIDevice* device);

        void onMusicalEvent(const MusicalEvent& event) override;      
        PortId getPortId() const override { return portId; }

    private:
        PortId portId;
        MIDIDevice* midiDevice;
};

#endif