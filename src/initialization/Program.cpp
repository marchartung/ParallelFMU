/*
 * Program.cpp
 *
 *  Created on: 08.03.2016
 *      Author: hartung
 */

#include "initialization/MainFactory.hpp"
#include "initialization/Program.hpp"
#include "initialization/XMLConfigurationReader.hpp"
#include "simulation/AbstractSimulation.hpp"

#include <mpi.h>

Initialization::Program::Program(const CommandLineArgs & cla)
        : _isInitialized(false),
          _usingMPI(false),
          _commandLineArgs(cla)
{
    initialize();
}

Initialization::Program::Program(int* argc, char** argv[])
        : _isInitialized(false),
          _usingMPI(false),
          _commandLineArgs(CommandLineArgs(argc, argv))
{
    initialize();
}

Initialization::Program::~Program()
{
    if (_isInitialized)
        deinitialize();
}

void Initialization::Program::initialize()
{
    if (_isInitialized)
        return;
    Util::Logger::initialize(_commandLineArgs.getLogSettings());
    Initialization::XMLConfigurationReader reader(_commandLineArgs.getConfigFilePath());
    //Util::Logger::setEnabled(false);
    ProgramPlan pp = reader.getProgramPlan();
    printProgramInfo(pp);
    int rank = 0, numRanks = 1;
    if (pp.simPlans.size() > 1)
    {

#ifdef USE_MPI

        if (!(_commandLineArgs.getProgramArgs() == make_tuple<const int *, const char ***>(nullptr, nullptr))
                && MPI_SUCCESS == MPI_Init(std::get<0>(_commandLineArgs.getProgramArgs()), std::get<1>(_commandLineArgs.getProgramArgs())))
        {
            MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
            if (numRanks < (long long int)pp.simPlans.size())
            {
                deinitialize();
                throw std::runtime_error("Program: Not enough mpi processes for given schedule.");
            }
            else if (numRanks > (long long int)pp.simPlans.size())
                LOGGER_WRITE("Program: More mpi process given than the simulation will use.", Util::LC_LOADER, Util::LL_WARNING);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            _usingMPI = true;
        }
#else
        //throw std::runtime_error("Program: MPI not supported by this system.");
#endif
    }

    MainFactory mf;
    _simulation = mf.createSimulation(pp.simPlans[rank]);
    _isInitialized = true;
}

void Initialization::Program::deinitialize()
{
    if (_isInitialized)
    {
        //_simulation->deinitialize();
#ifdef USE_MPI
        if (_usingMPI)
            MPI_Finalize();
#endif
    }
}

Simulation::AbstractSimulationSPtr Initialization::Program::getSimulation()
{
    return _simulation;
}

void Initialization::Program::printProgramInfo(const ProgramPlan& in) const
{
    LOGGER_WRITE("",Util::LC_LOADER,Util::LL_INFO);

    LOGGER_WRITE("Program info: ",Util::LC_LOADER,Util::LL_INFO);
    LOGGER_WRITE("numNodes:     " + to_string(in.simPlans.size()),Util::LC_LOADER,Util::LL_INFO);
    LOGGER_WRITE("Node infos:   ",Util::LC_LOADER,Util::LL_INFO);
    for(size_type i=0;i<in.simPlans.size();++i)
    {
        LOGGER_WRITE("Node      " + to_string(i) + ":",Util::LC_LOADER,Util::LL_INFO);
        size_type numFmus = 0;
        for(size_type j=0;j<in.simPlans[i].dataManager.solvers.size();++j)
            numFmus += in.simPlans[i].dataManager.solvers[j].size();
        LOGGER_WRITE("NumFmus:      " + to_string(numFmus),Util::LC_LOADER,Util::LL_INFO);
        LOGGER_WRITE("NumCores:     " +to_string(in.simPlans[i].dataManager.solvers.size()),Util::LC_LOADER,Util::LL_INFO);
        LOGGER_WRITE("\n",Util::LC_LOADER,Util::LL_INFO);
    }

}
