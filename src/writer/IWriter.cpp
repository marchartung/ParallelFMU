#include "writer/IWriter.hpp"

namespace Writer
{
    IWriter::IWriter(const Initialization::WriterPlan & in)
            : _resultFile(in.filePath),
              _initialized(false)
    {
    }

    IWriter::IWriter(const IWriter* in)
    : _resultFile(in->_resultFile), _initialized(false)
    {
    }

    const string_type& IWriter::getResultFile() const
    {
        return _resultFile;
    }

    void IWriter::initialize()
    {
        _initialized = true;
    }

    void IWriter::deinitialize()
    {
        _initialized = false;
    }

    bool IWriter::isInitialized() const
    {
        return _initialized;
    }

    void IWriter::write(tuple<real_type, vector<FMI::ValueCollection> > toWrite)
    {
        appendTime(get<0>(toWrite));
        string_type dummy("");
        for (size_type i = 0; i < get<1>(toWrite).size(); ++i)
            appendResults(dummy, (get<1>(toWrite)[i]));
        flushResults();
    }

} /* namespace Synchronization */

