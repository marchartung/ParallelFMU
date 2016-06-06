/*
 * NetworkFmu.hpp
 *
 *  Created on: 03.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_EMPTYFMU_HPP_
#define NETWORK_INCLUDE_EMPTYFMU_HPP_

#include <fmi/AbstractFmu.hpp>

namespace FMI
{

    class EmptyFmu : public AbstractFmu
    {
     public:
        EmptyFmu();

        virtual ~EmptyFmu();

        virtual AbstractFmu * duplicate();
        virtual void stepCompleted();
        virtual FMI::FmuEventInfo eventUpdate();
        virtual double getDefaultStart() const;
        virtual double getDefaultStop() const;
        virtual void initialize();

     protected:
        virtual void getValuesInternal(vector<real_type> & out, const vector<size_type> & references) const;
        virtual void getValuesInternal(vector<int_type> & out, const vector<size_type> & references) const;
        virtual void getValuesInternal(vector<bool_type> & out, const vector<size_type> & references) const;
        virtual void getValuesInternal(vector<string_type> & out, const vector<size_type> & references) const;

        virtual void setValuesInternal(const vector<real_type> & out, const vector<size_type> & references) const;
        virtual void setValuesInternal(const vector<int_type> & out, const vector<size_type> & references) const;
        virtual void setValuesInternal(const vector<bool_type> & out, const vector<size_type> & references) const;
        virtual void setValuesInternal(const vector<string_type> & out, const vector<size_type> & references) const;

        virtual void getStatesInternal(real_type * states) const;
        virtual void setStatesInternal(const real_type * states);

        virtual void getStateDerivativesInternal(real_type * stateDerivatives);
        virtual void getEventIndicatorsInternal(real_type * eventIndicators);
    };

} /* namespace FMI */

#endif /* NETWORK_INCLUDE_EMPTYFMU_HPP_ */
