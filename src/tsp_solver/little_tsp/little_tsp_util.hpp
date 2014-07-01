#ifndef TSP_SOLVER_LITTLE_TSP_UTIL_H
#define TSP_SOLVER_LITTLE_TSP_UTIL_H

#include "edge.hpp"
#include "util.hpp"

struct CostMatrixZero {
	Edge edge;
	int penalty;
	bool operator<(const CostMatrixZero& other) const
	{ return penalty < other.penalty; }
};

class NotAvailableError : public ImplementationError {
public:
	NotAvailableError(const char* msg) : ImplementationError{msg} {}

private:
	const char* msg_;
};


#endif  // TSP_SOLVER_LITTLE_TSP_UTIL_H
