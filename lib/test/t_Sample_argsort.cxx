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


// Test 1 : Sort nodes with argsort
void test_1()
{
    OStream fullprint(std::cout);
    fullprint << "+ Test 5 : sort nodes with argsort" << std::endl;

    // Create expected nodes and weights, then sort and check that nothing changed.
    Indices permutation = {9,5,1,6,10,11,4,2,8,13,12,14,0,3,7};
    Point pointColumn1 = {0.11, 0.11, 0.11, 0.11, 0.11, 0.5, 0.5, 0.5, 0.5, 0.5, 0.88, 0.88, 0.88, 0.88, 0.88};
    Point pointColumn2 = {0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95, 0.04, 0.23, 0.5, 0.76, 0.95};
    const Point weightsColumn = {0.03, 0.06, 0.07, 0.06, 0.03, 0.05, 0.10, 0.12, 0.10, 0.05, 0.03, 0.06, 0.07, 0.06, 0.03};
    UnsignedInteger size(pointColumn1.getDimension());
    UnsignedInteger dimension = 2;
    // Create expected nodes and weights, and shuffled nodes and weights
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
    // Lexicographic argsort
    Indices order(nodesShuffled.argsort());

    // Store sorted weights and nodes
    Point weightsSorted(size);
    Sample nodesSorted(size, dimension);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const UnsignedInteger index(order[i]);
      weightsSorted[i] = weightsShuffled[index];
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        nodesSorted(i, j) = nodesShuffled(index, j);
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
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
