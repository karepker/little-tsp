#ifndef TSP_SOLVER_FACTORY_H
#define TSP_SOLVER_FACTORY_H

#include <memory>
#include <string>

class TSPSolver;

std::unique_ptr<TSPSolver> CreateTSPSolver(const std::string& type);

#endif  // TSP_SOLVER_FACTORY_H
