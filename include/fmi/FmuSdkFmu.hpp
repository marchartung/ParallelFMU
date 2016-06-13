/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_FMUSDKFMU_HPP_
#define INCLUDE_FMI_FMUSDKFMU_HPP_

#include "fmi/AbstractFmu.hpp"
#include "fmusdk.h"

namespace FMI
{
    class FmuSdkFmu : public AbstractFmu
    {
     public:

        using AbstractFmu::getValuesInternal;
        using AbstractFmu::setValuesInternal;

        FmuSdkFmu(const Initialization::FmuPlan & in);

        FmuSdkFmu() = delete;

        virtual ~FmuSdkFmu();

        void load(const bool & alsoInit = true) override;
        void unload() override;
        void initialize() override;

        AbstractFmu * duplicate() override;

        void stepCompleted() override;
        FmuEventInfo eventUpdate() override;

        void setTime(const double & time) override;
        double getDefaultStart() const override;
        double getDefaultStop() const override;

        friend void deleteFmuSdkComponent(fmiComponent * in);

     protected:

        void getStatesInternal(real_type * states) const override;
        void setStatesInternal(const real_type * states) override;
        void getStateDerivativesInternal(real_type * stateDerivatives) override;
        void getEventIndicatorsInternal(real_type * eventIndicators) override;

        void fillNameVector(vector<string_type> &valueNames, const map<int, string_type> &valueNamesIdx);

        FMU* getFmuHandle(string_type fmuName) const;

        void getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const override;
        void getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const override;
        void getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const override;
        void getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const override;

        void setValuesInternal(const vector<real_type> & in, const vector<size_type> & references) override;
        void setValuesInternal(const vector<int_type> & in, const vector<size_type> & references) override;
        void setValuesInternal(const vector<bool_type> & in, const vector<size_type> & references) override;
        void setValuesInternal(const vector<string_type> & in, const vector<size_type> & references) override;

     private:

        void (*_Nullifier)(FMU*) = [](FMU * in)
        {   in = nullptr;};

        static map<string_type,tuple<FMU *,size_type>> _knownFmus;
        static map<string_type,FmuSdkFmu*> _presentFmus;

        FMU * _fmu;
        fmiComponent _component;
        fmiCallbackFunctions _callbacks;

        fmiEventInfo _componentEventInfo;

        /**
         * Returns StartValue
         */
        template<typename T>
        void addVariable(ScalarVariable * variable)
        {
            ValueStatus vs;
            T res = getStartValue<T>(variable, vs);
            size_type valueReference = getValueReference(variable);
            VarCausality varCausality = getVariableCausality(variable);
            string_type varName = string_type(getName((void*) variable));
            if (vs == valueDefined || vs == ValueStatus::valueMissing)
            {
                _valueInfo.addNameReferencePair<T>(varName, valueReference);
                _allValueReferences.getValues<T>().push_back(valueReference);

                if (getAlias(variable) == Enu::enu_noAlias)
                {
                    Enu variability = getVariability(variable);
                    switch (variability)
                    {
                        case Enu::enu_BAD_DEFINED:
                            throw std::runtime_error("Variable contains illegal or missing vars");
                        case Enu::enu_continuous:
                            _continousValueReferences.getValues<T>().push_back(valueReference);
                        case Enu::enu_discrete:
                            _eventValueReferences.getValues<T>().push_back(valueReference);
                        case Enu::enu_parameter:
                            if (vs == ValueStatus::valueDefined)
                            {
                                _startValueReferences.getValues<T>().push_back(valueReference);
                                _startValues.getValues<T>().push_back(res);
                            }
                            //else
                              //  _startValues.getValues<T>().push_back(0);
                        default:
                            ;
                    }
                }

            }
            else
                throw std::runtime_error("Variable contains illegal or missing vars");
            switch (varCausality)
            {
                case VarCausality::varCausalityInput:
                    _inputValueReferences.getValues<T>().push_back(valueReference);
                    //_startValueReferences.getValues<T>().push_back(valueReference);
                    //_startValues.getValues<T>().push_back(std::numeric_limits<T>::max());
                    break;
                case VarCausality::varCausalityOutput:
                    _outputValueReferences.getValues<T>().push_back(valueReference);
                    break;
                default:
                    ;
            }

        }

        template<typename T>
        T getStartValue(ScalarVariable * variable, ValueStatus & vs)
        {
            throw std::runtime_error("FmuSdk: Unkown type for start value.");
        }
    }
    ;

    template<>
    real_type FmuSdkFmu::getStartValue<real_type>(ScalarVariable * variable, ValueStatus & vs);
    template<>
    int_type FmuSdkFmu::getStartValue<int_type>(ScalarVariable * variable, ValueStatus & vs);
    template<>
    bool_type FmuSdkFmu::getStartValue<bool_type>(ScalarVariable * variable, ValueStatus & vs);
    template<>
    string_type FmuSdkFmu::getStartValue<string_type>(ScalarVariable * variable, ValueStatus & vs);

} /* namespace FMI */

#endif /* INCLUDE_FMI_FMUSDKFMU_HPP_ */
/**
 * @}
 */
