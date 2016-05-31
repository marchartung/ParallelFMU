/** @addtogroup Initialization
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_INITIALIZATION_FACTORYEXCEPTION_HPP_
#define INCLUDE_INITIALIZATION_FACTORYEXCEPTION_HPP_

#include "Stdafx.hpp"

namespace Initialization
{
    /**
     * An exception that is used by factory classes.
     */
    class FactoryException : public std::exception
    {
     public:
        /**
         * Create a new exception instance, described by the given message.
         * @param message The message that describes the occured exception.
         */
        FactoryException(string_type  message);
        /**
         * Delete the exception object.
         */
        virtual ~FactoryException();
        /**
         * Get the error message as c-string.
         * @return The error message.
         */
        virtual const char* what() const throw ();
        /**
         * Get the error message as c++-string.
         * @return The error message.
         */
        const string_type & getMessage() const;
        /**
         * Set the error message.
         * @param message The new message.
         */
        void setMessage(const string_type & message);

     private:
        string_type  _message;
    };

} /* namespace Initialization */

#endif /* INCLUDE_INITIALIZATION_FACTORYEXCEPTION_HPP_ */
/**
 * @}
 */
