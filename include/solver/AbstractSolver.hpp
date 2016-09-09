/** @addtogroup Solver
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SOLVER_ABSTRACTSOLVER_HPP_
#define INCLUDE_SOLVER_ABSTRACTSOLVER_HPP_

#include <type_traits>
#include <typeinfo>
#include <cfloat>
#include <cmath>

#include "fmi/AbstractFmu.hpp"
#include "synchronization/IDataManager.hpp"
#include "synchronization/AbstractConnection.hpp"
#include "solver/ISolver.hpp"
#include "solver/SolverStepInfo.hpp"
#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "util/Vector.hpp"
#include "ErrorInfo.hpp"
#include "initialization/Plans.hpp"

namespace Solver
{

    /**
     * This is an abstract class which serves as skeleton for time integration solvers. Concrete implementations
     * are currently Euler and Ros2.
     * A solver also stores information about occurred events.
     */
    template<class DataManagerClass, class FmuClass>
    class AbstractSolver : public ISolver
    {
        static_assert(std::is_base_of<Synchronization::IDataManager,DataManagerClass>::value, "AbstractSolver: Template argument FmuClass must be a specialization of IDataManager");
        static_assert(std::is_base_of<FMI::AbstractFmu,FmuClass>::value, "AbstractSolver: Template argument 2 must be a specialization of AbstractFMU");

     protected:
        virtual void doSolverStep(const real_type & h) = 0;

     public:
        virtual ErrorInfo getErrorInfo() const = 0;
        virtual size_type getSolverOrder() const = 0;

        typedef vector<real_type> vector1D;
        typedef vector<vector1D> vector2D;

        AbstractSolver() = delete;

        /**
         * Constructs AbstractSolver from given parameters.
         * @param id ID of the solver.
         * @param fmu
         * @param dataManager
         */
        //AbstractSolver(size_type id, FMI::FmuSPtr fmu, Synchronization::DataManagerSPtr dataManager);
        AbstractSolver(const Initialization::SolverPlan & in, const FmuClass & fmu, std::shared_ptr<DataManagerClass> & dataManager)
                : _numStates(0),
                  _numEvents(0),
                  _currentTime(in.startTime),
                  _prevTime(_currentTime),
                  _curStepSize(in.stepSize),
                  _initStepSize(_curStepSize),
                  _tmpStepSize(_curStepSize),
                  _endTime(in.endTime),
                  _eventCounter(0),
                  _tolerance(in.fmu->relTol),
                  _maxError(in.maxError),
                  _states(),
                  _prevStates(),
                  _stateDerivatives(),
                  _eventIndicators(),
                  _prevEventIndicators(),
                  _tmpEventIndicators(),
                  _freshStep(false),
                  _savedStep(false),
                  _fmu(fmu),
                  _id(in.id),
                  _dataManager(dataManager),
                  _depHist()
        {
            //assert(dataManager != nullptr);
        }

        /**
         * Destroy solver.
         */
        virtual ~AbstractSolver()
        {
            _fmu.unload();
        }

        virtual void initialize()
        {
            if (!_fmu.isLoaded())
                _fmu.load();

            _dataManager->addFmu(&_fmu);
            _sEventInfo = SolverEventInfo();
            _stepInfo = SolverStepInfo();
            _stepInfo.setWriteStep(true);

            _numStates = _fmu.getNumStates();
            _numEvents = _fmu.getNumEventIndicators();

            _states = vector1D(_numStates, 0.0);
            _prevStates = vector1D(_numStates, 0.0);

            _stateDerivatives = vector1D(_numStates, 0.0);

            _eventIndicators = vector1D(_numEvents, 0.0);
            _prevEventIndicators = vector1D(_numEvents, 0.0);
            _tmpEventIndicators = vector1D(_numEvents, 0.0);
            _currentTime = _fmu.getTime();
            _prevTime = _fmu.getTime();

            _tolerance = _fmu.getRelativeTolerance();
            _dependencyInfo.depStatus = DependencyStatus::BLOCKED;
            _fmu.getStates(_states);
            _prevStates = _states;
            _tmpValues = FMI::ValueCollection(_fmu.getValues(FMI::ReferenceContainerType::ALL));
            _fmu.getEventIndicators(_prevEventIndicators);

            if (_tolerance <= 0.0 || _maxError <= 0.0 || _initStepSize <= 0.0)
            {
                throw std::runtime_error("FMU values not set correctly. " + to_string(_tolerance) + "|" + to_string(_maxError) + "|" + to_string(_initStepSize));
            }

        }

        real_type getCurrentTime() const
        {
            return _currentTime;
        }

        real_type getEndTime() const
        {
            //return _fmu.getSimEndTime();
            return _endTime;
        }

        /**
         * Return step size h of the solver.
         * @return
         */
        real_type getCurrentStepSize() const
        {
            return _curStepSize;
        }

        /**
         * Set step size h for the solver.
         * @param in Step size h.
         */
        void setStepSize(const real_type & in)
        {
            _initStepSize = in;
            _curStepSize = in;
            _tmpStepSize = in;
        }

        real_type getStepSize() const
        {
            return _initStepSize;
        }

        bool_type isFinished() const
        {
            //return (_savedSolverStep && _currentTime >= _endTime);
            if (_dependencyInfo.depStatus == DependencyStatus::FREE && _currentTime >= _endTime && _savedStep)
                return true;
            else
                return false;
        }

        size_type getId() const
        {
            return _id;
        }

        const FMI::AbstractFmu* getFmu() const
        {
            return &_fmu;
        }

        FMI::AbstractFmu* getFmu()
        {
            return &_fmu;
        }

        real_type getTolerance() const
        {
            return _tolerance;
        }

        real_type getMaxError() const
        {
            return _maxError;
        }

        void setMaxError(const real_type & maxError) override
        {
            _maxError = maxError;
        }

        /**
         * Time integration algorithm of the solver.
         * @param numSteps Number of steps to perform.
         */
        virtual size_type solve(const size_type & numSteps = 1) override
        {
            size_type count = 0, rCount = 0;

            while (!isFinished() && count++ < numSteps)
            {

                if (!_savedStep)
                {
                    switch ((_dependencyInfo = _dataManager->getDependencyInfo(&_fmu)).depStatus)
                    {
                        case DependencyStatus::FREE:
                            if (_sEventInfo.eventOccured)
                            {
                                doEventStepping();
                            }
                            else
                            {
                                _savedStep = _dataManager->saveSolverStep(&_fmu, _stepInfo, getSolverOrder());
                                if (_savedStep)
                                    _stepInfo.clear();
                            }
                            break;
                        case DependencyStatus::EVENT:
                            _depHist.push_back(_dependencyInfo);
                            _sEventInfo.eventOccured = true;
                            _sEventInfo.eventTimeStart = std::min(_sEventInfo.eventTimeStart, _dependencyInfo.eventTimeStart);  // which event happend earlier?
                            _sEventInfo.eventTimeEnd = std::min(_sEventInfo.eventTimeEnd, _dependencyInfo.eventTimeEnd);  // which event happend earlier?
                            break;

                        case DependencyStatus::BLOCKED:
                            return rCount;
                            break;
                        case DependencyStatus::ABORT_SIM:
                            return std::numeric_limits<size_type>::max();
                            break;
                        default:
                            throw std::runtime_error("AbstractSolver: Unknown dependency");
                    }

                }
                else
                {
                    _curStepSize = _tmpStepSize;
                    _stepInfo.clear();
                    if (_currentTime + _curStepSize >= _dataManager->getNextOutputTime(_currentTime))
                    {
                        _tmpStepSize = _curStepSize;
                        _stepInfo.setWriteStep(true);
                        _curStepSize = std::min(_dataManager->getNextOutputTime(_currentTime), _endTime) - _currentTime;

                    }
                    doSolverStepErrorHandled(std::min(_curStepSize, _endTime - _currentTime));
                    handleEvents();
                    _savedStep = false;
                }
            }
            return count;
        }

        /**
         * Executes doSolverStep as long as a valid step is done. Due to error handling, it can be that
         * a step is invalid. So use doSolverStep only, if it is ensured that the error will be fine.
         * This function sets the member variable _curStepSize according to the error
         * @param real_type h the supposed time step size. The real used time step can be smaller.
         */
        virtual void doSolverStepErrorHandled(const real_type & h)
        {
            double step = h;
            if (_prevTime > _currentTime)
                throw std::runtime_error("Prevtime is gt than current.");
            if (0 > _curStepSize)
                throw std::runtime_error("Step size is negative.");
            _prevTime = _currentTime;
            _prevStates.swap(_states);  // save current states in previous states

            doSolverStep(step);
            while (getErrorInfo().isErrorHappend())
            {
                step = std::min(_initStepSize, getErrorInfo().getStepSize());
                doSolverStep(step);
            }
            _currentTime += step;
            _curStepSize = std::min(_initStepSize, getErrorInfo().getStepSize());
            if (0 > _curStepSize)
                throw std::runtime_error("Step size is negative.");

            _fmu.setTime(_currentTime);
            _fmu.setStates(_states);
            _fmu.stepCompleted();
        }

        virtual void doEventStepping()
        {
            //keep the new state values, because they are the after event start values
            //it's too late at this location to write the pre-event-values
            //_fmu.getStates(eventStateValues.data());
            LOGGER_WRITE("Event stepping: t0=" + to_string(_sEventInfo.eventTimeStart) + " , t1=" + to_string(_sEventInfo.eventTimeEnd), Util::LC_SOLVER, Util::LL_DEBUG);
            _currentTime = _sEventInfo.eventTimeStart;
            doSolverStep(_currentTime - _prevTime);
            _fmu.setTime(_currentTime);
            _fmu.setStates(_states);
            _dataManager->setFmuInputValuesAtT(_currentTime, &_fmu);

            _fmu.getStateDerivatives(_stateDerivatives);
            _stepInfo.setEvent<0>(_currentTime, _fmu.getValues(FMI::ReferenceContainerType::ALL));

            _currentTime = _sEventInfo.eventTimeEnd;
            doSolverStep(_currentTime - _prevTime);
            _fmu.setTime(_currentTime);
            _fmu.setStates(_states);
            _dataManager->setFmuInputValuesAtT(_currentTime, &_fmu);

            FMI::FmuEventInfo eventInfo;
            while (!eventInfo.isIterationConverged())
            {
                eventInfo = _fmu.eventUpdate();
            }

            if (eventInfo.isStateValuesChanged())
                _fmu.getStates(_states);
            // write results for after event
            //_fmu.getStateDerivatives(_stateDerivatives);
            _stepInfo.setEvent<1>(_currentTime, _fmu.getValues(FMI::ReferenceContainerType::ALL));
            _sEventInfo.eventOccured = false;
            ++_eventCounter;
            real_type newTime = std::max(_tolerance, _prevTime + _curStepSize - _currentTime);
            _prevTime = _currentTime;
            _prevStates = _states;
            doSolverStepErrorHandled(newTime);
            _fmu.getEventIndicators(_prevEventIndicators);
        }

        void setEndTime(const real_type & simTime)
        {
            _endTime = simTime;
        }

        const Synchronization::IDataManager* getDataManager() const
        {
            return *_dataManager;
        }

        void setTolerance(const real_type & tolerance)
        {
            _tolerance = tolerance;
            _fmu.setRelativeTolerance(tolerance);
        }

        Synchronization::IDataManager* getDataManager() override
        {
            return _dataManager.get();
        }

        size_type getEventCounter() const override
        {
            return _eventCounter;
        }

     protected:
        /**
         * This struct provides information concerning events during solver steps.
         */
        struct SolverEventInfo
        {
            bool_type eventOccured;
            real_type eventTimeStart;
            real_type eventTimeEnd;
        };

        /******************
         *   Attributes   *
         ******************/
        /// Total number of states of the FMUs which are solved by a particular solver.
        int_type _numStates;
        size_type _numEvents;

        real_type _currentTime;
        real_type _prevTime;
        real_type _curStepSize;
        real_type _initStepSize;
        real_type _tmpStepSize;
        real_type _endTime;

        size_type _eventCounter;

        real_type _tolerance;
        real_type _maxError;

        /// All states of the FMU which is solved by a particular solver.
        vector1D _states;

        /// All state values from last time step.
        vector1D _prevStates;
        /// All state derivatives of the FMU which is solved by a particular solver.
        vector1D _stateDerivatives;

        vector1D _eventIndicators;
        vector1D _prevEventIndicators;
        vector1D _tmpEventIndicators;

        DependencySolverInfo _dependencyInfo;
        //bool _writeResults;
        bool _freshStep;
        bool _savedStep;

        SolverEventInfo _sEventInfo;
        SolverStepInfo _stepInfo;

        FMI::ValueCollection _tmpValues;

        /// Pointer to the FMU which is handled/solved by this solver.
        FmuClass _fmu;

        std::list<DependencySolverInfo> _depHist;

        SolverEventInfo findZeroCrossing(const size_type & eventIndex)
        {
            real_type t = _prevTime, tMin = _prevTime, tMax = _currentTime;
            while (std::abs(tMin - tMax) > _tolerance)
            {
                if (t <= tMin || t >= tMax)
                {
                    t = tMin + 0.5 * (tMax - tMin);
                }

                _states = _prevStates;
                for (size_type i = 0; i < _states.size(); ++i)
                    _states[i] += (t - _prevTime) * _stateDerivatives[i];
                //_currentTime = t;
                _fmu.setTime(t);
                _fmu.setStates(_states);
                _fmu.getEventIndicators(_eventIndicators);

                if (hasZeroCrossing(eventIndex))
                    tMax = t;
                else
                    tMin = t;
            }
            LOGGER_WRITE(string_type("Interpolated event time point_type to ") + to_string(t), Util::LC_EVT, Util::LL_DEBUG);
            if (tMin <= _prevTime)
                tMin = std::nextafter(_prevTime, DBL_MAX);
            if (tMax >= _currentTime)
                tMax = std::nextafter(_currentTime, DBL_MIN);

            return
            {   true,std::max(tMin,t-_tolerance), std::min(tMax,t+_tolerance)};
        }

        /**
         * Linear interpolation of a concrete event time occurred during the last 2 solver steps with step-size Solver::AbstractSolver::getStepSize() .
         * @param eventIndex The event indicator that has changed during the event.
         * @param fmuNum The index of the FMU that has triggered the event.
         * @return The concrete point_type in time of the triggered event.
         */
        real_type interpolateZeroCrossing(const size_type & eventIndex)
        {
            real_type dY = _eventIndicators[eventIndex] - _tmpEventIndicators[eventIndex];
            real_type dX = _curStepSize;
            real_type m = dY / dX;
            real_type n = _tmpEventIndicators[eventIndex] - (m * _prevTime);

            return (-n) / m;
        }

        bool_type hasZeroCrossing(const size_type & eventIndex) const
        {
            return signbit(_eventIndicators[eventIndex]) != signbit(_prevEventIndicators[eventIndex]) && _prevEventIndicators[eventIndex] != 0.0;
        }

        size_type findEarliestEvent()
        {
            real_type h = std::numeric_limits<real_type>::max(), hTmp;
            size_type crosser = std::numeric_limits<size_type>::max();
            _tmpEventIndicators = _eventIndicators;
            _fmu.getEventIndicators(_eventIndicators);
            for (size_type k = 0; k < _eventIndicators.size(); ++k)
            {
                if (hasZeroCrossing(k))
                {
                    LOGGER_WRITE(string_type("(" + to_string(_fmu.getLocalId()) + ") Found event at index '") + to_string(k) + string_type("'"), Util::LC_EVT, Util::LL_DEBUG);
                    hTmp = interpolateZeroCrossing(k);
                    //LOGGER_WRITE(string_type("Interpolated event time point_type to ") + to_string(hTmp), Util::LC_EVT, Util::LL_DEBUG);
                    if (h > hTmp)
                    {
                        h = hTmp;
                        crosser = k;
                    }
                }
            }

            return crosser;
        }

        SolverEventInfo handleEvents()
        {
            size_t eventIndex;
            bool_type eventHappend = false;
            SolverEventInfo event;
            if (_eventIndicators.size() != 0)
            {
                eventIndex = findEarliestEvent();
                if (eventIndex < _eventIndicators.size())
                {
                    event = findZeroCrossing(eventIndex);
                    eventHappend = true;
                }
            }
            if (eventHappend)
                _sEventInfo = event;
            else
                _sEventInfo = {false, std::numeric_limits<real_type>::infinity(), std::numeric_limits<real_type>::infinity()};

            return _sEventInfo;
        }

     private:
        /// Unique identifier of a particular solver.
        size_type _id;

        /// DataManager which cares about data exchange between FMUs and output saving.
        std::shared_ptr<DataManagerClass> _dataManager;
    };

}
/* namespace Solver */

#endif /* INCLUDE_SOLVER_ABSTRACTSOLVER_HPP_ */
/**
 * @}
 */
