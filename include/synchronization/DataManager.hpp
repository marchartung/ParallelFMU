/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_DATAMANAGER_HPP_
#define INCLUDE_SYNCHRONIZATION_DATAMANAGER_HPP_

#include "fmi/AbstractFmu.hpp"
#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "solver/SolverStepInfo.hpp"
#include "fmi/InputMapping.hpp"
#include "synchronization/IDataManager.hpp"
#include "synchronization/AbstractDataHistory.hpp"
#include "synchronization/Communicator.hpp"
#include "util/Vector.hpp"
#include "initialization/Plans.hpp"

namespace Synchronization
{
    /**
     * Receives data from communicator and interpolates if necessary. One DataManager serves all FMUs on a (compute) node.
     */
    template<class HistoryClass, class WriterClass>
    class DataManager : public IDataManager
    {
        static_assert(std::is_base_of<AbstractDataHistory,HistoryClass>::value, "DataManager: Template argument 1 need to inherit AbstractDataHistory");
        static_assert(std::is_base_of<Writer::IWriter,WriterClass>::value, "DataManager: Template argument 2 need to inherit IWalther");

     public:
        /**
         * Create DataManager for the given FMUs, their connections and the communicator.
         * @param names Names of the FMUs to be handled by this DataManager.
         * @param connections ConnectionMappings of the FMUs.
         * @param comm Communicator.
         * @param writer Writer object to write simulation results.
         */
        //DataManager(const vector<FMI::InputMappingSPtr> & connections, value_type _simStart = 0.0, value_type _stepSize = 0.001);
        DataManager(const Initialization::DataManagerPlan & in, const HistoryClass & history, const WriterClass & writer, shared_ptr<Synchronization::Communicator> & communicator)
                : IDataManager(in),
                  _history(history),
                  _commPtr(communicator),
                  _communicator(*communicator),
                  _writer(writer),
                  _simStart(in.writer.startTime),
                  _stepSize((in.writer.endTime - in.writer.startTime) / in.writer.numSteps),
                  _numManagedFmus(0)
        {
        }

        ~DataManager()
        {
            while (_history.hasWriteOutput())
            {
                _writer.write(_history.getWriteOutput());
            }
        }

        /**
         * \brief Saves all necessary values of the FMU and sets the input values of [fmu] to the given time step
         * If every FMU reached the write-out time step, saveSolverStep calls writeFmuResults.
         * The inputs of [fmu] are interpolated to the given time step. The function frees part of history if possible.
         * @return Returns false, if the results couldn't be saved or when the necessary inputs are not yet present.
         */
        bool saveSolverStep(FMI::AbstractFmu* fmu, const Solver::SolverStepInfo & stepInfo, const size_type & solveOrder) override
        {
            //return true;
            /////////////////////////////////////////////////////////
            ////////////////////// GET INPUTS ///////////////////////
            /////////////////////////////////////////////////////////

            this->setFmuInputValuesAtT(fmu->getTime(), fmu);

            FMI::ValueCollection newValues = fmu->getValues(FMI::ReferenceContainerType::ALL);  // Collection in where inputs are set
            /////////////////////////////////////////////////////////
            ///////////////////// SEND OUTPUTS //////////////////////
            /////////////////////////////////////////////////////////
            if (!sendOutputs(fmu->getTime(), solveOrder, fmu, stepInfo))
                return false;

            //////////////////////////////////////////////////////////
            ////////////////// SAVE DATA IN HISTORY///////////////////
            //////////////////////////////////////////////////////////
            if (stepInfo.hasEventWrite())  // check if event happend
            {
                _history.insert(HistoryEntry(stepInfo.getEventTime<0>(), solveOrder, stepInfo.getEventValues<0>()), fmu->getLocalId(), WriteInfo::EVENTWRITE);  // event save slightly before event
                _history.insert(HistoryEntry(stepInfo.getEventTime<1>(), solveOrder, stepInfo.getEventValues<1>()), fmu->getLocalId(), WriteInfo::EVENTWRITE);  // event save slightly after event
            }
            _history.insert(HistoryEntry(fmu->getTime(), solveOrder, newValues), fmu->getLocalId(), (stepInfo.hasWriteStep()) ? WriteInfo::WRITE : WriteInfo::NOWRITE);  //normal save

            while (_history.hasWriteOutput())
            {
                LOGGER_WRITE("t:" + to_string(fmu->getTime()) + " Write results: " + fmu->getFmuName() + " fmu", Util::LC_SOLVER, Util::LL_DEBUG);
                _writer.write(_history.getWriteOutput());
            }

            return true;
        }

        /**
         * Sets values in fmu, which were send via connections. Only use if its sure that all values are present
         */
        void setFmuInputValuesAtT(const real_type & t, FMI::AbstractFmu* fmu) override
        {
            FMI::ValueCollection fmuValues = fmu->getValues(FMI::ReferenceContainerType::ALL);
            for (const size_type conId : _communicator.getInConnectionIds(fmu))
            {
                FMI::ValueCollection tmpColl = _history.getInputValues(conId, t);  // Implicit interpolation for time [curTime]
                _valuePacking[conId].unpack(fmuValues, tmpColl);
            }
            fmu->setValues(fmuValues);
        }

