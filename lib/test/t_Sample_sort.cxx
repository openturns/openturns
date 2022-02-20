//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample for argsort
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include <algorithm>
#include <functional>
#include <array>

using namespace OT;
using namespace OT::Test;

// Compare two points, according to lexicographic order
/** Returns -1 if node_1 < node_2, 
 *           0 if node1 == node_2,
 *           1 otherwise.
 */
UnsignedInteger comparePoints(const Point point_1, const Point point_2)
{
  const UnsignedInteger dimension(point_1.getDimension());
  if (!(point_2.getDimension() == dimension))
    throw InvalidArgumentException(HERE) << "Error: Dimension of point 1 is " << dimension << " but dimension of point 2 is " << point_2.getDimension();
  UnsignedInteger comparison = 0;
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    if (point_1[k] < point_2[k])
    {
      comparison = -1;
      break;
    }
    if (point_1[k] > point_2[k])
    {
      comparison = 1;
      break;
    }
  }
  return comparison;
}

// Simultaneously sort the nodes and weights
void sortNodesAndWeights(Sample & nodes, Point & weights)
{
  OStream fullprint(std::cout);
  fullprint << "+ Simultaneously sort the nodes and weights" << std::endl;
  const UnsignedInteger size(weights.getDimension());
  const UnsignedInteger dimension(nodes.getDimension());
  fullprint << "  size = " << size << std::endl;
  fullprint << "  dimension = " << dimension << std::endl;
  UnsignedInteger is_i_greater_than_j;
  for (UnsignedInteger i = 0; i < size - 1; ++i)
  {
    for (UnsignedInteger j = i + 1; j < size; ++j)
    {
      is_i_greater_than_j = comparePoints(nodes[i], nodes[j]);
      if (is_i_greater_than_j == 1)
      {
        const Point temp_point(nodes[i]);
        nodes[i] = nodes[j];
        nodes[j] = temp_point;
        const Scalar temp_weight(weights[i]);
        weights[i] = weights[j];
        weights[j] = temp_weight;
      }
    }
  }
}

// Print the nodes and weights
void printNodesAndWeights(Sample nodes, Point weights)
{
  OStream fullprint(std::cout);
  fullprint << "+ Print the nodes and weights" << std::endl;
  const UnsignedInteger size(weights.getDimension());
  const UnsignedInteger dimension(nodes.getDimension());
  fullprint << "  size = " << size << std::endl;
  fullprint << "  dimension = " << dimension << std::endl;
  fullprint << "weight, node" << std::endl;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    fullprint << weights[i] << " : [";
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
        if (j  == 0)
        {
          fullprint << nodes(i, j);
        } else {
          fullprint << ", " << nodes(i, j);
        }
    }
    fullprint << "]" << std::endl;
  }
  fullprint << std::endl;
}

// Test 1
void test_1()
{
    OStream fullprint(std::cout);
    // Create expected nodes and weights, then sort and check that nothing changed.
    fullprint << "+ Test 1 : sort with custom algorithm" << std::endl;
    Point column1 = {0.11, 0.11, 0.11, 0.11, 0.11, 0.5, 0.5, 0.5, 0.5, 0.5, 0.88, 0.88, 0.88, 0.88, 0.88};
    Point column2 = {0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95};
    UnsignedInteger size(column1.getDimension());
    UnsignedInteger dimension = 2;
    Sample nodesExpected(size, dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = column1[i];
      nodesExpected(i, 1) = column2[i];
    }
    const Point weightsExpected = {0.03, 0.06, 0.07, 0.06, 0.03, 0.05, 0.10, 0.12, 0.10, 0.05, 0.03, 0.06, 0.07, 0.06, 0.03};
    fullprint << "  nodesExpected = " << nodesExpected << std::endl;
    fullprint << "  weightsExpected = " << weightsExpected << std::endl;
    //
    Point weights(weightsExpected);
    Sample nodes(nodesExpected);
    //
    sortNodesAndWeights(nodes, weights);
    printNodesAndWeights(nodes, weights);
    //
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodesExpected, nodes, rtol, atol);
    assert_almost_equal(weightsExpected, weights, rtol, atol);
}

// Test 2 : sort with std::sort
void test_2()
{
    OStream fullprint(std::cout);
    fullprint << "+ Test 2 : sort with std::sort" << std::endl;
    UnsignedInteger sizeSort = 10;
    std::array<int, 10> s = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    // 2.1 : default comparison
    std::sort(s.begin(), s.end());
    fullprint << "  sorted with the default operator <" << std::endl;
    for (UnsignedInteger i = 0; i < sizeSort; ++i)
    {
        fullprint << s[i] << " ";
    }
    fullprint << std::endl;
    // 2.2 : custom comparison
    struct {
        bool operator()(int a, int b) const { return a < b; }
    } customLess;
    std::sort(s.begin(), s.end(), customLess);
    fullprint << "  sorted with custom operator" << std::endl;
    for (UnsignedInteger i = 0; i < sizeSort; ++i)
    {
        fullprint << s[i] << " ";
    }
    fullprint << std::endl;
}

