/*
 * NetworkPlan.cpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#include "NetworkPlan.hpp"


namespace Network
{
    void appendNetworkInformation(Initialization::ProgramPlan & plan, NetworkPlan & netPlan)
    {

    }

    NetworkPlan recvInitialNetworkInformation(const int & port)
    {
        NetworkPlan res;
        res._server = std::shared_ptr<NetOff::SimulationServer>(new NetOff::SimulationServer(port));

        return res;
    }

}
