#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"

#include <limits>

#include "edge.hpp"
#include "tsp_solver/little_tsp/util.hpp"
#include "util.hpp"

using std::numeric_limits;

const int infinity{numeric_limits<int>::max()};

CostMatrixInteger::CostMatrixInteger() : value_{-1}, infinite_{false},
	edge_{-1, -1} {}

CostMatrixInteger::CostMatrixInteger(int value, Edge edge) : value_{value},
	infinite_{false}, edge_{edge} {}

CostMatrixInteger CostMatrixInteger::Infinite() {
	CostMatrixInteger cmi{};
	cmi.SetInfinite();
	return cmi;
}

int CostMatrixInteger::operator()() const {
	if (infinite_) { return infinity; }
	return value_;
}

CostMatrixInteger& CostMatrixInteger::operator-=(CostMatrixInteger other) {
	if (infinite_) { return *this; }
	if (other.infinite_) {
		throw ImplementationError{"Should not be subtracting infinite value!"};
	}
	value_ -= other.value_;
	return *this;
}

CostMatrixInteger& CostMatrixInteger::operator+=(CostMatrixInteger other) {
	if (infinite_ || other.infinite_) {
		infinite_ = true;
		return *this;
	}
	value_ += other.value_;
	return *this;
}

bool CostMatrixInteger::operator<(CostMatrixInteger other) const {
	if (infinite_ && other.infinite_) { return false; }
	if (infinite_) { return false; }
	if (other.infinite_) { return true; }
	return value_ < other.value_;
}
