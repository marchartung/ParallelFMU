/*
 * InitialServer.cpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#include "InitialNetworkServer.hpp"
#include "initialization/MainFactory.hpp"
#include "util/FileHelper.hpp"

namespace Network
{

    InitialNetworkServer::InitialNetworkServer(const int & port, Initialization::ProgramPlan & programPlan)
            : _programPlan(programPlan),
              _networkPlan(),
              _offsets(4, 0),
              _tmpFmus()
    {
        _networkPlan.server = make_shared<NetOff::SimulationServer>(port);
    }

    NetworkPlan InitialNetworkServer::getNetworkPlan() const
    {
        return _networkPlan;
    }

    void InitialNetworkServer::start()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        if (!server.initializeConnection())
        {
            throw runtime_error("Couldn't initialize network connection.");
        }

        // The loop end, if the START signal is received. In the meanwhile other stuff can be done (e.g., several
        // simulations can be added and initialized).
        bool run = true;
        while (run)
        {
            NetOff::InitialClientMessageSpecifyer spec = server.getInitialClientRequest();
            switch (spec)
            {
                case NetOff::InitialClientMessageSpecifyer::ADD_SIM:
                    addSim();
                    break;
                case NetOff::InitialClientMessageSpecifyer::GET_FILE:
                    getFile();
                    break;
                case NetOff::InitialClientMessageSpecifyer::INIT_SIM:
                    initSim();
                    break;
                case NetOff::InitialClientMessageSpecifyer::CLIENT_INIT_ABORT:
                    server.deinitialize();
                    throw runtime_error("Client aborted connection in initialization.");
                    break;
                case NetOff::InitialClientMessageSpecifyer::START:
                    startSim();
                    run = false;
                    break;
            }
        }
    }

    void InitialNetworkServer::getFile()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        int newId = server.getLastSimId();
        string filePath = Util::FileHelper::find(_tmpFmus[newId]->getWorkingDirectory(), server.getSimulationFileName(),
                                                 true);
        if (filePath.empty())
        {
            if (server.getSimulationFileName().find('/') != string::npos)
            {
                filePath = Util::FileHelper::absoluteFilePath(server.getSimulationFileName());  // look from root
            }
            if (filePath.empty())
            {
                throw runtime_error("Couldn't find requested file.");
            }
        }
        server.confirmSimulationFile(newId, filePath);
    }

    void InitialNetworkServer::initSim()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        int newId = server.getLastSimId();

        FMI::AbstractFmu * fmu = _tmpFmus[newId].get();
        if (fmu->isLoaded())
        {
            const FMI::ValueReferenceCollection & refs = fmu->getAllValueReferences();
            const FMI::ValueInfo & vi = fmu->getValueInfo();

            _networkPlan.fmuNet[newId].outputMap = getMappingFromNameList(refs, server.getSelectedOutputVariables(newId),
                                                                         vi, true);
            _networkPlan.fmuNet[newId].inputMap = getMappingFromNameList(refs, server.getSelectedInputVariables(newId),
                                                                          vi, false);
        }
        else
        {
            throw runtime_error("Remote simulation failed. Fmu couldn't be initialized.");
        }

        FMI::ValueCollection tmpVals = _networkPlan.fmuNet[newId].outputMap.pack(
                fmu->getValues(FMI::ReferenceContainerType::ALL));
        NetOff::ValueContainer & initialValues = server.getOutputValueContainer(newId);
        initialValues.setRealValues(tmpVals.getValues<real_type>().data());
        initialValues.setIntValues(tmpVals.getValues<int_type>().data());
        initialValues.setBoolValues(tmpVals.getValues<bool_type>().data());
        server.confirmSimulationInit(newId, initialValues);
    }

    shared_ptr<Initialization::FmuPlan> InitialNetworkServer::findFmuInProgramPlan(const string fmuPath,
                                                                                   NetworkFmuInformation & netInfo)
    {
        shared_ptr<Initialization::FmuPlan> res = nullptr;
        size_type simId = 0, coreId = 0, solveId = 0;
        for (const auto & simPlanVec : _programPlan.simPlans)
        {
            for (const auto & simPlan : simPlanVec)
            {
                for (const auto & solvPlan : simPlan.dataManager.solvers)
                {
                    if (fmuPath == Util::FileHelper::absoluteFilePath(solvPlan->fmu->path)
                            || fmuPath == solvPlan->fmu->name || fmuPath == solvPlan->fmu->name + string(".fmu"))
                    {
                        res = solvPlan->fmu;
                        netInfo.mpiPos = simId;
                        netInfo.corePos = coreId;
                        netInfo.solverPos = solveId;
                        return res;
                    }
                    ++solveId;
                }
                ++coreId;
            }
            ++simId;
        }

        return res;
    }

    void InitialNetworkServer::addSim()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        vector<NetworkFmuInformation> & netFmuVec = _networkPlan.fmuNet;

        // Get <FmuPath, simId> for the most recently added simulation.
        auto fmuPathSimId = server.getAddedSimulation();
        auto fmuPath = get<0>(fmuPathSimId);
        auto newId = get<1>(fmuPathSimId);

        netFmuVec.resize(newId + 1);
        _tmpFmus.resize(newId + 1);

        auto fmuPlan = findFmuInProgramPlan(fmuPath, netFmuVec[newId]);
        if (!fmuPlan)
        {
            server.deinitialize();
            throw runtime_error("Couldn't find fmu with given path.");
        }

        Initialization::MainFactory mf;
        _tmpFmus[newId] = shared_ptr<FMI::AbstractFmu>(mf.createFmu(*fmuPlan));
        _tmpFmus[newId]->load();
        const FMI::ValueInfo & info = _tmpFmus[newId]->getValueInfo();

        NetOff::VariableList inputVarNames(info.getInputValueNames<real_type>(), info.getInputValueNames<int_type>(),
                                           info.getInputValueNames<bool_type>()), outputVarNames(
                info.getValueNames<real_type>(), info.getValueNames<int_type>(), info.getValueNames<bool_type>());
        server.confirmSimulationAdd(newId, inputVarNames, outputVarNames);
    }

    void InitialNetworkServer::startSim()
    {
        for (const auto & fmu : _tmpFmus)
        {
            fmu->unload();
        }
    }

    FMI::InputMapping InitialNetworkServer::getMappingFromNameList(const FMI::ValueReferenceCollection& refs,
                                                                   const NetOff::VariableList& vars,
                                                                   const FMI::ValueInfo& vi, bool fromFmu)
    {
        FMI::InputMapping mapping;
        addRefsToMapping<real_type>(mapping, vars.getReals(), vi, refs, fromFmu);
        addRefsToMapping<int_type>(mapping, vars.getInts(), vi, refs, fromFmu);
        addRefsToMapping<bool_type>(mapping, vars.getBools(), vi, refs, fromFmu);
        return mapping;
    }

}  // namespace Network
