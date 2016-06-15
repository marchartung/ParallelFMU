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
        size_type numFmus = 0, numCons = 0, netSimId = 0, netCoreId = 0;  //TODO own thread?!?!?
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
        plan.simPlans[netSimId][netCoreId].dataManager.solvers.push_back(sPlan);

        for (const auto & extender : netPlan.fmuNet)
        {
            std::shared_ptr<Initialization::ConnectionPlan> newInputCon;  // networkFmu -> dependent Fmu
            std::shared_ptr<Initialization::ConnectionPlan> newOutputCon;  // output/dependent fmu -> networkFmu
            newInputCon = std::shared_ptr<Initialization::ConnectionPlan>(new Initialization::ConnectionPlan());
            newInputCon->bufferSize = Initialization::DefaultValues::connectionPlan().bufferSize;
            newInputCon->destFmu = plan.simPlans[extender.simPos][extender.corePos].dataManager.solvers[extender.solverPos]->fmu->name;
            newInputCon->sourceFmu = sPlan->fmu->name;
            newInputCon->destRank = extender.simPos;
            newInputCon->sourceRank = netSimId;

            newOutputCon = std::shared_ptr<Initialization::ConnectionPlan>(new Initialization::ConnectionPlan(*newInputCon));

            newInputCon->startTag = numCons;
            newInputCon->inputMapping = extender.inputMap;
            ++numCons;

            newOutputCon->startTag = numCons;
            newOutputCon->inputMapping = extender.outputMap;
            ++numCons;

            if (extender.simPos != netSimId)
            {
                newInputCon->kind = "mpi";
                newOutputCon->kind = "mpi";

                plan.simPlans[extender.simPos][extender.corePos].dataManager.inConnections.push_back(newInputCon);
                plan.simPlans[extender.simPos][extender.corePos].dataManager.outConnections.push_back(newOutputCon);
            }
            else if (extender.corePos != netCoreId)
            {
                newInputCon->kind = "openmp";
                newOutputCon->kind = "openmp";

                plan.simPlans[extender.simPos][extender.corePos].dataManager.inConnections.push_back(newInputCon);
                plan.simPlans[extender.simPos][extender.corePos].dataManager.outConnections.push_back(newOutputCon);
            }
            else
            {
                newInputCon->kind = "serial";
                newOutputCon->kind = "serial";
            }

            //Add additional connection so network fmu's simulation plan
            plan.simPlans[netSimId][netCoreId].dataManager.outConnections.push_back(newInputCon);
            plan.simPlans[netSimId][netCoreId].dataManager.solvers.back()->outConnections.push_back(newInputCon);

            plan.simPlans[netSimId][netCoreId].dataManager.inConnections.push_back(newOutputCon);
            plan.simPlans[netSimId][netCoreId].dataManager.solvers.back()->inConnections.push_back(newOutputCon);

            //Add additional connection so dependend fmu's simulation plan
            plan.simPlans[extender.simPos][extender.corePos].dataManager.solvers[extender.solverPos]->inConnections.push_back(newInputCon);
            plan.simPlans[extender.simPos][extender.corePos].dataManager.solvers[extender.solverPos]->outConnections.push_back(newOutputCon);
        }
    }
}

