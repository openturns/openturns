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
  fullprint << "size = " << size << std::endl;
  fullprint << "dimension = " << dimension << std::endl;
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test 1
    // Create expected nodes and weights, then sort and check that nothing changed.
    fullprint << "+ Test 1" << std::endl;
    Point column_1 = {0.11, 0.11, 0.11, 0.11, 0.11, 0.5, 0.5, 0.5, 0.5, 0.5, 0.88, 0.88, 0.88, 0.88, 0.88};
    Point column_2 = {0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95};
    UnsignedInteger size(column_1.getDimension());
    UnsignedInteger dimension = 2;
    Sample nodes_expected(size, dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      nodes_expected(i, 0) = column_1[i];
      nodes_expected(i, 1) = column_2[i];
    }
    const Point weights_expected = {0.03, 0.06, 0.07, 0.06, 0.03, 0.05, 0.10, 0.12, 0.10, 0.05, 0.03, 0.06, 0.07, 0.06, 0.03};
    fullprint << "nodes_expected = " << nodes_expected << std::endl;
    fullprint << "weights_expected = " << weights_expected << std::endl;
    //
    Point weights(weights_expected);
    Sample nodes(nodes_expected);
    //
    sortNodesAndWeights(nodes, weights);
    fullprint << "nodes = " << nodes << std::endl;
    fullprint << "weights = " << weights << std::endl;
    //
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodes_expected, nodes, rtol, atol);
    assert_almost_equal(weights_expected, weights, rtol, atol);
    //
    // Test 2 : sort with std::sort
    fullprint << "+ Test 2 : sort with std::sort" << std::endl;
    UnsignedInteger size_sort = 10;
    std::array<int, 10> s = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    // 2.1 : default comparison
    std::sort(s.begin(), s.end());
    fullprint << "sorted with the default operator <" << std::endl;
    for (UnsignedInteger i = 0; i < size_sort; ++i)
    {
        fullprint << s[i] << " ";
    }
    fullprint << std::endl;
    // 2.2 : custom comparison
    struct {
        bool operator()(int a, int b) const { return a < b; }
    } customLess;
    std::sort(s.begin(), s.end(), customLess);
    fullprint << "sorted with custom operator" << std::endl;
    for (UnsignedInteger i = 0; i < size_sort; ++i)
    {
        fullprint << s[i] << " ";
    }
    fullprint << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
