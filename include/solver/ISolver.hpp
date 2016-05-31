/*
 * ISolver.hpp
 *
 *  Created on: 16.03.2016
 *      Author: hartung
 */

#ifndef INCLUDE_SOLVER_ISOLVER_HPP_
#define INCLUDE_SOLVER_ISOLVER_HPP_

#include "fmi/AbstractFmu.hpp"

namespace Solver
{
/**
 * Interface for every solver class. Enables high-level view and hides templates of explicit solver classes
 */
    class ISolver
    {
     public:
        virtual ~ISolver()
        { }

        virtual size_type solve(const size_type & numSteps) = 0;
        virtual void initialize() = 0;
        virtual void setEndTime(const real_type & simTime) = 0;
        virtual void setTolerance(const real_type & tolerance) = 0;
        virtual Synchronization::IDataManager* getDataManager() = 0;
        virtual real_type getCurrentTime() const = 0;
        virtual real_type getEndTime() const = 0;

        virtual real_type getCurrentStepSize() const = 0;

        virtual void setStepSize(const real_type & in) = 0;

        virtual real_type getStepSize() const = 0;

        virtual bool_type isFinished() const = 0;

        virtual size_type getId() const = 0;

        virtual const FMI::AbstractFmu* getFmu() const = 0;

        virtual FMI::AbstractFmu* getFmu() = 0;

        virtual real_type getTolerance() const = 0;

        virtual real_type getMaxError() const = 0;

        virtual void setMaxError(const real_type & maxError) = 0;

        virtual size_type getEventCounter() const = 0;

     private:

    };

} /* namespace Solver */

#endif /* INCLUDE_SOLVER_ISOLVER_HPP_ */
