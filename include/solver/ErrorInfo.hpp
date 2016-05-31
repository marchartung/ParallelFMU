/** @addtogroup Solver
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SOLVER_ERRORINFO_HPP_
#define INCLUDE_SOLVER_ERRORINFO_HPP_

#include "Stdafx.hpp"

namespace Solver
{
    /**
     * This class provides informations concerning the error of the solver step.
     */
    class ErrorInfo
    {
     public:

        /**
         * Default constructor. Attributes are set to default values.
         * \remark Make use of constructor delegation.
         */
        ErrorInfo()
                : ErrorInfo(0.0, 0.0)
        {
        }

        /**
         * Constructor with arguments.
         * @param stepSize Step size of the solver.
         * @param error
         */
        ErrorInfo(real_type stepSize, real_type error = 0.0)
                : _error(error),
                  _stepSize(stepSize),
                  _errorHappend(error != 0.0)
        {
        }

        /// Destroys ErrorInfo object.
        ~ErrorInfo()
        {
        }

        /// Return error value.
        real_type getError() const
        {
            return _error;
        }

        /// Return step size.
        real_type getStepSize() const
        {
            return _stepSize;
        }

        /// Return if a error happened.
        bool_type isErrorHappend() const
        {
            return _errorHappend;
        }

     private:
        /// The error value.
        real_type _error;
        /// The (current) step size of the solver.
        real_type _stepSize;
        bool_type _errorHappend;
    };

} /* namespace Solver */

#endif /* INCLUDE_SOLVER_ERRORINFO_HPP_ */
/**
 * @}
 */
