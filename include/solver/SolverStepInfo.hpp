/** @addtogroup Solver
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SOLVER_SOLVERSTEPINFO_HPP_
#define INCLUDE_SOLVER_SOLVERSTEPINFO_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "fmi/ValueCollection.hpp"

namespace Solver
{
    enum DependencyStatus
    {
        FREE,
        BLOCKED,
        EVENT
    };

    struct DependencySolverInfo
    {
        DependencyStatus depStatus;
        real_type eventTimeStart;
        real_type eventTimeEnd;
    };

    class SolverStepInfo
    {
     public:
        SolverStepInfo()
                : _write(false),
                  _eventWrite(false)
        {
        }

        virtual ~SolverStepInfo()
        {
        }

        bool_type hasWriteStep() const
        {
            return _write;
        }

        /**
         *
         */
        void setWriteStep(bool_type in)
        {
            _write = in;
        }

        bool_type hasEventWrite() const
        {
            return _eventWrite;
        }

        template<int_type n>
        void setEvent(real_type eventTime, const FMI::ValueCollection & collection)
        {
            _write = true;
            _eventWrite = true;
            get<n>(_timeEvents) = eventTime;
            get<n>(_collections) = collection;
        }

        template<int_type n>
        real_type getEventTime() const
        {
            return get<n>(_timeEvents);
        }

        template<int_type n>
        FMI::ValueCollection & getEventValues()
        {
            return get<n>(_collections);
        }

        template<int_type n>
        FMI::ValueCollection getEventValues() const
        {
            return get<n>(_collections);
        }

        void clear()
        {
            _write = false;
            _eventWrite = false;
            std::get<0>(_timeEvents) = -1.0;
            std::get<1>(_timeEvents) = -1.0;
        }

     private:
        bool_type _write;
        bool_type _eventWrite;

        tuple<real_type, real_type> _timeEvents;
        tuple<FMI::ValueCollection, FMI::ValueCollection> _collections;
    };

} /* namespace Solver */

#endif /* INCLUDE_SOLVER_SOLVERSTEPINFO_HPP_ */
/**
 * @}
 */
