#include "graph/edge_cost.hpp"

#include <limits>

#include "graph/edge.hpp"
#include "util.hpp"

using std::numeric_limits;

const int infinity{numeric_limits<int>::max()};
const int placeholder_value{-1};
const Edge placeholder_edge{-1, -1};

EdgeCost::EdgeCost() : value_{placeholder_value}, infinite_{false},
	edge_{placeholder_edge} {}

EdgeCost::EdgeCost(int value, Edge edge) : value_{value},
	infinite_{false}, edge_{edge} {}

EdgeCost EdgeCost::Infinite() {
	EdgeCost edge_cost{};
	edge_cost.SetInfinite();
	return edge_cost;
}

EdgeCost EdgeCost::Infinite(const Edge& edge) {
	EdgeCost edge_cost{infinity, edge};
	edge_cost.SetInfinite();
	return edge_cost;
}

int EdgeCost::operator()() const {
	if (infinite_) { return infinity; }
	return value_;
}

EdgeCost& EdgeCost::operator-=(EdgeCost other) {
	if (infinite_) { return *this; }
	if (other.infinite_) {
		throw ImplementationError{"Should not be subtracting infinite value!"};
	}
	value_ -= other.value_;
	return *this;
}

EdgeCost& EdgeCost::operator+=(EdgeCost other) {
	if (infinite_ || other.infinite_) {
		infinite_ = true;
		return *this;
	}
	value_ += other.value_;
	return *this;
}

EdgeCost EdgeCost::operator-(int other) const {
	if (infinite_) { return EdgeCost::Infinite(edge_); }
	return EdgeCost{value_ - other, edge_};
}

bool EdgeCost::operator<(EdgeCost other) const {
	if (infinite_ && other.infinite_) { return false; }
	if (infinite_) { return false; }
	if (other.infinite_) { return true; }
	return value_ < other.value_;
}
