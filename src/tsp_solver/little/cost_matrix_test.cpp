#include "tsp_solver/little/cost_matrix.hpp"

#include <cassert>

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <vector>

#include "graph/edge.hpp"
#include "graph/edge_cost.hpp"
#include "graph/mock.hpp"
#include "matrix.hpp"
#include "util.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::back_inserter;
using std::begin;
using std::end;
using std::max_element;
using std::min_element;
using std::numeric_limits;
using std::transform;
using std::unique_ptr;
using std::vector;

template <typename T>
using CostVector = CostMatrix::CostVector<T>;
using Row = CostMatrix::Row;
using Column = CostMatrix::Column;

using ::testing::Const;
using ::testing::Return;
using ::testing::ReturnRef;

const int infinity{numeric_limits<int>::max()};

const vector<Edge> include1;
const vector<Edge> exclude1_edges{{0, 0}};
const vector<Edge> include2{{0, 2}};
const vector<Edge> exclude2_edges{{0, 0}};

const Matrix<EdgeCost> graph_weights{
	MakeEdgeCosts({6, 2, 4, 4, 5, 3, 8, 9, 7}, 3)};

template <typename T>
static void CompareWithExpected(T& matrix, vector<int> expected);

class CostMatrixTest : public ::testing::Test {
public:
	CostMatrixTest();
protected:
	// I need unique_ptrs because graph functions get called in the constructor
	// of matrix, and I need time to set expectations for mock calls
	unique_ptr<CostMatrix> matrix1_ptr, matrix2_ptr;

private:
	MockGraph graph;
	Matrix<int> exclude1, exclude2;
};

CostMatrixTest::CostMatrixTest() {
	// set expectations for the graph
	for (int i{0}; i < graph_weights.GetNumRows(); ++i) {
		for (int j{0}; j < graph_weights.GetNumColumns(); ++j) {
			EXPECT_CALL(Const(graph), Predicate(i, j)).WillRepeatedly(
					ReturnRef(graph_weights(i, j)));
		}
	}

	EXPECT_CALL(graph, GetNumVertices()).WillRepeatedly(Return(3));

	exclude1.SetSize(3);
	exclude2.SetSize(3);
	for (const Edge& e : exclude1_edges) { exclude1(e.u, e.v) = 1; }
	for (const Edge& e : exclude2_edges) { exclude2(e.u, e.v) = 1; }

	// construct the matrix after we set expectations for the graph
	matrix1_ptr = unique_ptr<CostMatrix>{
		new CostMatrix{graph, include1, exclude1}};
	matrix2_ptr = unique_ptr<CostMatrix>{
		new CostMatrix{graph, include2, exclude2}};

	// reduce the matrices
	EXPECT_EQ(13, matrix1_ptr->ReduceMatrix());
	EXPECT_EQ(13, matrix2_ptr->ReduceMatrix());
}

TEST_F(CostMatrixTest, GetCondensedSize) {
	EXPECT_EQ(3, matrix1_ptr->GetCondensedSize());
	EXPECT_EQ(2, matrix2_ptr->GetCondensedSize());
}

TEST_F(CostMatrixTest, ReduceMatrix) {
	// make sure the matrices were reduced correctly
	CompareWithExpected(*matrix1_ptr, {infinity, 0, 2, 0, 2, 0, 0, 2, 0});
	CompareWithExpected(*matrix2_ptr, {0, 0, 0, 0});

	// matrix that cannot be row reduced
	const Matrix<EdgeCost> infinite_reduction{MakeEdgeCosts({
			-1, -1, 129, 86,
			39, -1, 112, 69,
			129, 112, -1, -1,
			86, 69, -1, -1}, 4)};
	MockGraph irreducible_graph;
	for (int i{0}; i < infinite_reduction.GetNumRows(); ++i) {
		for (int j{0}; j < infinite_reduction.GetNumColumns(); ++j) {
			EXPECT_CALL(Const(irreducible_graph), Predicate(i, j)).
				WillRepeatedly(ReturnRef(infinite_reduction(i, j)));
		}
	}
	EXPECT_CALL(irreducible_graph, GetNumVertices()).WillRepeatedly(Return(4));

	vector<Edge> infinite_include{{1, 0}, {0, 2}};
	vector<Edge> infinite_exclude_edges{
		{0, 0}, {1, 1}, {2, 2}, {3, 3}, {0, 1}, {2, 3}, {3, 2}};
	Matrix<int> infinite_exclude{4, {
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 1,
		0, 0, 1, 1}};
	CostMatrix cost_matrix{
		irreducible_graph, infinite_include, infinite_exclude};
	EXPECT_EQ(infinity, cost_matrix.ReduceMatrix());
}

TEST_F(CostMatrixTest, GetRow) {
	// iterate over matrix1 with rows
	CostMatrix::CostVector<Row> row0{
		matrix1_ptr->GetRow(matrix1_ptr->GetActualRowNum(0))};
	CostMatrix::CostVector<Row> row1{
		matrix1_ptr->GetRow(matrix1_ptr->GetActualRowNum(1))};
	CostMatrix::CostVector<Row> row2{
		matrix1_ptr->GetRow(matrix1_ptr->GetActualRowNum(2))};
	CompareWithExpected(row0, {infinity, 0, 2});
	CompareWithExpected(row1, {0, 2, 0});
	CompareWithExpected(row2, {0, 2, 0});

	auto row2_max_it = max_element(begin(row2), end(row2));
	int row2_max{(*row2_max_it)()};
	EXPECT_EQ(2, row2_max);
}

TEST_F(CostMatrixTest, GetColumn) {
	// iterate over matrix2 with columns
	CostMatrix::CostVector<Column> column0{
		matrix2_ptr->GetColumn(matrix2_ptr->GetActualColumnNum(0))};
	CostMatrix::CostVector<Column> column1{
		matrix2_ptr->GetColumn(matrix2_ptr->GetActualColumnNum(1))};
	CompareWithExpected(column0, {0, 0});
	CompareWithExpected(column1, {0, 0});

	auto column1_max_it = max_element(begin(column1), end(column1));
	int column1_max{(*column1_max_it)()};
	EXPECT_EQ(0, column1_max);
}

template <typename T>
void CompareWithExpected(T& matrix, vector<int> expected) {
	vector<int> actual;
	transform(begin(matrix), end(matrix), back_inserter(actual),
			[](const EdgeCost& cmi) { return cmi(); });
	EXPECT_EQ(expected, actual);
}
