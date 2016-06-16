#include "synchronization/mpi/MPIConnection.hpp"

namespace Synchronization
{

    MPIConnection::MPIConnection(const Initialization::ConnectionPlan & in)
            : AbstractConnection(in),
              _isFree(vector<MPI_Request>(in.bufferSize)),
              _target(),
              _numOpenConns(1u)
    {

    }

    MPIConnection::~MPIConnection()
    {
        //TODO close all connections, complicated. Needs a ping-ping between target - source
    }

    void MPIConnection::initialize(const std::string & fmuName)
    {
        if (!isOutgoing(fmuName))
        {

            _target = _plan.destRank;
            for (size_type i = 0; i < _numOpenConns; ++i)
            {
                //  // Call recv, so the corresponding MPI_Isends aren't blocked
                MPI_Irecv(_buffer[i].data(), _buffer[i].dataSize(), MPI_BYTE, _target, getStartTag() + i, MPI_COMM_WORLD, &_isFree[i]);
            }
        }
        else
            _target = _plan.sourceRank;
    }

    bool MPIConnection::send(const HistoryEntry & in)
    {
        //is free?
        int_type tmpBool = 0;
        if (_buffer[_currentSendIndex].isFree())
            tmpBool = 1;
        else
            MPI_Test(&_isFree[_currentSendIndex], &tmpBool, MPI_STATUS_IGNORE);
        //yes:
        if (tmpBool > 0)
        {
            _buffer[_currentSendIndex] = in;  // todo fast copy
            MPI_Isend(_buffer[_currentSendIndex].data(), _buffer[_currentSendIndex].dataSize(), MPI_BYTE, _target, getStartTag() + _currentSendIndex,
            MPI_COMM_WORLD,
                      &_isFree[_currentSendIndex]);
            _currentSendIndex = nextSendIndex();
            return true;
        }
        //no:
        return false;
    }

    HistoryEntry MPIConnection::recv()
    {
        int_type tmpBool = 0;

        MPI_Test(&_isFree[_currentReceiveIndex], &tmpBool, MPI_STATUS_IGNORE);
        if (tmpBool > 0)
        {
            HistoryEntry res = _buffer[_currentReceiveIndex];  // todo real copy
            _currentReceiveIndex = nextReceiveIndex();
            MPI_Irecv(_buffer[_currentReceiveIndex].data(), _buffer[_currentReceiveIndex].dataSize(), MPI_BYTE, _target, getStartTag() + _currentReceiveIndex,
            MPI_COMM_WORLD,
                      &_isFree[_currentReceiveIndex]);  //keep listening for the next communication on this buffer
            return res;
        }

        return HistoryEntry::invalid();
    }

    int_type MPIConnection::hasFreeBuffer()
    {
        int_type res = 0, tmpBool = 0;
        if (_buffer[_currentSendIndex].isFree())
        {
            if (_buffer[nextSendIndex()].isFree())
                res = 2;
            else
                res = 1;
        }
        else
        {
            MPI_Test(&_isFree[_currentSendIndex], &tmpBool, MPI_STATUS_IGNORE);
            if (tmpBool > 0)
            {
                if (_buffer[nextSendIndex()].isFree())
                    res = 2;

                else
                {
                    MPI_Test(&_isFree[nextSendIndex()], &tmpBool, MPI_STATUS_IGNORE);
                    if (tmpBool > 0)
                        res = 2;
                    else
                        res = 1;
                }
            }
        }
        return res;
    }

} /* namespace Synchronization */
