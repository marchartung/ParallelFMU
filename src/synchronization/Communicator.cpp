#include "synchronization/Communicator.hpp"

namespace Synchronization
{

    Communicator::Communicator(const Initialization::SimulationPlan & in)
    {
    }


    void Synchronization::Communicator::addFmu(const FMI::AbstractFmu* in, std::vector<ConnectionSPtr>& connList)
    {
        for(auto & con : connList)
            if(con->getLocalId()+1 > _connections.size())
            {
                _connections.resize(con->getLocalId()+1);
                _connections[con->getLocalId()] = con;
            }
        if(_outConnectionIds.size()<in->getLocalId()+1)
            _outConnectionIds.resize(in->getLocalId()+1);
        if(_inConnectionIds.size()<in->getLocalId()+1)
            _inConnectionIds.resize(in->getLocalId()+1);
        for(size_type i=0;i<connList.size();++i)
        {
            if(connList[i]->isOutgoing())
            {
                _outConnectionIds[in->getLocalId()].push_back(connList[i]->getLocalId());
            }
            else
            {
                _inConnectionIds[in->getLocalId()].push_back(connList[i]->getLocalId());
            }
        }
    }

    bool_type Communicator::send(HistoryEntry const & in, size_type communicationId)
    {
        return _connections[communicationId]->send(in);
    }

    HistoryEntry Communicator::recv(size_type communicationId)
    {
        return _connections[communicationId]->recv();
    }

    int_type Communicator::connectionIsFree(size_type communicationId)
    {
        return _connections[communicationId]->hasFreeBuffer();
    }

    const vector<size_type> & Communicator::getInConnectionIds(const FMI::AbstractFmu * in) const
    {
        return _inConnectionIds[in->getLocalId()];
    }

    const vector<size_type> & Communicator::getOutConnectionIds(const FMI::AbstractFmu * in) const
    {
        return _outConnectionIds[in->getLocalId()];
    }

    size_type Communicator::getNumInConnections() const
    {
        size_type sum = 0;
        for (size_type i = 0; i < _inConnectionIds.size(); ++i)
            sum += _inConnectionIds[i].size();
        return sum;
    }

    size_type Communicator::getNumOutConnections() const
    {
        size_type sum = 0;
        for (size_type i = 0; i < _outConnectionIds.size(); ++i)
            sum += _outConnectionIds[i].size();
        return sum;
    }

} /* namespace Synchronization */