        /**
         * Calculates the next output time.
         * @param value_type The current FMU time.
         * @return value_type Returns next output time regarding the given argument
         */
        real_type getNextOutputTime(const real_type & currentFmuTime) const override
        {
            double a = (currentFmuTime / _stepSize), b = _simStart / _stepSize;

            int numSteps = ceil(a - b);
            real_type res = numSteps * _stepSize + _simStart;

            if (std::abs(res - currentFmuTime) < _history.getHistory(0).getInterpolation().getTolerance())
                res = (numSteps + 1) * _stepSize + _simStart;
            ;
            if (res < currentFmuTime)
                throw std::runtime_error("getNextOutputTime is buggy\n");
            return res;
        }

        Solver::DependencySolverInfo getDependencyInfo(FMI::AbstractFmu* fmu) override
        {
            if (_communicator.getInConnectionIds(fmu).size() > 0)
            {
                Solver::DependencySolverInfo dsi = collectInputs(fmu->getTime(), fmu);
                if (dsi.depStatus == Solver::DependencyStatus::BLOCKED)
                    return dsi;
                else if (dsi.depStatus == Solver::DependencyStatus::EVENT)
                    _upcomingEvents.push_back(dsi);
                if (!_upcomingEvents.empty())
                    if (_upcomingEvents.front().eventTimeEnd <= fmu->getTime())
                    {
                        dsi = _upcomingEvents.front();
                        _upcomingEvents.pop_front();
                        return dsi;
                    }
            }
            return
            {   Solver::DependencyStatus::FREE, 0,0};
        }

        /**
         * Initialize the result file by defining the result file header for the given FMU.
         * @param fmuName The name of the FMU that should be added to the result file.
         * @param variables A collection of all FMU-variables that should be added to the result file.
         */
        void addFmu(FMI::AbstractFmu * fmu) override
        {
            fmu->setLocalId(_numManagedFmus++);
            size_type numNewCons = _communicator.addFmu(fmu, _valuePacking);

            _lastCommTime.resize(_lastCommTime.size() + numNewCons, -1.0 * std::numeric_limits<real_type>::infinity());
            _lastEventWritten.resize(_lastEventWritten.size() + numNewCons, -1.0 * std::numeric_limits<real_type>::infinity());
            _lastEventRead.resize(_lastEventRead.size() + numNewCons, -1.0 * std::numeric_limits<real_type>::infinity());
            _lastEventReadState.resize(_lastEventReadState.size() + numNewCons, true);
            _lastEventWriteState.resize(_lastEventReadState.size() + numNewCons, true);
            _history.addFmu(fmu, fmu->getConnections());

            if (!_writer.isInitialized())
            {
                _writer.initialize();
                _writer.appendTimeHeader();
            }
            _writer.appendHeader(fmu->getFmuName(), fmu->getValueInfo());
        }

        const AbstractDataHistory* getHistory() const override
        {
            return &_history;
        }

        // dirty, passes interface
        bool sendSingleOutput(real_type curTime, size_type solveOrder, const FMI::AbstractFmu* fmu, const size_type & conId)
        {
            if (_communicator.send(HistoryEntry(curTime, solveOrder, _valuePacking[conId].pack(fmu->getValues(FMI::ReferenceContainerType::ALL)), true), conId))
                _lastCommTime[conId] = curTime;
            else
                return false;
            return true;
        }

     protected:
        /**
         * Saves all state values at a specific time of the FMUs handled by this DataManager,
         * unless there aren't needed anymore.
         * Access via localId of the FMUs.
         */
        HistoryClass _history;

        /**
         * Communicator that handles data exchange (output values, input values) between several data managers.
         */
        shared_ptr<Synchronization::Communicator> _commPtr;
        Communicator & _communicator;

        /**
         * ConnectionMappings are for packing and unpacking of output and input values, accessible via connectionId.
         */
        vector<FMI::InputMapping> _valuePacking;

        /**
         * The last valid FMU values which has been send over a specific connection
         * (example: _lastCommTime[condId] = _currentTime, setting the connection described by conId to the current time)
         * see sendOuputs
         */
        vector<real_type> _lastCommTime;
        vector<real_type> _lastEventWritten;
        vector<real_type> _lastEventRead;
        /**
         * if _lastEventReadState is false, the next incomming input has to be an event
         */
        vector<bool> _lastEventReadState;
        vector<bool> _lastEventWriteState;

     private:
        WriterClass _writer;

        real_type _simStart;
        real_type _stepSize;

        size_type _numManagedFmus;

        std::list<Solver::DependencySolverInfo> _upcomingEvents;

