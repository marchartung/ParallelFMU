/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */
#ifdef USE_FMILIB

#ifndef INCLUDE_FMI_FMILIBFMU_HPP_
#define INCLUDE_FMI_FMILIBFMU_HPP_

extern "C"
{
#include <fmilib.h>
}

#include "Stdafx.hpp"
#include "fmi/AbstractFmu.hpp"

namespace FMI
{
    class FmiLibFmu : public AbstractFmu
    {
     public:
        using AbstractFmu::getValuesInternal;
        using AbstractFmu::setValuesInternal;

        FmiLibFmu(const Initialization::FmuPlan & in);
        virtual ~FmiLibFmu();

        void load(const bool & alsoInit = true) override;
        void unload() override;
        void initialize() override;

        AbstractFmu * duplicate() override;

        void stepCompleted() override;
        FmuEventInfo eventUpdate() override;

        void setTime(const double & time) override;
        double getDefaultStart() const override;
        double getDefaultStop() const override;

     protected:

        void getStatesInternal(real_type * states) const override;
        void setStatesInternal(const real_type * states) override;
        void getStateDerivativesInternal(real_type * stateDerivatives) override;
        void getEventIndicatorsInternal(real_type * eventIndicators) override;

        void getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const override;
        void getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const override;
        void getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const override;
        void getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const override;

        void setValuesInternal(const vector<real_type> & in, const vector<size_type> & references) override;
        void setValuesInternal(const vector<int_type> & in, const vector<size_type> & references) override;
        void setValuesInternal(const vector<bool_type> & in, const vector<size_type> & references) override;
        void setValuesInternal(const vector<string_type> & in, const vector<size_type> & references) override;

        fmi1_boolean_t toFmi1Boolean(bool_type value) const;
        size_type toSizeT(fmi1_value_reference_t value) const;

     private:

        static set<string_type> _loadedFmuObjects;
        set<string_type>::iterator _posPath;
        std::shared_ptr<fmi_import_context_t> _context;

        jm_callbacks _callbacks;
        std::shared_ptr<fmi1_import_t> _fmu;
        fmi1_event_info_t _fmuEventInfo;

        template<typename T>
        void addVariable(fmi1_import_variable_t * variable)
        {
            string_type varName = string_type(fmi1_import_get_variable_name(variable));
            size_type valueReference = fmi1_import_get_variable_vr(variable);
            _valueInfo.addNameReferencePair<T>(varName, valueReference);
            fmi1_causality_enu_t varCausality = fmi1_import_get_causality(variable);

            if (fmi1_import_get_variable_has_start(variable))
            {
                _startValues.getValues<T>().push_back(getStartValue<T>(variable));
                _startValueReferences.getValues<T>().push_back(valueReference);
            }
            _valueInfo.addNameReferencePair<T>(varName, valueReference);
            _allValueReferences.getValues<T>().push_back(valueReference);

            switch (varCausality)
            {
                case fmi1_causality_enu_t::fmi1_causality_enu_input:
                    _inputValueReferences.getValues<T>().push_back(valueReference);
                    //_startValues.getValues<T>().push_back(std::numeric_limits<T>::max());
                    //_startValueReferences.getValues<T>().push_back(valueReference);
                    break;
                case fmi1_causality_enu_t::fmi1_causality_enu_output:
                    _outputValueReferences.getValues<T>().push_back(valueReference);
                    break;
                default:
                    ;
            }
        }

        template<typename T>
        T getStartValue(fmi1_import_variable_t * variable)
        {
            throw std::runtime_error("FmiLibFmu: Unknown type for start value.");
        }
    };

    template<typename T>
    T getStartValue(fmi1_import_variable_t * variable)
    {
        throw std::runtime_error("FmiLibFmu: Unknown type for start value.");
    }

    template<>
    real_type FmiLibFmu::getStartValue<real_type>(fmi1_import_variable_t * variable);

    template<>
    int_type FmiLibFmu::getStartValue<int_type>(fmi1_import_variable_t * variable);

    template<>
    bool_type FmiLibFmu::getStartValue<bool_type>(fmi1_import_variable_t * variable);

    template<>
    string_type FmiLibFmu::getStartValue<string_type>(fmi1_import_variable_t * variable);

} /* namespace FMI */

#endif /* INCLUDE_FMI_FMILIBFMU_HPP_ */
/**
 * @}
 */

#endif
