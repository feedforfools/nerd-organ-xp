#ifndef INTERFACES_H
#define INTERFACES_H

#include "types/events.h"

#include <vector>

class IMusicalEventSink;

class IMusicalEventSource
{
    public:
        virtual ~IMusicalEventSource() = default;

        virtual void registerListener(IMusicalEventSink* sink)
        {
            if (sink)
            {
                listeners.push_back(sink);
            }
        }
        // virtual void unregisterListener(IMusicalEventSink* sink) = 0;
        virtual PortId getPortId() const = 0;

    protected:
        void fireEvent(const MusicalEvent& event);
        std::vector<IMusicalEventSink*> listeners;
};

class IMusicalEventSink
{
    public:
        virtual ~IMusicalEventSink() = default;

        virtual void onMusicalEvent(const MusicalEvent& event) = 0;
        virtual PortId getPortId() const = 0;
};

inline void IMusicalEventSource::fireEvent(const MusicalEvent& event)
{
    for (auto* listener : listeners)
    {
        listener->onMusicalEvent(event);
    }
}

class IMusicalEventProcessor : public IMusicalEventSource, public IMusicalEventSink
{
    public:
        virtual ~IMusicalEventProcessor() = default;
        virtual PortId getPortId() const override = 0;
};

#endif