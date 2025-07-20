#ifndef USB_HOST_MANAGER_H
#define USB_HOST_MANAGER_H

#include <USBHost_t36.h>
#include <vector>

#include "config/config.h"

enum class AppDeviceType // Avoiding RTTI
{
    MIDI,
    STORAGE,
    UNKNOWN
};

class IUsbDeviceListener;

class UsbHostManager
{
    public:
        UsbHostManager();
        void init();
        void update();

        void addListener(IUsbDeviceListener* listener);
        // void removeDeviceListener(IUsbDeviceListener* listener);

    private:
        struct DeviceState
        {
            bool isConnected = false;
            uint16_t vendorId = 0;
            uint16_t productId = 0;
        };

        void notifyDeviceConnected(USBDriver* device, AppDeviceType type);
        void notifyDeviceDisconnected(USBDriver* device, uint16_t vendorId, uint16_t productId, AppDeviceType type);
        
        USBHost usbHost;
        USBHub hub1{usbHost};

        USBDrive driveDevices[NUM_USB_STORAGE_DEVICES]{usbHost, usbHost, usbHost, usbHost};
        MIDIDevice_BigBuffer midiDevices[NUM_USB_MIDI_DEVICES]{usbHost, usbHost, usbHost, usbHost};

        USBDriver* allDrivers[TOTAL_USB_DRIVERS];
        DeviceState allDeviceStates[TOTAL_USB_DRIVERS];        

        std::vector<IUsbDeviceListener*> deviceListeners;

};

class IUsbDeviceListener
{
    public:
        virtual ~IUsbDeviceListener() = default;
        virtual void onUsbDeviceConnected(USBDriver* device, AppDeviceType type) = 0;
        virtual void onUsbDeviceDisconnected(USBDriver* device, AppDeviceType type) = 0;
};

#endif