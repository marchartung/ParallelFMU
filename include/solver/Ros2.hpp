/** @addtogroup Solver
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SOLVER_ROS2_HPP_
#define INCLUDE_SOLVER_ROS2_HPP_

#include "solver/AbstractSolver.hpp"
#include "util/Matrix.hpp"

namespace Solver
{
    /**
     * This class implements a 2nd order Rosenbrock solver.
     * \remark: For references, see http://oai.cwi.nl/oai/asset/1252/1252A.pdf
     */

    extern "C" int_type dgetrs_(char *trans, int_type *n, int_type *nrhs, double *a, int_type *lda, int_type *ipiv, double *b, int_type * ldb, int_type *info);
    extern "C" int_type dgetrf_(int_type *m, int_type *n, double *a, int_type * lda, int_type *ipiv, int_type *info);

    template<class DataManagerClass, class FmuClass>
    class Ros2 : public AbstractSolver<DataManagerClass, FmuClass>
    {
     public:
        typedef typename AbstractSolver<DataManagerClass, FmuClass>::vector1D vector1D;
        typedef vector<double *> matrix;

        /**
         * Constructor with arguments.
         * @param id Unique ID to identify the solver object.
         * @param fmu Pointer to the FMU that should be handled.
         * @param dataManager Pointer to the DataManager which handles the synchronization with other FMUs.
         */
        //Ros2(size_type id, FMI::FmuSPtr fmu, Synchronization::DataManagerSPtr dataManager);
        Ros2(const Initialization::SolverPlan & in, const FmuClass & fmu, std::shared_ptr<DataManagerClass> & dm)
                : AbstractSolver<DataManagerClass, FmuClass>(in, fmu, dm),
                  _lapackTrans('N'),
                  _info(1),
                  _dimRHS(1),
                  _diffQuotiant(1.0e-8),
                  _gamma(0.5)
        {
        }
        /**
         * Destructor. Frees allocates resources.
         */
        virtual ~Ros2()
        {
        }

        virtual void doSolverStep(const real_type & h)
        {
            //LOGGER_WRITE("in step size: " + to_string(h) + " time: " + to_string(_currentTime), Util::LC_SOLVER,Util::LL_DEBUG);
            _fmu.setStates(_prevStates);
            calcJacobi(-_gamma * h);
            calcDFDT(_gamma * h);

            dgetrf_(&_numStates, &_numStates, (real_type *) _jacobi[0], &_numStates, _pivot.data(), &_info);

            _fmu.getStateDerivatives(_stateDerivatives);
            for (size_type i = 0; i < _stateDerivatives.size(); ++i)
                _stateDerivatives[i] += _dfdt[i];

            dgetrs_(&_lapackTrans, &_numStates, &_dimRHS, (real_type*) _jacobi[0], &_numStates, _pivot.data(), _stateDerivatives.data(), &_numStates, &_info);  //3
            for (size_type i = 0; i < _stateDerivatives.size(); ++i)
                _states[i] += h * _stateDerivatives[i];

            _fmu.setTime(_currentTime + h);
            _fmu.setStates(_states);
            _fmu.getStateDerivatives(_stateDerivatives2);

            for (size_type i = 0; i < _stateDerivatives.size(); ++i)
                _stateDerivatives2[i] += _dfdt[i] - 2.0 * _stateDerivatives[i];

            dgetrs_(&_lapackTrans, &_numStates, &_dimRHS, (real_type *) _jacobi[0], &_numStates, _pivot.data(), _stateDerivatives2.data(), &_numStates, &_info);

            _fmu.setStates(_prevStates);
            double error = 0, tmpError;
            for (size_type i = 0; i < _stateDerivatives.size(); ++i)
            {
                _states[i] += h * std::abs(0.5 * _stateDerivatives[i] + 0.5 * _stateDerivatives2[i]);
                tmpError = (_stateDerivatives[i] + _stateDerivatives2[i]);
                tmpError /= (_prevStates[i] - std::max(0.0, h * (1.5 * _stateDerivatives[i] + 0.5 * _stateDerivatives2[i])) + 1.0) * _maxError;
                if (tmpError > error)
                    error = tmpError;
            }

            error *= 0.5 * h;

            if (error > _maxError)
            {
                double newS = 0.95 * sqrt(_maxError / (error + _maxError / 100)) * h;
                std::cout << "Step from " << h << " to " << newS << std::endl;
                _errorInfo = ErrorInfo(newS, error);
            }
            else
            {
                if (0.95 * sqrt(_maxError / (error + _maxError / 100)) > 2.0)
                    _errorInfo = ErrorInfo(2 * h);
                else
                    _errorInfo = ErrorInfo(h);
            }
            // ERROR HANDLING END

        }

        /// Return error info.
        virtual ErrorInfo getErrorInfo() const
        {
            return _errorInfo;
        }

        /// Return order of the solver. The 2nd order Rosenbrock solver has order 2.
        virtual size_type getSolverOrder() const
        {
            return 2;
        }

     protected:

        using AbstractSolver<DataManagerClass, FmuClass>::_numStates;
        using AbstractSolver<DataManagerClass, FmuClass>::_fmu;
        using AbstractSolver<DataManagerClass, FmuClass>::_states;
        using AbstractSolver<DataManagerClass, FmuClass>::_prevStates;
        using AbstractSolver<DataManagerClass, FmuClass>::_stateDerivatives;
        using AbstractSolver<DataManagerClass, FmuClass>::_currentTime;
        using AbstractSolver<DataManagerClass, FmuClass>::_maxError;

        /// Initializes the solver.
        virtual void initialize()
        {
            AbstractSolver<DataManagerClass, FmuClass>::initialize();
            _jacobi = vector<double *>(_numStates, nullptr);
            _jacobiSpace = std::shared_ptr<real_type>(new real_type[_numStates * _numStates]);
            for (size_type i = 0; i < _jacobi.size(); ++i)
                _jacobi[i] = &_jacobiSpace.get()[i * _jacobi.size()];
            _dfdt = vector1D(_numStates, 0.0);

            _stateDerivatives2 = vector1D(_numStates);

            _pivot = Util::vector<int_type>(_numStates);

            _diffQuotiant = 1.0e-8;
            _gamma = 1.0 - sqrt(2.0) / 2.0;
        }

        /// Compute Jacobian matrix for the given step size.
        void calcJacobi(const real_type & h)
        {
            real_type delta;
            throw std::runtime_error("Ros2 isn't implemented.");
            _fmu.getStates(_states);
            _tmpStates = _states;
            _fmu.getStateDerivatives(_stateDerivatives);

            for (size_type i = 0; i < _states.size(); ++i)
            {
                delta = _diffQuotiant * _tmpStates[i] + _diffQuotiant;
                _tmpStates[i] += delta;
                _fmu.setStates(_tmpStates);
                _fmu.getStateDerivatives(_jacobi[i]);
                for (size_type j = 0; j < _stateDerivatives.size(); ++j)
                {
                    _jacobi[i][j] -= _stateDerivatives[j];
                    _jacobi[i][j] *= h;
                    _jacobi[i][j] /= delta;
                }
                _jacobi[i][i] += 1.0;
                _tmpStates[i] -= delta;
            }
        }

        void calcDFDT(const real_type & h)
        {
            _fmu.setTime(_currentTime);
            _fmu.setStates(_states);
            _fmu.getStateDerivatives(_stateDerivatives);

            _fmu.setTime(_currentTime + _diffQuotiant);
            _fmu.setStates(_states);
            _fmu.getStateDerivatives(_tmpStates);
            for (size_type i = 0; i < _dfdt.size(); ++i)
            {
                _dfdt[i] = _tmpStates[i] - _stateDerivatives[i];
                _dfdt[i] *= h;
                _dfdt[i] /= _diffQuotiant;
            }
        }

        /******************
         *   Attributes   *
         ******************/
        char _lapackTrans;
        int_type _info;
        int_type _dimRHS;

        /// Jacobian matrix for a particular step size.
        matrix _jacobi;
        std::shared_ptr<real_type> _jacobiSpace;
        vector1D _dfdt;

        vector1D _stateDerivatives2;

        vector1D _tmpStates;

        Util::vector<int_type> _pivot;

        real_type _diffQuotiant;
        real_type _gamma;

        ErrorInfo _errorInfo;
    };

} /* namespace Solver */

#endif /* INCLUDE_SOLVER_ROS2_HPP_ */
/**
 * @}
 */
