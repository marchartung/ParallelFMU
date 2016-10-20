/*
 * RingBufferSubHistory.h
 *
 *  Created on: 20.05.2016
 *      Author: hartung
 */

#ifndef INCLUDE_SYNCHRONIZATION_RINGBUFFERSUBHISTORY_HPP_
#define INCLUDE_SYNCHRONIZATION_RINGBUFFERSUBHISTORY_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "synchronization/HistoryEntry.hpp"
#include "synchronization/Interpolation.hpp"

namespace Synchronization
{

    class RingBufferSubHistory
    {
     public:

        size_type size() const;

        real_type getNewestTime() const;

        FMI::ValueCollection interpolate(const real_type & time);

        FMI::ValueCollection operator[](const real_type & time);

        const Interpolation& getInterpolation() const;

        friend class AbstractDataHistory;

     private:
        size_type _curIndex;
        size_type _lastInsertedElem;
        std::vector<HistoryEntry> _entries;
        Interpolation _interpolation;

        size_type _numAddedElems;

        RingBufferSubHistory() = delete;

        RingBufferSubHistory(const Interpolation & interpolation, const FMI::ValueCollection & bufferScheme,
                             size_type size = 300);

        bool_type insert(const HistoryEntry & in);

        size_type deleteOlderThan(const HistoryEntry & in);

        /**
         * Check, if _numAddedElems is greater than 1 and returns ...
         * Otherwise, an exception is thrown.
         */
        tuple<size_type, size_type> checkValidEntries() const;
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_RINGBUFFERSUBHISTORY_HPP_ */
