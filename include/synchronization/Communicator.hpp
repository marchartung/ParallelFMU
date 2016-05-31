/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_COMMUNICATOR_HPP_
#define INCLUDE_COMMUNICATOR_HPP_

#include "synchronization/AbstractConnection.hpp"
#include "synchronization/HistoryEntry.hpp"
#include "fmi/AbstractFmu.hpp"
#include "fmi/ValueCollection.hpp"
#include "initialization/Plans.hpp"

// Todo: Doku anpassen, da nur noch ein Interface, keine abstrakte Klasse mehr!
namespace Synchronization
{

    /**
     * A communicator handles the communication of output values between all FMUs on one node.
     * Dependencies between the FMUs are expressed via objects of class Data::Connection. We distinguish between
     * incoming and outgoing connections.
     *
     * Example:
     *   - Communicator I:
     *      - Incoming connections: AB, DB
     *      - Outgoing connections: AC, AB
     *
     *   - Communicator II:
     *      - Incoming connections: AC, CD
     *      - Outgoing connections: CD, DB
     *
     *   NODE 1                NODE 2
     * __________            __________
     * | Comm I |            | Comm II|
     * ----------    MPI     ----------
     * | FMU A  |----------->| FMU C  |
     * |   |    |            |   |    |
     * |   |OMP |            |   |OMP |
     * |   |    |            |   |    |
     * |   V    |    MPI     |   V    |
     * | FMU B  |<-----------| FMU D  |
     * |________|            |________|
     */
    class Communicator
    {
     public:
        /**
         * Create a communicator from given FMUs and their connections.
         * \note Do not think of this class as an equivalent to MPI communicator.
         * @param names Name of the FMUs that call this communicator instance.
         * @param outConnectionIds
         * @param inConnectionIds
         */
        Communicator(const Initialization::SimulationPlan & in);

        /**
         * Destroy the communicator.
         */
        virtual ~Communicator()
        {
        }

        void addFmu(const FMI::AbstractFmu * in, std::vector<ConnectionSPtr> & connList);

        /**
         * Send output values to dependent FMU of particular connection. The send method for the connection is called.
         * @param in
         * @param communicationId
         * @return
         */
        bool_type send(const HistoryEntry & in, size_type communicationId);

        /**
         * Return DataHistoryElement
         * @param communicationId
         * @return
         */
        HistoryEntry recv(size_type communicationId);

        /**
         * Check if the buffer is free for the given communicationId, i.e., if the next communication can be push-started.
         * @param communicationId
         * @return 0, if its not free, other wise a number how many entries at least are free
         */
        int_type connectionIsFree(size_type communicationId);

        /**
         * For the given local FMU-ID, return all FMU-IDs with outgoing-connection.
         * @param fmuLocalId
         * @return
         */
        const vector<size_type> & getOutConnectionIds(const FMI::AbstractFmu * in) const;

        /**
         * For the given local FMU, return all FMU-IDs with in-connection.
         * @param fmuLocalId ID of the FMU.
         * @return
         */
        const vector<size_type> & getInConnectionIds(const FMI::AbstractFmu * in) const;

        /**
         * @return Number of in-going connections handled by the communicator.
         */
        size_type getNumInConnections() const;

        /**
         * @return Number of out-going connections handled by the communicator.
         */
        size_type getNumOutConnections() const;

     protected:

        /**
         * i holds entries for i-th localId, j holds all out-going connection ids
         */
        vector<vector<size_type> > _inConnectionIds;

        /**
         * i holds entries for i-th localId, j holds all in-going connection ids
         */
        vector<vector<size_type> > _outConnectionIds;

        /**
         * All connections managed by this communicator instance.
         */
        vector<ConnectionSPtr> _connections;

        //Todo: Create buffer for communication, i.e., to store received values (later transfered to DataManager::DataHistory).
    };

} /* namespace Synchronization */

#endif /* INCLUDE_COMMUNICATOR_HPP_ */
/**
 * @}
 */
