/** @addtogroup Simulation
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SIMULATION_SERIALSIMULATION_HPP_
#define INCLUDE_SIMULATION_SERIALSIMULATION_HPP_

#include <fmi/AbstractFmu.hpp>
#include "simulation/AbstractSimulation.hpp"
#include "Stdafx.hpp"
#include "solver/AbstractSolver.hpp"

namespace Simulation
{

    /**
     * This class represents a serial (i.e., non-parallel) simulation.
     * Several solvers and their associated FMUs can be handled.
     */
    class SerialSimulation : public AbstractSimulation
    {
     public:
        /**
         * Create serial simulation from given solvers.
         * @param solvers
         */
        //SerialSimulation(vector<Solver::AbstractSolverSPtr> solvers);
        SerialSimulation(const Initialization::SimulationPlan & in, const vector<std::shared_ptr<Solver::ISolver>> & solver);
        /**
         * Destructor. Destroys serial simulation object and frees allocates resources.
         */
        virtual ~SerialSimulation();

        /**
         * As long as the simulation end time is not reached, this method calls
         * the solve methods (time integration) for all solvers.
         */
        virtual void simulate();

        /**
         * Returns if a simulation is a MPI, OpenMP or Serial simulation
         * @return string_type One of: {"serial", "openmp", "mpi"}
         */
        virtual string_type getSimulationType() const;

    };

} /* namespace Simulation */

#endif /* INCLUDE_SIMULATION_SERIALSIMULATION_HPP_ */
/**
 * @}
 */
