/** @addtogroup Util
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_UTIL_DELEGATE_HPP_
#define INCLUDE_UTIL_DELEGATE_HPP_

#include "util/IDelegate.hpp"

namespace Util
{
    /**
     * The base class for a function pointer to a member-function that can be called if an event occurred.
     */
    template<class Sender, class Receiver, typename Arg>
    class Delegate : public IDelegate<Sender, Arg>
    {
     private:
        typedef void (Receiver::*functionPtr)(Sender&, Arg);
        functionPtr _functionPtr;
        Receiver *_instance;

     public:
        /**
         * Create a new encapsulated function pointer to the given member-function belonging to the receiver object.
         * @param functionPtr1 The pointer to the function that should be invoked if the invoke-method is called.
         * @param instance The object that should be used as "this" pointer for the call.
         */
        Delegate(functionPtr functionPtr1, Receiver* instance) : _functionPtr(functionPtr1), _instance(instance)
        {

        }
        /**
         * Delete the object without touching the known receiver instance.
         */
        virtual ~Delegate()
        {
        }
        void invoke(Sender& sender, Arg arg)
        {
            (_instance->*_functionPtr)(sender, arg);
        }
    };

} /* namespace Util */

#endif /* INCLUDE_UTIL_DELEGATE_HPP_ */
/**
 * @}
 */
