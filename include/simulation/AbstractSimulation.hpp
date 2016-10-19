/** @addtogroup Simulation
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SIMULATION_ABSTRACTSIMULATION_HPP_
#define INCLUDE_SIMULATION_ABSTRACTSIMULATION_HPP_

#include "Stdafx.hpp"
#include "initialization/Plans.hpp"
#include <memory>

namespace Simulation
{

    class AbstractSimulation
    {
     public:
        //AbstractSimulation(vector<Solver::AbstractSolverSPtr> solver);

        AbstractSimulation(const Initialization::SimulationPlan & in, const vector<shared_ptr<Solver::ISolver>> & solver);

        /**
         * Destructor. Destroys AbstractSimulation object and frees allocates resources.
         */
        virtual ~AbstractSimulation()
        {
        }

        /**
         * As long as the simulation end time is not reached, this method calls
         * the solve methods (time integration) for all solvers.
         */
        virtual void simulate() = 0;

        /**
         * Initialize all solvers, data-managers and FMUs.
         */
        virtual void initialize();

        /**
         * Get all solvers that are part of the simulation.
         * @return The list of solvers that are connected to the simulation.
         */
        virtual const vector<shared_ptr<Solver::ISolver>> & getSolver() const;

        virtual vector<shared_ptr<Solver::ISolver>> & getSolver();

        real_type getSimulationEndTime() const;

        size_type getMaxIterations() const;

        void setSimulationEndTime(const real_type simEnd);

        void setMaxIterations(const size_type num);

        /**
         * Returns if a simulation is a MPI, OpenMP or Serial simulation
         * @return string_type One of: {"serial", "openmp", "mpi"}
         */
        virtual string_type getSimulationType() const = 0;

        void addSolvers(const vector<Solver::SolverSPtr> & _solver);

        void deinitialize();

     protected:

        /**
         * All solvers handled by the simulation.
         */
        vector<shared_ptr<Solver::ISolver>> _solver;

     private:

        /**
         * End time of the simulation.
         */
        real_type _simulationEndTime;

        /**
         * Maximal number of iterations that should be executed.
         */
        size_type _maxIterations;
    };

} /* namespace Simulation */

#endif /* INCLUDE_SIMULATION_ABSTRACTSIMULATION_HPP_ */
