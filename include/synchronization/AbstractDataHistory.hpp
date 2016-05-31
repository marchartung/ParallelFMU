/** @addtogroup Synchronization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_SYNCHRONIZATION_ABSTRACTDATAHISTORY_HPP_
#define INCLUDE_SYNCHRONIZATION_ABSTRACTDATAHISTORY_HPP_

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "initialization/Plans.hpp"
#include "fmi/AbstractFmu.hpp"
#include "synchronization/RingBufferSubHistory.hpp"

namespace Synchronization
{

    enum WriteInfo
    {
        NOWRITE,
        WRITE,
        EVENTWRITE
    };

    class AbstractDataHistory
    {
     public:

        /**
         * Minimal constructor. Need the number of observed FMUs as argument.
         * @param numFmus Number of FMUs which should be observed by this DataHistory
         * @param numInConnections Number of input dependencies of the handled FMUs
         */
        //AbstractDataHistory(size_type numFmus, size_type numInConnections, InterpolationSPtr interpol);
        AbstractDataHistory(const Initialization::HistoryPlan & in);
        /**
         * Default destructor is virtual to enable heritage.
         */
        virtual ~AbstractDataHistory()
        {
        };

        virtual void addFmu(FMI::AbstractFmu * fmu,  std::vector<ConnectionSPtr> & connList);

        virtual const RingBufferSubHistory & getInputHistory(size_type idInputSubhistory) const = 0;

        bool_type insertInputs(const HistoryEntry & in, size_type conId);

        size_type size() const;

        /**
         * Deletes all entries in the [num]-th sub-history, which are older than [tIn]
         * @param tIn Point_type of time determine which entries should be deleted
         * @param id Determines which subhistory should be cleaned.
         */
        virtual void deleteOlderThan(real_type tIn, size_type id);

        /**
         * Inserts a HistoryEntryPtr into the num-th subhistory
         * @param in The entry which should be inserted into the history
         * @param id The number of sub-history in which the entry should be inserted
         * @return If false, the Entry couldn't be saved. This can happen, when too much data is already stored in the history. True on success.
         */
        virtual bool_type insert(const HistoryEntry & in, size_type id, WriteInfo write);

        /**
         * Getter for a const sub-history element.
         * Sub history should only be changed by the DataHistory, not by any other class. This ensures, that the DataHistory alone takes care about thread-safety.
         * @param id Id of the subhistory which should be returned.
         * @return Returns sub-history according to the passed id
         */
        virtual const RingBufferSubHistory & getHistory(size_type id) const = 0;

        virtual FMI::ValueCollection getInputValues(const size_type & id, const real_type & time) = 0;

        /**
         * Compares the latest entries of all sub-histories and returns the time of the oldest entry.
         * @return The saved timestamp of the FMU which is is slowest in time at the moment
         */
        virtual real_type getOldestSubHistoryTime() = 0;
        /**
         * Determines if enough data is collected, to write values of the observed FMUs in to a file.
         * If all FMUs passed a point_type of time, where data should be written to a file, the function returns true and getWriteOutput() returns than necessary data.
         * @return true, if all FMUs passed a write output time point.
         */
        virtual bool_type hasWriteOutput() = 0;

        /**
         * Returns data which should be written to file.
         * @return A tuple with a valid timestamp and a non-empty vector, where each element points to a ValueCollection if hasWriteOutput() was called before and returned true.
         *         If no data can't be written at the called time, it returns a std::numeric_limits<value_type>::max() as timestamp in the tuple.
         */
        virtual tuple<real_type, vector<FMI::ValueCollection> > getWriteOutput() = 0;

     protected:
        /// All states (at all time points) of all FMUs handled by this DataHistory.
        vector<RingBufferSubHistory> _history;
        /// All output values of predecessor FMUs, i.e.,
        vector<RingBufferSubHistory> _inputHistory;

        void createInputHistory(const std::list<shared_ptr<Initialization::ConnectionPlan> > & connections);
     private:
        std::set<std::string> _knownFmus;
    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_ABSTRACTDATAHISTORY_HPP_ */
/**
 * @}
 */
