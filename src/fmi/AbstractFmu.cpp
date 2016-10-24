/*
 * AbstractFmu.cpp
 *
 *  Created on: 19.10.2016
 *      Author: mf
 */

#include "fmi/AbstractFmu.hpp"

namespace FMI
{

    AbstractFmu::AbstractFmu(const Initialization::FmuPlan & in)
        : _id(in.id),
          _localId(std::numeric_limits<size_type>::max()),
          _sharedId(std::numeric_limits<size_type>::max()),
          _name(in.name),
          _path(in.path),
          _workingPath(in.workingPath),
          _loaded(false),
          _relativeTolerance(in.relTol),
          _toleranceControlled(in.tolControlled),
          _loggingEnabled(in.logEnabled),
          _intermediateResults(in.intermediateResults),
          _eventInfo(),
          _outputValueReferences(),
          _inputValueReferences(),
          _time(0.0),
          _valueInfo(),
          _numberOfStates(-1),
          _numberOfEventIndicators(-1)
    {
    }

    AbstractFmu::~AbstractFmu()
    {
        if (isLoaded())
            unload();
    }

    void AbstractFmu::load(const bool & alsoInit)
    {
        _loaded = true;
    }

    void AbstractFmu::unload()
    {
        _loaded = false;
    }

    const string & AbstractFmu::getPath() const
    {
        return _path;
    }

    const string & AbstractFmu::getWorkingDirectory() const
    {
        return _workingPath;
    }

    bool AbstractFmu::isLoaded() const
    {
        return _loaded;
    }

    const string& AbstractFmu::getFmuName() const
    {
        return _name;
    }

    size_type AbstractFmu::getId() const
    {
        return _id;
    }

    size_type AbstractFmu::getLocalId() const
    {
        return _localId;
    }

    void AbstractFmu::setLocalId(size_type localId)
    {
        _localId = localId;
    }

    size_type AbstractFmu::getNumEventIndicators() const
    {
        return _numberOfEventIndicators;
    }

    void AbstractFmu::setNumEventIndicators(size_type numberOfEventIndicators)
    {
        this->_numberOfEventIndicators = numberOfEventIndicators;
    }

    size_type AbstractFmu::getNumStates() const
    {
        return _numberOfStates;
    }

    void AbstractFmu::setNumStates(size_type numberOfStates)
    {
        this->_numberOfStates = numberOfStates;
    }

    size_type AbstractFmu::getNumInputs() const
    {
        return _inputValueReferences.size();
    }

    size_type AbstractFmu::getNumOutputs() const
    {
        return _outputValueReferences.size();
    }

    real_type AbstractFmu::getTime() const
    {
        return _time;
    }

    void AbstractFmu::setTime(const real_type & in)
    {
        _time = in;
    }

    const ValueReferenceCollection & AbstractFmu::getAllValueReferences() const
    {
        return _allValueReferences;
    }

    void AbstractFmu::setAllValueReferences(const ValueReferenceCollection& valueReferences)
    {
        _allValueReferences = valueReferences;
    }

    const ValueReferenceCollection & AbstractFmu::getOutputValueReferences() const
    {
        return _outputValueReferences;
    }

    void AbstractFmu::setStates(const real_type * states)
    {
        return setStatesInternal(states);
    }

    void AbstractFmu::setStates(const vector<real_type> & states)
    {
        return setStates(states.data());
    }

    void AbstractFmu::getStates(real_type * states) const
    {
        getStatesInternal(states);
    }

    void AbstractFmu::getStates(vector<real_type> & in) const
    {
        getStates(in.data());
    }

    vector<real_type> AbstractFmu::getStates() const
    {
        vector<real_type> res(getNumStates());
        getStates(res);
        return res;
    }

    ValueCollection AbstractFmu::getValues(ReferenceContainerType refType) const
    {
        const ValueReferenceCollection * refs = nullptr;
        switch(refType)
        {
            case ReferenceContainerType::ALL:
                refs = &_allValueReferences;
                break;
            case ReferenceContainerType::START:
                refs = &_startValueReferences;
                break;
            case ReferenceContainerType::EVENT:
                refs = &_eventValueReferences;
                break;
            case ReferenceContainerType::CONTINIOUS:
                refs = &_continousValueReferences;
                break;
        }
        ValueCollection res(refs->getValues<real_type>().size(), refs->getValues<int_type>().size(),
                            refs->getValues<bool_type>().size(), refs->getValues<string_type>().size());
        getAllValues(res);
        return res;
    }

    void AbstractFmu::getStateDerivatives(vector<real_type> & stateDerivatives)
    {
        getStateDerivatives(stateDerivatives.data());
    }

    void AbstractFmu::getStateDerivatives(real_type * stateDerivatives)
    {
        getStateDerivativesInternal(stateDerivatives);
    }

