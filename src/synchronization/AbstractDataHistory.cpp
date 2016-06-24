#include "synchronization/AbstractDataHistory.hpp"
#include "synchronization/AbstractConnection.hpp"

namespace Synchronization
{

    AbstractDataHistory::AbstractDataHistory(const Initialization::HistoryPlan& in)
    : _history(),
      _inputHistory()
    {
    }

    void AbstractDataHistory::addFmu(FMI::AbstractFmu* fmu, std::vector<ConnectionSPtr> & connList)
    {
        if(_knownFmus.insert(fmu->getFmuName()).second)
        {
            if(_history.size() < fmu->getLocalId()+1)
                _history.resize(fmu->getLocalId()+1,RingBufferSubHistory(Interpolation(),fmu->getValues(FMI::ReferenceContainerType::ALL)));
            for(ConnectionSPtr & con : connList)
            {
                if(_inputHistory.size() < con->getLocalId()+1)
                    _inputHistory.resize(con->getLocalId()+1,RingBufferSubHistory(Interpolation(),fmu->getValues(FMI::ReferenceContainerType::ALL)));
            }
        }
        else
        {
            LOGGER_WRITE("DataHistory: FMU " + fmu->getFmuName() + " already registered.", Util::LC_LOADER, Util::LL_ERROR);
            throw std::runtime_error("AbstractDataHistory initialization fail.");
        }
    }

    const RingBufferSubHistory & AbstractDataHistory::getInputHistory(size_type idInputSubhistory) const
    {
        return _inputHistory[idInputSubhistory];
    }

    bool_type AbstractDataHistory::insertInputs(const HistoryEntry & in, size_type conId)
    {
        return _inputHistory[conId].insert(in);
    }

    size_type AbstractDataHistory::size() const
    {
        return _history.size();
    }

    bool_type AbstractDataHistory::insert(const HistoryEntry & in, size_type fmuId, WriteInfo write)
    {
        //LOGGER_WRITE("Insert " + to_string(in.getTime()) + " into history " + to_string(fmuId),Util::LC_SOLVER,Util::LL_DEBUG);
        return _history[fmuId].insert(in);
    }

    void AbstractDataHistory::deleteOlderThan(real_type in, size_type id)
    {
        HistoryEntry tmp(in);
        _history[id].deleteOlderThan(tmp);
    }

    void AbstractDataHistory::createInputHistory(const std::list<shared_ptr<Initialization::ConnectionPlan> > & connections)
    {
        for(const auto& cp : connections)
        {
            _inputHistory.push_back(RingBufferSubHistory(Interpolation(),cp->inputMapping.getPackedValueCollection()));
        }
    }

} /* namespace Synchronization */

