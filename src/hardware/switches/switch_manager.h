#ifndef SWITCH_MANAGER_H
#define SWITCH_MANAGER_H

#include <Arduino.h>
#include <vector>

#include "config/config.h"
#include "types/events.h"

class SwitchManager
{
    public:
        SwitchManager();
        void init();
        void update();
        void registerListener(ISwitchEventListener* listener);

    private:
        void initializePins();
        void scanSwitches();
        void fireSwitchEvent(uint8_t switchId, uint8_t state);

        std::vector<ISwitchEventListener*> listeners;

        uint32_t lastScan;
        uint8_t previousSwitchesState;
};

#endif