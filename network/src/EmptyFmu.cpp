/*
 * NetworkFmu.cpp
 *
 *  Created on: 03.06.2016
 *      Author: hartung
 */

#include "../include/EmptyFmu.hpp"

namespace FMI
{

    EmptyFmu::EmptyFmu()
    {
        // TODO Auto-generated constructor stub

    }

    EmptyFmu::~EmptyFmu()
    {
        // TODO Auto-generated destructor stub
    }

    FMI::AbstractFmu* EmptyFmu::duplicate()
    {
        throw std::runtime_error("Network FMUs can't be duplicated.");
    }

    void EmptyFmu::stepCompleted()
    {
    }

    FMI::FmuEventInfo EmptyFmu::eventUpdate()
    {
        return FMI::FmuEventInfo();
    }

    double EmptyFmu::getDefaultStart() const
    {
        return 0.0;
    }

    double EmptyFmu::getDefaultStop() const
    {
        return 0.0;
    }

    void EmptyFmu::initialize()
    {
    }

    void EmptyFmu::getValuesInternal(vector<real_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::getValuesInternal(vector<int_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::getValuesInternal(vector<bool_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::getValuesInternal(vector<string_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::setValuesInternal(const vector<real_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::setValuesInternal(const vector<int_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::setValuesInternal(const vector<bool_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::setValuesInternal(const vector<string_type>& out, const vector<size_type>& references) const
    {
    }

    void EmptyFmu::getStatesInternal(real_type* states) const
    {
    }

    void EmptyFmu::setStatesInternal(const real_type* states)
    {
    }

    void EmptyFmu::getStateDerivativesInternal(real_type* stateDerivatives)
    {
    }

    void EmptyFmu::getEventIndicatorsInternal(real_type* eventIndicators)
    {
    }

} /* namespace Network */
