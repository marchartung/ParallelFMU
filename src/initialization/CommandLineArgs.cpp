/*
 * CommandLineArgs.cpp
 *
 *  Created on: 02.05.2016
 *      Author: hartung
 */

#include "initialization/CommandLineArgs.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace Initialization
{

    CommandLineArgs::CommandLineArgs(string configFile, const Util::LogLevel & logLvl)
            : _configFilePath(std::move(configFile)),
              _logSettings(Util::LogSettings()),
              _simulationServerPort(-1),
              _simulationClient(false),
              _argc(nullptr),
              _argv(nullptr)
    {
        _logSettings.setAll(logLvl);
    }

    CommandLineArgs::CommandLineArgs(int * argc, char ** argv[])
            : _configFilePath(""),
              _logSettings(Util::LogSettings()),
              _simulationServerPort(-1),
              _simulationClient(false),
              _argc(nullptr),
              _argv(nullptr)
    {
        initialize(argc, argv);
    }

    const string& CommandLineArgs::getConfigFilePath() const
    {
        return _configFilePath;
    }

    const Util::LogSettings & CommandLineArgs::getLogSettings() const
    {
        return _logSettings;
    }

    Util::LogSettings & CommandLineArgs::getLogSettings()
    {
        return _logSettings;
    }

    tuple<int *, char ***> CommandLineArgs::getProgramArgs()
    {
        //if(_argc == nullptr || _argv == nullptr)
        //throw std::runtime_error("CommandLineArgs: cannot access program arguments, they wasn't set.");
        return make_tuple(_argc, _argv);
    }

    void CommandLineArgs::initialize(int * argc, char ** argv[])
    {
        map<string, Util::LogCategory> logCatMap = getLogCatMap();
        map<string, Util::LogLevel> logLvlMap = getLogLvlMap();

        namespace po = boost::program_options;
        po::options_description desc("Options");
        desc.add_options()("help", "Print help messages")(
                "configFile,c", boost::program_options::value<string>()->required(),
                "Path to the configuration file containing detailed information about the FMUs")(
                "log-settings,V", po::value<vector<string> >(), "log information: loader, event, solver, system, other")
        //"numThreads,n", po::value<size_type>(), "The number of threads respectively processes to use")
        ("server,S", po::value<int>(),
         "Setting up an simulation server based on NetworkOffloader interface. Takes the port on which it should open")(
                "client,C", "Starting a client program for remote connection based on the NetworkOffload interface");

        po::variables_map vm;
        this->_argc = argc;
        this->_argv = argv;
        try
        {
            po::store(po::parse_command_line(*argc, *argv, desc), vm);

            if (vm.count("help") != 0u)
            {
                cout << "Basic Command Line Parameter App" << endl << desc << endl;
                return;
            }
            if (vm.count("server") != 0u)
            {
#ifdef USE_NETWORK_OFFLOADER
                cout << "Creating FMU calculation server." << endl;
                _simulationServerPort = vm["server"].as<int>();
#else
                throw runtime_error("FMU server is not supported, since submodule NetworkOffloader is missing or hasn't been build.");
#endif
            }

            if (vm.count("client") != 0u)
            {
#ifdef USE_NETWORK_OFFLOADER
                cout << "Creating FMU client." << endl;
                throw runtime_error("FMU client is not supported, yet.");
                _simulationClient = true;
#else
                throw runtime_error("FMU client is not supported, since submodule NetworkOffloader is missing or hasn't been build.");
#endif
            }

            if (vm.count("configFile") != 0u)
            {
                this->_configFilePath = vm["configFile"].as<string>();
            }
            else
            {
                throw runtime_error("Path to configuration file required, but missing.");
            }

            Util::LogSettings logSet;
            if (vm.count("log-settings") != 0u)
            {
                vector<string> log_vec = vm["log-settings"].as<vector<string> >(), tmpvec;
                for (auto & i : log_vec)
                {
                    tmpvec.clear();
                    boost::split(tmpvec, i, boost::is_any_of("="));

                    if (tmpvec.size() > 1 && logLvlMap.find(tmpvec[1]) != logLvlMap.end()
                            && (tmpvec[0] == "all" || logCatMap.find(tmpvec[0]) != logCatMap.end()))
                    {
                        if (tmpvec[0] == "all")
                        {
                            logSet.setAll(logLvlMap[tmpvec[1]]);
                            break;
                        }
                        else
                        {
                            logSet.modes[logCatMap[tmpvec[0]]] = logLvlMap[tmpvec[1]];
                        }
                    }
                    else
                    {
                        throw runtime_error("log-settings flags not supported: " + i + "\n");
                    }
                }
                this->_logSettings = logSet;
            }
        }
        catch (po::error& e)
        {
            throw runtime_error((string("Cannot parse command line arguments. ") + string(e.what())).c_str());
        }
    }

    map<string, Util::LogCategory> CommandLineArgs::getLogCatMap()
    {
        map<string, Util::LogCategory> logCatMap;
        logCatMap["loader"] = Util::LC_LOADER;
        logCatMap["event"] = Util::LC_EVT;
        logCatMap["solver"] = Util::LC_SOLVER;
        logCatMap["system"] = Util::LC_SYS;
        logCatMap["other"] = Util::LC_OTHER;
        return logCatMap;
    }

    bool CommandLineArgs::isSimulationServer() const
    {
        return _simulationServerPort != -1;
    }

    bool CommandLineArgs::isSimulationClient() const
    {
        return _simulationClient;
    }

    int CommandLineArgs::getSimulationServerPort() const
    {
        return _simulationServerPort;
    }

    map<string, Util::LogLevel> CommandLineArgs::getLogLvlMap()
    {
        map<string, Util::LogLevel> logLvlMap;
        logLvlMap["error"] = Util::LL_ERROR;
        logLvlMap["warning"] = Util::LL_WARNING;
        logLvlMap["info"] = Util::LL_INFO;
        logLvlMap["debug"] = Util::LL_DEBUG;
        return logLvlMap;
    }

}  // namespace Initialization

