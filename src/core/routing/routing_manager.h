#ifndef ROUTING_MANAGER_H
#define ROUTING_MANAGER_H

#include "core/interfaces.h"
#include <vector>
#include <map>

struct Route // Connection from source to sink
{
    IMusicalEventSource* source;
    IMusicalEventSink* sink;
};

class RoutingManager
{
    public:
        RoutingManager();
        void init();

        void addSource(IMusicalEventSource* source);
        void addSink(IMusicalEventSink* sink);
        void addProcessor(IMusicalEventProcessor* processor);

        void removeSource(IMusicalEventSource* source);
        void removeSink(IMusicalEventSink* sink);
        void removeProcessor(IMusicalEventProcessor* processor);

        bool createRoute(PortId sourcePortId, PortId sinkPortId);
        void destroyRoute(PortId sourcePortId, PortId sinkPortId);

    private:
        void createStaticComponents();

        std::vector<IMusicalEventSource*> sources;
        std::vector<IMusicalEventSink*> sinks;
        std::vector<IMusicalEventProcessor*> processors;
        std::vector<Route> routes;

        std::map<PortId, IMusicalEventSource*> sourceMap;
        std::map<PortId, IMusicalEventSink*> sinkMap;
};
        
#endif