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

    /**
     * Interface to a configuration reader. Currently, the one and only implementation is \ref XMLConfigurationReader.
     *
     * Maybe, this is useful, if someone implements a JSONConfigurationReader to parse JSON files...
     */
    class IConfigurationReader
    {
     public:
        virtual ~IConfigurationReader()
        {
        }

        virtual ProgramPlan getProgramPlan() = 0;

        virtual vector<vector<SimulationPlan>> getSimulationPlans(const list<SolverPlan> & solverPlans, const SchedulePlan & schedPlan,
                                                                  const SimulationPlan simPlan) = 0;
    };

} /* namespace Initialization */

#endif /* INCLUDE_DATAACCESS_ICONFIGURATIONREADER_HPP_ */
/**
 * @}
 */
