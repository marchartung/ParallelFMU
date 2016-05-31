/*
 * HistoryEntryBuffer.hpp
 *
 *  Created on: 24.02.2016
 *      Author: hartung
 */

#ifndef INCLUDE_SYNCHRONIZATION_HISTORYENTRYBUFFER_HPP_
#define INCLUDE_SYNCHRONIZATION_HISTORYENTRYBUFFER_HPP_

#include "synchronization/HistoryEntry.hpp"

/**
 * HistoryEntryBuffer is used to send data over (Abstract)Connections faster. MPI needs direct access to the data
 */

namespace Synchronization
{

    class HistoryEntryBuffer
    {
     public:
        HistoryEntryBuffer(const HistoryEntry & in, bool init = false);

        virtual ~HistoryEntryBuffer();

        operator HistoryEntry() const;

        HistoryEntryBuffer & operator=(const HistoryEntry & in);

        void* data();

        size_type dataSize() const;

        bool isFree() const;

        void setFree(const bool & in);

     private:
        bool _free;
        real_type _time;
        /**
         * contains the real value of the value collection
         */
        size_type _realSize;
        real_type * _realVals;


        /**
         * contains the int_type value of the value collection
         */
        size_type _intSize;
        int_type * _intVals;

        /**
         * contains the bool_type value of the value collection
         */
        size_type _bool_typeSize;
        bool_type * _bool_typeVals;

        /**
         * Contains number of bytes for all values
         */
        size_type _dataSize;
        /**
         * Contains all data. structure:
         * [time,solveOrder,hasEvent,ValueCollection]
         */
        std::shared_ptr<char> _data;

        void initialize(const HistoryEntry & in);

        real_type* getStartOfValueCollection();

        int* getStartOfAdditionalIntValues();

        bool_type* getStartOfAdditionalBoolValues();

        const int* getStartOfAdditionalIntValues() const;

        const bool_type* getStartOfAdditionalBoolValues() const;

    };

} /* namespace Synchronization */

#endif /* INCLUDE_SYNCHRONIZATION_HISTORYENTRYBUFFER_HPP_ */
