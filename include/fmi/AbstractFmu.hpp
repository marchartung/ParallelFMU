/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_ABSTRACTFMU_HPP_
#define INCLUDE_FMI_ABSTRACTFMU_HPP_

#include "BasicTypedefs.hpp"
#include "fmi/FmuEventInfo.hpp"
#include "fmi/ValueCollection.hpp"
#include "fmi/ValueInfo.hpp"
#include "fmi/ValueReferenceCollection.hpp"
#include "Stdafx.hpp"
#include "util/EventHandler.hpp"
#include "initialization/Plans.hpp"

namespace FMI
{
    enum ReferenceContainerType
    {
        ALL, START, EVENT, CONTINIOUS
    };


    /**
     * This class represents a loaded FMU that can access all functions that are described in the FMI 1.0
     * model-exchange standard. Additionally it provides some events and functions to interact with solvers
     * and external modules.
     */
    class AbstractFmu
    {
     public:

        /**
         * Creates a new instance of an object representing a FMU.
         * @param name A unique name for the FMU.
         * @param fmuPath The location where the FMU archive is stored.
         * @param loader The pointer to an loader-object that is able to load and interact with a FMU.
         * @param relativeTolerance If toleranceControlled is set, all numerical algorithms inside the FMU return values
         * that have an relative error smaller than this value.
         * @param toleranceControlled Set to true if the numerical algorithms inside the FMU should work with the given relative tolerance.
         * @param loggingEnabled Set to true if logging output of the FMU should be displayed.
         * @param intermediateResults Set to true if the event update function should return after every internal event iteration.
         */
        AbstractFmu(const Initialization::FmuPlan & in)
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

        /**
         * Destroy the FMU object and unload the resources if necessary.
         */
        virtual ~AbstractFmu()
        {
            if (isLoaded())
                unload();
        }

        virtual AbstractFmu * duplicate() = 0;
        virtual void stepCompleted() = 0;
        virtual FmuEventInfo eventUpdate() = 0;
        virtual double getDefaultStart() const = 0;
        virtual double getDefaultStop() const = 0;
        virtual void initialize() = 0;

     protected:
        virtual void getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const = 0;
        virtual void getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const = 0;
        virtual void getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const = 0;
        virtual void getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const = 0;

        virtual void setValuesInternal(const vector<real_type> & out, const vector<size_type> & references) = 0;
        virtual void setValuesInternal(const vector<int_type> & out, const vector<size_type> & references) = 0;
        virtual void setValuesInternal(const vector<bool_type> & out, const vector<size_type> & references) = 0;
        virtual void setValuesInternal(const vector<string_type> & out, const vector<size_type> & references) = 0;

        virtual void getStatesInternal(real_type * states) const = 0;
        virtual void setStatesInternal(const real_type * states) = 0;

        virtual void getStateDerivativesInternal(real_type * stateDerivatives) = 0;
        virtual void getEventIndicatorsInternal(real_type * eventIndicators) = 0;

     public:

        /**
         * Load the shared library of the FMU and instantiate and initialize an instance of the FMU.
         */
        virtual void load(const bool & alsoInit = true)
        {
            _loaded = true;
        }

        /**
         * Unload the shared library of the FMU (if not needed by other instances).
         */
        virtual void unload()
        {
            _loaded = false;
        }

        /**
         * Get the storage location of the FMU.
         * @return The path to the FMU-file.
         */
        const string & getPath() const
        {
            return _path;
        }

        const string & getWorkingDirectory() const
        {
            return _workingPath;
        }

        /**
         * Check if the FMU is already loaded.
         * @return True if the FMU is loaded.
         */
        bool isLoaded() const
        {
            return _loaded;
        }

        /**
         * Get the unique name of the FMU.
         * @return The stored name.
         */
        const string& getFmuName() const
        {
            return _name;
        }

        size_type getId() const
        {
            return _id;
        }

        size_type getLocalId() const
        {
            return _localId;
        }

        void setLocalId(size_type localId)
        {
            _localId = localId;
        }

        /**
         * Get the number of event indicators that are described in the model description.
         * @return The number of event indicators.
         */
        size_type getNumEventIndicators() const
        {
            return _numberOfEventIndicators;
        }

        /**
         * Set the number of event indicators that are describe the model.
         * @param numberOfEventIndicators
         */
        void setNumEventIndicators(size_type numberOfEventIndicators)
        {
            this->_numberOfEventIndicators = numberOfEventIndicators;
        }

        /**
         * Get the number of state, respectively state derivative, variables of the FMU-model.
         * @return The number of state variables.
         */
        size_type getNumStates() const
        {
            return _numberOfStates;
        }

        /**
         * Set the number of state, respectively state derivative, variables of the FMU-model.
         * @param numberOfStates The new number of state variables.
         */
        void setNumStates(size_type numberOfStates)
        {
            this->_numberOfStates = numberOfStates;
        }

