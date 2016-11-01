#include "initialization/XMLConfigurationReader.hpp"
#include "synchronization/Communicator.hpp"

#include <boost/optional/optional.hpp>

namespace Initialization
{

    XMLConfigurationReader::XMLConfigurationReader(string_type configurationFilePath)
            : IConfigurationReader(),
              _configurationFilePath(configurationFilePath)
    {
        read_xml(configurationFilePath, _propertyTree);
    }

    FmuPlan XMLConfigurationReader::getFmuPlan(const ptree::value_type & fmuElem, const SimulationPlan & simPlan,
                                               const size_type id)
    {
        FmuPlan res = DefaultValues::fmuPlan();
        res.path = fmuElem.second.get<string_type>("<xmlattr>.path", res.path);
        res.loader = fmuElem.second.get<string_type>("<xmlattr>.loader", res.loader);
        res.relTol = fmuElem.second.get<real_type>("<xmlattr>.relativeTolerance", simPlan.defaultTolerance);
        res.name = fmuElem.second.get<string_type>("<xmlattr>.name", res.name);
        if (id != std::numeric_limits<size_type>::max())
        {
            res.name += to_string(id);
        }
        res.tolControlled = fmuElem.second.get<bool>("<xmlattr>.toleranceControlled", res.tolControlled);
        res.intermediateResults = fmuElem.second.get<bool>("<xmlattr>.intermediateResults", res.intermediateResults);
        res.version = fmuElem.second.get<string_type>("<xmlattr>.version", res.version);
        res.logEnabled = fmuElem.second.get<bool>("<xmlattr>.loggingEnabled", res.logEnabled);
        res.workingPath = fmuElem.second.get<string_type>("<xmlattr>.workingPath", res.workingPath);

        checkForUndefinedValues(res.path, res.loader, res.name, res.version, res.workingPath);

        if (res.version != "1.0")
        {
            throw runtime_error("XMLConfigurationReader: FMUs with a version higher than 1.0 aren't supported yet.");
        }

        return res;
    }

    SolverPlan XMLConfigurationReader::getSolverPlan(const ptree::value_type &firstElem, const SimulationPlan & simPlan,
                                                     size_type id)
    {
        SolverPlan res = DefaultValues::solverPlan();
        res.id = id;
        res.kind = firstElem.second.get<string_type>("<xmlattr>.solver", res.kind);
        res.maxError = firstElem.second.get<real_type>("<xmlattr>.maxError", simPlan.defaultMaxError);
        res.startTime = firstElem.second.get<real_type>("<xmlattr>.startTime", simPlan.startTime);
        res.endTime = firstElem.second.get<real_type>("<xmlattr>.endTime", simPlan.endTime);
        res.stepSize = firstElem.second.get<real_type>("<xmlattr>.defaultStepSize", simPlan.defaultStepSize);
        res.eventInterval = firstElem.second.get<real_type>("<xmlattr>.eventInterval", simPlan.defaultEventInterval);
        checkForUndefinedValues(res.kind, res.id, res.maxError, res.startTime, res.endTime, res.stepSize,
                                res.eventInterval);
        return res;
    }

    list<SolverPlan> XMLConfigurationReader::getSolverPlanFromMultipleFmu(const ptree::value_type &firstElem,
                                                                          const size_type & startId,
                                                                          const SimulationPlan & simPlan)
    {
        list<SolverPlan> res;
        size_type num = firstElem.second.get<size_type>("<xmlattr>.num", 0);
        if (num > 0)
        {
            for (size_type i = 0; i < num; ++i)
            {
                res.push_back(getSolverPlanFromFmu(firstElem, startId + i, simPlan, false).back());
                // TODO(mf): range for init values isn't set
            }
        }
        return res;
    }

    list<SolverPlan> XMLConfigurationReader::getSolverPlanFromFmu(const ptree::value_type &firstElem,
                                                                  const size_type & id, const SimulationPlan & simPlan,
                                                                  const bool & single)
    {
        SolverPlan solv = getSolverPlan(firstElem, simPlan, id);
        solv.fmu = make_shared<FmuPlan>(
                getFmuPlan(firstElem, simPlan, (single) ? std::numeric_limits<size_type>::max() : id));
        solv.fmu->id = firstElem.second.get<size_type>("<xmlattr>.id", DefaultValues::getUndefinedValue<size_type>());
        if (DefaultValues::isUndefinedValue(solv.fmu->id))
        {
            solv.fmu->id = id;
        }
        return list<SolverPlan>(1, solv);
    }

