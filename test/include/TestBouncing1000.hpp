/*
 * TestSolver.hpp
 *
 *  Created on: 20.10.2015
 *      Author: marcus
 */

#ifndef TEST_INCLUDE_TESTBOUNCING1000_HPP_
#define TEST_INCLUDE_TESTBOUNCING1000_HPP_

#include "TestCommon.hpp"

class Bouncing1k : public TestCommon
{
 public:
    list<double> eventTimePoints;

    Bouncing1k()
            : TestCommon("./test/data/TestConfig_Bouncing1000.xml")
    {
    }

    ~Bouncing1k()
    {
    }
};


TEST_F (Bouncing1k, TestBouncing1000)
{
    ASSERT_EQ(simulation->getSimulationType(),"serial");
    Solver::AbstractSolverSPtr & tmp = simulation->getSolver()[0];
    FMI::ValueCollection tmpVC = tmp->getFmu()->getAllValues();
       size_type i=0;
       for(Solver::AbstractSolverSPtr & solv : simulation->getSolver())
       {
           tmpVC.getValues<real_type>()[0] = 1 + (real_type)i*(20.0/simulation->getSolver().size());
           solv->getFmu()->setStates(tmpVC.getValues<real_type>().data());
           ++i;
       }
    simulation->initialize();
    simulation->simulate();
    for(const Solver::AbstractSolverSPtr& solv : simulation->getSolver())
    {
        ASSERT_EQ(solv->getCurrentTime(),2.0);
    }
}



#endif /* TEST_INCLUDE_TESTBOUNCING1000_HPP_ */
