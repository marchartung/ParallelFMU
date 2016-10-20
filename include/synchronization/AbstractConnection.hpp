/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_ABSTRACTCONNECTION_HPP_
#define INCLUDE_ABSTRACTCONNECTION_HPP_

#include "initialization/Plans.hpp"
#include "synchronization/HistoryEntry.hpp"
#include "synchronization/HistoryEntryBuffer.hpp"
#include "fmi/ValueCollection.hpp"
#include "fmi/InputMapping.hpp"
#include "Stdafx.hpp"

namespace Synchronization
{
    /**
     * Abstract class that describes the data dependency direction between (exactly) two FMUs. The concrete
     * implementations are SerialConnection, OpenMPConnection and MPIConnection. A connection is specified
     * by a source which sends the data values and a receiver.
     */
    class AbstractConnection
    {
     public:
        /**
         * Create a connection between two FMUs.
         * @param bufferScheme Pattern for recv and send data
         */
        AbstractConnection(const Initialization::ConnectionPlan & in)
                : _buffer(vector<HistoryEntryBuffer>(
                          in.bufferSize,
                          HistoryEntryBuffer(HistoryEntry(in.inputMapping.getPackedValueCollection()), true))),
                  _currentReceiveIndex(0),
                  _currentSendIndex(0),
                  _localId(0),
                  _plan(in)
        {
        }

        /**
         * Destroy the connection between the two FMUs.
         */
        virtual ~AbstractConnection()
        {
        }

        /**
         * Send DataHistoryElement from source FMU to destination FMU.
         * @param in DataHistoryElement to send.
         * @return Success status.
         */
        virtual bool send(const HistoryEntry & in) = 0;

        /**
         * Receive DataHistoryElement from source FMU.
         * @return The received DataHistoryElement.
         */
        virtual HistoryEntry recv() = 0;
        /**
         * Checks if a free buffer for sends is present.
         * @return int_type 0 if a send operation would block.
         * It returns 2, if more than one buffer is free
         */
        virtual int_type hasFreeBuffer() = 0;

        virtual void initialize(const std::string & fmuName)
        {
        }

        bool isOutgoing(const std::string & fmuName) const
        {
            return _plan.sourceFmu == fmuName;
        }

        virtual bool isShared() const
        {
            return false;
        }

        size_type getLocalId() const
        {
            return _localId;
        }

        void setLocalId(size_type localId)
        {
            _localId = localId;
        }

        const FMI::InputMapping& getPacking() const
        {
            return _plan.inputMapping;
        }

        const size_type & getStartTag() const
        {
            return _plan.startTag;
        }

     protected:
        vector<HistoryEntryBuffer> _buffer;
        size_type _currentReceiveIndex;
        size_type _currentSendIndex;
        size_type _localId;
        const Initialization::ConnectionPlan _plan;

        size_t nextSendIndex() const
        {
            if (_currentSendIndex == _buffer.size() - 1)
                return (_currentSendIndex + 1) % _buffer.size();
            else
                return (_currentSendIndex + 1) % _buffer.size();
        }

        size_t nextReceiveIndex() const
        {
            return (_currentReceiveIndex + 1) % _buffer.size();
        }
    };

} /* namespace Synchronization */

#endif /* INCLUDE_ABSTRACTCONNECTION_HPP_ */
/**
 * @}
 */
