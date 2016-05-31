/*
 * IDataManager.hpp
 *
 *  Created on: 01.04.2016
 *      Author: hartung
 */

#ifndef INCLUDE_SYNCHRONIZATION_IDATAMANAGER_HPP_
#define INCLUDE_SYNCHRONIZATION_IDATAMANAGER_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "initialization/Plans.hpp"
#include "fmi/AbstractFmu.hpp"
#include "solver/SolverStepInfo.hpp"
#include "synchronization/AbstractDataHistory.hpp"

namespace Synchronization
{

    class IDataManager
    {
     public:
        IDataManager(const Initialization::DataManagerPlan & in)
        {
        }

        virtual ~IDataManager()
        {
        }

        virtual bool saveSolverStep(FMI::AbstractFmu* fmu, const Solver::SolverStepInfo & stepInfo, const size_type & solveOrder) = 0;

        virtual Solver::DependencySolverInfo getDependencyInfo(FMI::AbstractFmu* fmu) = 0;

        virtual void setFmuInputValuesAtT(const real_type & t, FMI::AbstractFmu* fmu) = 0;
        virtual real_type getNextOutputTime(const real_type & currentFmuTime) const = 0;
        virtual void addFmu(FMI::AbstractFmu * fmu) = 0;
        virtual const AbstractDataHistory* getHistory() const = 0;

    };
}

#endif /* INCLUDE_SYNCHRONIZATION_IDATAMANAGER_HPP_ */
