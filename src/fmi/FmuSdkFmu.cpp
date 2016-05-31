#include <boost/filesystem.hpp>
#include <fmi/FmuSdkFmu.hpp>
#include "fmi/ValueReferenceCollection.hpp"
#include <xml_parser.h>

namespace FMI
{
    map<string_type, tuple<FMU *, size_type>> FmuSdkFmu::_knownFmus;
    map<string_type, FmuSdkFmu*> FmuSdkFmu::_presentFmus;

    FmuSdkFmu::FmuSdkFmu(const Initialization::FmuPlan & in)
            : AbstractFmu(in),
              _fmu(nullptr),
              _component(nullptr)
    {
        //_callbacks.logger = nullptr;
        _callbacks.allocateMemory = calloc;
        _callbacks.freeMemory = free;
    }

    FmuSdkFmu::~FmuSdkFmu()
    {
    }

    void FmuSdkFmu::load()
    {
        _path = boost::filesystem::absolute(_path).string();

        _fmu = getFmuHandle(_path);
        _componentEventInfo = fmiEventInfo();
        if (_fmu == nullptr)
        {
            FmuSdkFmu * same = _presentFmus.at(_path);
            _fmu = same->_fmu;
            _workingPath = same->_workingPath;
            ModelDescription *modelDescription = _fmu->modelDescription;
            string_type guid = string_type(getString(modelDescription, att_guid));
            _component = _fmu->instantiateModel(getModelIdentifier(modelDescription), guid.c_str(), _callbacks, _loggingEnabled);

            _allValueReferences = same->_allValueReferences;
            _startValueReferences = same->_startValueReferences;
            _eventValueReferences = same->_eventValueReferences;
            _continousValueReferences = same->_continousValueReferences;
            _outputValueReferences = same->_outputValueReferences;
            _inputValueReferences = same->_inputValueReferences;

            _startValues = same->_startValues;
            _valueInfo = same->_valueInfo;
            _numberOfStates = same->_numberOfStates;
            _numberOfEventIndicators = same->_numberOfEventIndicators;
        }
        else
        {

            _workingPath = boost::filesystem::absolute(_workingPath).string();
            ModelDescription *modelDescription = _fmu->modelDescription;
            string_type guid = string_type(getString(modelDescription, att_guid));
            LOGGER_WRITE(string_type("Try to load FMU from ") + _path + string_type(" and work on ") + _workingPath, Util::LC_LOADER, Util::LL_DEBUG);

            _component = _fmu->instantiateModel(getModelIdentifier(modelDescription), guid.c_str(), _callbacks, _loggingEnabled);

            if (_component == nullptr)
                throw runtime_error("Could not instantiate FMU");

            for (size_type i = 0; modelDescription->modelVariables[i] != nullptr; i++)
            {
                ScalarVariable *variable = modelDescription->modelVariables[i];

                switch (getVariableType(variable))
                {
                    case VarType::varReal:
                        addVariable<real_type>(modelDescription->modelVariables[i]);
                        break;
                    case VarType::varInt:
                        addVariable<int_type>(modelDescription->modelVariables[i]);
                        break;
                    case VarType::varBool:
                        addVariable<bool_type>(modelDescription->modelVariables[i]);
                        break;
                    case VarType::varString:
                        addVariable<string_type>(modelDescription->modelVariables[i]);
                        break;
                    case VarType::varEnum:
                        addVariable<int_type>(modelDescription->modelVariables[i]);  // enums are treated as integers
                        break;
                    default:
                        throw std::runtime_error("FmuSdk: VarType unkown.");
                }
            }

            _numberOfStates = getNumberOfStates(modelDescription);
            _numberOfEventIndicators = getNumberOfEventIndicators(modelDescription);

            //std::cout << "start refs: [" << _startValueReferences << "]\n";
            //std::cout << "start vals: [" << _startValues << "]\n";
            _presentFmus[_path] = this;
        }
        AbstractFmu::load();
        initialize();
    }

    void FmuSdkFmu::unload()
    {
        AbstractFmu::unload();
        _fmu->freeModelInstance(_component);
        --std::get<1>(_knownFmus.at(std::string(_fmu->path)));
        if (std::get<1>(_knownFmus.at(std::string(_fmu->path))) == 0)
        {
            _knownFmus.erase(std::string(_fmu->path));
            unloadFMU(_fmu);
        }
        //TODO
    }

    inline void FmuSdkFmu::initialize()
    {
        fmiStatus fmiFlag;
        if (!_startValueReferences.getValues<real_type>().empty())
            _fmu->setReal(_component, _startValueReferences.getValues<real_type>().data(), _startValueReferences.getValues<real_type>().size(),
                          _startValues.getValues<real_type>().data());

        if (!_startValueReferences.getValues<bool_type>().empty())
            _fmu->setBoolean(_component, _startValueReferences.getValues<real_type>().data(), _startValueReferences.getValues<real_type>().size(),
                             _startValues.getValues<bool_type>().data());

        if (!_startValueReferences.getValues<int_type>().empty())
            _fmu->setInteger(_component, _startValueReferences.getValues<real_type>().data(), _startValueReferences.getValues<real_type>().size(),
                             _startValues.getValues<int_type>().data());

        fmiFlag = _fmu->initialize(_component, isToleranceControlled(), getRelativeTolerance(), &_componentEventInfo);
        if (fmiFlag > fmiWarning)
            throw runtime_error("Could not initialize model");
    }

    void FmuSdkFmu::stepCompleted()
    {
        char stepEvent;
        fmiStatus fmiFlag = _fmu->completedIntegratorStep(_component, &stepEvent);
        if (fmiFlag > fmiWarning)
            runtime_error("Could not complete integrator step of the given FMU.");
    }

