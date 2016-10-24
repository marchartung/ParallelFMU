#include "synchronization/Interpolation.hpp"

namespace Synchronization
{

    Interpolation::Interpolation(const real_type & tolerance)
            : _tolerance(tolerance)
    {
    }

    template<>
    vector<real_type> Interpolation::internalInterpolate(const real_type & curTime,
                                                         const list<const HistoryEntry*> & in) const
    {
        const vector<real_type> & v1 = in.front()->getValueCollection().getValues<real_type>(), &v2 = in.back()
                ->getValueCollection().getValues<real_type>();
        real_type t1 = in.front()->getTime(), t2 = in.back()->getTime();
        vector<real_type> res(v1.size());
        real_type h = (curTime - t1) / (t2 - t1);
        for (size_type i = 0; i < v1.size(); ++i)
        {
            res[i] = v1[i] + h * (v2[i] - v1[i]);
        }
        return res;
    }

    template<>
    vector<int_type> Interpolation::internalInterpolate(const real_type & curTime,
                                                        const list<const HistoryEntry*> & in) const
    {
        const vector<int_type> & v1 = in.front()->getValueCollection().getValues<int_type>(), &v2 = in.back()
                ->getValueCollection().getValues<int_type>();
        int_type t1 = in.front()->getTime(), t2 = in.back()->getTime();
        vector<int_type> res(v1.size());
        real_type h = (curTime - t1) / (t2 - t1);
        for (size_type i = 0; i < v1.size(); ++i)
            res[i] = (h > 0.5) ? v2[i] : v1[i];  //TODO maybe continuous interpolation??

        return res;
    }

    template<>
    vector<bool_type> Interpolation::internalInterpolate(const real_type & curTime,
                                                         const list<const HistoryEntry*> & in) const
    {
        const vector<bool_type> & v1 = in.front()->getValueCollection().getValues<bool_type>(), &v2 = in.back()
                ->getValueCollection().getValues<bool_type>();
        real_type t1 = in.front()->getTime(), t2 = in.back()->getTime();
        vector<bool_type> res(v1.size());
        real_type h = (curTime - t1) / (t2 - t1);
        for (size_type i = 0; i < v1.size(); ++i)
            res[i] = (h > 0.5) ? v2[i] : v1[i];

        return res;
    }

    template<>
    vector<string_type> Interpolation::internalInterpolate(const real_type & curTime,
                                                           const list<const HistoryEntry*> & in) const
    {
        const vector<string_type> & v1 = in.front()->getValueCollection().getValues<string_type>(), &v2 = in.back()
                ->getValueCollection().getValues<string_type>();
        real_type t1 = in.front()->getTime(), t2 = in.back()->getTime();
        vector<string_type> res(v1.size());
        real_type h = (curTime - t1) / (t2 - t1);
        for (size_type i = 0; i < v1.size(); ++i)
            res[i] = (h > 0.5) ? v2[i] : v1[i];

        return res;
    }

    FMI::ValueCollection Interpolation::interpolate(const real_type & curTime, set<HistoryEntry> const & dh,
                                                    int_type order) const
    {
        if (order != 1)
            throw runtime_error("Interpolation: order " + to_string(order) + " is not supported\n");

        auto it = dh.find(HistoryEntry(curTime));
        if (it != dh.end())
            return it->getValueCollection();

        list<const HistoryEntry*> collected = internalCollect(curTime, dh, order + 1);
        if (collected.size() == 1)
            return FMI::ValueCollection(collected.front()->getValueCollection());

        vector<real_type> valVec = internalInterpolate<real_type>(curTime, collected);
        vector<int_type> intVec = internalInterpolate<int_type>(curTime, collected);
        vector<bool_type> bool_typeVec = internalInterpolate<bool_type>(curTime, collected);
        vector<string_type> stringVec = internalInterpolate<string_type>(curTime, collected);

        return FMI::ValueCollection(valVec, intVec, bool_typeVec, stringVec);
    }

    FMI::ValueCollection Interpolation::interpolate(const real_type & curTime, const set<HistoryEntry> & dh) const
    {
        return interpolate(curTime, dh, 1);
    }

