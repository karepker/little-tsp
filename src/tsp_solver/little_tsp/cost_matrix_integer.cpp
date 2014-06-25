#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"

#include "edge.hpp"
#include "util.hpp"

#include <limits>

using std::numeric_limits;

const int infinity{numeric_limits<int>::max()};

CostMatrixInteger::CostMatrixInteger() : value_{-1}, infinite_{false},
	available_{false}, edge_{-1, -1} {}

CostMatrixInteger::CostMatrixInteger(int value, Edge edge) : value_{value}, 
	infinite_{false}, available_{true}, edge_{edge} {}

void CostMatrixInteger::SetInfinite() {
	if (!available_) { throw NotAvailableError{"Integer is not available"}; }
	infinite_ = true;
}

int CostMatrixInteger::operator()() const {
	if (!available_) { throw NotAvailableError{"Integer is not available"}; }
	if (infinite_) { return infinity; }
	return value_;
}

bool CostMatrixInteger::IsInfinite() const {
	if (!available_) { throw NotAvailableError{"Integer is not available"}; }
	return infinite_;
}

CostMatrixInteger& CostMatrixInteger::operator-=(CostMatrixInteger other) {
	CheckAvailability(other);
	if (infinite_) { return *this; }
	if (other.infinite_) {
		throw ImplementationError{"Should not be subtracting infinite value!"};
	}
	value_ -= other.value_;
	return *this;
}

CostMatrixInteger& CostMatrixInteger::operator+=(CostMatrixInteger other) {
	CheckAvailability(other);
	if (infinite_ || other.infinite_) {
		infinite_ = true;
		return *this;
	}
	value_ += other.value_;
	return *this;
}

bool CostMatrixInteger::operator<(CostMatrixInteger other) const {
	CheckAvailability(other);
	if (infinite_ && other.infinite_) { return false; }
	if (infinite_) { return false; }
	if (other.infinite_) { return true; }
	return value_ < other.value_;
}

void CostMatrixInteger::CheckAvailability(CostMatrixInteger other) const {
	if (available_ && other.available_) { return; }
	throw NotAvailableError{"Cannot operate on unavailable numbers!"};
}
