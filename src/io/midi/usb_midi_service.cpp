#include "usb_midi_service.h"

#include "core/sources/usb_midi_input_source.h"
#include "core/sinks/usb_midi_output_sink.h"
#include "utils/logger.h"

UsbMidiService::UsbMidiService(RoutingManager& routingManager)
    : routingManager(routingManager)
{
}

void UsbMidiService::init()
{
    initializePortIds();
    Logger::log("UsbMidiService initialized and listening for devices");
}

void UsbMidiService::update()
{
    for (auto const& [device, port] : activePorts)
    {
        if (port.source)
        {
            port.source->update();
        }
    }
}

void UsbMidiService::onUsbDeviceConnected(USBDriver* device, AppDeviceType type)
{
    switch (type)
    {
        case AppDeviceType::MIDI:
        {
            Logger::log("UsbMidiService: MIDI USB Device Connected!");
            MIDIDevice_BigBuffer* midiDevice = static_cast<MIDIDevice_BigBuffer*>(device);

            uint16_t vendorId = midiDevice->idVendor();
            uint16_t productId = midiDevice->idProduct();
            PortId newDevicePortId;

            if (vendorId == JD08_VENDOR_ID && productId == JD08_PRODUCT_ID)
            {
                Logger::log("UsbMidiService: JD08 MIDI device connected, proceeding with registration");
                newDevicePortId = PORT_ID_JD08; // Use predefined port ID for JD-08
                auto *newSource = new UsbMidiInputSource(newDevicePortId, midiDevice);
                auto *newSink = new UsbMidiOutputSink(newDevicePortId, midiDevice);
                routingManager.addSource(newSource); // Not used
                routingManager.addSink(newSink);
                routingManager.createRoute(PORT_ID_JD08_PROCESSOR, PORT_ID_JD08);
                activePorts[device] = {newSource, newSink};
                Logger::log("Created Sink for Roland JD08 USB MIDI device with Port ID: " + String(newDevicePortId));
            } 
            else if (vendorId == NERD_PICO_VENDOR_ID && productId == NERD_PICO_PRODUCT_ID)
            {
                Logger::log("UsbMidiService: Nerd Pico MIDI device connected, proceeding with registration");
                newDevicePortId = PORT_ID_NERD_PICO; // Use predefined port ID for Nerd Pico
                auto *newSource = new UsbMidiInputSource(newDevicePortId, midiDevice);
                auto *newSink = new UsbMidiOutputSink(newDevicePortId, midiDevice);
                routingManager.addSource(newSource);
                routingManager.addSink(newSink); // Not used
                routingManager.createRoute(PORT_ID_NERD_PICO, PORT_ID_JD08_PROCESSOR);
                activePorts[device] = {newSource, newSink};
                Logger::log("Created Source for Nerd Pico USB MIDI controller with Port ID: " + String(newDevicePortId));
            }
            else
            {
                Logger::log("UsbMidiService: Generic MIDI device connected, proceeding with registration");
                newDevicePortId = acquirePortId();
                if (newDevicePortId == 0)
                {
                    Logger::log("Error: No more USB port IDs available for new MIDI device. Registration failed.");
                    return;
                }
                // Allocate new source and sink for the device and register them
                auto* newSource = new UsbMidiInputSource(newDevicePortId, midiDevice);
                auto* newSink = new UsbMidiOutputSink(newDevicePortId, midiDevice);
                routingManager.addSource(newSource);
                routingManager.addSink(newSink);
                activePorts[device] = {newSource, newSink};
                Logger::log("Created Source/Sink for new USB MIDI device with Port ID: " + String(newDevicePortId));
            }

            break;
        }
        case AppDeviceType::STORAGE:
            // Handle storage device connection
            return; // Ignore storage devices for now
        default:
            Logger::log("UsbMidiService: Unknown USB device type connected, ignoring");
            return; // Ignore unknown types
    }
}

void UsbMidiService::onUsbDeviceDisconnected(USBDriver* device, AppDeviceType type)
{
    auto it = activePorts.find(device);
    if (it != activePorts.end())
    {
        Logger::log("UsbMidiService: USB MIDI Device Disconnected!");
        PortId portIdToRelease = it->second.source->getPortId();
        
        // Unregister and delete the source/sink
        routingManager.removeSource(it->second.source);
        routingManager.removeSink(it->second.sink);
        delete it->second.source;
        delete it->second.sink;
        
        releasePortId(portIdToRelease);
        activePorts.erase(it);

        Logger::log("UsbMidiService: USB MIDI Device unregistered and resources freed");
    }
}

void UsbMidiService::initializePortIds()
{
    // Initialize available port IDs
    for (size_t i = 0; i < MAX_USB_PORTS; ++i)
    {
        availablePortIds[i] = PORT_ID_USB_MAX - i;
    }
    numAvailablePortIds = MAX_USB_PORTS;
    Logger::log("Initialized USB MIDI port IDs from " + String(PORT_ID_USB_BASE) + " to " + String(PORT_ID_USB_MAX));
}

PortId UsbMidiService::acquirePortId()
{
    if (numAvailablePortIds == 0)
    {
        return 0; // No available port IDs
    }
    --numAvailablePortIds;
    return availablePortIds[numAvailablePortIds];
}

void UsbMidiService::releasePortId(PortId id)
{
    if (id >= PORT_ID_USB_BASE && id <= PORT_ID_USB_MAX)
    {
        if (numAvailablePortIds < MAX_USB_PORTS)
        {
            availablePortIds[numAvailablePortIds] = id;
            ++numAvailablePortIds;
            Logger::log("Released USB port ID: " + String(id));
        }
    }
}