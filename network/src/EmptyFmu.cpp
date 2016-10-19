/*
 * NetworkFmu.cpp
 *
 *  Created on: 03.06.2016
 *      Author: hartung
 */

#include "../include/EmptyFmu.hpp"

namespace FMI
{

    EmptyFmu::EmptyFmu(const Initialization::FmuPlan & in)
        : AbstractFmu(in)
    {

    }

    EmptyFmu::~EmptyFmu()
    {

    }

    AbstractFmu* EmptyFmu::duplicate()
    {
        throw std::runtime_error("Network FMUs can't be duplicated.");
    }

    void EmptyFmu::stepCompleted()
    {
    }

    FmuEventInfo EmptyFmu::eventUpdate()
    {
        return FmuEventInfo();
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
        _values.getValues<real_type>(out,references);
    }

    void EmptyFmu::getValuesInternal(vector<int_type>& out, const vector<size_type>& references) const
    {
        _values.getValues<int_type>(out,references);
    }

    void EmptyFmu::getValuesInternal(vector<bool_type>& out, const vector<size_type>& references) const
    {
        _values.getValues<bool_type>(out,references);
    }

    void EmptyFmu::getValuesInternal(vector<string_type>& out, const vector<size_type>& references) const
    {
        _values.getValues<string_type>(out,references);
    }

    void EmptyFmu::setValuesInternal(const vector<real_type>& out, const vector<size_type>& references)
    {
        _values.setValues<real_type>(out,references);
    }

    void EmptyFmu::setValuesInternal(const vector<int_type>& out, const vector<size_type>& references)
    {
        _values.setValues<int_type>(out,references);
    }

    void EmptyFmu::setValuesInternal(const vector<bool_type>& out, const vector<size_type>& references)
    {
        _values.setValues<bool_type>(out,references);
    }

    void EmptyFmu::setValuesInternal(const vector<string_type>& out, const vector<size_type>& references)
    {
        _values.setValues<string_type>(out,references);
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

    void EmptyFmu::setNumValues(const size_type& numReals, const size_type& numInts, const size_type& numBools, const size_type& numStrings)
    {
        _values = ValueCollection(numReals,numInts,numBools,numStrings);
        ValueReferenceCollection collection(numReals,numInts,numBools,numStrings);
        for(size_type i=0;i<collection.getValues<real_type>().size();++i)
            collection.getValues<real_type>()[i] = i;
        for(size_type i=0;i<collection.getValues<int_type>().size();++i)
            collection.getValues<int_type>()[i] = i;
        for(size_type i=0;i<collection.getValues<bool_type>().size();++i)
            collection.getValues<bool_type>()[i] = i;
        for(size_type i=0;i<collection.getValues<string_type>().size();++i)
            collection.getValues<string_type>()[i] = i;
        this->_allValueReferences = collection;
        this->_eventValueReferences = collection;
    }

    const ValueCollection & EmptyFmu::getEmptyFmuValues() const
    {
        return _values;
    }

    void EmptyFmu::getEventIndicatorsInternal(real_type* eventIndicators)
    {
    }

} /* namespace Network */
