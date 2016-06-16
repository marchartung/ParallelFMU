/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_OPENMPCONNECTION_HPP_
#define INCLUDE_SYNCHRONIZATION_OPENMPCONNECTION_HPP_

#include <omp.h>
#include "synchronization/AbstractConnection.hpp"

namespace Synchronization
{

    /**
     * This class implements a connection between two FMUs using OpenMP.
     */
    class OpenMPConnection : public AbstractConnection
    {
     public:
        OpenMPConnection(const Initialization::ConnectionPlan & in);

        ~OpenMPConnection();

        /**
         * Send DataHistoryElement from source FMU to destination FMU.
         * @param in DataHistoryElement to send.
         * @return Success status.
         */
        bool send(const HistoryEntry & in) override;

        bool isShared() const override
        {
            return true;
        }

        /**
         * Receive DataHistoryElement from source FMU.
         * @return The received DataHistoryElement.
         */
        HistoryEntry recv() override;

        /**
         * Checks if a free buffer for send operations is present.
         * @return bool_type False if a send operation would block.
         */
        int_type hasFreeBuffer() override;

     private:
        omp_lock_t _writersLock;
    };

} /* namespace Synchronization */
#endif /* INCLUDE_SYNCHRONIZATION_OPENMPCONNECTION_HPP_ */
/**
 * @}
 */
