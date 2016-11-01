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
     * Docu...
     *
     * If ParallelFMU serves as network server, a NetworkPlan is added to the simulation plans of the ProgramPlan.
     *
     */
    class Program
    {
     public:

        Program(CommandLineArgs  cla);

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

        /*! \brief Returns true, if the program is used as server. */
        bool isSimulationServer() const;

        /*! \brief Returns the port to use for the simulation. */
        int getSimulationServerPort() const;

     private:

        bool _isInitialized;
        bool _usingMPI;
        bool _usingOMP;

        CommandLineArgs _commandLineArgs;
        ProgramPlan _progPlan;

        std::vector<Simulation::AbstractSimulationSPtr> _simulations;

        /*! \brief Initializes the MPI communication.
         *
         * \param rank      Will be the MPI rank.
         * \param numRanks  Will hold the total number of MPI processes.
         * \return          True, if the MPI communication could successfully established.
         */
        bool initMPI(int & rank, int & numRanks);

        bool initNetworkConnection(const int & rank);

        void deinitMPI();
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_PROGRAM_HPP_ */
