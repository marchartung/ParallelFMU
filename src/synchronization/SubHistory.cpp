#include "synchronization/SubHistory.hpp"
#include "synchronization/Interpolation.hpp"

namespace Synchronization
{

    SubHistory::SubHistory(const Interpolation & interpolation, const FMI::ValueCollection & bufferScheme)
            : _interpolation(interpolation)
    {
    }

    size_type SubHistory::size() const
    {
        return _entries.size();
    }

    real_type SubHistory::getNewestTime() const
    {
        if (_entries.size() == 0)
            return -1.0 * std::numeric_limits<real_type>::infinity();
        return (*_entries.rbegin()).getTime();
    }

   /* list<const HistoryEntry *> SubHistory::getNewest(const size_type & num) const
    {
        list<const HistoryEntry *> res;
        size_type count = 0;
        for (std::set<HistoryEntry>::const_reverse_iterator it = _entries.crbegin(); it != _entries.crend(); ++it)
        {
            if (++count > num)
                break;
            res.push_front(&(*it));
        }
        return res;
    }*/

    bool_type SubHistory::insert(const HistoryEntry & in)
    {
        //LOGGER_WRITE("Subhistory:Insert timestamp " + to_string(in->getTime()), Util::LC_SOLVER, Util::LL_WARNING);
        if (in.getTime() == getNewestTime())
        {
            LOGGER_WRITE("Subhistory: Element with timestamp " + to_string(in.getTime()) + " already exists", Util::LC_SOLVER, Util::LL_WARNING);
            return true;
        }

        _entries.insert(_entries.end(), in);
        return true;
    }

    size_type SubHistory::deleteOlderThan(const HistoryEntry & in)
    {
        size_type res = _entries.size();
        _entries.erase(_entries.begin(), _entries.lower_bound(in));
        return res - _entries.size();
    }

    FMI::ValueCollection SubHistory::interpolate(const real_type & time) const
    {
        return _interpolation.interpolate(time, _entries);
    }

    FMI::ValueCollection SubHistory::operator[](const real_type & time) const
    {
        if (getNewestTime() >= time)
            return interpolate(time);
        return FMI::ValueCollection();
    }
    const Interpolation& SubHistory::getInterpolation() const {
    	return _interpolation;
    }

} /* namespace Synchronization */


