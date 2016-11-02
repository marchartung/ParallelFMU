/*
 * NetworkFunctions.cpp
 *
 *  Created on: 14.06.2016
 *      Author: hartung
 */

#include "NetworkFunctions.hpp"
#include "initialization/DefaultValues.hpp"

namespace Network
{

    void appendNetworkInformation(Initialization::ProgramPlan & plan, NetworkPlan & netPlan)
    {
        size_type numFmus = 0, numCons = 0, netMpiRank = 0, netCoreId = 0, netSolvId;  //TODO own thread?!?!?
        for (const auto & simVec : plan.simPlans)
            for (const auto & sim : simVec)
            {
                numFmus += sim.dataManager.solvers.size();
                numCons += sim.dataManager.inConnections.size();
            }

        std::shared_ptr<Initialization::SolverPlan> sPlan = std::shared_ptr<Initialization::SolverPlan>(
                new Initialization::SolverPlan(*plan.simPlans[0][0].dataManager.solvers[0]));
        sPlan->fmu = std::shared_ptr<Initialization::FmuPlan>(new Initialization::FmuPlan(*sPlan->fmu));
        sPlan->fmu->loader = "network";
        sPlan->fmu->id = numFmus;
        sPlan->fmu->name = "ParallelFmu_NetworkFmu0815";
        sPlan->networkPlan = netPlan;

        auto & networkDataManager = plan.simPlans[netMpiRank][netCoreId].dataManager;
        netSolvId = networkDataManager.solvers.size();
        networkDataManager.solvers.push_back(sPlan);
        auto & networkSolver = networkDataManager.solvers[netSolvId];

        /** in this step all real fmus has to be linked to the network fmu, if their values are important for simulation
         * 1. the output mapping in toExtend is a connection from real fmu -> network fmu
         * 2. the input mapping in toExten is a connection from network fmu -> to real fmu
         */
        for (const auto & toExtend : netPlan.fmuNet)
        {

            auto & realDataManager = plan.simPlans[toExtend.mpiPos][toExtend.corePos].dataManager;
            auto & realSolver = realDataManager.solvers[toExtend.solverPos];
            // set up a connection plan for in- and output but insert only the ones are needed

            std::shared_ptr<Initialization::ConnectionPlan> networkToReal;  // networkFmu -> dependent Fmu
            if (toExtend.inputMap.size() > 0)
            {
                networkToReal = std::shared_ptr<Initialization::ConnectionPlan>(new Initialization::ConnectionPlan());
                networkToReal->bufferSize = Initialization::DefaultValues::connectionPlan().bufferSize;
                networkToReal->destFmu = realSolver->fmu->name;
                networkToReal->sourceFmu = sPlan->fmu->name;
                networkToReal->destRank = toExtend.mpiPos;
                networkToReal->sourceRank = netMpiRank;
                networkToReal->inputMapping = toExtend.inputMap;
                networkToReal->startTag = numCons;
                ++numCons;
            }

            std::shared_ptr<Initialization::ConnectionPlan> realToNetwork;  // output/dependent fmu -> networkFmu
            if (toExtend.outputMap.size() > 0)
            {
                realToNetwork = std::shared_ptr<Initialization::ConnectionPlan>(new Initialization::ConnectionPlan());
                realToNetwork->bufferSize = Initialization::DefaultValues::connectionPlan().bufferSize;
                realToNetwork->destFmu = sPlan->fmu->name;
                realToNetwork->sourceFmu = realSolver->fmu->name;
                realToNetwork->destRank = netMpiRank;
                realToNetwork->sourceRank = toExtend.mpiPos;
                realToNetwork->inputMapping = toExtend.outputMap;
                realToNetwork->startTag = numCons;
                ++numCons;
            }

            // choose the connection type which is need and add the connection(s)

            if (toExtend.mpiPos != netMpiRank)
            {
                if (networkToReal)
                    networkToReal->kind = "mpi";

                if (realToNetwork)
                    realToNetwork->kind = "mpi";

            }
            else if (toExtend.corePos != netCoreId)
            {
                if (networkToReal)
                    networkToReal->kind = "openmp";

                if (realToNetwork)
                    realToNetwork->kind = "openmp";
            }
            else
            {
                if (networkToReal)
                    networkToReal->kind = "serial";

                if (realToNetwork)
                    realToNetwork->kind = "serial";

            }

            if (networkToReal)
            {
                realDataManager.inConnections.push_back(networkToReal);
                realSolver->inConnections.push_back(networkToReal);
                networkDataManager.outConnections.push_back(networkToReal);
                networkSolver->outConnections.push_back(networkToReal);
            }

            if (realToNetwork)
            {
                realDataManager.outConnections.push_back(realToNetwork);
                realSolver->outConnections.push_back(realToNetwork);
                networkDataManager.inConnections.push_back(realToNetwork);
                networkSolver->inConnections.push_back(realToNetwork);
            }
        }
    }
}