        /**
         * Get the number of variables that are declared as 'input' in the model description.
         * @return The number of input variables.
         */
        size_type getNumInputs() const
        {
            return _inputValueReferences.size();
        }

        size_type getNumOutputs() const
        {
            return _outputValueReferences.size();
        }

        real_type getTime() const
        {
            return _time;
        }

        virtual void setTime(const real_type & in)
        {
            _time = in;
        }

        /**
         * Get all value references that are part of the model description.
         * @return All value references.
         */
        const ValueReferenceCollection& getAllValueReferences() const
        {
            return _allValueReferences;
        }

        /**
         * Set all value references that should be used by the FMU.
         * @param valueReferences The new value references.
         * @todo Make the setAllValueReferences protected and use the library loader as friend.
         */
        void setAllValueReferences(const ValueReferenceCollection& valueReferences)
        {
            _allValueReferences = valueReferences;
        }

        /**
         * Get all value references that are related to output variables of the model description.
         * @return All references to output variables.
         */
        const ValueReferenceCollection& getOutputValueReferences() const
        {
            return _outputValueReferences;
        }

        void setStates(const real_type * states)
        {
            return setStatesInternal(states);
        }

        void setStates(const vector<real_type> & states)
        {
            return setStates(states.data());
        }

        void getStates(real_type * states) const
        {
            getStatesInternal(states);
        }

        void getStates(vector<real_type> & in) const
        {
            getStates(in.data());
        }

        vector<real_type> getStates() const
        {
            vector<real_type> res(getNumStates());
            getStates(res);
            return res;
        }

        ValueCollection getValues(ReferenceContainerType refType = ReferenceContainerType::EVENT) const
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
            FMI::ValueCollection res(refs->getValues<real_type>().size(), refs->getValues<int_type>().size(),
                                     refs->getValues<bool_type>().size(), refs->getValues<string_type>().size());
            getAllValues(res);
            return res;
        }

        void getStateDerivatives(vector<real_type> & stateDerivatives)
        {
            getStateDerivatives(stateDerivatives.data());
        }

        void getStateDerivatives(real_type * stateDerivatives)
        {
            getStateDerivativesInternal(stateDerivatives);
        }

        std::vector<real_type> getStateDerivatives()
        {
            std::vector<real_type> stateDerivatives(getNumStates());
            getStateDerivatives(stateDerivatives.data());
            return stateDerivatives;
        }

        virtual void getEventIndicators(vector<real_type> & eventIndicators)
        {
            getEventIndicators(eventIndicators.data());
        }

        virtual void getEventIndicators(real_type * eventIndicators)
        {
            getEventIndicatorsInternal(eventIndicators);
        }

        std::vector<real_type> getEventIndicators()
        {
            std::vector<real_type> eventIndicators(getNumEventIndicators());
            getStateDerivatives(eventIndicators.data());
            return eventIndicators;
        }

        void setValues(const FMI::ValueCollection& values)
        {
            setValuesInternal(values.getValues<real_type>(), _eventValueReferences.getValues<real_type>());
            setValuesInternal(values.getValues<int_type>(), _eventValueReferences.getValues<int_type>());
            setValuesInternal(values.getValues<bool_type>(), _eventValueReferences.getValues<bool_type>());
            setValuesInternal(values.getValues<string_type>(), _eventValueReferences.getValues<string_type>());
        }

        /**
         * Set all value references that are related to output variables.
         * @param outputValues The new output references.
         * @attention This function should only be called from the FMU loader during the loading/initialization process.
         * @todo Make the setOutputValueReferences protected and use the library loader as friend.
         */
        void setOutputValueReferences(const ValueReferenceCollection& outputValues)
        {
            this->_outputValueReferences = outputValues;
        }

        /**
         * Get the relative tolerance that is used for internal numeric calculations.
         * @return The relative tolerance.
         */
        double getRelativeTolerance() const
        {
            return _relativeTolerance;
        }

        /**
         * Check if the numerical algorithms of the FMU work with the given relative tolerance.
         * @return True if the relative tolerance is used.
         */
        bool isToleranceControlled() const
        {
            return _toleranceControlled;
        }

        /**
         * Get the references to all values that are marked as input values.
         * @return The references to all input variables.
         */
        const ValueReferenceCollection& getInputValueReferences() const
        {
            return _inputValueReferences;
        }

        /**
         * Set the references to all input values of the FMU.
         * @param inputValues The new input values
         * @todo Make the setInputValueReferences protected and use the library loader as friend.
         */
        void setInputValueReferences(const ValueReferenceCollection& inputValues)
        {
            this->_inputValueReferences = inputValues;
        }

        /**
         * Check if debug logging is enabled.
         * @return True if debug logging is enabled.
         */
        bool isLoggingEnabled() const
        {
            return _loggingEnabled;
        }

