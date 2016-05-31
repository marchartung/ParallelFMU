/** @addtogroup FMI
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_FMI_VALUESWITCH_HPP_
#define INCLUDE_FMI_VALUESWITCH_HPP_

#include <typeinfo>
#include "Stdafx.hpp"

namespace FMI
{

    /** This is a helper class. The method dataIndex provides a mapping for
     * data types int, bool_type, string_type and double to integer values representing the
     * array index in class \ref ValueReferenceCollection. \see ValueReferenceCollection
     */
    class ValueSwitch
    {
     public:
        ValueSwitch()
        {
        }
        virtual ~ValueSwitch()
        {
        }

     protected:
        /**
         * Provides a mapping for data types int, bool_type, string_type and double to integer values representing the
         * array index in class \ref ValueReferenceCollection.
         * double -> 0
         *    int_type -> 1
         *   bool_type -> 2
         * string_type -> 3
         */
        template<typename T>
        size_type dataIndex() const
        {
            string_type  str = typeid(T).name();
            throw runtime_error("FmuValueSwitch: type " + str + " was not collected.");
        }
    };

    template<> size_type ValueSwitch::dataIndex<double>() const;
    template<> size_type ValueSwitch::dataIndex<int_type>() const;
    template<> size_type ValueSwitch::dataIndex<bool_type>() const;
    template<> size_type ValueSwitch::dataIndex<string_type>() const;

} /* namespace FMI */

#endif /* INCLUDE_FMI_VALUESWITCH_HPP_ */
/**
 * @}
 */
