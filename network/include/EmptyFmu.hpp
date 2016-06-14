/*
 * NetworkFmu.hpp
 *
 *  Created on: 03.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_EMPTYFMU_HPP_
#define NETWORK_INCLUDE_EMPTYFMU_HPP_

#include "fmi/AbstractFmu.hpp"

namespace FMI
{

    class EmptyFmu : public AbstractFmu
    {
     public:
        EmptyFmu(const Initialization::FmuPlan & in);

        virtual ~EmptyFmu();

        virtual AbstractFmu * duplicate() override;
        virtual void stepCompleted() override;
        virtual FMI::FmuEventInfo eventUpdate() override;
        virtual double getDefaultStart() const override;
        virtual double getDefaultStop() const override;
        virtual void initialize() override;

        const FMI::ValueCollection & getEmptyFmuValues() const;
        void setNumValues(const size_type & numReals, const size_type & numInts, const size_type & numBools, const size_type & numStrings);

     protected:
        virtual void getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const override;
        virtual void getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const override;
        virtual void getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const override;
        virtual void getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const override;

        virtual void setValuesInternal(const vector<real_type> & out, const vector<size_type> & references) override;
        virtual void setValuesInternal(const vector<int_type> & out, const vector<size_type> & references) override;
        virtual void setValuesInternal(const vector<bool_type> & out, const vector<size_type> & references) override;
        virtual void setValuesInternal(const vector<string_type> & out, const vector<size_type> & references) override;

        virtual void getStatesInternal(real_type * states) const override;
        virtual void setStatesInternal(const real_type * states) override;

        virtual void getStateDerivativesInternal(real_type * stateDerivatives) override;
        virtual void getEventIndicatorsInternal(real_type * eventIndicators) override;

     private:

        FMI::ValueCollection _values;

    };

} /* namespace FMI */

#endif /* NETWORK_INCLUDE_EMPTYFMU_HPP_ */
