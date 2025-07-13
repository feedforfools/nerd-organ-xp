#include "control_mapper.h"
#include "utils/logger.h"

void ControlMapper::onSwitchEvent(const SwitchEvent& event)
{
    Logger::log("ControlMapper: Switch event received - ID: " + String(event.switchId) + ", State: " + String(event.state));
    for (const auto &mapping : mappings)
    {
        if (mapping.switchId == event.switchId && mapping.triggerState == event.state)
        {
            mapping.target->setParameter(mapping.parameter, mapping.value);
        }
    }
}

void ControlMapper::addMapping(const ControlMapping& mapping)
{
    mappings.push_back(mapping);
}
