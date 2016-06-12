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
                  _simServer(in.server),
                  _outputMappings(in.outputMappings),
                  _spec(NetOff::ClientMessageSpecifyer::UNPAUSE)
        {
            if(!_simServer->isActive())
                throw std::runtime_error("SimulationServer isn't active. Abort.");
        }

        size_type solve(const size_type & numSteps) override
        {
            unsigned count = 0;
            while(count++ < numSteps)
            {
                _spec = _simServer->getClientRequest();
                switch(_spec)
                {
                    case NetOff::ClientMessageSpecifyer::INPUTS:
                        confirmInputs();
                        break;
                    case NetOff::ClientMessageSpecifyer::PAUSE:
                        pauseSim();
                        break;
                    case NetOff::ClientMessageSpecifyer::UNPAUSE:
                        unpauseSim();
                        break;
                    case NetOff::ClientMessageSpecifyer::RESET:
                        abortSim();
                        break;
                    case NetOff::ClientMessageSpecifyer::CLIENT_ABORT:
                        abortSim();
                        break;
                }
            }
            return count;
        }

        void initialize() override
        {

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
            return 0.1;
        }

        void setStepSize(const real_type & in) override
        {

        }

        real_type getStepSize() const
        {
            return 0.1;
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

        FMI::ValueCollection values;

        std::shared_ptr<NetOff::SimulationServer> _simServer;
        std::vector<FMI::InputMapping> _outputMappings;

        NetOff::ClientMessageSpecifyer _spec;

        void confirmInputs()
        {
            //TODO!!!

        }

        void pauseSim();
        void unpauseSim();
        void abortSim();

    };
}

