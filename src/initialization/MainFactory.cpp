
#include "initialization/MainFactory.hpp"


namespace Initialization
{
    MainFactory::MainFactory()
    {
    }

    Simulation::AbstractSimulationSPtr MainFactory::createSimulation(const SimulationPlan & in) const
    {
        if(in.kind == "serial")
            return createSimulationWithKnownType<Simulation::SerialSimulation>(in);
        //else if(in.kind == "openmp")
        //    return createSimulationWithKnownType<Simulation::OpenMPSimulation>(in);
        else
            throw std::runtime_error("MainFactoy: There's no simulation type " + in.kind);
    }
} /* namespace Initialization */
