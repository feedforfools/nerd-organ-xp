#include "usb_midi_service.h"

#include "core/sources/usb_midi_input_source.h"
#include "core/sinks/usb_midi_output_sink.h"
#include "core/processing/note_processor.h"
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
    if (type == AppDeviceType::MIDI)
    {
        Logger::log("UsbMidiService: MIDI USB Device Connected!");

        MIDIDevice* midiDevice = static_cast<MIDIDevice*>(device);

        PortId newPortId = acquirePortId();
        if (newPortId == 0)
        {
            Logger::log("Error: No more USB port IDs available");
            return;
        }

        // Allocate new source and sink for the device and register them
        auto* newSource = new UsbMidiInputSource(newPortId, midiDevice);
        auto* newSink = new UsbMidiOutputSink(newPortId, midiDevice);
        auto* newProcessor = new NoteProcessor(240);
        routingManager.addSource(newSource);
        routingManager.addSink(newSink);
        routingManager.addProcessor(newProcessor);
        routingManager.createRoute(PORT_ID_KEYBED, newProcessor->getPortId());
        routingManager.createRoute(newProcessor->getPortId(), newSink->getPortId());

        activePorts[device] = {newSource, newSink};

        Logger::log("Created Source/Sink for new USB MIDI device with Port ID: " + String(newPortId));
    } else
    {
        Logger::log("UsbMidiService: Device is not a MIDI device, ignoring connection");
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
    Serial.flush();
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