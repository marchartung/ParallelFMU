/*
 * matFileCmp.cpp
 *
 *  Created on: 16.02.2016
 *      Author: hartung
 */

#include <MatFileCMP.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int_type main(int_type ac, char* av[])
{
    po::options_description desc("Allowed options");
    desc.add_options()(
            "help",
            "Program which compares two set of mat files. It compares every variable of a left set with a fitting one of the right set. If a variable isn't in one of both set of mat files, it isn't considered at all.")
            ("left", po::value<vector<string_type > >()->multitoken(), "setting all Mat files of the left set, example: --left mat1.mat mat2.mat")
            ("right", po::value<vector<string_type > >()->multitoken(), "setting all Mat files of the right set, example: --right mat3.mat mat4.mat")
            ("verbose", po::value<int_type>(), "turn additional output on (1) and off (0)");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    vector<string_type > lFiles, rFiles;
    int_type verboseLevel = 0;

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 0;
    }

    if (vm.count("verbose"))
        verboseLevel = vm["verbose"].as<int_type>();
    if(!(vm.count("left")) || !(vm.count("right")))
    {
        std::cout << "left or right sets not set.\n";
        return 1;
    }

    lFiles = vm["left"].as<vector<string_type >>();
    rFiles = vm["right"].as<vector<string_type >>();

    MatFileCMP mFC(lFiles,rFiles);
    mFC.setVerboseLevel(verboseLevel);
    std::cout << "The maximal relative error is " << mFC.getMaxError() << "\n";

    return 0;
}
