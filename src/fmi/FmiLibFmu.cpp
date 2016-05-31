#ifdef USE_FMILIB

#include <boost/filesystem.hpp>
#include "fmi/FmiLibFmu.hpp"

namespace FMI
{
    set<string_type> FmiLibFmu::_loadedFmuObjects;

    void deleteFmiLibContext(fmi_import_context_t * in)
    {
        fmi_import_free_context(in);
    }

    void deleteFmiLibFmuHandle(fmi1_import_t * in)
    {
        fmi1_import_free(in);
    }

    FmiLibFmu::FmiLibFmu(const Initialization::FmuPlan & in)
            : AbstractFmu(in),
              _callbacks(),
              _posPath(_loadedFmuObjects.end())
    {
        _callbacks.malloc = malloc;
        _callbacks.calloc = calloc;
        _callbacks.realloc = realloc;
        _callbacks.free = free;
        _callbacks.logger = jm_default_logger;
        _callbacks.log_level = jm_log_level_error;
        _callbacks.context = 0;
    }

    FmiLibFmu::~FmiLibFmu()
    {
        if (_posPath != _loadedFmuObjects.end())
            _loadedFmuObjects.erase(_posPath);
    }

    AbstractFmu * FmiLibFmu::duplicate()
    {
        throw runtime_error("The FMI library is not able to duplicate a fmu instance!");
    }

    void FMI::FmiLibFmu::load()
    {
        if (isLoaded())
        {
            LOGGER_WRITE("FMU already loaded", Util::LC_LOADER, Util::LL_WARNING);
            return;
        }
        LOGGER_WRITE("Using FMILibrary with tolerance", Util::LC_LOADER, Util::LL_DEBUG);
        fmi1_callback_functions_t callBackFunctions;

        fmi_version_enu_t version = fmi_version_1_enu;
        jm_status_enu_t status;

        callBackFunctions.logger = fmi1_log_forwarding;
        callBackFunctions.allocateMemory = calloc;
        callBackFunctions.freeMemory = free;

        _path = boost::filesystem::absolute(_path).string();
        _workingPath = boost::filesystem::absolute(_workingPath).string();

        if (_loadedFmuObjects.find(_path) != _loadedFmuObjects.end())
            throw runtime_error("FMI Library is not able to load a FMU multiple times!");

        //version = fmi_import_get_fmi_version(_context.get(), _path.c_str(), _workingPath.c_str());
        if (version != fmi_version_1_enu)
        {
            throw runtime_error("Only FMU version 1.0 is supported so far.");
        }

        _context = std::shared_ptr<fmi_import_context_t>(fmi_import_allocate_context(&_callbacks), deleteFmiLibContext);

        _fmu = std::shared_ptr<fmi1_import_t>(fmi1_import_parse_xml(_context.get(), _workingPath.c_str()), deleteFmiLibFmuHandle);

        if (!_fmu)
        {
            throw runtime_error("Error parsing XML in FMU, exiting.");
        }

        status = fmi1_import_create_dllfmu(_fmu.get(), callBackFunctions, 0);

        if (status == jm_status_error)
        {
            throw runtime_error("Could not create the DLL loading mechanism (C-API test).");
        }

        status = fmi1_import_instantiate_model(_fmu.get(), "Test ME model instance");
        if (status == jm_status_error)
        {
            throw runtime_error("fmi1_import_instantiate_model failed.");
        }

        fmi1_import_variable_t* var;
        fmi1_import_variable_list_t * vl = fmi1_import_get_variable_list(_fmu.get());

        for (unsigned k = 0; k < fmi1_import_get_variable_list_size(vl); ++k)
        {
            var = fmi1_import_get_variable(vl, k);
            switch (fmi1_import_get_variable_base_type(var))
            {
                case fmi1_base_type_real:
                    addVariable<real_type>(var);
                    break;
                case fmi1_base_type_int:
                    addVariable<int_type>(var);
                    break;
                case fmi1_base_type_bool:
                    addVariable<bool_type>(var);
                    break;
                case fmi1_base_type_str:
                    addVariable<string_type>(var);
                    break;
                default:
                    break;
            }
        }
        setNumStates(fmi1_import_get_number_of_continuous_states(_fmu.get()));
        setNumEventIndicators(fmi1_import_get_number_of_event_indicators(_fmu.get()));
        _posPath = _loadedFmuObjects.insert(_path).first;
        fmi1_import_free_variable_list(vl);
        initialize();
    }

    void FmiLibFmu::initialize()
    {
        fmi1_import_set_real(_fmu.get(), (const unsigned int *) _startValueReferences.getValues<real_type>().data(),
                             _startValueReferences.getValues<real_type>().size(), _startValues.getValues<real_type>().data());
        fmi1_import_set_integer(_fmu.get(), (const unsigned int *) _startValueReferences.getValues<int_type>().data(),
                                _startValueReferences.getValues<int_type>().size(), _startValues.getValues<int_type>().data());
        fmi1_import_set_boolean(_fmu.get(), (const unsigned int *) _startValueReferences.getValues<bool_type>().data(),
                                _startValueReferences.getValues<bool_type>().size(), _startValues.getValues<bool_type>().data());
        //fmi1_import_set_string_type(_fmu.get(), _startValueReferences.getValues<string_type >().data(), _startValueReferences.getValues<string_type >().size(),_startValues.getValues<string_type >().data());

        fmi1_import_initialize(_fmu.get(), toFmi1Boolean(isToleranceControlled()), getRelativeTolerance(), &_fmuEventInfo);
        _eventInfo.assign(_fmuEventInfo);
    }

