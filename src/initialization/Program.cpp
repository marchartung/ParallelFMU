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

#ifdef USE_MPI
#include <mpi.h>
#endif

#ifdef USE_NETWORK_OFFLOADER
#include "InitialNetworkServer.hpp"
#include "NetworkFunctions.hpp"
#endif

namespace Initialization
{

    Program::Program(CommandLineArgs cla)
            : _isInitialized(false),
              _usingMPI(false),
              _usingOMP(false),
              _commandLineArgs(std::move(cla)),
              _progPlan(),
              _simulations()
    {
    }

    Program::Program(int* argc, char** argv[])
            : Program::Program(CommandLineArgs(argc, argv))
    {
    }

    Program::~Program()
    {
        if (_isInitialized)
        {
            deinitialize();
        }
    }

    void Program::initialize()
    {
        // Already initialized?
        if (_isInitialized)
        {
            return;
        }

        Util::Logger::initialize(_commandLineArgs.getLogSettings());
        // Create simulation plan(s). If MPI should be used we have several simulation plans (One plan for each MPI process).
        // Read config file:
        XMLConfigurationReader reader(_commandLineArgs.getConfigFilePath());
        _progPlan = reader.getProgramPlan();

        // test for MPI initialization:
        int rank = 0, numRanks = 1;
        if (_progPlan.simPlans.size() > 1)
        {
            LOGGER_WRITE("Initialize MPI ...", Util::LC_LOADER, Util::LL_INFO);
            if (!initMPI(rank, numRanks))
            {
                throw runtime_error("Couldn't initialize simulation. MPI couldn't be initialized.");
            }
        }

        if (rank == 0)
        {
            printProgramInfo(_progPlan);
        }

        std::cout << "=1===============================\n";
        // initialize server if needed
        if (isSimulationServer())
        {
            if (!initNetworkConnection(rank))
            {
                throw runtime_error("Simulation server couldn't be initialized");
            }
            std::cout << "=2===============================\n";
        }

        // Let the factory create and initialize the simulation.
        MainFactory mf;

        // Not in parallel because FmuSdkFMU::load and FmiLibFmu::load is not save to call in parallel!
        for (auto & i : _progPlan.simPlans[rank])
        {
            _simulations.push_back(mf.createSimulation(i));
        }

        _isInitialized = true;
        std::cout << "=3===============================\n";
    }

    void Program::simulate()
    {
        size_type threadNum = 0;
        // not in parallel, Communicator::addFmu is not safe to call
        for (auto & sim : _simulations)
        {
            sim->initialize();
        }
#pragma omp parallel num_threads(_simulations.size())
        {
#ifdef USE_OPENMP
            threadNum = omp_get_thread_num();
#endif
            _simulations[threadNum]->simulate();
        }
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
#ifdef USE_OPENMP
        return _simulations[omp_get_thread_num()];
#else
        return _simulations[0];
#endif
    }

    void Program::printProgramInfo(const ProgramPlan& in) const
    {
        LOGGER_WRITE("", Util::LC_LOADER, Util::LL_INFO);

        LOGGER_WRITE("--~~~####ParallelFmu Simulation####~~~--", Util::LC_LOADER, Util::LL_INFO);
        LOGGER_WRITE("", Util::LC_LOADER, Util::LL_INFO);
        LOGGER_WRITE("simulating following FMUs:", Util::LC_LOADER, Util::LL_INFO);
        for (size_type i = 0; i < in.simPlans.size(); ++i)
        {
            for (size_type j = 0; j < in.simPlans[i].size(); ++j)
            {
                for (size_type k = 0; k < in.simPlans[i][j].dataManager.solvers.size(); ++k)
                {
                    const auto & fmu = *in.simPlans[i][j].dataManager.solvers[k]->fmu;
                    LOGGER_WRITE("name:              " + fmu.name, Util::LC_LOADER, Util::LL_INFO);
                    LOGGER_WRITE("path:              " + fmu.path, Util::LC_LOADER, Util::LL_INFO);
                    LOGGER_WRITE("working directory: " + fmu.workingPath, Util::LC_LOADER, Util::LL_INFO);
                    LOGGER_WRITE("node/core:         " + to_string(i) + "/" + to_string(j), Util::LC_LOADER,
                                 Util::LL_INFO);
                    LOGGER_WRITE("", Util::LC_LOADER, Util::LL_INFO);
                }
            }
        }
        LOGGER_WRITE("Program info: ", Util::LC_LOADER, Util::LL_INFO);
        LOGGER_WRITE("numNodes:     " + to_string(in.simPlans.size()), Util::LC_LOADER, Util::LL_INFO);
        LOGGER_WRITE("Node infos:   ", Util::LC_LOADER, Util::LL_INFO);
        for (size_type i = 0; i < in.simPlans.size(); ++i)
        {
            LOGGER_WRITE("Node      " + to_string(i) + ":", Util::LC_LOADER, Util::LL_INFO);
            size_type numFmus = 0;
            for (auto& simPlan : in.simPlans[i])
            {
                numFmus += simPlan.dataManager.solvers.size();
            }
            LOGGER_WRITE("NumFmus:      " + to_string(numFmus), Util::LC_LOADER, Util::LL_INFO);
            LOGGER_WRITE("NumCores:     " + to_string(in.simPlans[i].size()), Util::LC_LOADER, Util::LL_INFO);
            LOGGER_WRITE("\n", Util::LC_LOADER, Util::LL_INFO);
        }

    }

