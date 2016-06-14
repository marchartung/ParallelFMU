/*
 * NetworkFunctions.hpp
 *
 *  Created on: 14.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_NETWORKFUNCTIONS_HPP_
#define NETWORK_INCLUDE_NETWORKFUNCTIONS_HPP_

#include "initialization/Plans.hpp"

#include "NetworkPlan.hpp"

namespace Network
{
    void appendNetworkInformation(Initialization::ProgramPlan & plan, NetworkPlan & netPlan);
}


#endif /* NETWORK_INCLUDE_NETWORKFUNCTIONS_HPP_ */
