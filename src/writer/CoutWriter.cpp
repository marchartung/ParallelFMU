#include "writer/CoutWriter.hpp"
#include "fmi/ValueInfo.hpp"

namespace Writer
{

    CoutWriter::CoutWriter(const Initialization::WriterPlan & in)
            : IWriter(in), _lineEndingSign('\n'), _separator(',')
    {
        LOGGER_WRITE("Using CoutWriter", Util::LC_LOADER, Util::LL_INFO);

    }

    CoutWriter::~CoutWriter()
    {
        deinitialize();
    }

    void CoutWriter::initialize()
    {
        IWriter::initialize();
        if (std::cout.fail())
            throw runtime_error("Cannot create and open result file.");
    }

    void CoutWriter::deinitialize()
    {
        IWriter::deinitialize();
    }

    void CoutWriter::appendTimeHeader()
    {
        assert(isInitialized());
        std::cout << "time";
    }

    void CoutWriter::appendTime(double time)
    {
        assert(isInitialized());
        std::cout << _lineEndingSign << std::endl << time;
    }

    void CoutWriter::appendHeader(const string_type& fmuName, const FMI::ValueInfo& variables)
    {
        assert(isInitialized());
        for(string_type value : variables.getAllValueNames())
        {
            Util::StringHelper::replaceAll(value, ",", "_");
            std::cout << _separator << fmuName << "." << value;
        }
    }

    void CoutWriter::appendResults(const string_type& fmuName, const FMI::ValueCollection& values)
    {
        assert(isInitialized());
        for(real_type value : values.getValues<real_type>())
            std::cout << _separator << to_string(value);
        for(int_type value : values.getValues<int_type>())
            std::cout << _separator << to_string(value);
        for(bool_type value : values.getValues<bool_type>())
            std::cout << _separator << to_string(value);
        for(string_type value : values.getValues<string_type>())
            std::cout << _separator << "\"" << value << "\"";
    }

    void CoutWriter::flushResults()
    {
        assert(isInitialized());
        std::cout.flush();
    }

} /* namespace DataAccess */