    FmuEventInfo FmuSdkFmu::eventUpdate()
    {
        fmiStatus fmiFlag = _fmu->eventUpdate(_component, _intermediateResults, &_componentEventInfo);
        if (fmiFlag > fmiWarning)
            runtime_error("Could not perform event update.");
        _eventInfo.assign(_componentEventInfo);
        return _eventInfo;
    }

    double FmuSdkFmu::getDefaultStart() const
    {
        if (_fmu->modelDescription == NULL)
            runtime_error("No model description defined in FMU-structure.");

        return getDefaultStartTime(_fmu->modelDescription);
    }

    double FmuSdkFmu::getDefaultStop() const
    {
        if (_fmu->modelDescription == NULL)
            runtime_error("No model description defined in FMU-structure.");

        return getDefaultStopTime(_fmu->modelDescription);
    }

    void FmuSdkFmu::getStatesInternal(real_type * states) const
    {
        fmiStatus fmiFlag = _fmu->getContinuousStates(_component, states, getNumStates());
        if (fmiFlag > fmiWarning)
            runtime_error("Could not get the state variable indices of the given FMU.");
    }

    void FmuSdkFmu::getStateDerivativesInternal(real_type * stateDerivatives)
    {
        fmiStatus fmiFlag = _fmu->getDerivatives(_component, stateDerivatives, getNumStates());
        if (fmiFlag > fmiWarning)
            runtime_error("Could not get state derivatives of given FMU.");
    }

    void FmuSdkFmu::setStatesInternal(const real_type * states)
    {
        fmiStatus fmiFlag = _fmu->setContinuousStates(_component, states, getNumStates());
        if (fmiFlag > fmiWarning)
            runtime_error("Could not set states for given fmu.");
    }

    void FmuSdkFmu::setTime(const double & time)
    {
        AbstractFmu::setTime(time);
        fmiStatus fmiFlag = _fmu->setTime(_component, time);
        if (fmiFlag > fmiWarning)
            runtime_error("Could not set time for given fmu.");
    }

    void FmuSdkFmu::getEventIndicatorsInternal(real_type * eventIndicators)
    {
        fmiStatus fmiFlag = _fmu->getEventIndicators(_component, eventIndicators, getNumEventIndicators());
        if (fmiFlag > fmiWarning)
            runtime_error("Could not read event indicators from given FMU.");
    }

    FMU* FmuSdkFmu::getFmuHandle(string_type fmuName) const
    {
        auto iter = _knownFmus.find(fmuName);
        if (iter != _knownFmus.end())
        {
            ++std::get<1>(iter->second);
            return nullptr;
        }

        FMU *fmu = loadFMU(fmuName.c_str());
        _knownFmus.insert(pair<string_type, tuple<FMU*, size_type>>(fmuName, make_tuple(fmu, 1)));
        return fmu;
    }

    void FmuSdkFmu::fillNameVector(vector<string_type>& valueNames, const map<int, string_type>& valueNamesIdx)
    {
        for (auto iter : valueNamesIdx)
        {
            cout << "i:" << iter.first << ": " << iter.second << "\n";
            valueNames[iter.first] = iter.second;
        }
    }

    void FmuSdkFmu::getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const
    {
        _fmu->getReal(_component, references.data(), references.size(), out.data());
    }

    void FmuSdkFmu::getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const
    {
        _fmu->getInteger(_component, references.data(), references.size(), out.data());
    }

    void FmuSdkFmu::getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const
    {
        char charValues[references.size()];
        _fmu->getBoolean(_component, references.data(), references.size(), charValues);
        for (size_type i = 0; i < references.size(); i++)
            out[i] = charValues[i];
    }

    void FmuSdkFmu::getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const
    {
        const char* stringValues[references.size()];
        _fmu->getString(_component, references.data(), references.size(), stringValues);
        for (size_type i = 0; i < references.size(); i++)
            out[i] = string_type(stringValues[i]);
    }

    void FmuSdkFmu::setValuesInternal(const vector<real_type> & in, const vector<size_type> & references) const
    {
        _fmu->setReal(_component, references.data(), references.size(), in.data());
    }

    void FmuSdkFmu::setValuesInternal(const vector<int_type> & in, const vector<size_type> & references) const
    {
        _fmu->setInteger(_component, references.data(), references.size(), in.data());
    }

    void FmuSdkFmu::setValuesInternal(const vector<bool_type> & in, const vector<size_type> & references) const
    {
        _fmu->setBoolean(_component, references.data(), references.size(), in.data());
    }

    void FmuSdkFmu::setValuesInternal(const vector<string_type> & in, const vector<size_type> & references) const
    {
        const char* stringValues[references.size()];
        for (size_type i = 0; i < references.size(); i++)
            stringValues[i] = in[i].c_str();
        _fmu->setString(_component, references.data(), references.size(), stringValues);
    }

    template<>
    real_type FmuSdkFmu::getStartValue<real_type>(ScalarVariable * variable, ValueStatus & vs)
    {
        return getRealStartValue(variable, &vs);
    }
    template<>
    int_type FmuSdkFmu::getStartValue<int_type>(ScalarVariable * variable, ValueStatus & vs)
    {
        return getIntStartValue(variable, &vs);
    }
    template<>
    bool_type FmuSdkFmu::getStartValue<bool_type>(ScalarVariable * variable, ValueStatus & vs)
    {
        return getBoolStartValue(variable, &vs);
    }
    template<>
    string_type FmuSdkFmu::getStartValue<string_type>(ScalarVariable * variable, ValueStatus & vs)
    {
        return string_type(getStringStartValue(variable, &vs));
    }

    inline AbstractFmu* FmuSdkFmu::duplicate()
    {
        throw std::runtime_error("FmuSdkFmu: duplication not available.");
    }

} /* namespace FMI */
