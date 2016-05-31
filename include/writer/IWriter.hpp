/** @addtogroup Writer
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_WRITER_IWRITER_HPP_
#define INCLUDE_WRITER_IWRITER_HPP_

#include "fmi/ValueCollection.hpp"
#include "Stdafx.hpp"
#include "initialization/Plans.hpp"
#include "fmi/ValueInfo.hpp"

namespace Writer
{
    /**
     * An abstract writer class that can store various result values persistently. The concrete writer works
     * with an internal buffer. To write a result set, call DataAccess::IWriter::appendTime() and than
     * DataAccess::IWriter::appendResults() until all values have been added. The values are finally stored
     * by calling DataAccess::IWriter::flushResults().
     * @attention A call of appendTime indicates, that a new result-set follows. Thus, take care that this function is
     * called first if results should be written.
     */
    class IWriter
    {
     public:
        /**
         * Create a new writer instance that can store result values persistently.
         * @param resultFile The path to the file that should be written.
         * @param id A unique id of the writer instance.
         * @param simStart The simulation start time.
         * @param stepSize Defines in which intervals results should be written.
         */
        //IWriter(size_type id, string_type resultFile, size_type bufferSize = std::numeric_limits<size_type>::max());
        IWriter(const Initialization::WriterPlan & in);

        IWriter(const IWriter * in);

        /**
         * Delete the write instance and flush all results that are not already flashed.
         */
        virtual ~IWriter()
        {
        }

        /**
         * Append the time header to the result file. The time value must always be the first value
         * of each result row.
         */
        virtual void appendTimeHeader() = 0;

        /**
         * Append the time value to the result file.
         * @param time The point_type in time for the result values.
         */
        virtual void appendTime(double time) = 0;

        /**
         * Append the given header names to the result file.
         * @param fmuName The name of the FMU, which is used as prefix for all values.
         * @param variables The names of the variables that should be added.
         */
        virtual void appendHeader(const string_type &fmuName, const FMI::ValueInfo& variables) = 0;

        /**
         * Append all results of the given FMU to the result set.
         * @param fmuName The name of the FMU, which is used as prefix for all values.
         * @param values The values of the FMU that should be written.
         */
        virtual void appendResults(const string_type &fmuName, const FMI::ValueCollection &values) = 0;

        /**
         * Write all results that were appended after the last flush to the output file.
         */
        virtual void flushResults() = 0;


        /**
         * Writes given data to file. As Input a tuple containing a timestamp and the needed data in a vector.
         * The vector contains pointer to the resulting data. The index i points to the data of the i-th local fmu
         */
        virtual void write(tuple<real_type, vector<FMI::ValueCollection> > toWrite);

        /**
         * Get the path to the result file that should be used to store the results persistently.
         * @return The result file.
         */
        const string_type & getResultFile() const;

        /**
         * Initialize the result file writer and open all required streams.
         */
        virtual void initialize();

        /**
         * Deinitialize the result file writer and close all opened streams.
         */
        virtual void deinitialize();

        /**
         * Check if the writer was already initialized.
         * @return True if the initialized-function was already called for the writer.
         */
        bool isInitialized() const;

        void setBufferSize(size_type bufferSize);

     private:
        string_type _resultFile;
        bool _initialized;
    };

} /* namespace Writer */

#endif /* INCLUDE_WRITER_IWRITER_HPP_ */
/**
 * @}
 */
