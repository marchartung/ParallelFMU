/*
 * DefaultValues.cpp
 *
 *  Created on: 24.03.2016
 *      Author: hartung
 */

#include "initialization/DefaultValues.hpp"

namespace Initialization
{

    template<>
    real_type DefaultValues::getUndefinedValue<real_type>()
    {
        return std::numeric_limits<double>::infinity();
    }

    template<>
    size_type DefaultValues::getUndefinedValue<size_type>()
    {
        return std::numeric_limits<size_type>::max();
    }

    template<>
    int DefaultValues::getUndefinedValue<int>()
    {
        return std::numeric_limits<int>::max();
    }

    template<>
    string_type DefaultValues::getUndefinedValue<string_type>()
    {
        return string_type("");
    }

    template<>
    bool_type DefaultValues::getUndefinedValue()
    {
        static_assert(std::is_same<bool_type, char>(), "DefaultValues: Boolean values in FMUs are commonly chars. Please change code here.");
        return static_cast<bool_type>(255);
    }

    template<>
    FMI::InputMapping DefaultValues::getUndefinedValue()
    {
        return FMI::InputMapping();
    }

    template<>
    bool DefaultValues::isUndefinedValue(const FMI::InputMapping & in)
    {
        return in.size() == 0ul;
    }

    Initialization::FmuPlan DefaultValues::fmuPlan()
    {
        Initialization::FmuPlan res;
        res.id = getUndefinedValue<size_type>();  //decltype(res.id)
        res.intermediateResults = true;
        res.loader = "fmuSdk";
        res.logEnabled = false;
        res.name = getUndefinedValue<decltype(res.name)>();
        res.path = getUndefinedValue<decltype(res.path)>();
        res.relTol = 1.0e-6;
        res.solverId = getUndefinedValue<decltype(res.solverId)>();
        res.tolControlled = true;
        res.version = "1.0";
        res.workingPath = "./";

        //static_assert( sizeof(res.id) + sizeof(res.intermediateResults) + sizeof(res.loader) + sizeof(res.logEnabled) + sizeof(res) + sizeof(res.name) + sizeof(res.path) + sizeof(res.relTol)    + sizeof(res.solverId) + + sizeof(res.tolControlled) + sizeof(res.version) + + sizeof(res.workingPath)  == sizeof(res), "DefaultValues: Byte count mismatch. Maybe you haven't added a default value for FmuPlan in class DefaultValues.");
        return res;
    }

    Initialization::SolverPlan DefaultValues::solverPlan()
    {
        Initialization::SolverPlan res;
        res.endTime = -std::numeric_limits<real_type>::infinity();
        res.eventInterval = 2.0e-6;
        res.fmu = nullptr;
        res.id = getUndefinedValue<decltype(res.id)>();
        res.kind = "euler";
        res.maxError = 1.0e-5;
        res.startTime = 0.0;
        res.stepSize = -1.0e-3;

        //static_assert( sizeof(res.connections) + sizeof(res.endTime) + sizeof(res.eventInterval) + sizeof(res.fmu) + sizeof(res.id) + sizeof(res.kind) + sizeof(res.maxError) + sizeof(res.startTime) + sizeof(res.stepSize) == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for SolverPlan in class DefaultValues.");

        return res;
    }

    Initialization::HistoryPlan DefaultValues::historyPlan()
    {
        Initialization::HistoryPlan res;
        res.kind = "serial";
        //static_assert(sizeof(res.kind) + sizeof(res.size) == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for HistoryPlan in class DefaultValues.");
        return res;
    }

    Initialization::ConnectionPlan DefaultValues::connectionPlan()
    {
        Initialization::ConnectionPlan res;
        res.bufferSize = 100;
        res.destFmu = getUndefinedValue<decltype(res.destFmu)>();
        res.destRank = 0;
        res.inputMapping = getUndefinedValue<decltype(res.inputMapping)>();
        res.kind = "serial";
        res.sourceFmu = getUndefinedValue<decltype(res.sourceFmu)>();
        res.sourceRank = 0;
        res.startTag = getUndefinedValue<decltype(res.startTag)>();
        //static_assert(sizeof(res.bufferSize) + sizeof(res.destFmu) + sizeof(res.destRank) + sizeof(res.inputMapping) + sizeof(res.kind) + sizeof(res.sourceFmu) + sizeof(res.sourceRank) + sizeof(res.startTag) == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for ConnectionPlan in class DefaultValues.");
        return res;
    }

    Initialization::DataManagerPlan DefaultValues::dataManagerPlan()
    {
        Initialization::DataManagerPlan res;

        //static_assert(sizeof(res.history) + sizeof(res.writer) == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for DataManagerPlan in class DefaultValues.");
        return res;
    }

    Initialization::ProgramPlan DefaultValues::programPlan()
    {
        Initialization::ProgramPlan res;
        //static_assert(sizeof(res.simPlans)  == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for ProgramPlan in class DefaultValues.");
        return res;
    }

    Initialization::SchedulePlan DefaultValues::schedulePlan()
    {
        Initialization::SchedulePlan res;
        //static_assert(sizeof(res.numNodes) + sizeof(res.solverIdToNodeCoreNum) == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for SchedulePlan in class DefaultValues.");
        return res;
    }

    Initialization::SimulationPlan DefaultValues::simulationPlan()
    {
        Initialization::SimulationPlan res;
        res.defaultEventInterval = solverPlan().eventInterval;
        res.defaultMaxError = solverPlan().maxError;
        res.defaultStepSize = 1.0e-3;
        res.defaultTolerance = fmuPlan().relTol;
        res.kind = "serial";
        res.startTime = solverPlan().startTime;
        res.endTime = solverPlan().endTime;
        /*static_assert(sizeof(res.defaultEventInterval) + sizeof(res.defaultMaxError)
         + sizeof(res.defaultStepSize) + sizeof(res.defaultTolerance)
         + sizeof(res.dataManager) + sizeof(res.endTime) + sizeof(res.kind)
         + sizeof(res.solvers) + sizeof(res.startTime)
         == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for SimulationPlan in class DefaultValues.");*/
        return res;
    }

    Initialization::WriterPlan DefaultValues::writerPlan()
    {
        Initialization::WriterPlan res;
        res.filePath = getUndefinedValue<decltype(res.filePath)>();
        res.kind = "csvFileWriter";
        res.numSteps = 100;
        //static_assert(sizeof(res.filePath) + sizeof(res.kind) + sizeof(res.numSteps) == sizeof(res),"DefaultValues: Byte count mismatch. Maybe you haven't added a default value for WriterPlan in class DefaultValues.");
        return res;
    }

} // Namespace Initialization
