/*
 * TestCommon.hpp
 *
 *  Created on: 20.10.2015
 *      Author: marcus
 */

#ifndef TEST_INCLUDE_TESTCOMMON_HPP_
#define TEST_INCLUDE_TESTCOMMON_HPP_

#include <fmi/AbstractFmu.hpp>
#include <fmi/FmiLibFmu.hpp>
#include <fmi/FmuSdkFmu.hpp>
#include <gtest/gtest.h>

#include "initialization/CommandLineArgs.hpp"
#include "initialization/MainFactory.hpp"
#include "initialization/FactoryException.hpp"
#include "initialization/Program.hpp"
#include "initialization/XMLConfigurationReader.hpp"

#include "Stdafx.hpp"
#include "util/Delegate.hpp"
#include "solver/Euler.hpp"
#include "simulation/AbstractSimulation.hpp"

class TestCommon : public ::testing::Test
{
 public:
    Initialization::Program _program;
    Simulation::AbstractSimulationSPtr _simulation;

    TestCommon(const std::string & configFile)
            : _program(Initialization::Program(Initialization::CommandLineArgs(configFile,Util::LogLevel::LL_DEBUG)))
    {
    }

    ~TestCommon()
    {
    }

    virtual void SetUp()
    {
        _simulation = _program.getSimulation();
        //_simulation->initialize();
    }

    virtual void TearDown()
    {
        _simulation.reset();
    }
};

#endif /* TEST_INCLUDE_TESTCOMMON_HPP_ */
