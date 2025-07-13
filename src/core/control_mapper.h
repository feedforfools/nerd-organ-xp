#ifndef CONTROL_MAPPER_H
#define CONTROL_MAPPER_H

#include "types/controllable.h"
#include "types/events.h"

#include <vector>

class ControlMapper : public ISwitchEventListener
{
    public:
        ControlMapper() = default;

        void onSwitchEvent(const SwitchEvent& event) override;
        void addMapping(const ControlMapping& mapping);

    private:
        std::vector<ControlMapping> mappings;
};

#endif