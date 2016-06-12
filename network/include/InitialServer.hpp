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

        FMI::InputMapping getMappingFromNameList(const FMI::ValueReferenceCollection & refs, const NetOff::VariableList & vars, const FMI::ValueInfo & vi);

        template<typename T>
        void addRefsToMapping(FMI::InputMapping & mapping, const std::vector<std::string> & inputs, const FMI::ValueInfo & vi,
                              const FMI::ValueReferenceCollection & refs)
        {
            for (size_type i = 0; i < inputs.size(); ++i)
            {
                size_type ref = vi.getReference<T>(inputs[i]);
                for (size_type j = 0; j < refs.getValues<T>().size(); ++j)
                {
                    if (refs.getValues<T>()[j] == ref)
                    {
                        mapping.push_back<T>(std::make_tuple(j, i));
                    }
                }
            }
        }
    };
}
#endif /* NETWORK_INCLUDE_INITIALSERVER_HPP_ */
