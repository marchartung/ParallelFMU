/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SERIALCONNECTION_HPP_
#define INCLUDE_SERIALCONNECTION_HPP_

#include "synchronization/AbstractConnection.hpp"


namespace Synchronization
{
    /**
     * This class implements a serial connection between two FMUs.
     * A connection is specified by a source which sends the data values and a receiver.
     */
    class SerialConnection : public AbstractConnection
    {
     public:
        /**
         * Create a serial connection between two FMUs.
         */
        SerialConnection(const Initialization::ConnectionPlan & in, bool outgoing);

        /**
         * Destroy the connection between the two FMUs.
         */
        ~SerialConnection()
        {
        }

        /**
         * Send DataHistoryElement from source FMU to destination FMU.
         * @param in DataHistoryElement to send.
         * @return Success status.
         */
        bool send(const HistoryEntry & in) override;

        /**
         * Receive DataHistoryElement from source FMU.
         * @return The received DataHistoryElement.
         */
        HistoryEntry recv() override;
        /**
         * Checks if a free buffer for sends is present.
         * @return int_type 0 if a send operation would block.
         */
        int_type hasFreeBuffer() override;

     private:
        vector<bool_type> _isFree;
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SERIALCONNECTION_HPP_ */
/**
 * @}
 */
