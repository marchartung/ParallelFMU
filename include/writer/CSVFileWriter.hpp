/** @addtogroup Writer
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_WRITER_CSVFILEWRITER_HPP
#define INCLUDE_WRITER_CSVFILEWRITER_HPP

#include "Stdafx.hpp"
#include "writer/IWriter.hpp"

namespace Writer
{
    /**
     * This class is a concrete writer that is able to write CSV-files. The separators can be controlled
     * by using the constructor arguments.
     */
    class CSVFileWriter : public IWriter
    {
     public:
        /**
         * Create a new writer for CSV files.
         * @param resultFile The name of the result file that should be written.
         * @param separator The separator between the elements of the same row (column separator).
         * @param lineEndingSign The separator between different rows.
         * @param simStart Is the simulation start time
         * @param stepSize Is defines in which intervals results should be written
         * @attention If the result file already exists, it is overwritten by this class.
         */
        //CSVFileWriter(size_type id, string_type resultFile, const char separator = ',', const char lineEndingSign = ';', size_type bufferSize = std::numeric_limits<size_type>::max());
        CSVFileWriter(const Initialization::WriterPlan & in);

        CSVFileWriter(const CSVFileWriter & in);

        CSVFileWriter() = delete;

        ~CSVFileWriter();

        void appendTimeHeader();

        void appendTime(double time);

        void appendHeader(const string_type &fmuName, const FMI::ValueInfo& variables);

        void appendResults(const string_type &fmuName, const FMI::ValueCollection &values);

        void flushResults();

        /**
         * Open the stream for writing and set the initialized variable to true.
         */
        void initialize() override;
        /**
         * Close the stream and set the internal initialization variable to false.
         */
        void deinitialize() override;

     private:
        std::ofstream _outputStream;
        const char _lineEndingSign;
        const char _separator;

    };

} /* namespace Synchronization */

#endif /* INCLUDE_WRITER_CSVFILEWRITER_HPP */
/**
 * @}
 */