    list<SolverPlan> XMLConfigurationReader::getSolverPlans(const SimulationPlan & simPlan)
    {
        list<SolverPlan> res;

        ptree solverElem = _propertyTree.get_child("configuration.fmus");

        size_type idCount = 0;
        list<SolverPlan> tmp;
        for (ptree::value_type & firstElem : solverElem.get_child(""))
        {
            if (firstElem.first == string("fmu"))
            {
                tmp = getSolverPlanFromFmu(firstElem, idCount, simPlan, true);
            }
            else if (firstElem.first == string("multipleFmu"))
            {
                tmp = getSolverPlanFromMultipleFmu(firstElem, idCount, simPlan);
            }
            else
            {
                throw runtime_error("XMLConfigurationReader: Unknown fmu type.");
            }

            idCount += tmp.size();
            res.insert(res.end(), tmp.begin(), tmp.end());
        }
        return res;
    }

    ConnectionPlan XMLConfigurationReader::getConnectionPlan(ptree::value_type & mapElem)
    {
        ConnectionPlan res = DefaultValues::connectionPlan();
        string_type varType;
        tuple<size_type, size_type> con;

        if (mapElem.first != "connection")
        {
            LOGGER_WRITE(string("XMLConfigurationReader: Unknown connection type \"") + mapElem.first + string("\"."),
                         Util::LC_LOADER, Util::LL_WARNING);
            return res;
        }

        res.bufferSize = mapElem.second.get<size_type>("<xmlattr>.bufferSize", res.bufferSize);
        res.destFmu = mapElem.second.get<string_type>("<xmlattr>.dest", res.destFmu);
        res.sourceFmu = mapElem.second.get<string_type>("<xmlattr>.source", res.sourceFmu);

        for (ptree::value_type &varMapElem : mapElem.second.get_child(""))
        {
            if (varMapElem.first == "<xmlattr>")
            {
                continue;
            }

            varType = varMapElem.first;
            get<0>(con) = varMapElem.second.get<size_type>("<xmlattr>.out");
            get<1>(con) = varMapElem.second.get<size_type>("<xmlattr>.in");
            if (varType == "real")
            {
                res.inputMapping.push_back<double>(con);
            }
            else if (varType == "int")
            {
                res.inputMapping.push_back<int_type>(con);
            }
            else if (varType == "bool_type")
            {
                res.inputMapping.push_back<bool_type>(con);
            }
            else if (varType == "string")
            {
                res.inputMapping.push_back<string_type>(con);
            }
            else
            {
                throw runtime_error("XMLConfigurationReader: Unknown variable type " + varType);
            }
        }
        checkForUndefinedValues(res.bufferSize, res.destFmu, res.sourceFmu, res.inputMapping);
        return res;
    }

    list<ConnectionPlan> XMLConfigurationReader::getConnectionPlans()
    {
        list<ConnectionPlan> res;

        if (nullptr != _propertyTree.get_child_optional("configuration.connections"))
        {
            auto connElems = _propertyTree.get_child("configuration.connections");
            for (ptree::value_type & mapElem : connElems)
            {
                res.push_back(getConnectionPlan(mapElem));
            }
        }
        return res;
    }

    SimulationPlan XMLConfigurationReader::getDefaultSimulationPlan()
    {
        SimulationPlan res = DefaultValues::simulationPlan();
        ptree simElem = _propertyTree.get_child("configuration.simulation");

        res.defaultEventInterval = simElem.get<real_type>("<xmlattr>.defaultEventInterval", res.defaultEventInterval);
        res.defaultMaxError = simElem.get<real_type>("<xmlattr>.defaultStepSize", res.defaultMaxError);
        res.defaultTolerance = simElem.get<real_type>("<xmlattr>.defaultTolerance", res.defaultTolerance);
        res.endTime = simElem.get<real_type>("<xmlattr>.endTime");
        res.startTime = simElem.get<real_type>("<xmlattr>.startTime ", res.startTime);
        checkForUndefinedValues(res.defaultEventInterval, res.defaultMaxError, res.defaultTolerance, res.endTime,
                                res.startTime);
        return res;
    }

