#ifndef USB_MIDI_INPUT_SOURCE_H
#define USB_MIDI_INPUT_SOURCE_H

#include "core/interfaces.h"
#include "io/usb/usb_host_manager.h"

class UsbMidiInputSource : public IMusicalEventSource
{
    public:
        UsbMidiInputSource(PortId id, MIDIDevice_BigBuffer* device);

        void update();
        inline PortId getPortId() const override { return portId; }

    private:
        PortId portId;
        MIDIDevice_BigBuffer* midiDevice;
};

#endif