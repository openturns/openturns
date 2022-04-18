//                                               -*- C++ -*-
/**
 *  @brief The test file of class SmolyakExperiment for standard methods
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
#include <iostream>
#include <map>
#include <string>
#include <string_view>

using namespace OT;
using namespace OT::Test;

// Simultaneously sort the nodes and weights
void sortNodesAndWeights(Sample & nodes, Point & weights)
{
  const Indices order(nodes.argsort());
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger dimension = nodes.getDimension();
  Sample nodesUnordered(nodes);
  Point weightsUnordered(weights);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const int index(order[i]);
    weights[i] = weightsUnordered[index];
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      nodes(i, j) = nodesUnordered(index, j);
    } // loop over dimensions
  } // loop over points
}

// Sort the nodes 
void sortNodes(Sample & nodes)
{
  const Indices order(nodes.argsort());
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger dimension = nodes.getDimension();
  Sample nodesUnordered(nodes);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const int index(order[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      nodes(i, j) = nodesUnordered(index, j);
    } // loop over dimensions
  } // loop over points
}

// Simultaneously print the nodes and weights
void printNodesAndWeights(Sample & nodes, Point & weights)
{
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger dimension = nodes.getDimension();
  OStream fullprint(std::cout);
  fullprint << "printNodesAndWeights. Size = " << size << std::endl;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    fullprint << "[" << i << "] : [" ;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      fullprint << nodes(i, j) << " ";
    } // loop over dimensions
    fullprint << "] = " << weights[i] << std::endl;
  } // loop over points
}

// Round sample to given number of decimal digits
void roundSample(Sample & nodes, UnsignedInteger numberOfDigits)
{
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger dimension = nodes.getDimension();
  const Scalar factor = std::pow(10, numberOfDigits);
  long long rounded;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      rounded = (long long) (factor * nodes(i, j));
      nodes(i, j) = rounded / factor;
    } // loop over dimensions
  } // loop over points
}

// Test #1 : 2 experiments with dimensions 1
void test_1()
{
    Log::Show(Log::ALL);
    OStream fullprint(std::cout);
    fullprint << "test_1" << std::endl;
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);
    experimentCollection.add(marginalExperiment2);
    //
    const UnsignedInteger level = 3;
    SmolyakExperiment experiment(experimentCollection, level);
    Point weights(0);
    Sample nodes(experiment.generateWithWeights(weights));
    printNodesAndWeights(nodes, weights);
    const int experimentSize = experiment.getSize();
    assert_equal(experimentSize, 13);
    //
    UnsignedInteger numberOfDigits = 14;
    roundSample(nodes, numberOfDigits);
    sortNodesAndWeights(nodes, weights);
    //
    const int size(nodes.getSize());
    const int dimension(nodes.getDimension());
    const int weightDimension(weights.getDimension());
    assert_equal(size, 13);
    assert_equal(dimension, 2);
    assert_equal(weightDimension, 13);
    //
    Point column_1 = {0.112702, 0.211325, 0.211325, 0.211325, 0.5, 0.5, 0.5, 0.5, 0.5, 0.788675, 0.788675, 0.788675, 0.887298};
    Point column_2 = {0.5, 0.211325, 0.5, 0.788675, 0.112702, 0.211325, 0.5, 0.788675, 0.887298, 0.211325, 0.5, 0.788675, 0.5};
    Sample nodesExpected(size, dimension);
    for (int i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = column_1[i];
      nodesExpected(i, 1) = column_2[i];
    }
    Point weightsExpected = {0.277778, 0.25, -0.5, 0.25, 0.277778, -0.5, 0.888888, -0.5, 0.277778, 0.25, -0.5, 0.25, 0.277778};
    sortNodesAndWeights(nodesExpected, weightsExpected);
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodesExpected, nodes, rtol, atol);
    assert_almost_equal(weightsExpected, weights, rtol, atol);
    // Test generate()
    Sample nodesBis(experiment.generate());
    roundSample(nodesBis, numberOfDigits);
    sortNodes(nodesBis);
    assert_almost_equal(nodesExpected, nodesBis, rtol, atol);
}

// Test #2 : check hasUniformWeights
void test_2()
{
    OStream fullprint(std::cout);
    fullprint << "test_2" << std::endl;
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);  
    experimentCollection.add(marginalExperiment2);
    //
    const UnsignedInteger level = 3;
    SmolyakExperiment experiment(experimentCollection, level);
    const bool hasUniformWeights = experiment.hasUniformWeights();
    assert_equal(hasUniformWeights, false);
    
}

// Test #3 : 3 experiments with dimensions 1
void test_3()
{
    Log::Show(Log::ALL);
    OStream fullprint(std::cout);
    fullprint << "test_3" << std::endl;
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);
    experimentCollection.add(marginalExperiment2);
    // Marginal 2: Uniform
    const Uniform distribution3(0.0, 1.0);
    const GaussProductExperiment marginalExperiment3(distribution3);  
    experimentCollection.add(marginalExperiment3);
    //
    const UnsignedInteger level = 3;
    SmolyakExperiment experiment(experimentCollection, level);
    Point weights(0);
    Sample nodes(experiment.generateWithWeights(weights));
    const int experimentSize = experiment.getSize();
    assert_equal(experimentSize, 25);
    //
    UnsignedInteger numberOfDigits = 14;
    roundSample(nodes, numberOfDigits);
    sortNodesAndWeights(nodes, weights);
    //
    const int size(nodes.getSize());
    const int dimension(nodes.getDimension());
    const int weightDimension(weights.getDimension());
    assert_equal(size, 25);
    assert_equal(dimension, 3);
    assert_equal(weightDimension, 25);
    //
    Point column_1 = {0.11270167, 0.21132487, 0.21132487, 0.21132487, 0.21132487, 0.21132487, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.78867513, 0.78867513, 0.78867513, 0.78867513, 0.78867513, 0.88729833};
    Point column_2 = {0.5, 0.21132487, 0.5, 0.5, 0.5, 0.78867513, 0.11270167, 0.21132487, 0.21132487, 0.21132487, 0.5, 0.5, 0.5, 0.5, 0.5, 0.78867513, 0.78867513, 0.78867513, 0.88729833, 0.21132487, 0.5, 0.5, 0.5, 0.78867513, 0.5       };
    Point column_3 = {0.5, 0.5, 0.21132487, 0.5, 0.78867513, 0.5, 0.5, 0.21132487, 0.5, 0.78867513, 0.11270167, 0.21132487, 0.5, 0.78867513, 0.88729833, 0.21132487, 0.5, 0.78867513, 0.5, 0.5, 0.21132487, 0.5, 0.78867513, 0.5, 0.5};
    Sample nodesExpected(size, dimension);
    for (int i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = column_1[i];
      nodesExpected(i, 1) = column_2[i];
      nodesExpected(i, 2) = column_3[i];
    }
    Point weightsExpected = {0.277778, 0.25, 0.25,-1, 0.25, 0.25, 0.277778, 0.25,-1, 0.25, 0.277778,-1,2.33333,-1, 0.277778, 0.25,-1, 0.25, 0.277778, 0.25, 0.25,-1, 0.25, 0.25, 0.277778};
    sortNodesAndWeights(nodesExpected, weightsExpected);
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodesExpected, nodes, rtol, atol);
    assert_almost_equal(weightsExpected, weights, rtol, atol);
}

// Test Point comparison
void test_4()
{
    std::cout << "test_4" << std::endl;
    ResourceMap::SetAsScalar( "SmolyakExperiment-DefaultPointRelativeEpsilon", 1.e-2);
    ResourceMap::SetAsScalar( "SmolyakExperiment-DefaultPointAbsoluteEpsilon", 1.e-2);
    //
    Point x1({0.1, 0.2});
    Point y1({0.3, 0.4});
    //
    std::cout << "(1) Compare = " << x1 << " and " << y1 << std::endl;
    bool comparison1 = SmolyakExperiment::comparePointsApproximately(x1, y1);
    assert_equal(comparison1, true);
    //
    std::cout << "(2) Compare = " << y1 << " and " << x1 << std::endl;
    bool comparison1_bis = SmolyakExperiment::comparePointsApproximately(y1, x1);
    assert_equal(comparison1_bis, false);
    //
    std::cout << "(3) Compare = " << x1 << " and " << x1 << std::endl;
    bool comparison3 = SmolyakExperiment::comparePointsApproximately(x1, x1);
    assert_equal(comparison3, false);
    //
    Point x1_bis({0.1001, 0.2001});
    std::cout << "(4) Compare = " << x1_bis << " and " << x1 << std::endl;
    bool comparison4 = SmolyakExperiment::comparePointsApproximately(x1_bis, x1);
    assert_equal(comparison4, false);
    //
    std::cout << "(5) Compare = " << x1 << " and " << x1_bis << std::endl;
    bool comparison5 = SmolyakExperiment::comparePointsApproximately(x1, x1_bis);
    assert_equal(comparison5, false);
    //
    Point zero({0.0, 0.0});
    Point zero_close({0.0001, 0.0001});
    std::cout << "(6) Compare = " << zero << " and " << zero_close << std::endl;
    bool comparison6 = SmolyakExperiment::comparePointsApproximately(zero, zero_close);
    assert_equal(comparison6, false);
    //
    std::cout << "(7) Compare = " << zero_close << " and " << zero << std::endl;
    bool comparison7 = SmolyakExperiment::comparePointsApproximately(zero_close, zero);
    assert_equal(comparison7, false);
}

// Test #5 : 2 experiments, level = 1
void test_5()
{
    Log::Show(Log::ALL);
    OStream fullprint(std::cout);
    fullprint << "test_5" << std::endl;
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);
    experimentCollection.add(marginalExperiment2);
    //
    const UnsignedInteger level = 1;
    SmolyakExperiment experiment(experimentCollection, level);
    Point weights(0);
    Sample nodes(experiment.generateWithWeights(weights));
    printNodesAndWeights(nodes, weights);
    const int experimentSize = experiment.getSize();
    assert_equal(experimentSize, 1);
    //
    UnsignedInteger numberOfDigits = 14;
    roundSample(nodes, numberOfDigits);
    sortNodesAndWeights(nodes, weights);
    //
    const int size(nodes.getSize());
    const int dimension(nodes.getDimension());
    const int weightDimension(weights.getDimension());
    assert_equal(size, 1);
    assert_equal(dimension, 2);
    assert_equal(weightDimension, 1);
    //
    Point column_1 = {0.5};
    Point column_2 = {0.5};
    Sample nodesExpected(size, dimension);
    for (int i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = column_1[i];
      nodesExpected(i, 1) = column_2[i];
    }
    Point weightsExpected = {1.0};
    sortNodesAndWeights(nodesExpected, weightsExpected);
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodesExpected, nodes, rtol, atol);
    assert_almost_equal(weightsExpected, weights, rtol, atol);
    // Test generate()
    Sample nodesBis(experiment.generate());
    roundSample(nodesBis, numberOfDigits);
    sortNodes(nodesBis);
    assert_almost_equal(nodesExpected, nodesBis, rtol, atol);
}

// Test #6 : 3 experiments, level = 2
void test_6()
{
    Log::Show(Log::ALL);
    OStream fullprint(std::cout);
    fullprint << "test_6" << std::endl;
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);
    experimentCollection.add(marginalExperiment2);
    // Marginal 2: Uniform
    const Uniform distribution3(0.0, 1.0);
    const GaussProductExperiment marginalExperiment3(distribution3);
    experimentCollection.add(marginalExperiment3);
    //
    const UnsignedInteger level = 2;
    SmolyakExperiment experiment(experimentCollection, level);
    Point weights(0);
    Sample nodes(experiment.generateWithWeights(weights));
    printNodesAndWeights(nodes, weights);
    const int experimentSize = experiment.getSize();
    assert_equal(experimentSize, 7);
    //
    const int size(nodes.getSize());
    const int dimension(nodes.getDimension());
    const int weightDimension(weights.getDimension());
    assert_equal(size, 7);
    assert_equal(dimension, 3);
    assert_equal(weightDimension, 7);
}

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    // test_1();
    // test_2();
    // test_3();
    // test_4();
    // test_5();
    test_6();
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
