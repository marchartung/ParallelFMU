#include "synchronization/openmp/OpenMPConnection.hpp"

namespace Synchronization
{

    OpenMPConnection::OpenMPConnection(const Initialization::ConnectionPlan & in)
            : AbstractConnection(in)
    {
        omp_init_lock(&_writersLock);
        omp_unset_lock(&_writersLock);
    }

    OpenMPConnection::~OpenMPConnection()
    {
        omp_destroy_lock(&_writersLock);
    }

    bool OpenMPConnection::send(const HistoryEntry & in)
    {
        bool res = false;
        if (omp_test_lock(&_writersLock))
        {
            if (_buffer[_currentSendIndex].isFree())
            {
                //std::cout << "send " << to_string(in.getTime()) << " (event: " << ((in.hasEvent()) ? "true" : "false") << ")\n";
                _buffer[_currentSendIndex] = in;
                _buffer[_currentSendIndex].setFree(false);
                _currentSendIndex = nextSendIndex();
                res = true;
            }
            omp_unset_lock(&_writersLock);
        }
        return res;
    }

    HistoryEntry OpenMPConnection::recv()
    {
        HistoryEntry res(HistoryEntry::invalid());
        if (omp_test_lock(&_writersLock))
        {
            if (!_buffer[_currentReceiveIndex].isFree())
            {
                res = _buffer[_currentReceiveIndex];
                //std::cout << "recv " << to_string(res.getTime()) << " (event: " << ((res.hasEvent()) ? "true" : "false") << ")\n";
                _buffer[_currentReceiveIndex].setFree(true);
                _currentReceiveIndex = nextReceiveIndex();
            }
            omp_unset_lock(&_writersLock);
        }
        return res;
    }

    int_type OpenMPConnection::hasFreeBuffer()
    {
        int_type res = 0;
        if (omp_test_lock(&_writersLock))
        {
            if (_buffer[_currentSendIndex].isFree())
            {
                if (_buffer[nextSendIndex()].isFree())
                    res = 2;
                else
                    res = 1;
            }
            omp_unset_lock(&_writersLock);
        }
        return res;
    }

} /* namespace Synchronization */