        Solver::DependencySolverInfo collectInputs(real_type curTime, FMI::AbstractFmu* fmu)
        {
            Solver::DependencySolverInfo res = { Solver::DependencyStatus::FREE, std::numeric_limits<real_type>::max(), std::numeric_limits<real_type>::max() };
            for (const size_type conId : _communicator.getInConnectionIds(fmu))  // Checking all connections
            {
                while (_history.getInputHistory(conId).size() == 0 || _history.getInputHistory(conId).getNewestTime() < curTime)  // while till outputs were send by predecessor FMU
                {
                    HistoryEntry dhe = _communicator.recv(conId);
                    if (dhe.isValid())
                    {

                        if (dhe.hasEvent())
                        {
                            if (!_lastEventReadState[conId])
                            {
                                res.eventTimeStart = _lastEventRead[conId];
                                res.eventTimeEnd = dhe.getTime();
                                _history.insertInputs(dhe, conId);
                                _lastEventReadState[conId] = true;
                                if (std::abs(res.eventTimeStart - res.eventTimeEnd) > 1.0e-3)
                                {
                                    HistoryEntry dhe2 = _communicator.recv(conId);
                                    throw std::runtime_error("Somthings wrong");
                                }
                            }
                            else
                            {
                                HistoryEntry dhe2 = _communicator.recv(conId);
                                if (!dhe2.isValid())
                                {
                                    _lastEventReadState[conId] = false;
                                    _lastEventRead[conId] = dhe.getTime();
                                    _history.insertInputs(dhe, conId);
                                    res.depStatus = Solver::DependencyStatus::BLOCKED;
                                    break;
                                }
                                else
                                {
                                    res.eventTimeStart = dhe.getTime();
                                    res.eventTimeEnd = dhe2.getTime();
                                    _history.insertInputs(dhe, conId);
                                    _history.insertInputs(dhe2, conId);
                                    if (std::abs(res.eventTimeStart - res.eventTimeEnd) > 1.0e-3)
                                    {
                                        HistoryEntry dhe2 = _communicator.recv(conId);
                                        throw std::runtime_error("Somthings wrong");
                                    }
                                }
                            }
                            if (std::abs(res.eventTimeStart - res.eventTimeEnd) > 1.0e-3)
                                throw std::runtime_error("Somthings wrong");
                            res.depStatus = Solver::DependencyStatus::EVENT;
                            _lastEventReadState[conId] = true;
                            _lastEventRead[conId] = -std::numeric_limits<real_type>::infinity();
                            return res;
                        }
                        else
                            _history.insertInputs(dhe, conId);
                    }
                    else
                    {
                        res.depStatus = Solver::DependencyStatus::BLOCKED;
                        break;
                    }
                }
                if (res.depStatus == Solver::DependencyStatus::BLOCKED)
                    break;
            }
            return res;
        }

        bool sendOutputs(real_type curTime, size_type solveOrder, FMI::AbstractFmu* fmu, const Solver::SolverStepInfo & stepInfo)
        {
            const FMI::ValueCollection fmuValues = fmu->getValues(FMI::ReferenceContainerType::ALL);
            for (size_type conId : _communicator.getOutConnectionIds(fmu))
            {
                if (_lastCommTime[conId] < curTime)  // check if the time wasn't already written
                {
                    if (stepInfo.hasEventWrite() && _lastEventWritten[conId] < stepInfo.getEventTime<0>())  // check for events and if there weren't already written
                    {
                        // send event in two communications
                        if (!_communicator.send(HistoryEntry(stepInfo.getEventTime<0>(), solveOrder, _valuePacking[conId].pack(stepInfo.getEventValues<0>()), true), conId))
                            return false;
                        _lastEventWriteState[conId] = false;
                        _lastCommTime[conId] = curTime;

                        if (!_communicator.send(HistoryEntry(stepInfo.getEventTime<1>(), solveOrder, _valuePacking[conId].pack(stepInfo.getEventValues<1>()), true), conId))
                            return false;
                        _lastEventWritten[conId] = stepInfo.getEventTime<1>();
                        _lastEventWriteState[conId] = true;
                    }
                    else
                    {
                        if (!_communicator.send(HistoryEntry(curTime, solveOrder, _valuePacking[conId].pack(fmuValues), false), conId))
                            return false;
                        _lastCommTime[conId] = curTime;
                    }
                }
                else
                {
                    if (!_lastEventWriteState[conId])
                    {
                        if (!_communicator.send(HistoryEntry(stepInfo.getEventTime<1>(), solveOrder, _valuePacking[conId].pack(stepInfo.getEventValues<1>()), true), conId))
                            return false;
                        _lastEventWritten[conId] = stepInfo.getEventTime<1>();
                        _lastEventWriteState[conId] = true;
                    }
                    else
                        throw std::runtime_error("DataManager: Already send inputs.\n");

                }
            }
            return true;
        }
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_DATAMANAGER_HPP_ */
/**
 * @}
 */
