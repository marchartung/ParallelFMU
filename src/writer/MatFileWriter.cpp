#include "writer/MatFileWriter.hpp"
#include "fmi/ValueInfo.hpp"

namespace Writer
{

    MatFileWriter::MatFileWriter(const Initialization::WriterPlan & in)
            : IWriter(in),
              _dataHdrPos(),
              _dataEofPos(),
              _curser_position(0),
              _uiValueCount(0),
              _doubleMatrixData1(nullptr),
              _doubleMatrixData2(nullptr),
              _stringMatrix(nullptr),
              _pacString(nullptr),
              _intMatrix(nullptr),
              _headerWritten(false),
              _lineFinished(false),
              _currentTime(0.0),
              _data2Inited(false),
              _linesWrote(0),
              _numValues(0)
    {
        LOGGER_WRITE("Using MatFileWriter", Util::LC_LOADER, Util::LL_INFO);
    }


    MatFileWriter::MatFileWriter(const MatFileWriter& in)
        : IWriter(&in),
          _dataHdrPos(),
          _dataEofPos(),
          _curser_position(0),
          _uiValueCount(0),
          _doubleMatrixData1(nullptr),
          _doubleMatrixData2(nullptr),
          _stringMatrix(nullptr),
          _pacString(nullptr),
          _intMatrix(nullptr),
          _headerWritten(false),
          _lineFinished(false),
          _currentTime(0.0),
          _data2Inited(false),
          _linesWrote(0),
          _numValues(0)
    {
        if(in.isInitialized())
            this->isInitialized();
    }

    MatFileWriter::~MatFileWriter()
    {
        deinitialize();

        // free memory and initialize pointer
        if (_doubleMatrixData1 != nullptr)
            delete[] _doubleMatrixData1;
        if (_doubleMatrixData2 != nullptr)
            delete[] _doubleMatrixData2;
        if (_stringMatrix != nullptr)
            delete[] _stringMatrix;
        if (_pacString != nullptr)
            delete[] _pacString;
        if (_intMatrix != nullptr)
            delete[] _intMatrix;

        _doubleMatrixData1 = nullptr;
        _doubleMatrixData2 = nullptr;
        _stringMatrix = nullptr;
        _pacString = nullptr;
        _intMatrix = nullptr;

        if (_output_stream.is_open())
            _output_stream.close();
    }

    void MatFileWriter::initialize()
    {
        IWriter::initialize();
        std::string Aclass = "A1 bt. ir1 na  Tj  re  ac  nt  so   r   y   ";  // special header string

        if (_output_stream.is_open())
            _output_stream.close();

        // open new file
        _output_stream.open(getResultFile(), ios::binary | ios::trunc);

        // write header matrix
        writeMatVer4Matrix("Aclass", 4, 11, Aclass.c_str(), sizeof(char));

        // initialize help variables
        _uiValueCount = 0;
        _dataHdrPos = 0;
        _dataEofPos = 0;

        _doubleMatrixData1 = nullptr;
        _doubleMatrixData2 = nullptr;
        _stringMatrix = nullptr;
        _pacString = nullptr;
        _intMatrix = nullptr;
        _data2Inited = false;
        _linesWrote = 0;
        _numValues = 0;
    }

    void MatFileWriter::deinitialize()
    {

        writeMatVer4MatrixHeader("data_2",_numValues,_linesWrote,sizeof(double));
        IWriter::deinitialize();
        if (_output_stream.is_open())
            _output_stream.close();
    }

    void MatFileWriter::writeMatVer4MatrixHeader(const char *name, int_type rows, int_type cols, size_type size)
    {
        // matrix header struct
        typedef struct MHeader
        {
            size_type type;
            size_type mrows;
            size_type ncols;
            size_type imagf;
            size_type namelen;
        } MHeader_t;
        const int_type endian_test = 1;
        MHeader_t hdr;

        // every data type has an own type value
        int_type type = 0;
        if (size == 1)  //char
            type = 51;
        if (size == 4)  //int32
            type = 20;

        // initializing header
        hdr.type = 1000 * ((*(char*) &endian_test) == 0) + type;
        hdr.mrows = rows;
        hdr.ncols = cols;
        hdr.imagf = 0;
        hdr.namelen = strlen(name) + 1;

        // special treatment for "data_2" matrix. After new simualtion data, header has to be written
        if ((strcmp(name, "data_2") == 0) && (_dataHdrPos != _output_stream.tellp()))
        {
            _dataEofPos = _output_stream.tellp();
            _output_stream.seekp(_dataHdrPos);
            _output_stream.write((char*) &hdr, sizeof(MHeader_t));
            _output_stream.write(name, sizeof(char) * hdr.namelen);
            _output_stream.seekp(_dataEofPos);
        }
        else  // standard routine for header
        {
            _output_stream.write((char*) &hdr, sizeof(MHeader_t));
            _output_stream.write(name, sizeof(char) * hdr.namelen);
        }
    }

