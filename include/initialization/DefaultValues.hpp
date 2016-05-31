/*
 * DefaultValues.hpp
 *
 *  Created on: 23.03.2016
 *      Author: hartung
 */

#ifndef INCLUDE_INITIALIZATION_DEFAULTVALUES_HPP_
#define INCLUDE_INITIALIZATION_DEFAULTVALUES_HPP_

#include <limits>
#include "initialization/Plans.hpp"

namespace Initialization
{

    class DefaultValues
    {
     public:

        DefaultValues() = delete;

        static Initialization::FmuPlan fmuPlan();

        static Initialization::SolverPlan solverPlan();

        static Initialization::HistoryPlan historyPlan();

        static Initialization::ConnectionPlan connectionPlan();

        static Initialization::DataManagerPlan dataManagerPlan();

        static Initialization::ProgramPlan programPlan();

        static Initialization::SchedulePlan schedulePlan();

        static Initialization::SimulationPlan simulationPlan();

        static Initialization::WriterPlan writerPlan();

        /*static Initialization Plan()
         {
         Initialization res;

         return res;
         }*/

        template<typename T>
        static T getUndefinedValue()
        {
            throw std::runtime_error("DefaultValues: Unknown type for undefined value creation.");
        }


        template<typename T>
        static bool isUndefinedValue(const T & in)
        {
            return in == getUndefinedValue<T>();
        }
    };
    template<>
    real_type DefaultValues::getUndefinedValue<real_type>();
    template<>
    size_type DefaultValues::getUndefinedValue<size_type>();
    template<>
    int DefaultValues::getUndefinedValue<int>();
    template<>
    string_type DefaultValues::getUndefinedValue<string_type>();
    template<>
    bool_type DefaultValues::getUndefinedValue();

    template<>
    FMI::InputMapping DefaultValues::getUndefinedValue();

    template<>
    bool DefaultValues::isUndefinedValue(const FMI::InputMapping & in);

}

#endif /* INCLUDE_INITIALIZATION_DEFAULTVALUES_HPP_ */
