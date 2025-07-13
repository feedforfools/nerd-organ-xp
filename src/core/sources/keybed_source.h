#ifndef KEYBED_SOURCE_H
#define KEYBED_SOURCE_H

#include "config/config.h"
#include "core/interfaces.h"
#include "types/events.h"

class KeybedSource : public IMusicalEventSource, public IKeyEventListener
{
    public:
        KeybedSource();

        void onKeyEvent(const KeyEvent& event) override;
        inline PortId getPortId() const override { return PORT_ID_KEYBED; }

    private:
        bool velocityNoteOnSent[NUM_KEYS];

        int calculateVelocity(uint32_t t);
};

#endif