/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_MPICONNECTION_HPP_
#define INCLUDE_SYNCHRONIZATION_MPICONNECTION_HPP_

#include <mpi.h>
#include "synchronization/AbstractConnection.hpp"

namespace Synchronization
{

    /**
     * This class implements a connection between two FMUs using OpenMP.
     */
    class MPIConnection : public AbstractConnection
    {
     public:
        MPIConnection(const Initialization::ConnectionPlan & in, bool outgoing);

        ~MPIConnection();

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
         * Checks if a free buffer for send operations is present.
         * @return bool_type False if a send operation would block.
         */
        int_type hasFreeBuffer() override;

     private:
        vector<MPI_Request> _isFree;
        int _target;
        size_type _numOpenConns;
    };

} /* namespace Synchronization */
#endif /* INCLUDE_SYNCHRONIZATION_MPICONNECTION_HPP_ */
/**
 * @}
 */
