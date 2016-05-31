/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_HISTORYENTRY_HPP_
#define INCLUDE_SYNCHRONIZATION_HISTORYENTRY_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "fmi/ValueCollection.hpp"

namespace Synchronization
{
    /**
     * The class HistoryEntry represents the state values of an FMU at a certain point_type in simulation time which is stored
     * in the DataHistory class. A HistoryEntry is precisely defined by the three attributes: point_type of time, order of the
     * solver and a pointer to the values itself (\ref FMI::ValueCollection).
     */
    class HistoryEntry
    {
     public:
        /**
         * Create HistoryEntry element from parameters.
         * @param time Point_type in simulation time.
         * @param solverOrder Order of the interpolation solver [default: 0].
         * @param vals Values to store in this HistoryEntry.
         */
        HistoryEntry(real_type time, size_type solverOrder = 0, const FMI::ValueCollection & vals = FMI::ValueCollection(), bool_type event = false);

        HistoryEntry(real_type time, size_type solverOrder, FMI::ValueCollection && vals, bool_type event);

        HistoryEntry(const FMI::ValueCollection & vals);

        HistoryEntry();

        /**
         *  Delete HistoryEntry element.
         */
        ~HistoryEntry()
        {
        }

        static HistoryEntry invalid();

        bool_type isValid() const;

        bool_type hasEvent() const;

        /**
         * Comparison operator to obtain a strict weak ordering criterion for the set container. The class
         * \ref Synchronization::SubHistory uses this class HistoryEntry as set container.
         * @param in
         * @return Return true if time value of lhs object (= this) is smaller than rhs.
         */
        bool operator<(const HistoryEntry & in) const;

        real_type getTime() const;

        size_type getSolverOrder() const;

        FMI::ValueCollection & getValueCollection();

        const FMI::ValueCollection & getValueCollection() const;

        void setTime(real_type t);

        void setSolverOrder(size_type so);

     private:
        real_type _time;
        size_type _solverOrder;
        bool_type _hasEvent;
        FMI::ValueCollection _element;
    };


} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_HISTORYENTRY_HPP_ */
/**
 * @}
 */
