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

    class Program
    {
     public:

        Program(const CommandLineArgs & cla);

        Program(int * argc, char **argv[]);

        virtual ~Program();

        void initialize();

        void deinitialize();

        void simulate();

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
        ProgramPlan _pp;

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
