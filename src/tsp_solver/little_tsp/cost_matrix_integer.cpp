#include "tsp_solver/little_tsp/cost_matrix_integer.hpp"

#include <limits>

using std::numeric_limits;

const int infinity{numeric_limits<int>::max()};

CostMatrixInteger::CostMatrixInteger() : value_{-1}, infinite_{false},
	available_{false} {}

CostMatrixInteger::CostMatrixInteger(int value) : value_{value}, 
	infinite_{false}, available_{true} {}

CostMatrixInteger::CostMatrixInteger(bool infinite, bool available) :
	value_{-1}, infinite_{infinite}, available_{available} {}

CostMatrixInteger::CostMatrixInteger(int value, bool infinite, bool available) :
	value_{value}, infinite_{infinite}, available_{available} {}

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

CostMatrixInteger CostMatrixInteger::operator-(CostMatrixInteger other) const {
	CheckAvailability(other);
	if (infinite_) { return CostMatrixInteger{true, true}; }
	if (other.infinite_) {
		throw ImplementationError{"Should not be subtracting infinite value!"};
	}

	// normal operation
	return CostMatrixInteger{value_ - other.value_};
}

CostMatrixInteger CostMatrixInteger::operator+(CostMatrixInteger other) const {
	CheckAvailability(other);
	if (infinite_ || other.infinite_) { return CostMatrixInteger{true, true}; }
	return CostMatrixInteger(value_ + other.value_);
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
