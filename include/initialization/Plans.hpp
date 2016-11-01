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
#include "NetworkPlan.hpp"
#endif

namespace Initialization
{

    struct FmuPlan
    {
        string name;
        string path;
        string workingPath;
        string version;

        size_type id;

        string loader;
        bool intermediateResults;
        bool tolControlled;
        real_type relTol;

        bool logEnabled;

        int solverId;
    };

    struct ConnectionPlan
    {
        string kind;
        size_type bufferSize;

        size_type sourceRank;
        size_type destRank;

        string sourceFmu;
        string destFmu;

        size_type startTag;

        FMI::InputMapping inputMapping;

        ConnectionPlan swapSourceAndDest() const
        {
            ConnectionPlan res;
            std::swap(res.destFmu,res.sourceFmu);
            std::swap(res.sourceRank,res.destRank);
            return res;
        }
    };

    struct SolverPlan
    {
        string kind;
        size_type id;

        shared_ptr<FmuPlan> fmu;

        real_type startTime;
        real_type endTime;
        real_type stepSize;

        real_type maxError;
        real_type eventInterval;

        list<shared_ptr<ConnectionPlan>> outConnections;
        list<shared_ptr<ConnectionPlan>> inConnections;

#ifdef USE_NETWORK_OFFLOADER
        Network::NetworkPlan networkPlan;
#endif
    };

    struct WriterPlan
    {
        string kind;
        real_type startTime;
        real_type endTime;
        size_type numSteps;
        string filePath;
    };

    struct HistoryPlan
    {
        string kind;
    };

    struct DataManagerPlan
    {
        WriterPlan writer;
        HistoryPlan history;
        shared_ptr<Synchronization::Communicator> commnicator;
        vector<shared_ptr<SolverPlan> > solvers;
        list<shared_ptr<ConnectionPlan>> outConnections;
        list<shared_ptr<ConnectionPlan>> inConnections;
    };

    struct SchedulePlan
    {
        vector<vector<vector<size_type> > > nodeStructure; // node -> core -> solv
        vector<tuple<size_type, size_type>> solvIdToCore; // solv -> (node,core)
    };

    struct SimulationPlan
    {
        string kind;
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
        //vec2D represents node/thread mapping. simPlans[nodeNum][threadNum]
        vector<vector<SimulationPlan>> simPlans; //for mpi
    };

} // namespace Initialization

#endif /* INCLUDE_INITIALIZATION_PLANS_HPP_ */
