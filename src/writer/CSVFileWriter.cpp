#include "writer/CSVFileWriter.hpp"
#include "fmi/ValueInfo.hpp"

namespace Writer
{

    CSVFileWriter::CSVFileWriter(const Initialization::WriterPlan & in)
            : IWriter(in),
              _outputStream(),
              _lineEndingSign('\n'),
              _separator(',')
    {
        LOGGER_WRITE("Using CSVFileWriter", Util::LC_LOADER, Util::LL_INFO);

    }

    CSVFileWriter::CSVFileWriter(const CSVFileWriter& in)
            : IWriter(&in),
              _outputStream(),
              _lineEndingSign(in._lineEndingSign),
              _separator(in._separator)
    {
        if (in.isInitialized())
            this->isInitialized();
    }

    CSVFileWriter::~CSVFileWriter()
    {
        deinitialize();
    }

    void CSVFileWriter::initialize()
    {
        _outputStream.open(getResultFile().c_str(), std::ios_base::out | std::ios_base::trunc);
        IWriter::initialize();
        if (_outputStream.fail())
            throw runtime_error("Cannot create and open result file.");
    }

    void CSVFileWriter::deinitialize()
    {
        if (_outputStream.is_open())
            _outputStream.close();
        IWriter::deinitialize();
    }

    void CSVFileWriter::appendTimeHeader()
    {
        assert(isInitialized());
        _outputStream << "time";
    }

    void CSVFileWriter::appendTime(double time)
    {
        assert(isInitialized());
        _outputStream << _lineEndingSign << time;
    }

    void CSVFileWriter::appendHeader(const string_type& fmuName, const FMI::ValueInfo& variables)
    {
        assert(isInitialized());
        auto vars = variables.getAllValueNames();
        for (auto it = vars.begin(); it != vars.end(); ++it)
        {
            //Util::StringHelper::replaceAll(value, ",", "_");
            _outputStream << _separator << fmuName << "." << *it;
        }
    }

    void CSVFileWriter::appendResults(const string_type& fmuName, const FMI::ValueCollection& values)
    {
        assert(isInitialized());
        for (real_type value : values.getValues<real_type>())
            _outputStream << _separator << to_string(value);
        for (int_type value : values.getValues<int_type>())
            _outputStream << _separator << to_string(value);
        for (bool_type value : values.getValues<bool_type>())
            _outputStream << _separator << to_string(value);
        for (string_type value : values.getValues<string_type>())
            _outputStream << _separator << "\"" << value << "\"";
    }

    void CSVFileWriter::flushResults()
    {
        assert(isInitialized());
        _outputStream.flush();
    }

} /* namespace DataAccess */