// Test 3 : Sort weights with pairs
void test_3()
{
    OStream fullprint(std::cout);
    fullprint << "+ Test 3 : sort weights with pairs" << std::endl;

    // Create expected nodes and weights, then sort and check that nothing changed.
    Point column1 = {0.11, 0.11, 0.11, 0.11, 0.11, 0.5, 0.5, 0.5, 0.5, 0.5, 0.88, 0.88, 0.88, 0.88, 0.88};
    Point column2 = {0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95};
    UnsignedInteger size(column1.getDimension());
    UnsignedInteger dimension = 2;
    Sample nodesExpected(size, dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = column1[i];
      nodesExpected(i, 1) = column2[i];
    }
    const Point weightsExpected = {0.03, 0.06, 0.07, 0.06, 0.03, 0.05, 0.10, 0.12, 0.10, 0.05, 0.03, 0.06, 0.07, 0.06, 0.03};
    fullprint << "  nodesExpected = " << nodesExpected << std::endl;
    fullprint << "  weightsExpected = " << weightsExpected << std::endl;
    // Create pairs
    Collection< std::pair<Scalar, UnsignedInteger> > weightsPairs(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      weightsPairs[i] = std::pair<Scalar, UnsignedInteger>(weightsExpected[i], i);

    std::sort(weightsPairs.begin(), weightsPairs.end());

    // Store sorted weights and nodes
    Point weightsSorted(size);
    Sample nodesSorted(size, dimension);
    UnsignedInteger sortedIndex;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      sortedIndex = weightsPairs[i].second;
      weightsSorted[i] = weightsExpected[sortedIndex];
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        nodesSorted(i, j) = nodesExpected(sortedIndex, j);
      }
    }
    printNodesAndWeights(nodesSorted, weightsSorted);
}

// Test 4 : Sort nodes with pairs
void test_4()
{
    OStream fullprint(std::cout);
    fullprint << "+ Test 4 : sort nodes with pairs" << std::endl;

    // Create expected nodes and weights, then sort and check that nothing changed.
    Indices permutation = {9,5,1,6,10,11,4,2,8,13,12,14,0,3,7};
    Point pointColumn1 = {0.11, 0.11, 0.11, 0.11, 0.11, 0.5, 0.5, 0.5, 0.5, 0.5, 0.88, 0.88, 0.88, 0.88, 0.88};
    Point pointColumn2 = {0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95};
    const Point weightsColumn = {0.03, 0.06, 0.07, 0.06, 0.03, 0.05, 0.10, 0.12, 0.10, 0.05, 0.03, 0.06, 0.07, 0.06, 0.03};
    UnsignedInteger size(pointColumn1.getDimension());
    UnsignedInteger dimension = 2;
    Sample nodesExpected(size, dimension);
    Point weightsExpected(size);
    Sample nodesShuffled(size, dimension);
    Point weightsShuffled(size);
    UnsignedInteger index;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = pointColumn1[i];
      nodesExpected(i, 1) = pointColumn2[i];
      weightsExpected[i] = weightsColumn[i];
      index = permutation[i];
      nodesShuffled(i, 0) = pointColumn1[index];
      nodesShuffled(i, 1) = pointColumn2[index];
      weightsShuffled[i] = weightsColumn[index];
    }
    fullprint << "  Expected : " << std::endl;
    printNodesAndWeights(nodesExpected, weightsExpected);
    fullprint << "  Shuffled : " << std::endl;
    printNodesAndWeights(nodesShuffled, weightsShuffled);
    // Create pairs
    Collection< std::pair<Point, UnsignedInteger> > pointsPairs(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      pointsPairs[i] = std::pair<Point, UnsignedInteger>(nodesShuffled[i], i);

    std::sort(pointsPairs.begin(), pointsPairs.end());

    // Store sorted weights and nodes
    Point weightsSorted(size);
    Sample nodesSorted(size, dimension);
    UnsignedInteger sortedIndex;
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      sortedIndex = pointsPairs[i].second;
      weightsSorted[i] = weightsShuffled[sortedIndex];
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        nodesSorted(i, j) = nodesShuffled(sortedIndex, j);
      }
    }
    printNodesAndWeights(nodesSorted, weightsSorted);
}

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    test_1();
    test_2();
    test_3();
    test_4();
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
