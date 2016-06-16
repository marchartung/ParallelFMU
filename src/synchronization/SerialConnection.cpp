#include "synchronization/SerialConnection.hpp"
#include "fmi/ValueCollection.hpp"

namespace Synchronization
{

    SerialConnection::SerialConnection(const Initialization::ConnectionPlan & in)
            : AbstractConnection(in),
              _isFree(vector<bool_type>(in.bufferSize, true))
    {
    }

    bool SerialConnection::send(const HistoryEntry & in)
    {
        if (_isFree[_currentSendIndex])
        {
            _buffer[_currentSendIndex] = in;
            _isFree[_currentSendIndex] = false;
            _currentSendIndex = nextSendIndex();
            return true;
        }
        else
            return false;
    }

    HistoryEntry SerialConnection::recv()
    {
        //! If Buffer is not free, return invalid DHE-object
        //!return DataHistoryElement::invalid();
        //! Else return DHE
        //!return ...
        //! Dummy element:
        if (!_isFree[_currentReceiveIndex])
        {
            _isFree[_currentReceiveIndex] = true;
            //LOGGER_WRITE("Recv: " + to_string(*(_buffer[_currentReceiveIndex]->getValueCollection())),Util::LC_SOLVER, Util::LL_DEBUG);
            size_t tmp = _currentReceiveIndex;
            _currentReceiveIndex = nextReceiveIndex();
            return _buffer[tmp];
        }
        else
            return HistoryEntry::invalid();
    }

    int_type SerialConnection::hasFreeBuffer()
    {
        if (_isFree[nextSendIndex()] && _isFree[_currentSendIndex])
        {
            return 2;
        }
        return _isFree[_currentSendIndex];

    }

} /* namespace Synchronization */

