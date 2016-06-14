/*
 * FileHelper.cpp
 *
 *  Created on: 08.06.2016
 *      Author: hartung
 */

#include <boost/filesystem.hpp>

#include "util/FileHelper.hpp"

namespace Util
{
    std::string FileHelper::find(const std::string & path, const std::string & fileName, const bool & recursive)
    {
        namespace fs = boost::filesystem;
        std::string res;
        fs::path bpath(path);
        if (!fs::is_directory(bpath))
        {
            std::cout << "Given path is not a directory";
        }
        else
        {
            for (fs::directory_iterator it = fs::directory_iterator(bpath); it != fs::directory_iterator(); ++it)
            {
                if (fs::is_directory(*it))
                {
                    if (recursive)
                    {
                        res = find(bpath.string(), fileName, recursive);
                    }
                }
                else if (fs::is_regular(*it) && it->path().filename() == fileName)
                {
                    res = it->path().string();
                }
                if (!res.empty())
                    break;
            }
        }
        return res;
    }
}

std::string Util::FileHelper::absoluteFilePath(const std::string& fileName)
{
    std::string res("");
    namespace fs = boost::filesystem;
    fs::path bpath(fileName);
    if(fs::is_regular(bpath))
        res = bpath.string();
    return res;
}
