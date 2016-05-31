/** @addtogroup Util
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_UTIL_IDELEGATE_H_
#define INCLUDE_UTIL_IDELEGATE_H_

namespace Util
{
    /**
     * Interface class for all delegates. A concrete delegate is a callable object, i.e. a function pointer.
     * The delegate is for example used for event notifications.
     */
    template<class Sender, typename Arg>
    class IDelegate
    {
     public:
        /**
         * Delete the delegate object.
         */
        virtual ~IDelegate()
        {
        }
        /**
         * Invoke the concrete callable object and pass the given arguments.
         * @param sender The sender-object of the event.
         * @param arg Additional event-arguments.
         */
        virtual void invoke(Sender& sender, Arg arg) = 0;
    };

} /* namespace Util */

#endif /* INCLUDE_UTIL_IDELEGATE_H_ */
/**
 * @}
 */
