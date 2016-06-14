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

        Simulation::AbstractSimulationSPtr getSimulation();

        void printProgramInfo(const ProgramPlan & in) const;

     private:

        bool _isInitialized;
        bool _usingMPI;

        CommandLineArgs _commandLineArgs;
        ProgramPlan _pp;

        Simulation::AbstractSimulationSPtr _simulation;

        bool initMPI(int & rank, int & numRanks);
        bool initNetworkConnection(const int & rank);

        void deinitMPI();
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_PROGRAM_HPP_ */
