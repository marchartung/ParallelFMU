#include "synchronization/Communicator.hpp"

namespace Synchronization
{

    Communicator::Communicator()
            : _numManagedCons(0),
              _numManagedFmus(0)
    {
    }

    size_type Synchronization::Communicator::addFmu(FMI::AbstractFmu* in, vector<FMI::InputMapping> & valuePacking)
    {
        vector<ConnectionSPtr>& connList = in->getConnections();
        size_type numNewCons = 0;
        for (auto & con : connList)
        {
            con->initialize(in->getFmuName());
            size_type conId;
            auto it = _knownConIds.find(con->getStartTag());
            if (it == _knownConIds.end())
            {
                valuePacking.push_back(con->getPacking());
                conId = _numManagedCons++;
                _knownConIds[con->getStartTag()] = conId;
                ++numNewCons;
            }
            else
            {
                conId = it->second;
                if (con->isShared())
                {
                    ++numNewCons;
                    valuePacking.push_back(con->getPacking());
                }
            }
            con->setLocalId(conId);
        }
        in->setSharedId(_numManagedFmus++);

        for (auto & con : connList)
        {
            if (con->getLocalId() + 1 > _connections.size())
            {
                _connections.resize(con->getLocalId() + 1);
                _connections[con->getLocalId()] = con;
            }
        }
        if (_outConnectionIds.size() < in->getSharedId() + 1)
        {
            _outConnectionIds.resize(in->getSharedId() + 1);
        }
        if (_inConnectionIds.size() < in->getSharedId() + 1)
        {
            _inConnectionIds.resize(in->getSharedId() + 1);
        }
        for (auto & i : connList)
        {
            if (i->isOutgoing(in->getFmuName()))
            {
                _outConnectionIds[in->getSharedId()].push_back(i->getLocalId());
            }
            else
            {
                _inConnectionIds[in->getSharedId()].push_back(i->getLocalId());
            }
        }
        return numNewCons;
    }

    bool_type Communicator::send(HistoryEntry const & in, size_type communicationId)
    {
        return static_cast<bool_type>(_connections[communicationId]->send(in));
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
        return _inConnectionIds[in->getSharedId()];
    }

    const vector<size_type> & Communicator::getOutConnectionIds(const FMI::AbstractFmu * in) const
    {
        return _outConnectionIds[in->getSharedId()];
    }

    size_type Communicator::getNumInConnections() const
    {
        size_type sum = 0;
        for (const auto & _inConnectionId : _inConnectionIds)
        {
            sum += _inConnectionId.size();
        }
        return sum;
    }

    size_type Communicator::getNumOutConnections() const
    {
        size_type sum = 0;
        for (const auto & _outConnectionId : _outConnectionIds)
        {
            sum += _outConnectionId.size();
        }
        return sum;
    }

} /* namespace Synchronization */

