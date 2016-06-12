/*
 * InitialServer.cpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#include "InitialServer.hpp"
#include "initialization/MainFactory.hpp"
#include "util/FileHelper.hpp"

namespace Network
{
    InitialServer::InitialServer(const int & port, Initialization::ProgramPlan & plan)
        :_port(port),
         _plan(plan)
    {

    }

    InitialServer::~InitialServer()
    {

    }

    NetworkPlan InitialServer::getNetworkPlan()
    {
        return _networkPlan;
    }

    void InitialServer::start()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        _networkPlan.server = std::shared_ptr<NetOff::SimulationServer>(new NetOff::SimulationServer(_port));
        if (!server.initializeConnection())
            throw std::runtime_error("Couldn't initialize network connection.");

        bool run = true;
        while (run)
        {
            NetOff::InitialClientMessageSpecifyer spec;
            switch (spec)
            {
                case NetOff::InitialClientMessageSpecifyer::ADD_SIM:
                {
                    addSim();
                    break;
                }
                case NetOff::InitialClientMessageSpecifyer::GET_FILE:
                    getFile();
                    break;
                case NetOff::InitialClientMessageSpecifyer::INIT_SIM:
                    initSim();
                    break;
                case NetOff::InitialClientMessageSpecifyer::CLIENT_INIT_ABORT:
                    server.deinitialize();
                    throw std::runtime_error("Client aborted connection in initialization.");
                    break;
                case NetOff::InitialClientMessageSpecifyer::START:
                    run = false;
                    break;
            }
        }
    }

    void InitialServer::getFile()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        int newId = server.getLastSimId();
        std::string filePath = Util::FileHelper::find(_tmpFmus[newId]->getWorkingDirectory(), server.getSimulationFileName(), true);
        if (filePath.empty())
        {
            if (server.getSimulationFileName().find('/') != std::string::npos)
            {
                filePath = Util::FileHelper::absoluteFilePath(server.getSimulationFileName());  // look from root
            }
            if (filePath.empty())
                throw std::runtime_error("Couldn't find requested file.");
        }
        server.confirmSimulationFile(newId, filePath);
    }

    void InitialServer::initSim()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        int newId = server.getLastSimId();
        FMI::AbstractFmu * fmu = _tmpFmus[newId].get();

        const FMI::ValueReferenceCollection & refs = fmu->getAllValueReferences();
        const FMI::ValueInfo & vi = fmu->getValueInfo();

        FMI::InputMapping inputMapping = getMappingFromNameList(refs,server.getSelectedInputVariables(newId),vi);
        FMI::InputMapping outputMapping = getMappingFromNameList(refs,server.getSelectedOutputVariables(newId),vi);

        fmu->load(true);
        //TODO use input values in initialization
        NetOff::ValueContainer initialValues = outputMapping.pack(fmu->getValues());
        server.confirmSimulationInit(newId, initialValues);
    }

    std::shared_ptr<Initialization::FmuPlan> InitialServer::findFmuInProgramPlan(const std::string fmuPath)
    {
        std::shared_ptr<Initialization::FmuPlan> res;
        for(const auto & simPlan : _plan.simPlans)
            for(const std::shared_ptr<Initialization::SolverPlan> & solvPlan : simPlan.dataManager.solvers)
            {
                if(fmuPath == solvPlan->fmu->path || fmuPath == solvPlan->fmu->name)
                    res = solvPlan->fmu;
            }
        return res;
    }

    void InitialServer::addSim()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        std::vector<Network::NetworkFmuInformation> & netFmuVec = _networkPlan.fmuNet;

        Initialization::MainFactory mf;
        int newId = std::get<1>(server.getAddedSimulation());

        netFmuVec.resize(newId + 1);
        _tmpFmus.resize(newId + 1);

        std::shared_ptr<Initialization::FmuPlan> fmuPlan = findFmuInProgramPlan(std::get<0>(server.getAddedSimulation()));
        netFmuVec[newId].fmuId = fmuPlan->id;
        if (!fmuPlan)
            throw std::runtime_error("Couldn't find fmu with given path.");

        _tmpFmus[newId] = std::shared_ptr<FMI::AbstractFmu>(mf.createFmu(*fmuPlan));
        _tmpFmus[newId]->load();
        const FMI::ValueInfo & info = _tmpFmus[newId]->getValueInfo();

        NetOff::VariableList inputVarNames(info.getInputValueNames<real_type>(), info.getInputValueNames<int_type>(), info.getInputValueNames<bool_type>()),
                outputVarNames(info.getValueNames<real_type>(), info.getValueNames<int_type>(), info.getValueNames<bool_type>());
        server.confirmSimulationAdd(newId, inputVarNames, outputVarNames);
    }

    FMI::InputMapping InitialServer::getMappingFromNameList(const FMI::ValueReferenceCollection& refs, const NetOff::VariableList& vars,
                                                            const FMI::ValueInfo& vi)
    {
        FMI::InputMapping mapping;
        addRefsToMapping<real_type>(mapping,vars.getReals(),vi,refs);
        addRefsToMapping<int_type>(mapping,vars.getInts(),vi,refs);
        addRefsToMapping<bool_type>(mapping,vars.getBools(),vi,refs);

    }

}