    double FmiLibFmu::getDefaultStart() const
    {
        return fmi1_import_get_default_experiment_start(_fmu.get());
    }

    double FmiLibFmu::getDefaultStop() const
    {
        return fmi1_import_get_default_experiment_stop(_fmu.get());
    }

    size_type FmiLibFmu::toSizeT(fmi1_value_reference_t value) const
    {
        return value;
    }

    fmi1_boolean_t FmiLibFmu::toFmi1Boolean(bool_type value) const
    {
        if (value)
            return fmi1_true;
        return fmi1_false;
    }

    void FmiLibFmu::getStatesInternal(real_type * states) const
    {
        fmi1_import_get_continuous_states(_fmu.get(), states, getNumStates());
    }

    FmuEventInfo FmiLibFmu::eventUpdate()
    {
        fmi1_import_eventUpdate(_fmu.get(), _intermediateResults, &_fmuEventInfo);
        _eventInfo.assign(_fmuEventInfo);
        return _eventInfo;
    }

    void FmiLibFmu::getEventIndicatorsInternal(real_type * eventIndicators)
    {
        fmi1_import_get_event_indicators(_fmu.get(), eventIndicators, getNumEventIndicators());
    }

    void FmiLibFmu::unload()
    {
        AbstractFmu::unload();
    }

    void FmiLibFmu::getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const
    {
        fmi1_import_get_real(_fmu.get(), (const unsigned int *) references.data(), references.size(), out.data());
    }

    void FmiLibFmu::getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const
    {
        fmi1_import_get_integer(_fmu.get(), (const unsigned int *) references.data(), references.size(), out.data());
    }

    void FmiLibFmu::getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const
    {
        fmi1_import_get_boolean(_fmu.get(), (const unsigned int *) references.data(), references.size(), out.data());
    }

    void FmiLibFmu::getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const
    {
        //const char* stringValues[references.size()];
        //fmi1_import_get_string(_fmu.get(), references.data(), references.size(), stringValues);
        //for (size_type i = 0; i < references.size(); i++)
        //    out[i] = string_type(stringValues[i]);
    }

    void FmiLibFmu::setValuesInternal(const vector<real_type> & in, const vector<size_type> & references) const
    {
        fmi1_import_set_real(_fmu.get(), (const unsigned int *) references.data(), references.size(), in.data());
    }

    void FmiLibFmu::setValuesInternal(const vector<int_type> & in, const vector<size_type> & references) const
    {
        fmi1_import_set_integer(_fmu.get(), (const unsigned int *) references.data(), references.size(), in.data());
    }

    void FmiLibFmu::setValuesInternal(const vector<bool_type> & in, const vector<size_type> & references) const
    {
        char charValues[references.size()];
        for (size_type i = 0; i < references.size(); i++)
            charValues[i] = in[i];
        fmi1_import_set_boolean(_fmu.get(), (const unsigned int *) references.data(), references.size(), charValues);
    }

    void FmiLibFmu::setValuesInternal(const vector<string_type> & in, const vector<size_type> & references) const
    {
        const char* stringValues[references.size()];
        for (size_type i = 0; i < references.size(); i++)
            stringValues[i] = in[i].c_str();
        fmi1_import_set_string(_fmu.get(), (const unsigned int *) references.data(), references.size(), stringValues);
    }

    void FmiLibFmu::stepCompleted()
    {
        fmi1_boolean_t callEventUpdate = fmi1_false;
        fmi1_import_completed_integrator_step(_fmu.get(), &callEventUpdate);
    }

    void FmiLibFmu::setStatesInternal(const real_type * states)
    {
        fmi1_import_set_continuous_states(_fmu.get(), states, getNumStates());
    }

    void FmiLibFmu::getStateDerivativesInternal(real_type * stateDerivatives)
    {
        fmi1_import_get_derivatives(_fmu.get(), stateDerivatives, getNumStates());
    }

    void FmiLibFmu::setTime(const double & time)
    {
        AbstractFmu::setTime(time);
        fmi1_import_set_time(_fmu.get(), time);
    }

    template<>
    real_type FmiLibFmu::getStartValue<real_type>(fmi1_import_variable_t * variable)
    {
        return fmi1_import_get_real_variable_start(fmi1_import_get_variable_as_real(variable));
    }
    template<>
    int_type FmiLibFmu::getStartValue<int_type>(fmi1_import_variable_t * variable)
    {
        return fmi1_import_get_integer_variable_start(fmi1_import_get_variable_as_integer(variable));
    }
    template<>
    bool_type FmiLibFmu::getStartValue<bool_type>(fmi1_import_variable_t * variable)
    {
        return fmi1_import_get_boolean_variable_start(fmi1_import_get_variable_as_boolean(variable));
    }
    template<>
    string_type FmiLibFmu::getStartValue<string_type>(fmi1_import_variable_t * variable)
    {
        return string_type(fmi1_import_get_string_variable_start(fmi1_import_get_variable_as_string(variable)));
    }

} /* namespace FMI */

#endif
