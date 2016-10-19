/*
 * Program.hpp
 *
 *  Created on: 08.03.2016
 *      Author: hartung
 */

#ifndef INCLUDE_INITIALIZATION_PROGRAM_HPP_
#define INCLUDE_INITIALIZATION_PROGRAM_HPP_

#include "IConfigurationReader.hpp"
#include "CommandLineArgs.hpp"
#include "Plans.hpp"

namespace Initialization
{

    /**
     * Docu
     *
     * If ParallelFMU serves as network server, a NetworkPlan is added to the simulation plans of the ProgramPlan.
     *
     */
    class Program
    {
     public:

        Program(const CommandLineArgs & cla);

        Program(int * argc, char **argv[]);

        virtual ~Program();

        /** \brief Initializes the program.
         * The ProgramPlan is created from the configuration file.
         * MPI is initialized, if used.
         * Network connection is established, if ParallelFMU is used as server.
         * The simulations are created (but not initialized!).
         */
        void initialize();

        void deinitialize();

        /** \brief Runs the simulations.
         *
         * The simulations are initialized and than run/executed.
         */
        void simulate();

        /**
         * Returns a shared pointer to the simulations.
         * If OpenMP is used, a shared pointer to the simulation i-th simulation is returned, where i is the OpenMP thread number.
         */
        Simulation::AbstractSimulationSPtr getSimulation();

        void printProgramInfo(const ProgramPlan & in) const;

     private:

        bool _isInitialized;
        bool _usingMPI;
        bool _usingOMP;

        CommandLineArgs _commandLineArgs;
        ProgramPlan _progPlan;

        vector<Simulation::AbstractSimulationSPtr> _simulations;

        bool initMPI(int & rank, int & numRanks);
        bool initNetworkConnection(const int & rank);

        void deinitMPI();
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_PROGRAM_HPP_ */
