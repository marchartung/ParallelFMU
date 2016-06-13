/*
 * NetworkSolver.hpp
 *
 *  Created on: 03.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_NETWORKSOLVER_HPP_
#define NETWORK_INCLUDE_NETWORKSOLVER_HPP_

#include "../../include/solver/AbstractSolver.hpp"
#include "SimulationServer.hpp"

namespace Solver
{
    template<class DataManagerClass>
    class AbstractSolver<DataManagerClass, FMI::EmptyFmu> : public ISolver
    {
     public:
        AbstractSolver() = delete;

        ~AbstractSolver()
        {

        }

        AbstractSolver(const Initialization::SolverPlan & in, std::shared_ptr<DataManagerClass> & dataManager)
                : _id(in.id),
                  _dataManager(dataManager),
                  _currentTime(in.startTime),
                  _endTime(in.endTime),
                  _simServer(in.networkPlan.server),
                  _spec(NetOff::ClientMessageSpecifyer::UNPAUSE),
                  _solvPlan(in),
                  _lastRequestHandled(true),
                  _outputsSent(false)
        {
            if(!_simServer->isActive())
                throw std::runtime_error("SimulationServer isn't active. Abort.");
            initialize();
        }

        size_type solve(const size_type & numSteps) override
        {
            unsigned count = 0, testVar = 1;
            while(count++ < numSteps && testVar == 1)
            {
                if(_lastRequestHandled)
                    _spec = _simServer->getClientRequest();
                switch(_spec)
                {
                    case NetOff::ClientMessageSpecifyer::INPUTS:
                        testVar = confirmInputs();
                        break;
                    case NetOff::ClientMessageSpecifyer::PAUSE:
                        testVar = pauseSim();
                        break;
                    case NetOff::ClientMessageSpecifyer::UNPAUSE:
                        testVar = unpauseSim();
                        break;
                    case NetOff::ClientMessageSpecifyer::RESET:
                        testVar = abortSim();
                        break;
                    case NetOff::ClientMessageSpecifyer::CLIENT_ABORT:
                        testVar = abortSim();
                        break;
                }
            }
            return (testVar == 1) ? count : testVar;
        }

        void initialize() override
        {
            // set up values
            size_type numReals = 0, numInts = 0, numBools = 0, numStrings = 0;

            for(const auto & fmuCon : _solvPlan.networkPlan.fmuNet)
            {
                numReals += fmuCon.inputMap.size<real_type>() + fmuCon.outputMap.size<real_type>();
                numInts += fmuCon.inputMap.size<int_type>() + fmuCon.outputMap.size<int_type>();
                numBools += fmuCon.inputMap.size<bool_type>() + fmuCon.outputMap.size<bool_type>();
                numStrings += fmuCon.inputMap.size<string_type>() + fmuCon.outputMap.size<string_type>();
            }
            _fmu.setNumValues(numReals,numInts,numBools,numStrings);

            _dataManager->addFmu(&_fmu);


            //TODO outConns
            _simServer->confirmStart();
        }

        void setEndTime(const real_type & simTime) override
        {
            _endTime = simTime;
        }

        void setTolerance(const real_type & tolerance) override
        {

        }

        Synchronization::IDataManager* getDataManager() override
        {
            return _dataManager.get();
        }

        real_type getCurrentTime() const
        {
            return _currentTime;
        }

        real_type getEndTime() const
        {
            return _endTime;
        }

        real_type getCurrentStepSize() const
        {
            return 0.01;
        }

        void setStepSize(const real_type & in) override
        {

        }

        real_type getStepSize() const
        {
            return 0.01;
        }

        bool_type isFinished() const
        {
            return _simServer->isActive();
        }

        size_type getId() const
        {
            return _id;
        }

        const FMI::AbstractFmu* getFmu() const
        {
            return nullptr;
        }

        FMI::AbstractFmu* getFmu() override
        {
            return nullptr;
        }

        real_type getTolerance() const
        {
            return 1.0e-5;
        }

        real_type getMaxError() const
        {
            return 0.0;
        }

        void setMaxError(const real_type & maxError) override
        {

        }

        size_type getEventCounter() const
        {
            return 0;
        }

     private:
        size_type _id;
        std::shared_ptr<DataManagerClass> _dataManager;
        real_type _currentTime;
        real_type _endTime;

        std::shared_ptr<NetOff::SimulationServer> _simServer;
        std::vector<Synchronization::ConnectionSPtr> outConns; // refs simNum to connection
        NetOff::ClientMessageSpecifyer _spec;

        const Initialization::SolverPlan _solvPlan;
        FMI::EmptyFmu _fmu;

        bool _lastRequestHandled;
        bool _outputsSent;

        size_type confirmInputs()
        {
            int fmuId = _simServer->getLastSimId();
            real_type remoteTime = _simServer->getLastReceivedTime(fmuId);
            if(!_outputsSent)
                if(_dataManager->sendSingleOutput(remoteTime,1,&_fmu, outConns[fmuId]->getLocalId()))
                    _outputsSent = true;
                else
                {
                    _lastRequestHandled = false;
                    _outputsSent = false;
                    return 1;
                }
            if(std::abs(_currentTime-remoteTime) > 1.0e-10 )
            {
                // get values
                if(_dataManager->getDependencyInfo(_fmu) != Solver::DependencyStatus::BLOCKED)
                {
                    _dataManager->setFmuInputValuesAtT(remoteTime,_fmu);
                    _currentTime = remoteTime;
                }
                else
                {
                    _lastRequestHandled = false;
                    return 1;
                }
            }
            NetOff::ValueContainer & vals = _simServer->getOutputValueContainer(fmuId);
            const FMI::ValueCollection & fmuVals = _fmu.getEmptyFmuValues();
            FMI::ValueCollection tmp = _solvPlan.networkPlan.fmuNet[fmuId].outputMap.pack(fmuVals); // TODO alloc
            vals.setRealValues(tmp.getValues<real_type>().data());
            vals.setIntValues(tmp.getValues<int_type>().data());
            vals.setBoolValues(tmp.getValues<bool_type>().data());

            _lastRequestHandled = _simServer->sendOutputValues(fmuId,_currentTime, vals);
            return 1;
        }

        size_type pauseSim()
        {
            return 1;
        }

        size_type unpauseSim()
        {
            return 1;
        }

        size_type abortSim()
        {
            _simServer->deinitialize();
            return std::numeric_limits<size_type>::max();
        }

    };
}

