#include "simulation/SerialSimulation.hpp"
//#include <omp.h>

namespace Simulation
{

    SerialSimulation::SerialSimulation(const Initialization::SimulationPlan & in,
                                       const vector<shared_ptr<Solver::ISolver>> & solvers)
            : AbstractSimulation(in, solvers)
    {
    }

    SerialSimulation::~SerialSimulation()
    {
    }

    void SerialSimulation::simulate()
    {
        bool running = true;
        size_type iterationCount = 0;
        //const vector<shared_ptr<Solver::ISolver>>& solver = getSolver();

        const auto& solver = getSolver();

        std::vector<size_type> numSteps(solver.size(), 15);
        size_type tmpStepCount;
        double s(0.0), e(0.0);
        //s = omp_get_wtime();
        while (running && getMaxIterations() > ++iterationCount)
        {
            LOGGER_WRITE("Running at iteration " + to_string(iterationCount), Util::LC_SOLVER, Util::LL_DEBUG);
            running = false;
            for (size_type i = 0; i < solver.size(); ++i)
            {
                if ((tmpStepCount = solver[i]->solve(numSteps[i])) == std::numeric_limits<size_type>::max())
                {
                    LOGGER_WRITE("Abort simulation at " + to_string(solver[i]->getCurrentTime()), Util::LC_SOLVER,
                                 Util::LL_ERROR);
                    return;
                }
                if (tmpStepCount < numSteps[i])
                    --numSteps[i];
                else
                    ++numSteps[i];
                //LOGGER_WRITE("(" + to_string(i) + ") numSteps: " + to_string(numSteps[i]), Util::LC_SOLVER, Util::LL_ERROR);
                if (solver[i]->getCurrentTime() < getSimulationEndTime())
                    running = true;
                else
                    LOGGER_WRITE("(" + to_string(i) + ") Stopping at " + to_string(solver[i]->getCurrentTime()),
                                 Util::LC_SOLVER, Util::LL_DEBUG);
            }
        }
        //e = omp_get_wtime();
        LOGGER_WRITE("thread 0 time: " + to_string(e - s), Util::LC_SOLVER, Util::LL_INFO);
    }

    string_type SerialSimulation::getSimulationType() const
    {
        return "serial";
    }

}  // namespace Simulation
