#include "routing_manager.h"
#include "utils/logger.h"

RoutingManager::RoutingManager() {};

void RoutingManager::init()
{
    // Nothing to do here for now
}

void RoutingManager::addSource(IMusicalEventSource* source)
{
    if (source)
    {
        sources.push_back(source);
        sourceMap[source->getPortId()] = source;
        Logger::log("Added source: " + String(source->getPortId()));
    }
}

void RoutingManager::addSink(IMusicalEventSink* sink)
{
    if (sink)
    {
        sinks.push_back(sink);
        sinkMap[sink->getPortId()] = sink;
        Logger::log("Added sink: " + String(sink->getPortId()));
    }
}

void RoutingManager::addProcessor(IMusicalEventProcessor* processor)
{
    if (processor)
    {
        processors.push_back(processor);
        sourceMap[processor->getPortId()] = processor;
        sinkMap[processor->getPortId()] = processor;
        Logger::log("Added processor: " + String(processor->getPortId()));
    }
}

void RoutingManager::removeSource(IMusicalEventSource* source)
{
    if (source)
    {
        PortId portId = source->getPortId();
        sourceMap.erase(portId);

        // Remove any routes that involve this source
        routes.erase(std::remove_if(routes.begin(), routes.end(),
            [portId](const Route& route) {
                return route.source->getPortId() == portId;
            }), routes.end());
        
        // Remove the actual source from the vector
        auto it = std::find(sources.begin(), sources.end(), source);
        if (it != sources.end())
        {
            sources.erase(it);
        }

        Logger::log("Removed source: " + String(portId));
    }
}

void RoutingManager::removeSink(IMusicalEventSink* sink)
{
    if (sink)
    {
        PortId portId = sink->getPortId();
        sinkMap.erase(portId);

        // Remove any routes that involve this sink
        routes.erase(std::remove_if(routes.begin(), routes.end(),
            [portId](const Route& route) {
                return route.sink->getPortId() == portId;
            }), routes.end());

        // Remove the actual sink from the vector
        auto it = std::find(sinks.begin(), sinks.end(), sink);
        if (it != sinks.end())
        {
            sinks.erase(it);
        }
        Logger::log("Removed sink: " + String(portId));
    }
}

void RoutingManager::removeProcessor(IMusicalEventProcessor* processor)
{
    if (processor)
    {
        auto it = std::find(processors.begin(), processors.end(), processor);
        if (it != processors.end())
        {
            processors.erase(it);
            sourceMap.erase(processor->getPortId());
            sinkMap.erase(processor->getPortId());
            Logger::log("Removed processor: " + String(processor->getPortId()));
        }
    }
}

bool RoutingManager::createRoute(PortId sourcePortId, PortId sinkPortId)
{
    auto sourceIt = sourceMap.find(sourcePortId);
    auto sinkIt = sinkMap.find(sinkPortId);

    if (sourceIt != sourceMap.end() && sinkIt != sinkMap.end())
    {
        Route route{sourceIt->second, sinkIt->second};
        routes.push_back(route);
        sourceIt->second->registerListener(sinkIt->second);
        Logger::log("Created route from " + String(sourcePortId) + " to " + String(sinkPortId));
        return true;
    }
    Logger::log("Failed to create route: Source or Sink not found");
    return false;
}

void RoutingManager::destroyRoute(PortId sourcePortId, PortId sinkPortId)
{
    // TODO: Find the route in the vector and call unregisterListener
}