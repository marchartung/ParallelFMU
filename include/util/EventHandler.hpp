/** @addtogroup Util
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_UTIL_EVENTHANDLER_HPP_
#define INCLUDE_UTIL_EVENTHANDLER_HPP_

#include "Stdafx.hpp"
#include "util/IDelegate.hpp"

namespace Util
{
    /**
     * This class can be used to handle various events. It manages a set of delegates that are called if the
     * invoke-method is triggered. To register a new delegate, you can either use the operators (C# like) or
     * the register and unregister functions.
     */
    template<class Sender, typename Arg>
    class EventHandler
    {
     private:
        set<IDelegate<Sender, Arg>*> knownDelegates;

     public:
        /**
         * Create a new event handler with an empty delegate list.
         */
        EventHandler()
                : knownDelegates()
        {
        }
        /**
         * Delete the object without touching the registered delegates.
         */
        virtual ~EventHandler()
        {
        }
        /**
         * Invoke all known delegates and pass the given sender and event argument to them.
         * @param sender The sender which has triggered the event.
         * @param arg Additional event arguments.
         */
        void invoke(Sender& sender, Arg arg)
        {
            for (IDelegate<Sender, Arg>* iter : knownDelegates)
                iter->invoke(sender, arg);
        }
        /**
         * Register a new delegate object for the event. A delegate-object can only be added once.
         * @note No operation is performed if the delegate was already registered.
         * @param newDelegate The new delegate that should be registered.
         */
        void registerDelegate(IDelegate<Sender, Arg>* newDelegate)
        {
            assert(knownDelegates.find(newDelegate) == knownDelegates.end());
            if (knownDelegates.find(newDelegate) == knownDelegates.end())
                knownDelegates.insert(newDelegate);
        }
        /**
         * Unregister a known delegate object from the event. If the delegate was not registered before,
         * the function does nothing.
         * @note No operation is performed if the delegate was not already registered.
         * @param knownDelegate The delegate that should be removed from the event.
         */
        void unregisterDelegate(IDelegate<Sender, Arg>* knownDelegate)
        {
            assert(knownDelegates.find(knownDelegate) != knownDelegates.end());
            knownDelegates.erase(knownDelegate);
        }
        /**
         * @copydoc Util::EventHandler::registerDelegate
         */
        EventHandler<Sender, Arg>& operator+=(IDelegate<Sender, Arg> *newDelegate)
        {
            registerDelegate(newDelegate);
            return *this;
        }
        /**
         * @copydoc Util::EventHandler::unregisterDelegate
         */
        EventHandler<Sender, Arg>& operator-=(IDelegate<Sender, Arg> *knownDelegate)
        {
            unregisterDelegate(knownDelegate);
            return *this;
        }
    };

} /* namespace Util */

#endif /* INCLUDE_UTIL_EVENTHANDLER_HPP_ */
/**
 * @}
 */