    bool Program::initMPI(int & rank, int & numRanks)
    {
#ifdef USE_MPI

        if (!(_commandLineArgs.getProgramArgs() == make_tuple<const int *, const char ***>(nullptr, nullptr))
                && MPI_SUCCESS
                        == MPI_Init(get<0>(_commandLineArgs.getProgramArgs()),
                                    get<1>(_commandLineArgs.getProgramArgs())))
        {
            MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
            if (numRanks < static_cast<int>(_progPlan.simPlans.size()))
            {
                deinitialize();
                throw runtime_error("Program: Not enough mpi processes for given schedule.");
            }
            else if (numRanks > static_cast<int>(_progPlan.simPlans.size()))
            {
                LOGGER_WRITE("Program: More mpi process given than the simulation will use.", Util::LC_LOADER,
                             Util::LL_WARNING);
            }
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            _usingMPI = true;
            return true;
        }
        {
            return false;
        }
#else
        //throw std::runtime_error("Program: MPI not supported by this system.");
#endif
    }

    bool Program::initNetworkConnection(const int & rank)
    {
        // Need to tread special cases. In a network server case the socket can only be hold by one process. The additional information need to be send via MPI.
#ifdef USE_NETWORK_OFFLOADER
        Network::NetworkPlan np;
        if (rank == 0)
        {
            // initial network phase. Gather which information need to be collected and send to the client:
            Network::InitialNetworkServer initServer(getSimulationServerPort(), _progPlan);
            initServer.start();
            // The NetworkPlan holds a shared_pointer to a server which is copied there
            np = initServer.getNetworkPlan();
#ifdef USE_MPI
            if (_usingMPI)
            {
                int num = np.fmuNet.size();
                MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                for (const auto & addFmu : np.fmuNet)
                {
                    // sim num
                    MPI_Bcast((void*) (&addFmu.simPos), 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
                    // core num
                    MPI_Bcast((void*) (&addFmu.corePos), 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
                    // solver num
                    MPI_Bcast((void*) (&addFmu.solverPos), 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

                    // inputs
                    num = addFmu.inputMap.size<real_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.inputMap.data<real_type>(), 2 * addFmu.inputMap.size<real_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    num = addFmu.inputMap.size<int_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.inputMap.data<int_type>(), 2 * addFmu.inputMap.size<int_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    num = addFmu.inputMap.size<bool_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.inputMap.data<bool_type>(), 2 * addFmu.inputMap.size<bool_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    num = addFmu.inputMap.size<string_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.inputMap.data<string_type>(), 2 * addFmu.inputMap.size<string_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    //outputs
                    num = addFmu.outputMap.size<real_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.outputMap.data<real_type>(), 2 * addFmu.outputMap.size<real_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    num = addFmu.outputMap.size<int_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.outputMap.data<int_type>(), 2 * addFmu.outputMap.size<int_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    num = addFmu.outputMap.size<bool_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.outputMap.data<bool_type>(), 2 * addFmu.outputMap.size<bool_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);

                    num = addFmu.outputMap.size<string_type>();
                    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
                    MPI_Bcast((void*) addFmu.outputMap.data<string_type>(), 2 * addFmu.outputMap.size<string_type>(),
                    MPI_UINT32_T,
                              0, MPI_COMM_WORLD);
                }
            }
#endif
        }
        // Not rank 0
        else
        {
#ifdef USE_MPI
            if (!_usingMPI)
            {
                throw runtime_error("Cannot start mpi simulation.");
            }
            // quick and dirty bcast for the remote simulation data aka. for the NetworkPlan
            int num = 0;
            MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<tuple<size_type, size_type> > tmp1, tmp2, tmp3, tmp4;
            for (int i = 0; i < num; ++i)
            {
                np.fmuNet.push_back(Network::NetworkFmuInformation());

                // sim num
                MPI_Recv((void*) (&np.fmuNet.back().simPos), 1, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // core num
                MPI_Recv((void*) (&np.fmuNet.back().corePos), 1, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // solver num
                MPI_Recv((void*) (&np.fmuNet.back().solverPos), 1, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

                //inputs
                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp1.resize(num);
                MPI_Recv((void*) tmp1.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp2.resize(num);
                MPI_Recv((void*) tmp2.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp3.resize(num);
                MPI_Recv((void*) tmp3.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp4.resize(num);
                MPI_Recv((void*) tmp4.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                np.fmuNet.back().inputMap = FMI::InputMapping(tmp1, tmp2, tmp3, tmp4);

                //inputs
                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp1.resize(num);
                MPI_Recv((void*) tmp1.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp2.resize(num);
                MPI_Recv((void*) tmp2.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp3.resize(num);
                MPI_Recv((void*) tmp3.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Recv(&num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                tmp4.resize(num);
                MPI_Recv((void*) tmp4.data(), 2 * num, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                np.fmuNet.back().outputMap = FMI::InputMapping(tmp1, tmp2, tmp3, tmp4);
            }

#else
            // test if the rank is on default (0), if not something is wrong with @param rank.
            throw runtime_error("Internal error. Rank mismatch in network initialization.");
#endif
        }
        // adding network fmu and connections to program plan
        Network::appendNetworkInformation(_progPlan, np);
#endif
        return true;
    }

    void Program::deinitMPI()
    {
        _isInitialized = false;
#ifdef USE_MPI
        if (_usingMPI)
        {
            MPI_Finalize();
        }
#endif
    }

    bool Program::isSimulationServer() const
    {
        return _commandLineArgs.isSimulationServer();
    }

    int Program::getSimulationServerPort() const
    {
        return _commandLineArgs.getSimulationServerPort();
    }

}  // namespace Initialization
