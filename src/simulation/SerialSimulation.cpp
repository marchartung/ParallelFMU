#include "simulation/SerialSimulation.hpp"
#include <omp.h>
namespace Simulation
{

    SerialSimulation::SerialSimulation(const Initialization::SimulationPlan & in, const vector<std::shared_ptr<Solver::ISolver>> & solvers)
            : AbstractSimulation(in,solvers)
    {
    }

    SerialSimulation::~SerialSimulation()
    {
    }

    void SerialSimulation::simulate()
    {
        bool_type running = true;  //MF only set, not used
        size_type iterationCount = 0;
        const vector<shared_ptr<Solver::ISolver>>& solver = getSolver();
        double s,e;
        s = omp_get_wtime();
        while (running && getMaxIterations() > ++iterationCount)
        {
            running = false;
            for (size_type i = 0; i < solver.size(); ++i)
            {
                solver[i]->solve(10);
                if (solver[i]->getCurrentTime() < getSimulationEndTime())
                    running = true;
                else
                    LOGGER_WRITE("(" + to_string(i) + ") Stopping at " + to_string(solver[i]->getCurrentTime()), Util::LC_SOLVER,Util::LL_DEBUG);
            }
        }
        e = omp_get_wtime();
        LOGGER_WRITE("thread " + to_string(omp_get_thread_num()) + " time: " + to_string(e - s), Util::LC_SOLVER, Util::LL_INFO);
    }

    string_type SerialSimulation::getSimulationType() const
    {
        return "serial";
    }

} /* namespace Simulation */