    list<const HistoryEntry*> Interpolation::internalCollect(const real_type & curTime, const set<HistoryEntry> & dh,
                                                             size_type n) const
    {
        set<HistoryEntry>::const_iterator it = dh.upper_bound(HistoryEntry(curTime));
        list<const HistoryEntry*> res;
        if (it == dh.end())
        {
            it = dh.lower_bound(HistoryEntry(curTime));

            if (it != dh.end() && std::abs(curTime - it->getTime()) < _tolerance)
            {
                res.push_back(&(*(dh.crbegin())));
                return res;
            }
            else
                throw runtime_error("Interpolation1: SubHistory contains no time after " + to_string(curTime) + ".");
        }
        if (dh.size() < n)
            throw runtime_error(
                    "Interpolation: SubHistory contains only " + to_string(dh.size()) + " (size " + to_string(n)
                            + " needed)");
        set<HistoryEntry>::const_reverse_iterator rit(++it);  // c++ increments once the rev iterator during cast and since it != end a incrementation of it undos this
        for (size_type i = 0; i < n; ++i)
        {
            res.push_front(&(*rit));
            ++rit;
        }
        return res;
    }

    real_type Interpolation::getTolerance() const
    {
        return _tolerance;
    }

    FMI::ValueCollection Interpolation::interpolateHistory(const vector<HistoryEntry> & entries,
                                                           const tuple<size_type, size_type> & range,
                                                           const size_type & curI, const size_type & time)
    {
        size_type iStart = get<0>(range), iEnd = get<1>(range);
        return FMI::ValueCollection(interpolateValues<real_type>(entries, iStart, iEnd, time),
                                    interpolateValues<int_type>(entries, iStart, iEnd, time),
                                    interpolateValues<bool_type>(entries, iStart, iEnd, time),
                                    interpolateValues<string_type>(entries, iStart, iEnd, time));
    }

    template<>
    inline vector<real_type> Interpolation::interpolateValues(const vector<HistoryEntry>& entries,
                                                              const size_type& startI, const size_type& endI,
                                                              const size_type& time)
    {
        const vector<real_type> & v1 = entries[startI].getValueCollection().getValues<real_type>(), v2 = entries[endI]
                .getValueCollection().getValues<real_type>();
        real_type t1 = entries[startI].getTime(), t2 = entries[endI].getTime();
        vector<real_type> res(v1.size());
        real_type h = (time - t1) / (t2 - t1);
        for (size_type i = 0; i < v1.size(); ++i)
            res[i] = v1[i] + h * (v2[i] - v1[i]);

        return res;
    }

    template<>
    inline vector<int_type> Interpolation::interpolateValues(const vector<HistoryEntry>& entries,
                                                             const size_type& startI, const size_type& endI,
                                                             const size_type& time)
    {
        size_type bestI = startI;
        real_type timeDiff = std::abs(time - entries[startI].getTime());
        for (size_type i = startI + 1; i < endI; ++i)
            if (std::abs(time - entries[i].getTime()) < timeDiff)
            {
                timeDiff = std::abs(time - entries[i].getTime());
                bestI = i;
            }
        return entries[bestI].getValueCollection().getValues<int_type>();
    }

    template<>
    inline vector<bool_type> Interpolation::interpolateValues(const vector<HistoryEntry>& entries,
                                                              const size_type& startI, const size_type& endI,
                                                              const size_type& time)
    {
        size_type bestI = startI;
        real_type timeDiff = std::abs(time - entries[startI].getTime());
        for (size_type i = startI + 1; i < endI; ++i)
            if (std::abs(time - entries[i].getTime()) < timeDiff)
            {
                timeDiff = std::abs(time - entries[i].getTime());
                bestI = i;
            }
        return entries[bestI].getValueCollection().getValues<bool_type>();
    }

    template<>
    inline vector<string_type> Interpolation::interpolateValues(const vector<HistoryEntry>& entries,
                                                                const size_type& startI, const size_type& endI,
                                                                const size_type& time)
    {
        size_type bestI = startI;
        real_type timeDiff = std::abs(time - entries[startI].getTime());
        for (size_type i = startI + 1; i < endI; ++i)
            if (std::abs(time - entries[i].getTime()) < timeDiff)
            {
                timeDiff = std::abs(time - entries[i].getTime());
                bestI = i;
            }
        return entries[bestI].getValueCollection().getValues<string_type>();
    }

} /* namespace Synchronization */

