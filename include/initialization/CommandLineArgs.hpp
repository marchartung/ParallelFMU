/*
 * CommandLineArgs.hpp
 *
 *  Created on: 02.05.2016
 *      Author: hartung
 */

#ifndef INCLUDE_INITIALIZATION_COMMANDLINEARGS_HPP_
#define INCLUDE_INITIALIZATION_COMMANDLINEARGS_HPP_

#include "Stdafx.hpp"

namespace Initialization
{

    class CommandLineArgs
    {
     public:

        CommandLineArgs(const string & configFile, const Util::LogLevel & logLvl);

        CommandLineArgs(int * argc, char ** argv[]);

        const string & getConfigFilePath() const;

        const Util::LogSettings & getLogSettings() const;

        Util::LogSettings & getLogSettings();

        tuple<int *, char ***> getProgramArgs();

        tuple<const int *, const char ***> getProgramArgs() const;

        bool isSimulationServer() const;

        bool isSimulationClient() const;

        int getSimulationServerPort() const;

     private:

        string _configFilePath = "";
        Util::LogSettings _logSettings;
        int _simulationServerPort;
        bool _simulationClient;

        int * _argc;
        char *** _argv;

        void initialize(int * argc, char ** argv[]);

        static map<string, Util::LogCategory> getLogCatMap();

        static map<string, Util::LogLevel> getLogLvlMap();
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_COMMANDLINEARGS_HPP_ */
