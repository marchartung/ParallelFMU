/*
 * RingBufferSubHistory.cpp
 *
 *  Created on: 20.05.2016
 *      Author: hartung
 */

#include "synchronization/RingBufferSubHistory.hpp"

namespace Synchronization
{

    size_type RingBufferSubHistory::size() const
    {
        return _numAddedElems;
    }

    real_type RingBufferSubHistory::getNewestTime() const
    {
        return _entries[_lastInsertedElem].getTime();
    }

    FMI::ValueCollection RingBufferSubHistory::interpolate(const real_type& time)
    {
        while(_entries[_curIndex].getTime() < time)
        {
            if(++_curIndex == _entries.size())
                _curIndex = 0;
        }
        if(std::abs(_entries[_curIndex].getTime()-time) < _interpolation.getTolerance())
            return _entries[_curIndex].getValueCollection();
        tuple<size_type,size_type> range = checkValidEntries();
        return _interpolation.interpolateHistory(_entries,range,_curIndex,time);
    }

    FMI::ValueCollection RingBufferSubHistory::operator [](const real_type& time)
    {
        return interpolate(time);
    }

    const Interpolation& RingBufferSubHistory::getInterpolation() const
    {
        return _interpolation;
    }

    RingBufferSubHistory::RingBufferSubHistory(const Interpolation& interpolation, const FMI::ValueCollection& bufferScheme, size_type size)
            : _curIndex(0),
              _lastInsertedElem(size - 1),
              _entries(std::vector<HistoryEntry>(size, HistoryEntry(bufferScheme))),
              _interpolation(interpolation),
              _numAddedElems(0)
    {
    }

    bool_type RingBufferSubHistory::insert(const HistoryEntry& in)
    {
        bool res = true;
        //LOGGER_WRITE("Insert " + to_string(in.getTime()) + " into history.",Util::LC_SOLVER, Util::LL_DEBUG);
        if (in.getTime() >= _entries[_lastInsertedElem].getTime())
        {
            if (++_lastInsertedElem == _entries.size())
                _lastInsertedElem = 0;
            _entries[_lastInsertedElem] = in;
            ++_numAddedElems;
        }
        else
        {
            throw std::runtime_error("RingBufferSubHistory: Can't insert values older than the newest in the history");
            res = false;
        }
        return res;
    }

    size_type RingBufferSubHistory::deleteOlderThan(const HistoryEntry& in)
    {
        // ring buffer, it deletes it naturally
        return 1u;
    }

    tuple<size_type, size_type> RingBufferSubHistory::checkValidEntries() const
    {
        if(_numAddedElems > 1)
        {
            return std::make_tuple(((_curIndex == 0) ? _entries.size()-1 : _curIndex-1),_curIndex);
        }
        else
            throw std::runtime_error("RingBufferSubHistory: Not enough data for interpolation.");
    }

} /* namespace Synchronization */

