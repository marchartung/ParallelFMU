#include <synchronization/HistoryEntry.hpp>

namespace Synchronization
{
    HistoryEntry::HistoryEntry(real_type time, size_type solverOrder, const FMI::ValueCollection& vals, bool_type event)
            : _time(time),
              _solverOrder(solverOrder),
              _hasEvent(event),
              _element(vals)
    {
    }

    HistoryEntry::HistoryEntry(real_type time, size_type solverOrder, FMI::ValueCollection&& vals, bool_type event)
            : _time(time),
              _solverOrder(solverOrder),
              _hasEvent(event),
              _element(vals)
    {
    }

    HistoryEntry::HistoryEntry()
            : _time(-1.0),
              _solverOrder(0),
              _hasEvent(false)
    {
    }

    HistoryEntry::HistoryEntry(const FMI::ValueCollection& vals)
            : _time(-std::numeric_limits<real_type>::infinity()),
              _solverOrder(0),
              _hasEvent(false),
              _element(vals)
    {
    }

    HistoryEntry HistoryEntry::invalid()
    {
        return HistoryEntry(std::numeric_limits<real_type>::infinity());
    }

    bool_type HistoryEntry::isValid() const
    {
        return _time != std::numeric_limits<real_type>::infinity();
    }

    bool HistoryEntry::operator<(const HistoryEntry & in) const
    {
        return _time < in._time;
    }

    real_type HistoryEntry::getTime() const
    {
        return _time;
    }

    size_type HistoryEntry::getSolverOrder() const
    {
        return _solverOrder;
    }

    FMI::ValueCollection& HistoryEntry::getValueCollection()
    {
        return _element;
    }

    const FMI::ValueCollection & HistoryEntry::getValueCollection() const
    {
        return _element;
    }

    void HistoryEntry::setTime(real_type t)
    {
        _time = t;
    }

    bool_type HistoryEntry::hasEvent() const
    {
        return _hasEvent;
    }

    void HistoryEntry::setSolverOrder(size_type so)
    {
        _solverOrder = so;
    }

} /* namespace Synchronization */

