/*
 * OpenMPCounter.cpp
 *
 *  Created on: 18.12.2015
 *      Author: hartung
 */

#include <iostream>
#include <synchronization/openmp/OpenMPCounter.hpp>

namespace Synchronization
{

    OpenMPCounter::OpenMPCounter()
            : _counter(0), destroyed(false)
    {
        omp_init_lock(&_lock);
    }

    OpenMPCounter::OpenMPCounter(size_type init)
            : _counter(init), destroyed(false)
    {
        omp_init_lock(&_lock);
    }

    OpenMPCounter::~OpenMPCounter()
    {
        omp_destroy_lock(&_lock);
        destroyed = true;
    }

    OpenMPCounter& OpenMPCounter::operator ++()
    {
        omp_set_lock(&_lock);
        ++_counter;
        omp_unset_lock(&_lock);
        return *this;
    }

    void OpenMPCounter::reset()
    {
        omp_set_lock(&_lock);
        _counter = 0;
        omp_unset_lock(&_lock);
    }

    OpenMPCounter::operator size_type()
    {
        size_type res;
        if(destroyed)
        	std::cout << "Counter already disabled\n";
        omp_set_lock(&_lock);
        res = _counter;
        omp_unset_lock(&_lock);

        return res;
    }

} /* namespace Synchronization */
