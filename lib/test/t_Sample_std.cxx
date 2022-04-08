//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

using namespace OT;
using namespace OT::Test;

class TestObject : public Sample
{
public:
  TestObject() : Sample(1, 1) {}
  virtual ~TestObject() {}
};


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
    String separator("");
    for (UnsignedInteger j = 0; j < dimension; ++j, separator = ", ")
    {
      fullprint << separator << nodes(i, j);
    }
    fullprint << "]" << std::endl;
  }
  fullprint << std::endl;
}

// Test argsort : argsort
void test_argsort()
{
    Log::Show(Log::ALL);
    OStream fullprint(std::cout);
    fullprint << "+ Test argsort" << std::endl;

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
    fullprint << "    Shuffled :" << std::endl;
    printNodesAndWeights(nodesShuffled, weightsShuffled);
    // Lexicographic argsort
    Indices order(nodesShuffled.argsort());
    fullprint << "    order = " << order << std::endl;
    Indices orderExpected = {12,2,7,13,6,1,3,14,8,0,4,5,10,9,11};
    assert_equal(order, orderExpected);
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
    fullprint << "    Sorted :" << std::endl;
    printNodesAndWeights(nodesSorted, weightsSorted);
    // Test
    assert_equal(nodesSorted, nodesExpected);
    assert_equal(weightsSorted, weightsExpected);
    // Lexicographic argsort (decreasing order)
    Indices orderDecreasing(nodesShuffled.argsort(false));
    fullprint << "    orderDecreasing = " << orderDecreasing << std::endl;
    Indices orderDecreasingExpected = {11,9,10,5,4,0,8,14,3,1,6,13,7,2,12};
    assert_equal(orderDecreasing, orderDecreasingExpected);
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

    // We create an empty Sample
    Sample sample1(0, 2);
    sample1.setName("Sample1");

    // We populate the empty sample
    Point point1(2);
    point1[0] = 10.;
    point1[1] = 20.;
    Point point2(2);
    point2[0] = 11.;
    point2[1] = 21.;
    Point point3(2);
    point3[0] = 12.;
    point3[1] = 22.;
    sample1.add(point1);
    sample1.add(point2);
    sample1.add(point3);
    fullprint << "sample1=" << sample1 << std::endl;

    // We get the second element of the sample
    Point secondElement = sample1[1];
    fullprint << "second element=" << secondElement << std::endl;

    // We set the third element to a valid new element
    Point newPoint(2);
    newPoint[0] = 1000;
    newPoint[1] = 2000;
    sample1[2] = newPoint;
    fullprint << "sample1=" << sample1 << std::endl;

    Point translation(2, 5.0);
    sample1 += translation;
    fullprint << "after a translation of vector=" << translation << " sample1=" << sample1 << std::endl;

    Point scaling(2, 2.0);
    sample1 *= scaling;
    fullprint << "after a scaling of vector=" << scaling << " sample1=" << sample1 << std::endl;

    Sample sample2(10, point1);
    fullprint << "sample2=" << sample2 << std::endl;

    sample2[5] = point2;
    fullprint << "sample2=" << sample2 << std::endl;


    // We try to create a sample with 5 times the same point
    Point samePoint(3);
    samePoint[0] = 1000.;
    samePoint[1] = 2000.;
    samePoint[2] = 3000.;
    Sample sample3(5, samePoint);
    fullprint << "sample3=" << sample3 << std::endl;

    // We append a new point to the sample...
    Point anotherNewPoint(3);
    anotherNewPoint[0] = -1000.;
    anotherNewPoint[1] = -2000.;
    anotherNewPoint[2] = -3000.;
    sample3.add(anotherNewPoint);
    fullprint << "sample3=" << sample3 << std::endl;

    // ... then the first point again
    sample3.add(samePoint);
    fullprint << "sample3=" << sample3 << std::endl;

    // We find the new point into the sample
    UnsignedInteger pos = sample3.find(anotherNewPoint);
    //fullprint << "pos=" << pos << std::endl;
    if (pos != 5)
    {
      OSS oss;
      oss << "Error in finding point into sample. Expected 5. Got " << pos;
      throw TestFailed(oss);
    }

    //for(UnsignedInteger i=0; i<sample3.getSize(); ++i)
    //  fullprint << "sample3[" << i << "]=" << sample3[i] << std::endl;

    // We erase the new point from the sample
    sample3.erase(pos);
    fullprint << "sample3=" << sample3 << std::endl;

    // Partial copy constructor containing sample2[4:7]
    Sample sample4(sample2, 4, 8);
    fullprint << "sample4=" << sample4 << std::endl;

    // Internal storage
    Sample sample5(3, 1);
    sample5[0][0] = 1.0;
    sample5[1][0] = 3.0;
    sample5[2][0] = 2.0;
    fullprint << "sample5=" << sample5.asPoint() << std::endl;
    fullprint << "sample5 (linear)=" << sample5.asPoint() << std::endl;

    // Selection
    Indices indices(5);
    indices[0] = 1;
    indices[1] = 0;
    indices[2] = 2;
    indices[3] = 1;
    indices[4] = 2;
    fullprint << "selection=" << sample5.select(indices) << std::endl;
    
    // argsort
    test_argsort();
    try
    {
      // We get the tenth element of the sample
      // THIS SHOULD NORMALY FAIL
      Point tenthElement = sample1.at(9);

      // Normally, we should never go here
      throw TestFailed("Exception has NOT been thrown or catched !");

    }
    catch (OutOfBoundException & ex)
    {
      // Nothing to do
    }


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
