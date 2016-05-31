/**
 * @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_BASICTYPEDEFS_HPP_
#define INCLUDE_BASICTYPEDEFS_HPP_

#include <memory>
#include <set>
#include <string>

typedef double real_type;
typedef int int_type;
typedef char bool_type;
typedef std::string string_type;

typedef unsigned int size_type;

namespace FMI
{
}

namespace Synchronization
{
    class AbstractConnection;
    class AbstractDataHistory;
    class Communicator;
    class HistoryEntry;
    class Interpolation;
    class SerialDataHistory;

    typedef std::shared_ptr<AbstractConnection> ConnectionSPtr;
    typedef std::shared_ptr<AbstractDataHistory> DataHistorySPtr;
    typedef std::shared_ptr<Communicator> CommunicatorSPtr;
    typedef std::shared_ptr<Interpolation> InterpolationSPtr;

    //struct CompareTwoEntryPtr;
    //typedef std::set<HistoryEntry> EntrySet;
}

namespace Writer
{
    class IWriter;
    typedef std::shared_ptr<IWriter> WriterSPtr;
}

namespace Initialization
{
    /*class Configuration;
    class ConfigurationGraph;
    struct FmuLoaderConfiguration;
    struct ConfigurationGraphNode;
    struct ConfigurationGraphEdge;
    struct WriterConfiguration;
    struct SolverConfiguration;
    struct SimulationConfiguration;
    class XMLConfigurationReader;*/
    class MainFactory;

    /*typedef std::shared_ptr<ConfigurationGraphEdge> ConfigurationGraphEdgeSPtr;
    typedef std::shared_ptr<ConfigurationGraph> ConfigurationGraphSPtr;
    typedef std::shared_ptr<ConfigurationGraphNode> ConfigurationGraphNodeSPtr;
    typedef std::shared_ptr<Configuration> ConfigurationSPtr;
    typedef std::shared_ptr<FmuLoaderConfiguration> FmuLoaderConfigurationSPtr;
    typedef std::shared_ptr<WriterConfiguration> WriterConfigurationSPtr;
    typedef std::shared_ptr<SolverConfiguration> SolverConfigurationSPtr;
    typedef std::shared_ptr<SimulationConfiguration> SimulationConfigurationSPtr;
    typedef std::shared_ptr<XMLConfigurationReader> XMLConfigurationReaderSPtr;*/
    typedef std::shared_ptr<MainFactory> MainFactorySPtr;
}

namespace Simulation
{
    class AbstractSimulation;

    typedef std::shared_ptr<AbstractSimulation> AbstractSimulationSPtr;
}

namespace Solver
{
    class ISolver;
    typedef std::shared_ptr<ISolver> SolverSPtr;
}

#endif /* INCLUDE_BASICTYPEDEFS_HPP_ */
/**
 * @}
 */
