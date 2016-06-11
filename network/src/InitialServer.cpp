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
        std::string filePath = Util::FileHelper::find(_tmpFmus[newId]->getWorkingDirectory(), server.getSimulationFileName(),true);
        if(filePath.empty())
            throw std::runtime_error("Couldn't find requested file.");
        server.confirmSimulationFile(newId,filePath);
    }

    void InitialServer::initSim()
    {
        NetOff::SimulationServer & server = *_networkPlan.server.get();
        int newId = server.getLastSimId();
        NetOff::ValueContainer initialValues;
        FMI::AbstractFmu * fmu = _tmpFmus[newId].get();
        //TODO create InputMapping for vars valueInfo -> references -> name cmp -> vector<int> -> valuzereferences
        FMI::InputMapping mapping;
        auto refs = fmu->getAllValueReferences();
        NetOff::VariableList inputNames = server.getSelectedInputVariables(newId);
        const FMI::ValueInfo & vi = fmu->getValueInfo();

        const std::vector<std::string> & realInputs  = inputNames.getReals();
        for(size_type i=0;i<realInputs.size();++i)
        {
            size_type ref = vi.getReference<real_type>(realInputs[i]);
            for(size_type j=0;j<refs.getValues<real_type>().size();++j)
            {
                if(refs.getValues<real_type>()[j] == ref)
                {
                    mapping.push_back<real_type>(std::make_tuple(j,i));
                }
            }
        }

        const std::vector<std::string> & intInputs  = inputNames.getInts();
        for(size_type i=0;i<realInputs.size();++i)
        {
            size_type ref = vi.getReference<int_type>(intInputs[i]);
            for(size_type j=0;j<refs.getValues<int_type>().size();++j)
            {
                if(refs.getValues<int_type>()[j] == ref)
                {
                    mapping.push_back<int_type>(std::make_tuple(j,i));
                }
            }
        }

        const std::vector<std::string> & realInputs  = inputNames.getReals();
        for(size_type i=0;i<realInputs.size();++i)
        {
            size_type ref = vi.getReference<real_type>(realInputs[i]);
            for(size_type j=0;j<refs.getValues<real_type>().size();++j)
            {
                if(refs.getValues<real_type>()[j] == ref)
                {
                    mapping.push_back<real_type>(std::make_tuple(j,i));
                }
            }
        }

        //TODO init fmu, take values


        server.confirmSimulationInit(newId,initialValues);
    }

    std::shared_ptr<Initialization::FmuPlan> InitialServer::findFmuInProgramPlan(const std::string fmuPath)
    {
        std::shared_ptr<Initialization::FmuPlan> res;
        //TODO
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

        NetOff::VariableList inputVarNames(info.getInputValueNames<real_type>(),info.getInputValueNames<int_type>(),info.getInputValueNames<bool_type>())
                           , outputVarNames(info.getValueNames<real_type>(),info.getValueNames<int_type>(),info.getValueNames<bool_type>());
        server.confirmSimulationAdd(newId,inputVarNames,outputVarNames);
    }
}
