#ifndef KEYBOARD_SCANNER_H
#define KEYBOARD_SCANNER_H

#include <Arduino.h>
#include <vector>

#include "config/config.h"
#include "types/events.h"
#include "types/types.h"

class KeyboardScanner
{
    public:
        KeyboardScanner();
        void init();
        void scan();
        void updateTimers();

        void registerListener(IKeyEventListener *listener);

    private:
        bank_t banks[NUM_BANKS];
        bank_t prev_banks[NUM_BANKS];
        key_fatar_t keys[NUM_KEYS];
        
        teensy_port_reg mplxEnable[NUM_KEYZONES];
        teensy_port_reg mplxAddress[NUM_BIT_MPLX];
        teensy_port_reg gridMatrixInput[NUM_BANKS];

        std::vector<IKeyEventListener*> listeners;

        void initializePins();
        void initializeKeys();
        void processScanResults();
        byte compactDataRegister(word dataRegister, teensy_port_reg arrangement[], int size);
        void triggerKeyEvent(uint8_t keyIndex, event_t event);
};

#endif