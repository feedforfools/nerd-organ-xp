#include "switch_manager.h"

SwitchManager::SwitchManager() : lastScan(0), previousSwitchesState(0) {}

void SwitchManager::init()
{
    initializePins();
}

void SwitchManager::initializePins()
{
    for (int i = 0; i < NUM_SWITCH_PINS; ++i)
    {
        pinMode(SWITCH_PINS[i], INPUT_PULLUP);
    }
}

void SwitchManager::update()
{
    if (millis() - lastScan >= SWS_TIMER)
    {
        lastScan = millis(); // Consider scan time within the timer
        scanSwitches();
    }
}

void SwitchManager::registerListener(ISwitchEventListener* listener)
{
    listeners.push_back(listener);
}

void SwitchManager::scanSwitches()
{
    uint8_t currentSwitchesState = 0;
    for (int i = 0; i < NUM_SWITCH_PINS; ++i)
    {
        if (!digitalReadFast(SWITCH_PINS[i]))
        {
            bitSet(currentSwitchesState, i);
        }
    }

    if ((currentSwitchesState & 0b11) != (previousSwitchesState & 0b11))
    {
        fireSwitchEvent(THREE_WAY_SWITCH_ID, currentSwitchesState & 0b11);
    }
    if ((currentSwitchesState >> 2) != (previousSwitchesState >> 2))
    {
        fireSwitchEvent(TWO_WAY_SWITCH_ID, currentSwitchesState >> 2);
    }
    previousSwitchesState = currentSwitchesState;
}

void SwitchManager::fireSwitchEvent(uint8_t switchId, uint8_t state)
{
    SwitchEvent event = { switchId, state };
    for (auto& listener : listeners)
    {
        listener->onSwitchEvent(event);
    }
}