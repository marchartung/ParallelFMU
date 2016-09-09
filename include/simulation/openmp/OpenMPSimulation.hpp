/** @addtogroup Simulation
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Nov 2015
 */

#ifndef INCLUDE_SIMULATION_OPENMPSIMULATION_HPP_
#define INCLUDE_SIMULATION_OPENMPSIMULATION_HPP_

#include <fmi/AbstractFmu.hpp>
#include "Stdafx.hpp"
#include "solver/AbstractSolver.hpp"
#include "simulation/SerialSimulation.hpp"

namespace Simulation
{
    /**
     * This class represents an OpenMP parallel simulation for shared memory systems.
     * Several solvers and their associated FMUs can be handled.
     */
    class OpenMPSimulation : public SerialSimulation
    {
     public:
        /**
         * Create OpenMP simulation from given solvers.
         * @param solvers
         */
        //OpenMPSimulation(vector<Solver::AbstractSolverSPtr> solvers, const string_type & scheduleKind = "auto");
        OpenMPSimulation(const Initialization::SimulationPlan & in, const vector<std::shared_ptr<Solver::ISolver>> & solver);

        /**
         * Destructor. Destroys OpenMP simulation object and frees allocates resources.
         */
        virtual ~OpenMPSimulation();

        /**
         * As long as the simulation end time is not reached, this method calls
         * the solve methods (time integration) for all solvers in parallel.
         * \remark: OpenMP parallel for loop is used.
         */
        virtual void simulate();

        /**
         * Returns if a simulation is a MPI, OpenMP or Serial simulation
         * @return string_type One of: {"serial", "openmp", "mpi"}
         */
        virtual string_type getSimulationType() const;

     private:
        /**
         * It is possible to specify the OpenMP loop schedule algorithm.
         * Possible values are: static, dynamic, guided, auto, runtime.
         * Default: auto
         *
         * Todo: Add the possibility to control schedule value of OpenMP loops through config file and/or command line.
         */
        string_type _scheduleKind;
    };

} /* namespace Simulation */

#endif /* INCLUDE_SIMULATION_OPENMPSMULATION_HPP_ */
/**
 * @}
 */
