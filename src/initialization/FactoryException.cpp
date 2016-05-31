#include "initialization/FactoryException.hpp"

namespace Initialization
{

    FactoryException::FactoryException(string_type message)
            : _message(message)
    {
    }

    FactoryException::~FactoryException()
    {
    }

    const string_type& FactoryException::getMessage() const
    {
        return _message;
    }

    const char* FactoryException::what() const throw ()
    {
        return _message.c_str();
    }

    void FactoryException::setMessage(const string_type& message)
    {
        _message = message;
    }

} /* namespace Initialization */