    vector<real_type> AbstractFmu::getStateDerivatives()
    {
        std::vector<real_type> stateDerivatives(getNumStates());
        getStateDerivatives(stateDerivatives.data());
        return stateDerivatives;
    }

    void AbstractFmu::getEventIndicators(vector<real_type> & eventIndicators)
    {
        getEventIndicators(eventIndicators.data());
    }

    void AbstractFmu::getEventIndicators(real_type * eventIndicators)
    {
        getEventIndicatorsInternal(eventIndicators);
    }

    vector<real_type> AbstractFmu::getEventIndicators()
    {
        vector<real_type> eventIndicators(getNumEventIndicators());
        getStateDerivatives(eventIndicators.data());
        return eventIndicators;
    }

    void AbstractFmu::setValues(const ValueCollection & values)
    {
        setValuesInternal(values.getValues<real_type>(), _eventValueReferences.getValues<real_type>());
        setValuesInternal(values.getValues<int_type>(), _eventValueReferences.getValues<int_type>());
        setValuesInternal(values.getValues<bool_type>(), _eventValueReferences.getValues<bool_type>());
        setValuesInternal(values.getValues<string_type>(), _eventValueReferences.getValues<string_type>());
    }

    void AbstractFmu::setOutputValueReferences(const ValueReferenceCollection& outputValues)
    {
        this->_outputValueReferences = outputValues;
    }

    double AbstractFmu::getRelativeTolerance() const
    {
        return _relativeTolerance;
    }

    bool AbstractFmu::isToleranceControlled() const
    {
        return _toleranceControlled;
    }

    const ValueReferenceCollection& AbstractFmu::getInputValueReferences() const
    {
        return _inputValueReferences;
    }

    void AbstractFmu::setInputValueReferences(const ValueReferenceCollection & inputValues)
    {
        this->_inputValueReferences = inputValues;
    }

    bool AbstractFmu::isLoggingEnabled() const
    {
        return _loggingEnabled;
    }

    void AbstractFmu::setLoggingEnabled(bool loggingEnabled)
    {
        _loggingEnabled = true;
    }

    const ValueInfo & AbstractFmu::getValueInfo() const
    {
        return _valueInfo;
    }

    void AbstractFmu::setValueInfo(const ValueInfo & valueInfo)
    {
        _valueInfo = valueInfo;
    }

    bool AbstractFmu::hasIntermediateResults() const
    {
        return _intermediateResults;
    }

    void AbstractFmu::setRelativeTolerance(real_type relativeTolerance)
    {
        _relativeTolerance = relativeTolerance;
    }

    void AbstractFmu::getValues(ValueCollection & values) const
    {
        getValuesInternal(values.getValues<real_type>(), _eventValueReferences.getValues<real_type>());
        getValuesInternal(values.getValues<int_type>(), _eventValueReferences.getValues<int_type>());
        getValuesInternal(values.getValues<bool_type>(), _eventValueReferences.getValues<bool_type>());
        getValuesInternal(values.getValues<string_type>(), _eventValueReferences.getValues<string_type>());
    }

    void AbstractFmu::getAllValues(ValueCollection & values) const
    {
        getValuesInternal(values.getValues<real_type>(), _allValueReferences.getValues<real_type>());
        getValuesInternal(values.getValues<int_type>(), _allValueReferences.getValues<int_type>());
        getValuesInternal(values.getValues<bool_type>(), _allValueReferences.getValues<bool_type>());
        //getValuesInternal(values.getValues<string_type>(), _allValueReferences.getValues<string_type>());
    }

    const FmuEventInfo& AbstractFmu::getEventInfo() const
    {
        return _eventInfo;
    }

    vector<Synchronization::ConnectionSPtr> & AbstractFmu::getConnections()
    {
        return _fmuCons;
    }

    const vector<Synchronization::ConnectionSPtr> & AbstractFmu::getConnections() const
    {
        return _fmuCons;
    }

    void AbstractFmu::setConnections(const vector<Synchronization::ConnectionSPtr> & in)
    {
        _fmuCons = in;
    }

    size_type AbstractFmu::getSharedId() const
    {
        return _sharedId;
    }

    void AbstractFmu::setSharedId(size_type sharedId)
    {
        _sharedId = sharedId;
    }

    void AbstractFmu::setValues(ValueCollection & values, ValueReferenceCollection & references)
    {
        throw std::runtime_error("AbstractFmu: setValues(ValueCollection&,ValueReferenceCollection&) is deprecated.");
        assert(values.getValues<double>().size() == references.getValues<double>().size());
        assert(values.getValues<int_type>().size() == references.getValues<int_type>().size());
        assert(values.getValues<bool_type>().size() == references.getValues<bool_type>().size());
        assert(values.getValues<string_type>().size() == references.getValues<string_type>().size());
    }

} /* namespace FMI */

