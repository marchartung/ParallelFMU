/*
 * Plans.hpp
 *
 *  Created on: 08.03.2016
 *      Author: hartung
 */

#ifndef INCLUDE_INITIALIZATION_PLANS_HPP_
#define INCLUDE_INITIALIZATION_PLANS_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "fmi/InputMapping.hpp"

#ifdef USE_NETWORK_OFFLOADER
#include "SimulationServer.hpp"
#endif

namespace Initialization
{
    struct FmuPlan
    {
        std::string name;
        std::string path;
        std::string workingPath;
        std::string version;

        size_type id;

        std::string loader;
        bool intermediateResults;
        bool tolControlled;
        real_type relTol;

        bool logEnabled;

        int solverId;
    };

    struct ConnectionPlan
    {
        std::string kind;
        size_type bufferSize;

        size_type sourceRank;
        size_type destRank;

        std::string sourceFmu;
        std::string destFmu;

        size_type startTag;

        FMI::InputMapping inputMapping;

        ConnectionPlan swapSourceAndDest() const
        {
            ConnectionPlan res;
            std::swap(destFmu,sourceFmu);
            std::swap(sourceRank,destRank);
            return res;
        }
    };


    struct SolverPlan
    {
        std::string kind;
        size_type id;

        shared_ptr<FmuPlan> fmu;

        real_type startTime;
        real_type endTime;
        real_type stepSize;

        real_type maxError;
        real_type eventInterval;

        std::list<shared_ptr<ConnectionPlan>> outConnections;
        std::list<shared_ptr<ConnectionPlan>> inConnections;

#ifdef USE_NETWORK_OFFLOADER
        Network::NetworkPlan networkPlan;
#endif
    };

    struct WriterPlan
    {
        std::string kind;
        real_type startTime;
        real_type endTime;
        size_type numSteps;
        std::string filePath;
    };

    struct HistoryPlan
    {
        std::string kind;
    };

    struct DataManagerPlan
    {
        WriterPlan writer;
        HistoryPlan history;
        vector<vector<shared_ptr<SolverPlan> > > solvers;
        std::list<shared_ptr<ConnectionPlan>> outConnections;
        std::list<shared_ptr<ConnectionPlan>> inConnections;
    };



    struct SchedulePlan
    {
        std::vector<std::vector<std::vector<size_type> > > nodeStructure; // node -> core -> solv

        std::vector<tuple<size_type, size_type>> solvIdToCore; // solv -> (node,core)
    };

    struct SimulationPlan
    {
        std::string kind;
        real_type startTime;
        real_type endTime;

        real_type defaultStepSize;
        real_type defaultEventInterval;
        real_type defaultMaxError;
        real_type defaultTolerance;


        DataManagerPlan dataManager;
    };

    struct ProgramPlan
    {
        vector<SimulationPlan> simPlans; //for mpi
    };



} /* namespace Synchronization */

#endif /* INCLUDE_INITIALIZATION_PLANS_HPP_ */
