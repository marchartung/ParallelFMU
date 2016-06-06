/*
 * NetworkPlan.hpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_NETWORKPLAN_HPP_
#define NETWORK_INCLUDE_NETWORKPLAN_HPP_

#include "VariableList.hpp"
#include "SimulationServer.hpp"
namespace Network
{
    struct NetworkFmuInformation
    {
        std::string fmuName;
        NetOff::VariableList inputList;
        NetOff::VariableList outputList;
    };

    struct NetworkPlan
    {
        std::shared_ptr<NetOff::SimulationServer> _server;
        std::vector<NetworkFmuInformation> fmuNet;
    };

    void appendNetworkInformation(Initialization::ProgramPlan & plan, NetworkPlan & netPlan);

    NetworkPlan recvInitialNetworkInformation(const int & port);

}

#endif /* NETWORK_INCLUDE_NETWORKPLAN_HPP_ */
