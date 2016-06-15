/** @addtogroup Main
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
 */

/*
 * Code Style:   - 4 spaces instead of tabs
 *               - names of member variables should be written in camelCase with leading "_"
 *               - function names should be written in camel case
 *               - put all used std classes into Stdafx.h
 *               - names of header and code files always start with a capital letter
 *               - includes to header files of the project are always related to the input-folder and enclosed in "" signs
 *               - includes to thirdparty header files are enclosed in < >
 */

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "initialization/Program.hpp"
#include "ParallelFmu.hpp"

string_type getWorkingDirectory()
{
    boost::filesystem::path fullPath(boost::filesystem::current_path());
    return string_type(fullPath.c_str());
}

int_type main(int_type argc, char *argv[])
{
    try
    {
        Initialization::Program prog(&argc, &argv);
        prog.initialize();
        prog.simulate();
        prog.deinitialize();

    }
    catch (exception &ex)
    {
        LOGGER_WRITE(string_type("Execution failed. Error: ") + string_type(ex.what()), Util::LC_OTHER, Util::LL_ERROR);
        return -1;
    }

    return 0;
}
/**
 * @}
 */
