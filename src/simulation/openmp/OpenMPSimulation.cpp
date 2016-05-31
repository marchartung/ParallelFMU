#include "simulation/openmp/OpenMPSimulation.hpp"
#include <omp.h>

namespace Simulation
{

    OpenMPSimulation::OpenMPSimulation(const Initialization::SimulationPlan & in, const vector<std::shared_ptr<Solver::ISolver>> & solver)
            : SerialSimulation(in,solver),
              _scheduleKind("static")
    {
    }

    OpenMPSimulation::~OpenMPSimulation()
    {
    }

// [MF, 09.11.2015] This is a first rough implementation of parallel simulation using parallel loop execution.
    void OpenMPSimulation::simulate()
    {
        vector<shared_ptr<Solver::ISolver>>& solver = getSolver();
        for(size_type i = 0;i<solver.size();++i)
            LOGGER_WRITE("FMU" + to_string(i) + " to " + to_string(solver[i]->getFmu()->getValues().getValues<real_type>()[0]), Util::LC_SOLVER, Util::LL_INFO);
#pragma omp parallel default(none) shared(solver)
        {
            bool_type running = true;
            size_type iterationCount = 0, ompRank = omp_get_thread_num(), numOmpRanks = omp_get_num_threads(), maxS = solver.size(), jmpOmp = std::ceil(
                    (double) maxS / (double) numOmpRanks);

            size_type sidStart = ompRank * jmpOmp, sidEnd = std::min((ompRank + 1) * jmpOmp, maxS);
#pragma omp critical //Just for debugging
            LOGGER_WRITE("Solving fmu " + to_string(sidStart) + " to " + to_string(sidEnd), Util::LC_SOLVER, Util::LL_INFO);
            double s, e;
            s = omp_get_wtime();
            while (running && getMaxIterations() > ++iterationCount)
            {
                running = false;
                for (size_type i = sidStart; i < sidEnd; ++i)
                {
                    //LOGGER_WRITE(solver[i]->getFmu()->getName() + " at " + to_string(solver[i]->getCurrentTime()), Util::LC_SOLVER, Util::LL_ERROR);
                    solver[i]->solve(100);
                    running = !solver[i]->isFinished();

                }
            }
            e = omp_get_wtime();
            LOGGER_WRITE("thread " + to_string(omp_get_thread_num()) + " time: " + to_string(e - s), Util::LC_SOLVER, Util::LL_INFO);

        }
    }

    string_type OpenMPSimulation::getSimulationType() const
    {
        return "openmp";
    }

} /* namespace Simulation */