    void MatFileWriter::appendTimeHeader()
    {
        if (_headerWritten)
            throw std::runtime_error("The matfile-writer requires time as first column.");
    }

    void MatFileWriter::appendTime(double time)
    {
        if (!_lineFinished)
            throw std::runtime_error("The matfile-writer requires time as first entry per row.");
        _currentTime = time;
    }

    void MatFileWriter::appendHeader(const string_type& fmuName, const FMI::ValueInfo& variables)
    {
        size_type uilongest = 12;  // help variable for temp buffer size
        size_type uilongestName = 5;    // because of "Time"
        size_type uilongestDesc = 12;  // because of "Time in [s]"
        std::vector<string_type> valueNamesList = variables.getAllValueNames();
        vector<string_type> valueNames = vector<string_type>(valueNamesList.begin(), valueNamesList.end());
        size_type uiVarCount = valueNames.size() + 1;  // all variables, all parameters + time


        // get longest string_type of the variable/parameter names
        for (const string_type & str: valueNames)
        {
            if (str.size() > uilongestName)
                uilongestName = str.size() + 1;  // +1 because of string_type end
        }


        // get longest string. is needed for temp buffer
        uilongest = max(uilongestName, uilongestDesc);

        vector<char> cVec(uiVarCount * uilongest,' ');
        cVec[0] = 't'; cVec[1] = 'i'; cVec[2] = 'm'; cVec[3] = 'e';
        size_type i = 1;
        for(const string_type & str : valueNames)
        {
            std::cout << i << ". " << str << "\n";
            for(size_type j = 0;j<str.size();++j)
            {
                cVec[i*uilongest+j] = str[j];
                //std::cout << "(" << i << "," << j << ")\n";
            }
            ++i;
        }
        writeMatVer4Matrix("name", (int) uilongest, (int) uiVarCount, cVec.data(), sizeof(char));

        vector<double> dataInfo(4*(valueNames.size()+1));
        dataInfo[0] = 2; dataInfo[1] = 1; dataInfo[2] = 0; dataInfo[3] = -1; // for time
        for(size_type i=1;i<valueNames.size();++i)
        {
            dataInfo[4*i] = 2;
            dataInfo[4*i+1] = i+1;
            dataInfo[4*i+2] = 0;
            dataInfo[4*i+3] = -1;
        }

        writeMatVer4Matrix("dataInfo",4, (int)valueNames.size(),dataInfo.data(),sizeof(double));



        _headerWritten = true;
        _lineFinished = true;
    }

    void MatFileWriter::appendResults(const string_type& fmuName, const FMI::ValueCollection& values)
    {
        ++_linesWrote;
        if(!_data2Inited)
        {
            _data2Inited = true;
            _numValues = values.size()+1; // for time
            _dataHdrPos = _output_stream.tellp();
            writeMatVer4MatrixHeader("data_2",_numValues,_linesWrote,sizeof(double));
            _output_stream.seekp(0,std::ofstream::ios_base::end);
        }
        translateValueCollection(values);

        _output_stream.write((char *)_translated.data(),_translated.size()*sizeof(double));
    }

    void MatFileWriter::flushResults()
    {
        _output_stream.flush();
    }

    void MatFileWriter::writeMatVer4Matrix(const char *name, int_type rows, int_type cols, const void *matrixData, size_type size)
    {
        // first matrix header has to be written
        writeMatVer4MatrixHeader(name, rows, cols, size);

        // special treatment for "data_2" matrix. cols = 1
        if (strcmp(name, "data_2") == 0)
        {
            _output_stream.write((const char*) matrixData, (size) * rows * 1);
            //dataDummy = _output_stream.tellp(); // workaround: because with gcc compiled, sporadicaly the last simulation data is not written to file
        }
        else  // standard write routine
        {
            _output_stream.write((const char*) matrixData, (size) * rows * cols);
        }
    }

    void MatFileWriter::translateValueCollection(const FMI::ValueCollection & in)
    {
        size_type curPos = in.getValues<real_type>().size();
        if(_translated.size() != _numValues)
            _translated.resize(_numValues);
        _translated[0] = _currentTime;
        std::copy(in.getValues<real_type>().begin(),in.getValues<real_type>().end(),_translated.begin()+1);

        for(const auto& elem : in.getValues<int_type>())
        {
            _translated[curPos++] = static_cast<double>(elem);
        }
        for(const auto& elem : in.getValues<bool_type>())
        {
            _translated[curPos++] = (elem) ? 1.0 : 0.0;
        }
        for(const auto& elem : in.getValues<string_type >())
        {
            _translated[curPos++] = (elem.empty()) ? 0.0 : 1.0; // not supported
        }
    }

} /* namespace DataAccess */

