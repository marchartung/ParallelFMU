#include "synchronization/openmp/OpenMPDataHistory.hpp"

namespace Synchronization
{
    OpenMPDataHistory::OpenMPDataHistory(const Initialization::HistoryPlan& in)
            : AbstractDataHistory(in),
              _writerId(0),  // which thread id is responsible for writing
              _nextWriteTime(0.0)
    {
        omp_init_lock(&_writeStackLock);

    }

    OpenMPDataHistory::~OpenMPDataHistory()
    {
        omp_destroy_lock(&_writeStackLock);
        for (size_type i = 0; i < _subHistoryLocks.size(); ++i)
        {
            omp_destroy_lock(&_subHistoryLocks[i]);
        }
    }

    void OpenMPDataHistory::deleteOlderThan(real_type in, size_type id)
    {
        omp_set_lock(&_subHistoryLocks[id]);
        AbstractDataHistory::deleteOlderThan(in, id);
        omp_unset_lock(&_subHistoryLocks[id]);
    }

    bool_type OpenMPDataHistory::insert(const HistoryEntry & in, size_type id, WriteInfo write)
    {

        bool_type res = true;
        omp_set_lock(&_subHistoryLocks[id]);
        res = AbstractDataHistory::insert(in, id, write);
        omp_unset_lock(&_subHistoryLocks[id]);

        if (write != WriteInfo::NOWRITE)
        {
            //if (write == WriteInfo::EVENTWRITE)
                //LOGGER_WRITE("(" + to_string(id) + ") Writing event at " + to_string(in.getTime()), Util::LC_SOLVER, Util::LL_DEBUG);
            WriteStack::iterator it;
            omp_set_lock(&_writeStackLock);
            // insert new write time
            it = _toWriteStack.find(in.getTime());
            if (it == _toWriteStack.end())
                it = _toWriteStack.insert(
                        WriteStackPair(
                                in.getTime(),
                                (WriteCollectionLine ) { write == WriteInfo::EVENTWRITE, OpenMPCounter(0ul), vector<FMI::ValueCollection>(
                                                _history.size()) })).first;
            size_type tmp = 0;
            if (it->second.data[id].empty())
            {
                it->second.data[id] = in.getValueCollection();
                tmp = ++((it->second).count);
            }
            else
                throw std::runtime_error("Trying to write same point_type of time twice. (time=" + to_string(it->first) + ")");
            if (tmp == _history.size())
            {
                popWriteStack();
            }

            omp_unset_lock(&_writeStackLock);
        }

        return res;
    }

    FMI::ValueCollection OpenMPDataHistory::getInputValues(const size_type& id, const real_type& time)
    {
         return _inputHistory[id][time];
    }

    void OpenMPDataHistory::popWriteStack()
    {
        WriteStack::iterator it = _toWriteStack.begin();
        size_type startStackSize = _toWriteStack.size();
        bool_type reachedWrite = false;
        real_type lastTime = 0.0;
        ;
        while (it != _toWriteStack.end())
        {
            reachedWrite = it->second.count == _history.size();
            lastTime = it->first;
            for (size_type i = 0; i < _history.size(); ++i)
            {
                omp_set_lock(&_subHistoryLocks[i]);

                if (it->second.data[i].empty())
                    it->second.data[i] = _history[i].interpolate(it->first);

                omp_unset_lock(&_subHistoryLocks[i]);
            }
            _readyToWrite.push_back(make_tuple(it->first, it->second.data));

            it = _toWriteStack.erase(_toWriteStack.begin());
            if (reachedWrite)
                it = _toWriteStack.end();
        }

        for (size_type i = 0; i < _history.size(); ++i)
            deleteOlderThan(lastTime, i);
        if (startStackSize == _toWriteStack.size())
            throw std::runtime_error("OpenMPDataHistory: Result writing is corrupted. Stack size has to reduce (size:" + to_string(startStackSize) + ")");

    }

    real_type OpenMPDataHistory::getOldestSubHistoryTime()
    {
        real_type minT = 0;
        for (size_type i = 0; i < size(); ++i)
        {
            omp_set_lock(&_subHistoryLocks[i]);
            if (_history[i].getNewestTime() < minT)
                minT = _history[i].getNewestTime();
            omp_unset_lock(&_subHistoryLocks[i]);
        }
        return minT;
    }

    const RingBufferSubHistory & OpenMPDataHistory::getInputHistory(size_type fmuId) const
    {
        return _inputHistory[fmuId];
    }

    const RingBufferSubHistory & OpenMPDataHistory::getHistory(size_type fmuId) const
    {
        return _history[fmuId];
    }

    bool_type OpenMPDataHistory::hasWriteOutput()
    {

        bool_type res = (size_type) omp_get_thread_num() == _writerId && !_readyToWrite.empty();

        return res;
    }

    tuple<real_type, vector<FMI::ValueCollection> > OpenMPDataHistory::getWriteOutput()
    {
        tuple<real_type, vector<FMI::ValueCollection> > res;
        if (_readyToWrite.empty())
            throw std::runtime_error("OpenMPDataHistory: Call of getWriteOuput without any data to write.\n");
        res = _readyToWrite.front();
        _readyToWrite.pop_front();

        return res;
    }

    void OpenMPDataHistory::addFmu(FMI::AbstractFmu * fmu, std::vector<ConnectionSPtr> & connList)
    {
        AbstractDataHistory::addFmu(fmu, connList);
        _subHistoryLocks.push_back(omp_lock_t());
        omp_init_lock(&_subHistoryLocks.back());
    }

} /* namespace Synchronization */

