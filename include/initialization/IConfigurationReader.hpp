/** @addtogroup Initialization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_INITIALIZATION_ICONFIGURATIONREADER_HPP_
#define INCLUDE_INITIALIZATION_ICONFIGURATIONREADER_HPP_

#include "Stdafx.hpp"
#include "initialization/Plans.hpp"

namespace Initialization
{
    class IConfigurationReader
    {
     public:
        virtual ~IConfigurationReader()
        {
        }

        virtual ProgramPlan getProgramPlan() = 0;

        virtual vector<SimulationPlan> getSimulationPlans(const std::list<SolverPlan> & solverPlans, const SchedulePlan & schedPlan,
                                                          const SimulationPlan simPlan) = 0;
    };
} /* namespace Initialization */

#endif /* INCLUDE_DATAACCESS_ICONFIGURATIONREADER_HPP_ */
/**
 * @}
 */
