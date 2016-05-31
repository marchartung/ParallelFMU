#include "simulation/AbstractSimulation.hpp"
#include "solver/AbstractSolver.hpp"

namespace Simulation
{

    AbstractSimulation::AbstractSimulation(const Initialization::SimulationPlan & in, const vector<std::shared_ptr<Solver::ISolver>> & solver)
            : _solver(solver),
              _maxIterations(std::numeric_limits<size_type>::max() - 2)
    {

        assert(solver.size() > 0);
        _simulationEndTime = std::numeric_limits<real_type>::max();
        for (size_type i = 0; i < _solver.size(); ++i)
            if (_solver[i]->getEndTime() < _simulationEndTime)  // select simulation end by fmu which is shortest in time
                _simulationEndTime = _solver[i]->getEndTime();
    }

    const vector<Solver::SolverSPtr>& AbstractSimulation::getSolver() const
    {
        return _solver;
    }

    vector<Solver::SolverSPtr>& AbstractSimulation::getSolver()
    {
        return _solver;
    }

    real_type AbstractSimulation::getSimulationEndTime() const
    {
        return _simulationEndTime;
    }

    void AbstractSimulation::initialize(int_type * argc, char **argv[])
    {
        for (Solver::SolverSPtr& solver : _solver)
        {
            solver->initialize();
        }
    }

    void AbstractSimulation::setSimulationEndTime(real_type simEnd)
    {
        _simulationEndTime = simEnd;
        for (size_type i = 0; i < _solver.size(); ++i)
            _solver[i]->setEndTime(simEnd);
    }

    size_type AbstractSimulation::getMaxIterations() const
    {
        return _maxIterations;
    }

    void AbstractSimulation::setMaxIterations(size_type num)
    {
        _maxIterations = num;
    }

} /* namespace Simulation */
