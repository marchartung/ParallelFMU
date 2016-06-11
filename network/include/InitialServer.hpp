/*
 * InitialServer.hpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_INITIALSERVER_HPP_
#define NETWORK_INCLUDE_INITIALSERVER_HPP_

#include "NetworkPlan.hpp"
namespace Network
{
    class InitialServer
    {
     public:
        InitialServer(const int & port, Initialization::ProgramPlan & plan);
        ~InitialServer();

        NetworkPlan getNetworkPlan();

     private:
        int _port;
        Initialization::ProgramPlan & _plan;
        NetworkPlan _networkPlan;

        std::vector<std::shared_ptr<FMI::AbstractFmu>> _tmpFmus;

        void start();

        void addSim();

        void getFile();

        void initSim();

        std::shared_ptr<Initialization::FmuPlan> findFmuInProgramPlan(const std::string fmuPath);
    };
}
#endif /* NETWORK_INCLUDE_INITIALSERVER_HPP_ */
