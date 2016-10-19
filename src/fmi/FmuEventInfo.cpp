#include "fmi/FmuEventInfo.hpp"

namespace FMI
{

    FmuEventInfo::FmuEventInfo(bool_type iterationConverged, bool_type stateValueReferencesChanged,
                               bool_type stateValuesChanged, bool_type terminateSimulation, bool_type upcomingTimeEvent,
                               double nextEventTime)
            : _iterationConverged(iterationConverged),
              _stateValueReferencesChanged(stateValueReferencesChanged),
              _stateValuesChanged(stateValuesChanged),
              _terminateSimulation(terminateSimulation),
              _upcomingTimeEvent(upcomingTimeEvent),
              _nextEventTime(nextEventTime)
    {
    }

    bool_type FmuEventInfo::isIterationConverged() const
    {
        return _iterationConverged;
    }

    void FmuEventInfo::setIterationConverged(bool_type iterationConverged)
    {
        this->_iterationConverged = iterationConverged;
    }

    double FmuEventInfo::getNextEventTime() const
    {
        return _nextEventTime;
    }

    void FmuEventInfo::setNextEventTime(double nextEventTime)
    {
        this->_nextEventTime = nextEventTime;
    }

    bool_type FmuEventInfo::isStateValueReferencesChanged() const
    {
        return _stateValueReferencesChanged;
    }

    void FmuEventInfo::setStateValueReferencesChanged(bool_type stateValueReferencesChanged)
    {
        this->_stateValueReferencesChanged = stateValueReferencesChanged;
    }

    bool_type FmuEventInfo::isStateValuesChanged() const
    {
        return _stateValuesChanged;
    }

    void FmuEventInfo::setStateValuesChanged(bool_type stateValuesChanged)
    {
        this->_stateValuesChanged = stateValuesChanged;
    }

    bool_type FmuEventInfo::isTerminateSimulation() const
    {
        return _terminateSimulation;
    }

    void FmuEventInfo::setTerminateSimulation(bool_type terminateSimulation)
    {
        this->_terminateSimulation = terminateSimulation;
    }

    bool_type FmuEventInfo::isUpcomingTimeEvent() const
    {
        return _upcomingTimeEvent;
    }

    void FmuEventInfo::setUpcomingTimeEvent(bool_type upcomingTimeEvent)
    {
        this->_upcomingTimeEvent = upcomingTimeEvent;
    }

    void FmuEventInfo::assign(const fmiEventInfo & eventInfoSource)
    {
        setIterationConverged(eventInfoSource.iterationConverged);
        setNextEventTime(eventInfoSource.nextEventTime);
        setStateValueReferencesChanged(eventInfoSource.stateValueReferencesChanged);
        setStateValuesChanged(eventInfoSource.stateValuesChanged);
        setTerminateSimulation(eventInfoSource.terminateSimulation);
        setUpcomingTimeEvent(eventInfoSource.upcomingTimeEvent);

    }

#ifdef USE_FMILIB
    void FmuEventInfo::assign(const fmi1_event_info_t & eventInfoSource)
    {

        setIterationConverged(eventInfoSource.iterationConverged);
        setNextEventTime(eventInfoSource.nextEventTime);
        setStateValueReferencesChanged(eventInfoSource.stateValueReferencesChanged);
        setStateValuesChanged(eventInfoSource.stateValuesChanged);
        setTerminateSimulation(eventInfoSource.terminateSimulation);
        setUpcomingTimeEvent(eventInfoSource.upcomingTimeEvent);
    }
#endif

} /* namespace FMI */
