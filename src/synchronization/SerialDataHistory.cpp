#include "synchronization/SerialDataHistory.hpp"

namespace Synchronization
{

    SerialDataHistory::SerialDataHistory(const Initialization::HistoryPlan & in)
            : AbstractDataHistory(in)
    {
    }

    SerialDataHistory::~SerialDataHistory()
    {
    }

    void SerialDataHistory::deleteOlderThan(real_type in, size_type id)
    {
        AbstractDataHistory::deleteOlderThan(in, id);
    }

    void SerialDataHistory::addFmu(FMI::AbstractFmu * fmu, std::vector<ConnectionSPtr> & connList)
    {
        AbstractDataHistory::addFmu(fmu,connList);
    }

    bool_type SerialDataHistory::insert(const HistoryEntry & in, size_type id, WriteInfo write)
    {
        bool_type res = AbstractDataHistory::insert(in, id, write);
        if (write != WriteInfo::NOWRITE)
        {
            if(write == WriteInfo::EVENTWRITE)
                LOGGER_WRITE("(" + to_string(id) + ") Writing event at " + to_string(in.getTime()),Util::LC_SOLVER,Util::LL_DEBUG);
            WriteStack::iterator it;
            // insert new write time
            it = _toWriteStack.find(in.getTime());
            if (it == _toWriteStack.end())
                it = _toWriteStack.insert(
                        WriteStackPair(in.getTime(),
                                       (WriteCollectionLine ) { 0, WriteInfo::EVENTWRITE == write, vector<FMI::ValueCollection>(_history.size()) }))
                        .first;

            if (it->second.data[id].empty())
            {
                it->second.data[id] = in.getValueCollection();
                ++it->second.count;
            }
            else
            {
                throw std::runtime_error("Trying to write same point_type of time twice. (time=" + to_string(it->first) + ")");
            }
            if (it->second.count == _history.size())
            {
                //LOGGER_WRITE("Popping stack at " + to_string(it->first), Util::LC_SOLVER, Util::LL_DEBUG);
                popWriteStack();
            }
        }
        return res;
    }

    FMI::ValueCollection SerialDataHistory::getInputValues(const size_type & id, const real_type& time)
    {
        return _inputHistory[id][time];
    }

    const RingBufferSubHistory& SerialDataHistory::getInputHistory(size_type id) const
    {
        return _inputHistory[id];
    }

    void SerialDataHistory::popWriteStack()
    {
        WriteStack::iterator it = _toWriteStack.begin();
        size_type startStackSize = _toWriteStack.size();
        bool_type reachedWrite = false;
        real_type lastTime = 0.0;
        while (it != _toWriteStack.end())
        {
            reachedWrite = it->second.count == _history.size();
            lastTime = it->first;
            for (size_type i = 0; i < _history.size(); ++i)
            {
                if (it->second.data[i].empty() && it->first <= _history[i].getNewestTime())
                    it->second.data[i] = _history[i].interpolate(it->first);
            }
            _readyToWrite.push_back(make_tuple(it->first, it->second.data));

           it = _toWriteStack.erase(_toWriteStack.begin());
           if(reachedWrite)
               it = _toWriteStack.end();
        }

        for (size_type i = 0; i < _history.size(); ++i)
            deleteOlderThan(lastTime, i);
        if (startStackSize == _toWriteStack.size())
            throw std::runtime_error("SerialDataHistory: Result writing is corrupted. Stack size has to reduce (size:" + to_string(startStackSize) + ")");

    }

    real_type SerialDataHistory::getOldestSubHistoryTime()
    {
        real_type minT = 0;
        for (size_type i = 0; i < size(); ++i)
            if (_history[i].getNewestTime() < minT)
                minT = _history[i].getNewestTime();
        return minT;
    }

    const RingBufferSubHistory & SerialDataHistory::getHistory(size_type fmuId) const
    {
        return _history[fmuId];
    }

    bool_type SerialDataHistory::hasWriteOutput()
    {

        bool_type res = !_readyToWrite.empty();
        return res;
    }

    tuple<real_type, vector<FMI::ValueCollection> > SerialDataHistory::getWriteOutput()
    {
        tuple<real_type, vector<FMI::ValueCollection> > res;
        if (_readyToWrite.empty())
            throw std::runtime_error("SerialDataHistory: Call of getWriteOuput without any data to write.\n");
        res = _readyToWrite.front();
        _readyToWrite.pop_front();
        return res;
    }

} /* namespace Synchronization */
