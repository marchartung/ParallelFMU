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

        CommandLineArgs(const std::string & configFile, const Util::LogLevel & logLvl);

        CommandLineArgs(int * argc, char ** argv[]);

        const std::string & getConfigFilePath() const;

        const Util::LogSettings & getLogSettings() const;

        Util::LogSettings & getLogSettings();

        tuple<int *, char ***> getProgramArgs();

        tuple<const int *, const char ***> getProgramArgs() const;

     private:

        std::string _configFilePath = "";
        Util::LogSettings _logSettings;
        int * _argc;
        char *** _argv;

        void initialize(int * argc, char ** argv[]);

        static map<std::string, Util::LogCategory> getLogCatMap();

        static map<std::string, Util::LogLevel> getLogLvlMap();
    };
}

#endif /* INCLUDE_INITIALIZATION_COMMANDLINEARGS_HPP_ */
