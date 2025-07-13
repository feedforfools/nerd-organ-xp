#ifndef CONTROLLABLE_H
#define CONTROLLABLE_H

enum class ControllableParameter
{
    // NoteProcessor parameters
    PROCESSOR_ENABLE,
    PROCESSOR_TRANSPOSE,
    PROCESSOR_MIDI_CHANNEL,
    PROCESSOR_HIGH_TRIGGER_MODE,
    PROCESSOR_MIN_NOTE,
    PROCESSOR_MAX_NOTE,

    // TODO: what other parameters here?
};

class IControllable
{
    public:
        virtual ~IControllable() = default;

        virtual void setParameter(ControllableParameter param, int value) = 0;
        // virtual int getParameter(ControllableParameter param) const = 0;
};

struct ControlMapping
{
    int switchId;
    int triggerState; // Support also multi-state switches
    IControllable* target;
    ControllableParameter parameter;
    int value;
};

#endif