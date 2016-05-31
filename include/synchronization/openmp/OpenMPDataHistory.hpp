/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_OPENMPDATAHISTORY_HPP_
#define INCLUDE_SYNCHRONIZATION_OPENMPDATAHISTORY_HPP_

#include "synchronization/AbstractDataHistory.hpp"
#include "synchronization/openmp/OpenMPCounter.hpp"
#include <omp.h>

namespace Synchronization
{
    class OpenMPDataHistory : public AbstractDataHistory
    {
     public:
        //OpenMPDataHistory(size_type numFMUs, size_type numConns, InterpolationSPtr interpol);
        OpenMPDataHistory(const Initialization::HistoryPlan & in);
        virtual ~OpenMPDataHistory();

        void addFmu(FMI::AbstractFmu * fmu,  std::vector<ConnectionSPtr> & connList) override;

        /**
         * Deletes all entries in the [num]-th sub-history, which are older than [tIn]
         * @param tIn Point_type of time determine which entries should be deleted
         * @param id Determines which subhistory should be cleaned.
         */
        void deleteOlderThan(real_type tIn, size_type id) override;

        /**
         * Inserts a HistoryEntryPtr into the num-th subhistory
         * @param in The entry which should be inserted into the history
         * @param id The number of sub-history in which the entry should be inserted
         * @return If false, the Entry couldn't be saved. This can happen, when too much data is already stored in the
         *         history. True on success.
         */
        virtual bool_type insert(const HistoryEntry & in, size_type id, WriteInfo write) override;

        FMI::ValueCollection getInputValues(const size_type & id, const real_type & time) override;

        /**
         * Getter for a const sub-history element.
         * Sub history should only be changed by the DataHistory, not by any other class. This ensures, that the DataHistory
         * alone takes care about thread-safety.
         * @param id Id of the subhistory which should be returned.
         * @return Returns sub-history according to the passed id
         */
        const RingBufferSubHistory & getInputHistory(size_type idInputSubhistory) const override;

        /**
         * Getter for a const sub-history element.
         * Sub history should only be changed by the DataHistory, not by any other class. This ensures, that the DataHistory
         * alone takes care about thread-safety.
         * @param id Id of the subhistory which should be returned.
         * @return Returns sub-history according to the passed id
         */
        const RingBufferSubHistory & getHistory(size_type id) const override;

        /**
         * Compares the latest entries of all sub-histories and returns the time of the oldest entry.
         * @return The saved timestamp of the FMU which is is slowest in time at the moment
         */
        real_type getOldestSubHistoryTime() override;
        /**
         * Determines if enough data is collected, to write values of the observed FMUs in to a file.
         * If all FMUs passed a point_type of time, where data should be written to a file, the function returns true and
         * getWriteOutput() returns than necessary data.
         * @return true, if all FMUs passed a write output time point.
         */
        virtual bool_type hasWriteOutput() override;

        /**
         * Returns data which should be written to file.
         * @return A tuple with a valid timestamp and a non-empty vector, where each element points to a ValueCollection
         *         if hasWriteOutput() was called before and returned true.
         *         If no data can't be written at the called time, it returns a std::numeric_limits<value_type>::max() as
         *         timestamp in the tuple.
         */
        virtual tuple<real_type, vector<FMI::ValueCollection> > getWriteOutput() override;


     protected:

        struct WriteCollectionLine
        {
            bool_type isEvent;
            OpenMPCounter count;
            vector<FMI::ValueCollection> data;
        };
        typedef std::map<real_type, WriteCollectionLine> WriteStack;
        typedef pair<real_type, WriteCollectionLine> WriteStackPair;

        typedef list<tuple<real_type,vector<FMI::ValueCollection> > > WriteList;

        WriteStack _toWriteStack;
        WriteList _readyToWrite;


        vector<omp_lock_t> _subHistoryLocks;
        size_type _writerId;
        real_type _nextWriteTime;
        omp_lock_t _writeStackLock;

        void popWriteStack();
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_OPENMPDATAHISTORY_HPP_ */
/**
 * @}
 */
