/** @addtogroup Initialization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_INITIALIZATION_MAINFACTORY_HPP_
#define INCLUDE_INITIALIZATION_MAINFACTORY_HPP_

#include "BasicTypedefs.hpp"
#include "Stdafx.hpp"

#include "fmi/AbstractFmu.hpp"
#include "fmi/FmuSdkFmu.hpp"
#ifdef USE_FMILIB
#include "fmi/FmiLibFmu.hpp"
#endif

#include "solver/Euler.hpp"
#include "solver/Ros2.hpp"

#include "writer/CSVFileWriter.hpp"
#include "writer/MatFileWriter.hpp"
#include "writer/CoutWriter.hpp"

#include "synchronization/AbstractDataHistory.hpp"
#include "synchronization/Interpolation.hpp"

#include "synchronization/DataManager.hpp"
#include "synchronization/SerialConnection.hpp"
#include "synchronization/SerialDataHistory.hpp"
#include "simulation/SerialSimulation.hpp"

#ifdef USE_OPENMP
#include "synchronization/openmp/OpenMPConnection.hpp"
#include "synchronization/openmp/OpenMPDataHistory.hpp"
#include "simulation/openmp/OpenMPSimulation.hpp"
#endif

#ifdef USE_MPI
#include "synchronization/mpi/MPIConnection.hpp"
#endif

namespace Initialization
{
    /**
     * This factory class can create various objects that are required for an FMU based simulation. All created
     * objects are internally cached by their id or unique name. So if an object creation depends on another object,
     * take care that the depended object is created first.
     * A common order is:
     * 1. Create all fmu loader objects
     * 2. Create all fmu objects
     * 3. Create all solver objects
     */
    class MainFactory
    {
     public:
        MainFactory();

        ~MainFactory() = default;

        template<class DataManagerClass, class FmuClass>
        Solver::ISolver * createSolverWithKnownFmu(std::shared_ptr<DataManagerClass> & dm, const SolverPlan & in) const
        {
            Solver::ISolver * res;
            if (in.kind == "euler")
                res = new Solver::Euler<DataManagerClass, FmuClass>(in, FmuClass(*in.fmu), dm);
            else if (in.kind == "ros2")
                res = new Solver::Ros2<DataManagerClass, FmuClass>(in, FmuClass(*in.fmu), dm);
            else
                throw std::runtime_error("MainFactory: Unkown solver type " + in.kind);
            return res;
        }

        template<class DataManagerClass>
        Solver::ISolver * createSolver(std::shared_ptr<DataManagerClass> & dm, const SolverPlan & in) const
        {
            Solver::ISolver * res;
            if (in.fmu->loader == "fmuSdk")
                res = createSolverWithKnownFmu<DataManagerClass, FMI::FmuSdkFmu>(dm, in);
#ifdef USE_FMILIB
            else if (in.fmu->loader == "fmiLib")
                res = createSolverWithKnownFmu<DataManagerClass, FMI::FmiLibFmu>(dm, in);
#endif
            else
                throw std::runtime_error("MainFactory: Unkown fmu loader type " + in.fmu->loader);
            return res;
        }

        Synchronization::ConnectionSPtr createConnection(const ConnectionPlan & in, bool outgoing) const
        {
            Synchronization::ConnectionSPtr res;
            if (in.kind == "serial")
                res = Synchronization::ConnectionSPtr(new Synchronization::SerialConnection(in, outgoing));
            else if (in.kind == "openmp")
                res = Synchronization::ConnectionSPtr(new Synchronization::OpenMPConnection(in, outgoing));
            else if (in.kind == "mpi")
                res = Synchronization::ConnectionSPtr(new Synchronization::MPIConnection(in, outgoing));
            else
                throw std::runtime_error("MainFactory: Unkown connection type " + in.kind);
            return res;
        }

        template<class HistoryClass>
        HistoryClass createHistory(const HistoryPlan & in) const
        {
            return HistoryClass(in);
        }

        template<class WriterClass>
        WriterClass createWriter(const WriterPlan & in) const
        {
            return WriterClass(in);
        }

        template<class HistoryClass, class WriterClass>
        std::shared_ptr<Synchronization::DataManager<HistoryClass, WriterClass> > createDataManager(const SimulationPlan & in) const
        {
            return std::shared_ptr<Synchronization::DataManager<HistoryClass, WriterClass> >(
                    new Synchronization::DataManager<HistoryClass, WriterClass>(in.dataManager, createHistory<HistoryClass>(in.dataManager.history),
                                                                                createWriter<WriterClass>(in.dataManager.writer),
                                                                                Synchronization::Communicator(in)));
        }

        std::vector<Synchronization::ConnectionSPtr> createConnectionsOfSolver(const SolverPlan & in) const
        {
            std::vector<Synchronization::ConnectionSPtr> res;
            for (auto con : in.outConnections)
            {
                res.push_back(createConnection(*con, true));
            }
            for (auto con : in.inConnections)
            {
                res.push_back(createConnection(*con, false));
            }
            return res;
        }

        template<class HistoryClass, class WriterClass>
        vector<std::shared_ptr<Solver::ISolver>> createSolversWithDataManager(const SimulationPlan & in) const
        {
            std::shared_ptr<Synchronization::DataManager<HistoryClass, WriterClass> > dm = createDataManager<HistoryClass, WriterClass>(in);
            size_type numSolvers = 0;
            for (auto& l : in.dataManager.solvers)
                numSolvers += l.size();
            vector<std::shared_ptr<Solver::ISolver>> res(numSolvers);
            size_type i = 0;
            for (const auto & vsp : in.dataManager.solvers)
                for (const auto & sp : vsp)
                {
                    res[i] = std::shared_ptr<Solver::ISolver>(createSolver<Synchronization::DataManager<HistoryClass, WriterClass>>(dm, *sp));
                    auto conns = createConnectionsOfSolver(*sp);
                    res[i].get()->getFmu()->setConnections(conns);
                    ++i;
                }
            return res;
        }

        template<class HistoryClass>
        vector<std::shared_ptr<Solver::ISolver>> createSolvers(const SimulationPlan & in) const
        {
            if (in.dataManager.writer.kind == "csvFileWriter")
            {
                return createSolversWithDataManager<HistoryClass, Writer::CSVFileWriter>(in);
            }
            else if (in.dataManager.writer.kind == "coutWriter")
            {
                return createSolversWithDataManager<HistoryClass, Writer::CoutWriter>(in);
            }
            else if (in.dataManager.writer.kind == "matFileWriter")
            {
                return createSolversWithDataManager<HistoryClass, Writer::MatFileWriter>(in);
            }
            else
                throw std::runtime_error("MainFactory: Unkown writer type " + in.dataManager.writer.kind);
        }

        template<class SimulationClass>
        Simulation::AbstractSimulationSPtr createSimulationWithKnownType(const SimulationPlan & in) const
        {
            Simulation::AbstractSimulationSPtr res;

            if (in.dataManager.history.kind == "serial")
            {
                res = Simulation::AbstractSimulationSPtr(new SimulationClass(in, createSolvers<Synchronization::SerialDataHistory>(in)));
            }
            else if (in.dataManager.history.kind == "openmp")
            {
                res = Simulation::AbstractSimulationSPtr(new SimulationClass(in, createSolvers<Synchronization::OpenMPDataHistory>(in)));
            }
            return res;

        }

        Simulation::AbstractSimulationSPtr createSimulation(const SimulationPlan & in) const;
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_MAINFACTORY_HPP_ */
/**
 * @}
 */
