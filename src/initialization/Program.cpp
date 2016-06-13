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

#ifdef USE_NETWORK_OFFLOADER
#include "../../network/include/InitialNetworkServer.hpp"
#endif

namespace Initialization
{
    Program::Program(const CommandLineArgs & cla)
            : _isInitialized(false),
              _usingMPI(false),
              _commandLineArgs(cla)
    {
        initialize();
    }

    Program::Program(int* argc, char** argv[])
            : _isInitialized(false),
              _usingMPI(false),
              _commandLineArgs(CommandLineArgs(argc, argv))
    {
        initialize();
    }

    Program::~Program()
    {
        if (_isInitialized)
            deinitialize();
    }

    void Program::initialize()
    {
        if (_isInitialized)
            return;
        Util::Logger::initialize(_commandLineArgs.getLogSettings());

        // test for MPI initialization:
        int rank = 0, numRanks = 1;
        if (_pp.simPlans.size() > 1)
            if (!initMPI(rank, numRanks))
                throw std::runtime_error("Couldn't initialize simulation. MPI couldn't be initialized.");

        // read config file:
        XMLConfigurationReader reader(_commandLineArgs.getConfigFilePath());

        // initialize server if needed
        if (_commandLineArgs.isSimulationServer())
        {
            if (!initNetworkConnection(rank, reader))
                throw std::runtime_error("Simulation server couldn't be initialized");
        }

        //create simulation plan, i.e. multiple simulation plans if mpi should be used (for each mpi process one plan)
        _pp = reader.getProgramPlan();
        printProgramInfo(_pp);

        // initialize and create simulation:
        MainFactory mf;
        _simulation = mf.createSimulation(_pp.simPlans[rank]);
        _isInitialized = true;
    }

    void Program::deinitialize()
    {
        if (_isInitialized)
        {
            //_simulation->deinitialize();
            deinitMPI();
        }
    }

    Simulation::AbstractSimulationSPtr Program::getSimulation()
    {
        return _simulation;
    }

    void Program::printProgramInfo(const ProgramPlan& in) const
    {
        LOGGER_WRITE("", Util::LC_LOADER, Util::LL_INFO);

        LOGGER_WRITE("Program info: ", Util::LC_LOADER, Util::LL_INFO);
        LOGGER_WRITE("numNodes:     " + to_string(in.simPlans.size()), Util::LC_LOADER, Util::LL_INFO);
        LOGGER_WRITE("Node infos:   ", Util::LC_LOADER, Util::LL_INFO);
        for (size_type i = 0; i < in.simPlans.size(); ++i)
        {
            LOGGER_WRITE("Node      " + to_string(i) + ":", Util::LC_LOADER, Util::LL_INFO);
            size_type numFmus = 0;
            for (size_type j = 0; j < in.simPlans[i].dataManager.solvers.size(); ++j)
                numFmus += in.simPlans[i].dataManager.solvers[j].size();
            LOGGER_WRITE("NumFmus:      " + to_string(numFmus), Util::LC_LOADER, Util::LL_INFO);
            LOGGER_WRITE("NumCores:     " + to_string(in.simPlans[i].dataManager.solvers.size()), Util::LC_LOADER, Util::LL_INFO);
            LOGGER_WRITE("\n", Util::LC_LOADER, Util::LL_INFO);
        }

    }

    bool Program::initMPI(int & rank, int & numRanks)
    {
#ifdef USE_MPI

        if (!(_commandLineArgs.getProgramArgs() == make_tuple<const int *, const char ***>(nullptr, nullptr))
                && MPI_SUCCESS == MPI_Init(std::get<0>(_commandLineArgs.getProgramArgs()), std::get<1>(_commandLineArgs.getProgramArgs())))
        {
            MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
            if (numRanks < (long long int) _pp.simPlans.size())
            {
                deinitialize();
                throw std::runtime_error("Program: Not enough mpi processes for given schedule.");
            }
            else if (numRanks > (long long int) _pp.simPlans.size())
                LOGGER_WRITE("Program: More mpi process given than the simulation will use.", Util::LC_LOADER, Util::LL_WARNING);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            _usingMPI = true;
            return true;
        }
        else
            return false;
#else
        //throw std::runtime_error("Program: MPI not supported by this system.");
#endif
    }