    vector<vector<size_type>> XMLConfigurationReader::getNodeSchedule(ptree::value_type & nodeElem)
    {
        vector<vector<size_type>> res;

        for (ptree::value_type &coreElem : nodeElem.second.get_child(""))
        {
            if (coreElem.first == "core")
            {
                res.push_back(vector<size_type>(coreElem.second.get<size_type>("<xmlattr>.numFmus", 0)));
            }
            else if (coreElem.first == "cores")
            {
                res.resize(res.size() + coreElem.second.get<size_type>("<xmlattr>.numCores", 0),
                           vector<size_type>(coreElem.second.get<size_type>("<xmlattr>.numFmusPerCore", 0), 0));
            }
        }
        return res;
    }

    SchedulePlan XMLConfigurationReader::getSchedulePlan()
    {
        SchedulePlan res = DefaultValues::schedulePlan();
        if (nullptr != _propertyTree.get_child_optional("configuration.scheduling"))
        {
            auto schedElem = _propertyTree.get_child("configuration.scheduling");
            if (!schedElem.empty())
            {
                for (ptree::value_type &nodeElem : schedElem.get_child(""))
                {
                    if (nodeElem.first == "node")
                    {
                        res.nodeStructure.push_back(getNodeSchedule(nodeElem));
                    }
                    else if (nodeElem.first == "nodes")
                    {
                        size_type numNewNodes = nodeElem.second.get<size_type>("<xmlattr>.numNodes", 0);
                        if (numNewNodes == 0)
                        {
                            throw runtime_error("In tag <nodes> the attribute 'numNodes' is missing.");
                        }
                        size_type numNewCores = nodeElem.second.get<size_type>("<xmlattr>.numCoresPerNode", 0);
                        if (numNewCores == 0)
                        {
                            throw runtime_error("In tag <nodes> the attribute 'numCoresPerNode' is missing.");
                        }
                        size_type numNewFmus = nodeElem.second.get<size_type>("<xmlattr>.numFmusPerCore", 0);
                        if (numNewFmus == 0)
                        {
                            throw runtime_error("In tag <nodes> the attribute 'numFmusPerCore' is missing.");
                        }

                        res.nodeStructure.resize(res.nodeStructure.size() + numNewNodes,
                                                 vector<vector<size_type>>(numNewCores, vector<size_type>(numNewFmus)));
                    }
                    else
                    {
                        throw runtime_error("XMLConfigurationReader: In scheduling was an unknown node identifier.");
                    }
                }
            }
        }
        return res;
    }

    void XMLConfigurationReader::appendConnectionInformation(list<SolverPlan> & solverPlans,
                                                             list<ConnectionPlan> & connPlans,
                                                             const SchedulePlan & schedPlan)
    {
        map<string_type, SolverPlan*> fmuNameToSolver;
        shared_ptr<ConnectionPlan> tmp;
        size_type tags = 0;
        for (SolverPlan & sp : solverPlans)
        {
            fmuNameToSolver[sp.fmu->name] = &sp;
        }

        for (ConnectionPlan & cp : connPlans)
        {
            tmp = make_shared<ConnectionPlan>(cp);
            tuple<size_type, size_type> destId, sourceId;
            destId = schedPlan.solverIdToCore[fmuNameToSolver[cp.destFmu]->id];
            sourceId = schedPlan.solverIdToCore[fmuNameToSolver[cp.sourceFmu]->id];
            if (destId == sourceId)
            {  // same node and core
                tmp->kind = "serial";
            }
            else if (std::get<0>(destId) == get<0>(sourceId))
            {  // same node
                tmp->kind = "openmp";
            }
            else
            {
                tmp->kind = "mpi";
                tmp->startTag = tags;
                tags += tmp->bufferSize;
                tmp->destRank = get<0>(destId);
                tmp->sourceRank = get<0>(sourceId);
            }
            fmuNameToSolver[cp.destFmu]->inConnections.push_back(tmp);
            fmuNameToSolver[cp.sourceFmu]->outConnections.push_back(tmp);
        }
        connPlans.clear();
    }

    WriterPlan XMLConfigurationReader::getWriterPlan()
    {
        WriterPlan res = DefaultValues::writerPlan();
        ptree writerElem = _propertyTree.get_child("configuration.writer");
        ptree::value_type & elem = writerElem.get_child("").front();

        res.kind = elem.first;
        res.numSteps = elem.second.get<size_type>("<xmlattr>.numOutputSteps", res.numSteps);
        res.filePath = elem.second.get<string_type>("<xmlattr>.resultFile", res.filePath);
        if (res.filePath == "")
        {
            throw runtime_error("XMLConfigurationReader: Result file path not set");
        }
        // TODO(mf): read vars which you want to write

        return res;
    }

