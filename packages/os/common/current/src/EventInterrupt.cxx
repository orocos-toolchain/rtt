
#include "os/EventInterruptInterface.hpp"
#include "os/ISRInterface.hpp"
#include "os/DSRInterface.hpp"

#include <algorithm>

namespace ORO_OS
{

    std::vector<EventInterruptInterface*> EventInterruptInterface::_instances;

    EventInterruptInterface* EventInterruptInterface::InstanceGet(unsigned int irq)
    {
        std::vector<EventInterruptInterface*>::iterator itl = _instances.begin();
        while ((*itl)->IRQGet() != irq && itl != _instances.end() )
            ++itl;
        return (itl == _instances.end() ? 0 : *itl);
    }

    void EventInterruptInterface::InstanceAdd(EventInterruptInterface* ei)
    {
        _instances.push_back(ei);
    }

    void EventInterruptInterface::InstanceRemove(EventInterruptInterface* ei)
    {
        std::vector<EventInterruptInterface*>::iterator itl;
        itl = std::find(_instances.begin(), _instances.end(), ei);
        if ( itl != _instances.end() ) _instances.erase(itl);
    }

};
