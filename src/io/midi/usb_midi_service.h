#ifndef USB_MIDI_SERVICE_H
#define USB_MIDI_SERVICE_H

#include "io/usb/usb_host_manager.h"
#include "core/routing/routing_manager.h"
#include "core/sources/usb_midi_input_source.h"
#include "core/sinks/usb_midi_output_sink.h"

#include <map>
#include <array>

class UsbMidiService : public IUsbDeviceListener
{
    public:
        UsbMidiService(RoutingManager& routingManager);
        void init();
        void update();

        // IUsbDeviceListener methods
        void onUsbDeviceConnected(USBDriver* device, AppDeviceType type) override;
        void onUsbDeviceDisconnected(USBDriver* device, AppDeviceType type) override;

    private:
        struct UsbPort
        {
            UsbMidiInputSource* source;
            UsbMidiOutputSink* sink;
        };

        RoutingManager& routingManager;

        std::map<USBDriver*, UsbPort> activePorts; // Maps USB devices to their assigned ports
        static constexpr size_t MAX_USB_PORTS = PORT_ID_USB_MAX - PORT_ID_USB_BASE + 1;
        std::array<PortId, MAX_USB_PORTS> availablePortIds; // List of available port IDs
        size_t numAvailablePortIds;

        void initializePortIds();
        PortId acquirePortId();
        void releasePortId(PortId portId);
};

#endif