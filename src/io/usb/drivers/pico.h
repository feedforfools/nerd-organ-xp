#ifndef PICO_H
#define PICO_H

#include <USBHost_t36.h>

#define PICO_VENDOR_ID 0x4452
#define PICO_PRODUCT_ID 0x03C0

class PICO_MIDI : public MIDIDevice
{
    public:
        PICO_MIDI(USBHost &host) : MIDIDevice(host), bClaimed(false) {}

    protected:
        virtual bool claim(Device_t *dev, int type, const uint8_t *descriptors, uint32_t len) override
        {
            if (bClaimed) return false; // Added to prevent multiple claims => PICO has two MIDI interfaces (TODO: needs investigation)
            if (type != 1) return false; // Claim only at interface level
            if (dev->idVendor != PICO_VENDOR_ID || dev->idProduct != PICO_PRODUCT_ID) return false; // Skip non-PICO devices

            // This logic is adapted directly from the original MIDIDeviceBase::claim in USBHost_t36/midi.cpp
            // but removing the initial check for bInterfaceClass => this enables also vendor-specific device to be accepted
            const uint8_t *p = descriptors;
            const uint8_t *end = p + len;
            uint8_t local_rx_ep = 0, local_tx_ep = 0;
            uint16_t local_rx_size = 0, local_tx_size = 0;
            uint8_t local_rx_ep_type = 0, local_tx_ep_type = 0;
            if (p[0] < 9 || p[1] != 4) return false;

            // Iterating through the descriptors of an interface to find the right kind of endpoints (Bulk or Interrupt => compatible with MIDI)
            const uint8_t* current_descriptor = p + p[0];
            while (current_descriptor < end)
            {
                uint8_t desc_len = current_descriptor[0];
                if (current_descriptor + desc_len > end) break;
                uint8_t desc_type = current_descriptor[1];

                // We only care about Endpoint descriptors
                if (desc_type == 5)
                {
                    // Endpoint type 2 is Bulk, type 3 is Interrupt => both are fine for MIDI
                    // Type 1 is Isochronous (audio) => we must ignore
                    if (desc_len >= 7 && (current_descriptor[3] == 2 || current_descriptor[3] == 3))
                    { 
                        if ((current_descriptor[2] & 0x80) && local_rx_ep == 0)         // IN Endpoint
                        {
                            local_rx_ep = current_descriptor[2] & 0x0F;
                            local_rx_ep_type = current_descriptor[3];
                            local_rx_size = current_descriptor[4] | (current_descriptor[5] << 8);
                        }
                        else if (!(current_descriptor[2] & 0x80) && local_tx_ep == 0)   // OUT Endpoint
                        {
                            local_tx_ep = current_descriptor[2];
                            local_tx_ep_type = current_descriptor[3];
                            local_tx_size = current_descriptor[4] | (current_descriptor[5] << 8);
                        }
                    }
                }
                current_descriptor += desc_len;
            }

            // Claim only if we found at least one Bulk or Interrupt endpoint
            if (local_rx_ep || local_tx_ep)
            {
                Serial.println("JD08_MIDI driver claimed a valid MIDI (Bulk/Interrupt) interface!");
                
                rx_ep = local_rx_ep; tx_ep = local_tx_ep;
                rx_ep_type = local_rx_ep_type; tx_ep_type = local_tx_ep_type;
                rx_size = local_rx_size; tx_size = local_tx_size;

                if (rx_ep && rx_size <= max_packet_size)
                {
                    rxpipe = new_Pipe(dev, rx_ep_type, rx_ep, 1, rx_size);
                    if (rxpipe)
                    {
                        rxpipe->callback_function = rx_callback;
                        queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);
                        rx_packet_queued = true;
                    }
                }
                if (tx_ep && tx_size <= max_packet_size)
                {
                    txpipe = new_Pipe(dev, tx_ep_type, tx_ep, 0, tx_size);
                    if (txpipe)
                    {
                        txpipe->callback_function = tx_callback;
                        tx1_count = 0;
                        tx2_count = 0;
                    }
                }
                bClaimed = true;
                return true;
            }
            return false;
        }

        virtual void disconnect() override
        {
            bClaimed = false; // Reset claimed state
            MIDIDevice::disconnect();
        }
    
    private:
        bool bClaimed;
};

#endif