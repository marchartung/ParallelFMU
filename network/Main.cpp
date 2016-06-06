/** @addtogroup Main
 *  @{
 *  \copyright TU Dresden ZIH. All rights reserved.
 *  \authors Martin Flehmig, Marc Hartung, Marcus Walther
 *  \date Oct 2015
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

        Simulation::AbstractSimulationSPtr simulation = prog.getSimulation();
        simulation->initialize(&argc,&argv);
        simulation->simulate();

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
