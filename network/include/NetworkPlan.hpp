/*
 * NetworkPlan.hpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_NETWORKPLAN_HPP_
#define NETWORK_INCLUDE_NETWORKPLAN_HPP_

#include <memory>

#include <VariableList.hpp>
#include <SimulationServer.hpp>

#include "Stdafx.hpp"
#include "fmi/InputMapping.hpp"

namespace Network
{
    struct NetworkFmuInformation
    {
        size_type mpiPos;
        size_type corePos;
        size_type solverPos;
        FMI::InputMapping inputMap;
        FMI::InputMapping outputMap;
    };

    struct NetworkPlan
    {
        std::shared_ptr<NetOff::SimulationServer> server;
        std::vector<NetworkFmuInformation> fmuNet;
    };
}

#endif /* NETWORK_INCLUDE_NETWORKPLAN_HPP_ */
