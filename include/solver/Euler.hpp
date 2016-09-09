/** @addtogroup Solver
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SOLVER_EULER_HPP_
#define INCLUDE_SOLVER_EULER_HPP_

#include "solver/AbstractSolver.hpp"

namespace Solver
{

    /**
     * This class implements the forward Euler solver for time integration.
     */
    template<class DataManagerClass, class FmuClass>
    class Euler : public AbstractSolver<DataManagerClass, FmuClass>
    {
     public:
        typedef typename AbstractSolver<DataManagerClass, FmuClass>::vector1D vector1D;

        /**
         * Creates Euler solver for a particular FMU.
         * @param fmu Pointer to the FMU that should be solved.
         * @param dataManager Data manager that is able to store values and communicate.
         */
        //Euler(size_type id, FMI::FmuSPtr fmu, Synchronization::DataManagerSPtr dataManager);
        Euler(const Initialization::SolverPlan & in, const FmuClass & fmu, std::shared_ptr<DataManagerClass> & dm)
                : AbstractSolver<DataManagerClass, FmuClass>(in, fmu, dm)
        {
        }

        /**
         * Destroy Euler solver and free resources.
         */
        virtual ~Euler()
        {
        }

        /**
         * Time integration algorithm of forward Euler solver.
         * @param numSteps Number of steps to perform.
         */
        virtual void doSolverStep(const real_type & h)
        {
            // INTEGRATION STEP START
            //LOGGER_WRITE(to_string(_currentTime) + ((_states.size()>0) ? (" [0]: " + to_string(_states[0])) : "") + ((_states.size()>1) ? (" [1]: " + to_string(_states[1])) : ""), Util::LC_SOLVER, Util::LL_DEBUG);
            _fmu.getStateDerivatives(_stateDerivatives);
            for (unsigned i = 0; i < _states.size(); ++i)
                _states[i] = _prevStates[i] + (h * _stateDerivatives[i]);
        }

        /// Return error info.
        virtual ErrorInfo getErrorInfo() const
        {
            return ErrorInfo(_curStepSize);
        }

        /// Return order of the solver. The forward Euler solver has order 1.
        virtual size_type getSolverOrder() const
        {
            return 1;
        }

     protected:
        using AbstractSolver<DataManagerClass, FmuClass>::_numStates;
        using AbstractSolver<DataManagerClass, FmuClass>::_fmu;
        using AbstractSolver<DataManagerClass, FmuClass>::_states;
        using AbstractSolver<DataManagerClass, FmuClass>::_prevStates;
        using AbstractSolver<DataManagerClass, FmuClass>::_stateDerivatives;
        using AbstractSolver<DataManagerClass, FmuClass>::_currentTime;
        using AbstractSolver<DataManagerClass, FmuClass>::_curStepSize;
        using AbstractSolver<DataManagerClass, FmuClass>::_maxError;
    };

} /* namespace Solver */

#endif /* INCLUDE_SOLVER_EULER_HPP_ */
/**
 * @}
 */
