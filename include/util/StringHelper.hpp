/** @addtogroup Util
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

#ifndef INCLUDE_UTIL_STRINGHELPER_HPP_
#define INCLUDE_UTIL_STRINGHELPER_HPP_

#include "Stdafx.hpp"

namespace Util
{
    class StringHelper
    {
     public:
        StringHelper();
        virtual ~StringHelper();

        static bool_type replace(string_type & str, const string_type & from, const string_type & to) {
            size_t start_pos = str.find(from);
            if(start_pos == string_type ::npos)
                return false;
            str.replace(start_pos, from.length(), to);
            return true;
        }

        static void replaceAll(string_type & str, const string_type & from, const string_type & to) {
            if(from.empty())
                return;
            size_t start_pos = 0;
            while((start_pos = str.find(from, start_pos)) != string_type::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
            }
        }
    };

} /* namespace Util */

#endif /* INCLUDE_UTIL_STRINGHELPER_HPP_ */
/**
 * @}
 */
