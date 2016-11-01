/*
 * InitialServer.hpp
 *
 *  Created on: 06.06.2016
 *      Author: hartung
 */

#ifndef NETWORK_INCLUDE_INITIALNETWORKSERVER_HPP_
#define NETWORK_INCLUDE_INITIALNETWORKSERVER_HPP_

#include "Stdafx.hpp"
#include "initialization/Plans.hpp"
#include "fmi/AbstractFmu.hpp"

namespace Network
{

    class InitialNetworkServer : private FMI::ValueSwitch
    {
     public:
        InitialNetworkServer(const int & port, Initialization::ProgramPlan & plan);

        ~InitialNetworkServer() = default;

        NetworkPlan getNetworkPlan() const;

        void start();

     private:
        Initialization::ProgramPlan & _plan;
        NetworkPlan _networkPlan;
        vector<size_type> _offsets;

        vector<shared_ptr<FMI::AbstractFmu>> _tmpFmus;

        void addSim();

        void getFile();

        void initSim();

        void startSim();

        shared_ptr<Initialization::FmuPlan> findFmuInProgramPlan(const string fmuPath,
                                                                 Network::NetworkFmuInformation & netInfo);

        FMI::InputMapping getMappingFromNameList(const FMI::ValueReferenceCollection & refs,
                                                 const NetOff::VariableList & vars, const FMI::ValueInfo & vi,
                                                 bool fromFmu);

        template<typename T>
        void addRefsToMapping(FMI::InputMapping & mapping, const vector<string> & inputs, const FMI::ValueInfo & vi,
                              const FMI::ValueReferenceCollection & refs, bool fromFmu)
        {
            for (size_type i = 0; i < inputs.size(); ++i)  // i is index of network vars
            {
                size_type ref = vi.getReference<T>(inputs[i]);
                for (size_type j = 0; j < refs.getValues<T>().size(); ++j)  // j is index of fmu
                {
                    if (refs.getValues<T>()[j] == ref)
                    {
                        mapping.push_back<T>(
                                ((!fromFmu) ?
                                        make_tuple(i + _offsets[dataIndex<T>()], j) :
                                        make_tuple(j, i + _offsets[dataIndex<T>()])));
                        ++_offsets[dataIndex<T>()];
                        break;
                    }
                }
            }
        }
    };

} /* namespace Network */

#endif /* NETWORK_INCLUDE_INITIALNETWORKSERVER_HPP_ */