    void XMLConfigurationReader::createMapping(const list<SolverPlan> & solverPlans, SchedulePlan & schedPlan)
    {
        size_type spaceForSolvers = 0, count = 0, coreNum, nodeNum = 0;
        schedPlan.solverIdToCore.resize(solverPlans.size());
        for (auto & node : schedPlan.nodeStructure)
        {
            coreNum = 0;
            for (auto & core : node)
            {
                spaceForSolvers += core.size();
                for (auto & solvSpace : core)
                {
                    schedPlan.solverIdToCore[count] = make_tuple(nodeNum, coreNum);
                    solvSpace = count++;  // i-th solver to i-th space
                }
                ++coreNum;
            }
            ++nodeNum;
        }
        if (spaceForSolvers != solverPlans.size())
        {
            throw runtime_error("XmlConfigurationReader: There is an error in the scheduling.");
        }
    }

    vector<vector<SimulationPlan>> XMLConfigurationReader::getSimulationPlans(const list<SolverPlan> & solverPlans,
                                                                              const SchedulePlan & schedPlan,
                                                                              const SimulationPlan simPlan)
    {
        vector<vector<SimulationPlan>> res(schedPlan.nodeStructure.size(), vector<SimulationPlan>(0));
        for (size_type i = 0; i < res.size(); ++i)
        {
            res[i].resize(schedPlan.nodeStructure[i].size(), simPlan);
        }
        vector<size_type> numSolver(res.size(), 0);
        WriterPlan wp = getWriterPlan();

        //Setting solver:
        for (const SolverPlan & sp : solverPlans)
        {
            size_type nodeId, coreId;
            std::tie(nodeId, coreId) = schedPlan.solverIdToCore[sp.id];
            DataManagerPlan & dm = res[nodeId][coreId].dataManager;
            dm.solvers.push_back(std::make_shared<SolverPlan>(sp));
            dm.outConnections.insert(res[nodeId][coreId].dataManager.outConnections.begin(), sp.outConnections.begin(),
                                     sp.outConnections.end());
            dm.inConnections.insert(res[nodeId][coreId].dataManager.inConnections.begin(), sp.inConnections.begin(),
                                    sp.inConnections.end());
            dm.writer = wp;
            ++numSolver[nodeId];
        }
        // Setting history and Sim kind:
        for (size_type i = 0; i < res.size(); ++i)
        {
            auto tmpCom = make_shared<Synchronization::Communicator>();
            for (size_type j = 0; j < res[i].size(); ++j)
            {
                res[i][j].dataManager.writer.startTime = simPlan.startTime;  // Maybe different to the others
                res[i][j].dataManager.writer.endTime = simPlan.endTime;
                res[i][j].dataManager.writer.filePath = string("p") + to_string(i) + string("_")
                        + res[i][j].dataManager.writer.filePath;
                res[i][j].dataManager.history.kind = "serial";  // is only for extension (work in progress) to support different kinds of data histories
                res[i][j].dataManager.commnicator = tmpCom;

                res[i][j].kind = "serial";
            }
        }
        return res;
    }

    ProgramPlan XMLConfigurationReader::getProgramPlan()
    {
        ProgramPlan res;

        SimulationPlan simPlan = getDefaultSimulationPlan();
        list<SolverPlan> solverPlans = getSolverPlans(simPlan);
        list<ConnectionPlan> connPlans = getConnectionPlans();

        SchedulePlan schedPlan = getSchedulePlan();
        if (schedPlan.nodeStructure.empty())
        {
            // TODO(mf): do scheduling
            schedPlan.nodeStructure = vector<vector<vector<size_type>>>(1, vector<vector<size_type>>(1, vector<size_type>(1)));
            schedPlan.solverIdToCore.resize(solverPlans.size());
            for(size_type i = 0; i < solverPlans.size(); ++i)
            {
                schedPlan.nodeStructure[0][0].push_back(i);  // assign all solvers to one core
                schedPlan.solverIdToCore[i] = make_tuple(static_cast<size_type>(0), static_cast<size_type>(0));
            }
        }
        else
        {
            createMapping(solverPlans, schedPlan);  // spread the solver on cores
        }

        appendConnectionInformation(solverPlans, connPlans, schedPlan);

        res.simPlans = getSimulationPlans(solverPlans, schedPlan, simPlan);
        return res;
    }

} /* namespace Initialization */

