/*
 * Program.cpp
 *
 *  Created on: 08.03.2016
 *      Author: hartung
 */

#include "initialization/MainFactory.hpp"

namespace Initialization
{

    FMI::AbstractFmu * MainFactory::createFmu(const FmuPlan & plan) const
    {
        FMI::AbstractFmu * res;
        if (plan.loader == "fmuSdk")
            res = new FMI::FmuSdkFmu(plan);
#ifdef USE_FMILIB
        else if (plan.loader == "fmiLib")
        res = new FMI::FmiLibFmu(plan);
#endif
#ifdef USE_NETWORK_OFFLOADER
        else if (plan.loader == "network")
            res = new FMI::EmptyFmu(plan);
#endif
        else
            throw std::runtime_error("MainFactory: Unknown FMU loader type " + plan.loader);
        return res;
    }

    Synchronization::ConnectionSPtr MainFactory::createConnection(const ConnectionPlan & in, bool outgoing) const
    {
        Synchronization::ConnectionSPtr res;
        if (in.kind == "serial")
            res = Synchronization::ConnectionSPtr(new Synchronization::SerialConnection(in));
#ifdef USE_OPENMP
        else if (in.kind == "openmp")
        res = Synchronization::ConnectionSPtr(new Synchronization::OpenMPConnection(in));
#endif
#ifdef USE_MPI
        else if (in.kind == "mpi")
            res = Synchronization::ConnectionSPtr(new Synchronization::MPIConnection(in));
#endif
        else
            throw std::runtime_error("MainFactory: Unknown connection type " + in.kind);
        return res;
    }

    Simulation::AbstractSimulationSPtr MainFactory::createSimulation(SimulationPlan & in) const
    {
        if (in.kind == "serial")
            return createSimulationWithKnownType<Simulation::SerialSimulation>(in);
        //else if(in.kind == "openmp")
        //    return createSimulationWithKnownType<Simulation::OpenMPSimulation>(in);
        else
            throw std::runtime_error("MainFactoy: There's no simulation type " + in.kind);
    }

    vector<Synchronization::ConnectionSPtr> MainFactory::createConnectionsOfSolver(const SolverPlan & in) const
    {
        vector<Synchronization::ConnectionSPtr> res;
        for (auto con : in.outConnections)
            res.push_back(createConnection(*con, true));

        for (auto con : in.inConnections)
            res.push_back(createConnection(*con, false));

        return res;
    }

} /* namespace Initialization */
