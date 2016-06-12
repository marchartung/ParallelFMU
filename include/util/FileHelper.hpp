/*
 * FileHelper.hpp
 *
 *  Created on: 08.06.2016
 *      Author: hartung
 */

#ifndef INCLUDE_UTIL_FILEHELPER_HPP_
#define INCLUDE_UTIL_FILEHELPER_HPP_

#include <string>
namespace Util
{
    class FileHelper
    {
        FileHelper() = delete;
        FileHelper(const FileHelper &) = delete;
     public:

        static std::string find(const std::string & path, const std::string & fileName, const bool & recursive);

        static std::string absoluteFilePath( const std::string & fileName);
    };

}

#endif /* INCLUDE_UTIL_FILEHELPER_HPP_ */
