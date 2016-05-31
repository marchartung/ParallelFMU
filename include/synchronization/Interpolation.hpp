/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_INTERPOLATION_HPP_
#define INCLUDE_SYNCHRONIZATION_INTERPOLATION_HPP_

#include "fmi/ValueCollection.hpp"
#include "synchronization/HistoryEntry.hpp"
#include "Stdafx.hpp"
#include "util/Vector.hpp"

namespace Synchronization
{
    class Interpolation
    {
     public:
        Interpolation(const real_type & tolerance = 1.0e-5);

        /**
         * Interpolates a new ValueCollection out of Data in the given DataHistory regarding the given point_type of time
         * @arg curTime the given point_type of time, where for the interpolation takes place
         * @arg dh DataHistory containing the needed data. dh should enclose curTime
         * @arg order order+1 time stamps are used for interpolation.
         */
        FMI::ValueCollection interpolate(const real_type & curTime, const set<HistoryEntry> & dh, int_type order) const;
        /**
         * Interpolates a new ValueCollection out of Data in the given DataHistory regarding the given point_type of time
         * @arg curTime the given point_type of time, where for the interpolation takes place
         * @arg dh DataHistory containing the needed data. dh should enclose curTime
         */
        FMI::ValueCollection interpolate(const real_type & curTime, const set<HistoryEntry> & dh) const;

        real_type getTolerance() const;

        FMI::ValueCollection interpolateHistory(const std::vector<HistoryEntry> & entries, const tuple<size_type, size_type> & range, const size_type & curI,
                                                const size_type & time);

     private:
        template<typename T>
        vector<T> internalInterpolate(const real_type & curTime, const list<const HistoryEntry*> & in) const
        {
            throw std::runtime_error("Interpolation: order not supported");
        }

        list<const HistoryEntry*> internalCollect(const real_type & curTime, const set<HistoryEntry> & dh, size_type n) const;

        template<typename T>
        std::vector<T> interpolateValues(const std::vector<HistoryEntry> & entries, const size_type & startI, const size_type & endI, const size_type & time)
        {
            throw std::runtime_error("Interpolation: type not supported");
        }

        real_type _tolerance;
    };
    template<>
    vector<real_type> Interpolation::interpolateValues(const std::vector<HistoryEntry> & entries, const size_type & startI, const size_type & endI, const size_type & time);
    template<>
    vector<int_type> Interpolation::interpolateValues(const std::vector<HistoryEntry> & entries, const size_type & startI, const size_type & endI, const size_type & time);
    template<>
    vector<bool_type> Interpolation::interpolateValues(const std::vector<HistoryEntry> & entries, const size_type & startI, const size_type & endI, const size_type & time);
    template<>
    vector<string_type> Interpolation::interpolateValues(const std::vector<HistoryEntry> & entries, const size_type & startI, const size_type & endI, const size_type & time);

/* namespace Synchronization */
}

#endif /* INCLUDE_SYNCHRONIZATION_INTERPOLATION_HPP_ */
/**
 * @}
 */
