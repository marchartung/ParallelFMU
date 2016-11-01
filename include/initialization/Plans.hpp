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

    /*! \brief A SchedulePlan holds the mapping of solvers to the physical cores.
     *
     * == nodeStructure ==
     * For a given node i this mapping holds the IDs of the solvers which runs on core j.
     *          C O R E S
     *       0   1   2   3
     *     -------------------->
     * N 0 | S1  S2  S3
     * O 1 | S4  S5  S6  S7
     * D 2 | S8
     * E 3 | S9  (S10,S11)
     * S   |
     *     V
     * For example, the solvers with ID 10 and 11 run on core 1 of node 3, i.e, nodeStructure[3][1] = {10,11}.
     *
     * == solverIdToCore ==
     * This structure holds for all solvers on which node and core a particular solver runs.
     * For example, solverIdToCore[0] = (1, 3) means, that solver with ID 0 runs on core 3 of node 1.
     */
    struct SchedulePlan
    {
        vector<vector<vector<size_type>>> nodeStructure;     ///< node -> core -> solverIDs
        vector<tuple<size_type, size_type>> solverIdToCore;  ///< solverID -> (node,core)
    };

    /*! \brief A SimulationPlan holds the settings for a particular simulation.
     *
     */
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

    /*! \brief A ProgramPlan holds the \ref SimulationPlans for all simulations.
     *
     * == simPlans ==
     * This structure represents the mapping of the simulations to the nodes and threads, i.e., simPlans[nodeNum][threadNum]
     * holds the \ref SimulationPlan for the simulation which runs on node nodeNum and thread threadNum.
     *
     */
    struct ProgramPlan
    {
        //vec2D represents node/thread mapping. simPlans[nodeNum][threadNum]
        vector<vector<SimulationPlan>> simPlans; //for mpi
    };

} // namespace Initialization

#endif /* INCLUDE_INITIALIZATION_PLANS_HPP_ */
