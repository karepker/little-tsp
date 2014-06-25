#include "cost_matrix.hpp"

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <vector>

#include "cost_matrix_integer.hpp"
#include "edge.hpp"
#include "graph/mock.hpp"
#include "util.hpp"
#include "matrix.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using std::back_inserter;
using std::max_element;
using std::min_element;
using std::numeric_limits;
using std::transform;
using std::unique_ptr;
using std::vector;

using ::testing::Return;

const int infinity{numeric_limits<int>::max()};

const vector<Edge> include1;
const vector<Edge> exclude1{{0, 0}};
const vector<Edge> include2{{0, 2}};
const vector<Edge> exclude2{{0, 0}};

const Matrix<int> graph_weights{3, {6, 2, 4, 4, 5, 3, 8, 9, 7}};

template<typename T>
static void CompareWithVector(T matrix, vector<int> expected);

class CostMatrixTest : public ::testing::Test {
public:
	CostMatrixTest(); 
protected:
	// I need unique_ptrs because graph functions get called in the constructor
	// of matrix, and I need time to set expectations for mock calls
	unique_ptr<CostMatrix> matrix1_ptr, matrix2_ptr;

private:
	MockGraph graph;
};

CostMatrixTest::CostMatrixTest() {
	// set expectations for the graph
	for (int i{0}; i < graph_weights.GetNumRows(); ++i) {
		for (int j{0}; j < graph_weights.GetNumColumns(); ++j) {
			EXPECT_CALL(graph, Predicate(i, j)).Times(2).WillRepeatedly(
					Return(graph_weights(i, j)));
		}
	}

	EXPECT_CALL(graph, GetNumVertices()).WillRepeatedly(Return(3));

	// construct the matrix after we set expectations for the graph
	matrix1_ptr = unique_ptr<CostMatrix>{
		new CostMatrix{graph, include1, exclude1}};
	matrix2_ptr = unique_ptr<CostMatrix>{
		new CostMatrix{graph, include2, exclude2}};

	// reduce the matrices
	EXPECT_EQ(13, matrix1_ptr->ReduceMatrix());
	EXPECT_EQ(13, matrix2_ptr->ReduceMatrix());
}

TEST_F(CostMatrixTest, size) {
	EXPECT_EQ(3, matrix1_ptr->size());
	EXPECT_EQ(3, matrix2_ptr->size());
}

TEST_F(CostMatrixTest, ReduceMatrix) {
	// make sure the matrices were reduced correctly
	CompareWithVector(*matrix1_ptr, {infinity, 0, 2, 0, 2, 0, 0, 2, 0});
	CompareWithVector(*matrix2_ptr, {0, 0, 0, 0});
}

TEST_F(CostMatrixTest, GetRow) {
	// iterate over matrix1 with rows
	EXPECT_TRUE(matrix1_ptr->IsRowAvailable(0));
	EXPECT_TRUE(matrix1_ptr->IsRowAvailable(1));
	EXPECT_TRUE(matrix1_ptr->IsRowAvailable(2));

	CostMatrix::CostRow row0{matrix1_ptr->GetRow(0)};
	CostMatrix::CostRow row1{matrix1_ptr->GetRow(1)};
	CostMatrix::CostRow row2{matrix1_ptr->GetRow(2)};
	CompareWithVector(row0, {infinity, 0, 2});
	CompareWithVector(row1, {0, 2, 0});
	CompareWithVector(row2, {0, 2, 0});

	auto row2_max_it = max_element(row2.begin(), row2.end());
	int row2_max{row2_max_it->operator()()};
	EXPECT_EQ(2, row2_max);
}

TEST_F(CostMatrixTest, GetColumn) {
	// iterate over matrix2 with columns
	EXPECT_TRUE(matrix2_ptr->IsColumnAvailable(0));
	EXPECT_TRUE(matrix2_ptr->IsColumnAvailable(1));
	EXPECT_FALSE(matrix2_ptr->IsColumnAvailable(2));
	CostMatrix::CostColumn column0{matrix2_ptr->GetColumn(0)};
	CostMatrix::CostColumn column1{matrix2_ptr->GetColumn(1)};
	EXPECT_THROW(matrix2_ptr->GetColumn(2), NotAvailableError);
	CompareWithVector(column0, {0, 0});
	CompareWithVector(column1, {0, 0});

	auto column1_max_it = max_element(column1.begin(), column1.end());
	int column1_max{column1_max_it->operator()()};
	EXPECT_EQ(0, column1_max);
}

template<typename T>
void CompareWithVector(T matrix, vector<int> expected) {
	vector<int> actual;
	transform(matrix.begin(), matrix.end(), back_inserter(actual),
			[](const CostMatrixInteger& cmi) { return cmi(); });
	EXPECT_EQ(expected, actual);
}