    bool Program::initNetworkConnection(const int & rank, XMLConfigurationReader & reader)
    {
        // Need to tread special cases. In a network server case the socket can only be hold by one process. The addional information need to be send via mpi
#ifdef USE_NETWORK_OFFLOADER
        Network::NetworkPlan np;
        if (rank == 0)
        {
            // initial network phase. Gather which information need to be collected and send to the client:
            Network::InitialNetworkServer initServer(_commandLineArgs.getSimulationServerPort(), _pp);
            np = initServer.getNetworkPlan();
#ifdef USE_MPI
            int num = np.fmuNet.size();
            MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
            for(const auto & addFmu : np.fmuNet)
            {
                // sim num
                MPI_Bcast((void*)(&addFmu.simPos),1,MPI_UINT32_T,0,MPI_COMM_WORLD);
                // core num
                MPI_Bcast((void*)(&addFmu.corePos),1,MPI_UINT32_T,0,MPI_COMM_WORLD);
                // solver num
                MPI_Bcast((void*)(&addFmu.solverPos),1,MPI_UINT32_T,0,MPI_COMM_WORLD);

                // inputs
                num = addFmu.inputMap.size<real_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.inputMap.data<real_type>(),2*addFmu.inputMap.size<real_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                num = addFmu.inputMap.size<int_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.inputMap.data<int_type>(),2*addFmu.inputMap.size<int_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                num = addFmu.inputMap.size<bool_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.inputMap.data<bool_type>(),2*addFmu.inputMap.size<bool_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                num = addFmu.inputMap.size<string_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.inputMap.data<string_type>(),2*addFmu.inputMap.size<string_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                //outputs
                num = addFmu.outputMap.size<real_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.outputMap.data<real_type>(),2*addFmu.outputMap.size<real_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                num = addFmu.outputMap.size<int_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.outputMap.data<int_type>(),2*addFmu.outputMap.size<int_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                num = addFmu.outputMap.size<bool_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.outputMap.data<bool_type>(),2*addFmu.outputMap.size<bool_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);

                num = addFmu.outputMap.size<string_type>();
                MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast((void*)addFmu.outputMap.data<string_type>(),2*addFmu.outputMap.size<string_type>(),MPI_UINT32_T,0,MPI_COMM_WORLD);
            }
#endif
        }
        else
        {
#ifdef USE_MPI

            // quick and dirty bcast for the remote simulation data aka. for the NetworkPlan
            int num = 0;
            MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            std::vector<std::tuple<size_type,size_type> > tmp1, tmp2, tmp3, tmp4;
            for(int i = 0;i<num;++i)
            {
                np.fmuNet.push_back(Network::NetworkFmuInformation());

                // sim num
                MPI_Recv((void*)(&np.fmuNet.back().simPos),1,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                // core num
                MPI_Recv((void*)(&np.fmuNet.back().corePos),1,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                // solver num
                MPI_Recv((void*)(&np.fmuNet.back().solverPos),1,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                //inputs
                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp1.resize(num);
                MPI_Recv((void*)tmp1.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp2.resize(num);
                MPI_Recv((void*)tmp2.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp3.resize(num);
                MPI_Recv((void*)tmp3.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp4.resize(num);
                MPI_Recv((void*)tmp4.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                np.fmuNet.back().inputMap = FMI::InputMapping(tmp1,tmp2,tmp3,tmp4);

                //inputs
                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp1.resize(num);
                MPI_Recv((void*)tmp1.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp2.resize(num);
                MPI_Recv((void*)tmp2.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp3.resize(num);
                MPI_Recv((void*)tmp3.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                MPI_Recv(&num,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                tmp4.resize(num);
                MPI_Recv((void*)tmp4.data(),2*num,MPI_UINT32_T,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

                np.fmuNet.back().outputMap = FMI::InputMapping(tmp1,tmp2,tmp3,tmp4);
            }


#else
            // test if the rank is on default (0), if not somethings wrong with @param rank.
            throw std::runtime_error("Internal error. Rank missmatch in network initialization.");
#endif
        }
        // adding network fmu and connections to program plan
        Network::appendNetworkInformation(_pp, np);
#endif
        return true;
    }

    void Program::deinitMPI()
    {
#ifdef USE_MPI
        if (_usingMPI)
            MPI_Finalize();
#endif
    }
}
