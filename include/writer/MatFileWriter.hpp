/** @addtogroup Writer
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_WRITER_MATFILEWRITER_HPP
#define INCLUDE_WRITER_MATFILEWRITER_HPP

#include "Stdafx.hpp"
#include "BasicTypedefs.hpp"
#include "writer/IWriter.hpp"
#include <fstream>

namespace Writer
{

    /**
     * This class is a concrete writer that is able to write Matlab-result-files.
     * \remark The implementation was taken from the OpenModelica project.
     */
    class MatFileWriter : public IWriter
    {
     public:
        //MatFileWriter(size_type id, string_type resultFile, size_type bufferSize = std::numeric_limits<size_type>::max());
        MatFileWriter(const Initialization::WriterPlan & in);

        MatFileWriter(const MatFileWriter & in);

        MatFileWriter() = delete;

        ~MatFileWriter();

        void appendTimeHeader() override;

        void appendTime(double time) override;

        void appendHeader(const string_type & fmuName, const FMI::ValueInfo & variables) override;

        void appendResults(const string_type & fmuName, const FMI::ValueCollection & values) override;

        void flushResults() override;

        /**
         * function changes names of derivated variables e.g. der(secondOrder.y) is changed in secondOrder.der(y).
         * @param pcStr pointer to changing string.
         * @param uiLength length of changing string.
         */
        static inline void vFixVarName(char * pcStr, size_type uiLength)
        {
            char* pcDot;

            if (uiLength < 6)
                return;

            while (strncmp(pcStr, "der(", 4) == 0 && (pcDot = strrchr(pcStr, '.')) != nullptr)
            {
                size_type uiPos = (size_type) (pcDot - pcStr) + 1;

                for (size_type uiIndex = 4; uiIndex < uiPos; ++uiIndex)
                    pcStr[uiIndex - 4] = pcStr[uiIndex];

                strncpy(&pcStr[uiPos - 4], "der(", 4);
            }
        }

        void writeMatVer4MatrixHeader(const char * name, int_type rows, int_type cols, size_type size);

        void writeMatVer4Matrix(const char * name, int_type rows, int_type cols, const void * matrixData, size_type size);

        void initialize() override;

        void deinitialize() override;

     protected:
        std::ofstream _output_stream;
        std::ofstream::pos_type _dataHdrPos;
        std::ofstream::pos_type _dataEofPos;
        size_type _curser_position;
        size_type _uiValueCount;
        double * _doubleMatrixData1;
        double * _doubleMatrixData2;
        char * _stringMatrix;
        char * _pacString;
        int_type * _intMatrix;
        bool_type _headerWritten;
        bool_type _lineFinished;
        double _currentTime;

        vector<double> _translated;
        bool_type _data2Inited;
        size_type _linesWrote;
        size_type _numValues;

        void translateValueCollection(const FMI::ValueCollection & in);

    };

} /* namespace Writer */

#endif /* INCLUDE_WRITER_MATFILEWRITER_HPP */
/**
 * @}
 */
