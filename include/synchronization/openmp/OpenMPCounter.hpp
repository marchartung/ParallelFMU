/*
 * OpenMPCounter.hpp
 *
 *  Created on: 18.12.2015
 *      Author: hartung
 */

#ifndef INCLUDE_SYNCHRONIZATION_OPENMPCOUNTER_HPP_
#define INCLUDE_SYNCHRONIZATION_OPENMPCOUNTER_HPP_

#include <omp.h>
#include "BasicTypedefs.hpp"

namespace Synchronization
{

    class OpenMPCounter
    {
     public:
        OpenMPCounter();

        OpenMPCounter(size_type init);

        virtual ~OpenMPCounter();

        OpenMPCounter & operator++();

        void reset();

        operator size_type ();

     private:
        size_type _counter;
        omp_lock_t _lock;
        bool_type destroyed;
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_OPENMPCOUNTER_HPP_ */
