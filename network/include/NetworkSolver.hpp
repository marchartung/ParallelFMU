/*
 * NetworkSolver.hpp
 *
 *  Created on: 03.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_NETWORKSOLVER_HPP_
#define NETWORK_INCLUDE_NETWORKSOLVER_HPP_

#include "solver/AbstractSolver.hpp"
#include "SimulationServer.hpp"

namespace Solver
{

    /*! \brief
     *
     * This is a specialization of template class AbstractSolver with FMUClass=FMI::EmptyFMU.
     */
    template<class DataManagerClass>
    class NetworkSolver : public ISolver
    {
     public:
        NetworkSolver() = delete;

        ~NetworkSolver() = default;

        NetworkSolver(const Initialization::SolverPlan & in, const FMI::EmptyFmu & fmu,
                      shared_ptr<DataManagerClass> & dataManager)
                : _id(in.id),
                  _dataManager(dataManager),
                  _currentTime(in.startTime),
                  _endTime(in.endTime),
                  _simServer(in.networkPlan.server),
                  _spec(NetOff::ClientMessageSpecifyer::UNPAUSE),
                  _solvPlan(in),
                  _fmu(fmu),
                  _lastRequestHandled(true),
                  _outputsSent(false)
        {
            if (!_simServer->isActive())
            {
                throw runtime_error("SimulationServer isn't active. Abort.");
            }
        }

        size_type solve(const size_type & numSteps) override
        {
            unsigned count = 0, testVar = 1;
            while (count++ < numSteps && testVar == 1)
            {
                if (_lastRequestHandled)
                {
                    _spec = _simServer->getClientRequest();
                    _lastRequestHandled = false;

                }
                switch (_spec)
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

            for (const auto & fmuCon : _solvPlan.networkPlan.fmuNet)
            {
                numReals += fmuCon.inputMap.template size<real_type>() + fmuCon.outputMap.template size<real_type>();
                numInts += fmuCon.inputMap.template size<int_type>() + fmuCon.outputMap.template size<int_type>();
                numBools += fmuCon.inputMap.template size<bool_type>() + fmuCon.outputMap.template size<bool_type>();
                numStrings += fmuCon.inputMap.template size<string_type>()
                        + fmuCon.outputMap.template size<string_type>();
            }
            _fmu.setNumValues(numReals, numInts, numBools, numStrings);

            _dataManager->addFmu(&_fmu);

            for (auto& con : _fmu.getConnections())
            {
                if (!con->isOutgoing(_fmu.getFmuName()))
                {
                    _sendToVis.push_back(con);
                }
            }
            _simServer->confirmStart();
        }

        ErrorInfo getErrorInfo() const
        {
            return ErrorInfo();
        }

        size_type getSolverOrder() const
        {
            return 1u;
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
            return &_fmu;
        }

        FMI::AbstractFmu* getFmu() override
        {
            return &_fmu;
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
        shared_ptr<DataManagerClass> _dataManager;
        real_type _currentTime;
        real_type _endTime;

        SolverStepInfo _stepInfo;
        DependencySolverInfo _depInfo;

        shared_ptr<NetOff::SimulationServer> _simServer;
        vector<Synchronization::ConnectionSPtr> _sendToVis;  // refs simNum to connection
        NetOff::ClientMessageSpecifyer _spec;

        const Initialization::SolverPlan _solvPlan;
        FMI::EmptyFmu _fmu;

        bool _lastRequestHandled;
        bool _outputsSent;

        size_type confirmInputs()
        {
            int fmuId = _simServer->getLastSimId();
            real_type remoteTime = _simServer->getLastReceivedTime(fmuId);

            if (setFmuValues(remoteTime, fmuId))
            {
                NetOff::ValueContainer & vals = _simServer->getOutputValueContainer(fmuId);
                const FMI::ValueCollection & fmuVals = _fmu.getEmptyFmuValues();
                FMI::ValueCollection tmp = _solvPlan.networkPlan.fmuNet[fmuId].outputMap.pack(fmuVals);  // TODO alloc
                vals.setRealValues(tmp.getValues<real_type>().data());
                vals.setIntValues(tmp.getValues<int_type>().data());
                vals.setBoolValues(tmp.getValues<bool_type>().data());
                LOGGER_WRITE("T: " + to_string(remoteTime) + ") val: " + to_string(tmp.getValues<real_type>()[0]),
                             Util::LC_SOLVER, Util::LL_DEBUG);
                _lastRequestHandled = _simServer->sendOutputValues(fmuId, _currentTime /*, vals*/);
                return 1;
            }

            return 0;
        }

        /**
         * scatters input values from vis client to fmus and trys to gathers output values for vis client
         * @return Returns true if the values could be gathered at time [remoteTime], false otherwise
         */
        bool setFmuValues(const real_type & remoteTime, const int & fmuId)
        {
            _fmu.setTime(remoteTime);
            _depInfo = _dataManager->getDependencyInfo(&_fmu);
            if (_depInfo.depStatus == Solver::DependencyStatus::BLOCKED)
            {
                return false;
            }

            if (_depInfo.depStatus == Solver::DependencyStatus::EVENT)
            {
                _stepInfo.clear();
                if (_depInfo.eventTimeEnd < remoteTime)
                {
                    return false;
                }
            }

            if (_dataManager->saveSolverStep(&_fmu, _stepInfo, getSolverOrder()))
            {
                _stepInfo.clear();
                return true;
            }

            return false;
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

}  // namespace Solver

#endif // NETWORK_INCLUDE_NETWORKSOLVER_HPP_