        /**
         * Disable or enable the debug logging.
         * @param loggingEnabled True if the debug logging should be enabled.
         */
        void setLoggingEnabled(bool loggingEnabled)
        {
            _loggingEnabled = true;
        }

        /**
         * Get the value info of the FMU, describing all values.
         * @return The stored value info.
         */
        const ValueInfo& getValueInfo() const
        {
            return _valueInfo;
        }

        /**
         * Set the value info for the FMU, describing all values.
         * @param valueInfo The new value info.
         */
        void setValueInfo(const ValueInfo& valueInfo)
        {
            _valueInfo = valueInfo;
        }

        /**
         * Check if the event update function should return after every internal event iteration.
         * @return True if intermediate results are returned.
         */
        bool hasIntermediateResults() const
        {
            return _intermediateResults;
        }

        /// Set the relative tolerance to the given value.
        void setRelativeTolerance(real_type relativeTolerance)
        {
            _relativeTolerance = relativeTolerance;
        }

        void getValues(ValueCollection& values) const
        {
            getValuesInternal(values.getValues<real_type>(), _eventValueReferences.getValues<real_type>());
            getValuesInternal(values.getValues<int_type>(), _eventValueReferences.getValues<int_type>());
            getValuesInternal(values.getValues<bool_type>(), _eventValueReferences.getValues<bool_type>());
            getValuesInternal(values.getValues<string_type>(), _eventValueReferences.getValues<string_type>());
        }

        void getAllValues(ValueCollection& values) const
        {
            getValuesInternal(values.getValues<real_type>(), _allValueReferences.getValues<real_type>());
            getValuesInternal(values.getValues<int_type>(), _allValueReferences.getValues<int_type>());
            getValuesInternal(values.getValues<bool_type>(), _allValueReferences.getValues<bool_type>());
            //getValuesInternal(values.getValues<string_type>(), _allValueReferences.getValues<string_type>());
        }

        const FmuEventInfo& getEventInfo() const
        {
            return _eventInfo;
        }

        vector<Synchronization::ConnectionSPtr> & getConnections()
        {
            return _fmuCons;
        }

        const vector<Synchronization::ConnectionSPtr> & getConnections() const
        {
            return _fmuCons;
        }

        void setConnections(const vector<Synchronization::ConnectionSPtr> & in)
        {
            _fmuCons = in;
        }

        size_type getSharedId() const
        {
            return _sharedId;
        }

        void setSharedId(size_type sharedId)
        {
            _sharedId = sharedId;
        }

     protected:
        /**
         * Set arbitrary values of the FMU.
         * @param values The values that should be set.
         * @param references The references to the values.
         */
        void setValues(ValueCollection& values, ValueReferenceCollection& references)
        {
            throw std::runtime_error("AbstractFmu: setValues(ValueCollection&,ValueReferenceCollection&) is deprecated.");
            assert(values.getValues<double>().size() == references.getValues<double>().size());
            assert(values.getValues<int_type>().size() == references.getValues<int_type>().size());
            assert(values.getValues<bool_type>().size() == references.getValues<bool_type>().size());
            assert(values.getValues<string_type>().size() == references.getValues<string_type>().size());
        }

        /**
         * Set the values stored as type T.
         * @param values The values that should be stored.
         * @param references The references that are associated with the given values.
         */
        template<typename T>
        void setValues(ValueCollection& values, ValueReferenceCollection& references)
        {
            throw std::runtime_error("AbstractFmu: setValues(ValueCollection&,ValueReferenceCollection&) is deprecated.");
            vector<T>& vals = values.getValues<T>();
            vector<T>& valRefs = references.getValues<T>();
            assert(vals.size() == valRefs.size());
            //
        }

        /// Unique identifier.
        const size_type _id;
        size_type _localId;
        size_type _sharedId;
        /// Name of the FMU.
        string _name;
        /// (Absolute) Path to the FMU.
        string _path;
        string _workingPath;
        bool _loaded;
        real_type _relativeTolerance;
        bool _toleranceControlled;
        bool _loggingEnabled;
        bool _intermediateResults;
        FmuEventInfo _eventInfo;

        ValueReferenceCollection _allValueReferences;  // containing every variable
        ValueReferenceCollection _startValueReferences;  // all - constant vars
        ValueReferenceCollection _eventValueReferences;  // start - parameter vars
        ValueReferenceCollection _continousValueReferences;  // event - discrete vars

        ValueReferenceCollection _outputValueReferences;
        ValueReferenceCollection _inputValueReferences;

        ValueCollection _startValues;
        real_type _time;

        ValueInfo _valueInfo;
        size_type _numberOfStates;
        size_type _numberOfEventIndicators;

        vector<Synchronization::ConnectionSPtr> _fmuCons;
    };
} /* namespace FMI */

#endif /* INCLUDE_FMI_ABSTRACTFMU_HPP_ */
/**
 * @}
 */
