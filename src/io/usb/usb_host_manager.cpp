#include "usb_host_manager.h"
#include "config/config.h"
#include "utils/logger.h"

UsbHostManager::UsbHostManager()
{
    int driverIndex = 0;
    for (int i = 0; i < NUM_USB_MIDI_DEVICES; ++i)
    {
        allDrivers[driverIndex++] = &midiDevices[i];
    }
    for (int i = 0; i < NUM_USB_STORAGE_DEVICES; ++i)
    {
        allDrivers[driverIndex++] = &driveDevices[i];
    }
}

void UsbHostManager::init()
{
    // Power up the USB host with the pin defined in config.h
    pinMode(USB_POWER_PIN, OUTPUT);
    digitalWrite(USB_POWER_PIN, HIGH);
    delay(100); // Small delay for power to stabilize
    usbHost.begin();
    Logger::log("UsbHostManager initialized");
}

void UsbHostManager::update()
{
    usbHost.Task();

    for (int i = 0; i < TOTAL_USB_DRIVERS; ++i)
    {
        USBDriver* driver = allDrivers[i];
        bool isCurrentlyConnected = (*driver);

        AppDeviceType deviceType = AppDeviceType::UNKNOWN;

        if (i < NUM_USB_MIDI_DEVICES)
        {
            deviceType = AppDeviceType::MIDI;
        }
        else if (i < NUM_USB_MIDI_DEVICES + NUM_USB_STORAGE_DEVICES)
        {
            deviceType = AppDeviceType::STORAGE;
        }

        if (isCurrentlyConnected && !allDeviceStates[i].isConnected)
        {
            allDeviceStates[i].isConnected = true;
            allDeviceStates[i].vendorId = driver->idVendor();
            allDeviceStates[i].productId = driver->idProduct();
            notifyDeviceConnected(driver, deviceType);
        }
        else if (!isCurrentlyConnected && allDeviceStates[i].isConnected)
        {
            notifyDeviceDisconnected(driver, allDeviceStates[i].vendorId, allDeviceStates[i].productId, deviceType);
            allDeviceStates[i] = DeviceState(); // Reset state
        }
    }
}


void UsbHostManager::addListener(IUsbDeviceListener* listener)
{
    deviceListeners.push_back(listener);
}

void UsbHostManager::notifyDeviceConnected(USBDriver* device, AppDeviceType type)
{
    for (auto& listener : deviceListeners)
    {
        listener->onUsbDeviceConnected(device, type);
    }
    Logger::log("USB Device Connected: " + String(device->idVendor(), HEX) + ":" + String(device->idProduct(), HEX));
}

void UsbHostManager::notifyDeviceDisconnected(USBDriver* device, uint16_t vendorId, uint16_t productId, AppDeviceType type)
{
    for (auto& listener : deviceListeners)
    {
        listener->onUsbDeviceDisconnected(device, type);
    }
    Logger::log("USB Device Disconnected: " + String(vendorId, HEX) + ":" + String(productId, HEX));
}