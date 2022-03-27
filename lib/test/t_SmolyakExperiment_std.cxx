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

// Simultaneously print the nodes and weights
void printNodesAndWeights(Sample & nodes, Point & weights)
{
  const UnsignedInteger size = nodes.getSize();
  const UnsignedInteger dimension = nodes.getDimension();
  OStream fullprint(std::cout);
  fullprint << "printNodesAndWeights" << std::endl;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    fullprint << "[" << i << "] " << weights[i] << " : (" ;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      fullprint << nodes(i, j) << " ";
    } // loop over dimensions
    fullprint << ")" << std::endl;
  } // loop over points
}

// Round sample to given number of digits
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
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform, with 3 nodes
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform, with 5 nodes
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);  
    experimentCollection.add(marginalExperiment2);
    //
    const UnsignedInteger level = 3;
    SmolyakExperiment experiment(experimentCollection, level);
    Point weights(0);
    fullprint << "generateWithWeights()" << std::endl;
    Sample nodes(experiment.generateWithWeights(weights));
    //
    UnsignedInteger numberOfDigits = 10;
    roundSample(nodes, numberOfDigits);
    sortNodesAndWeights(nodes, weights);
    fullprint << "nodes = " << std::endl;
    printNodesAndWeights(nodes, weights);
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
    fullprint << "Expected :" << std::endl;
    sortNodesAndWeights(nodesExpected, weightsExpected);
    printNodesAndWeights(nodesExpected, weightsExpected);
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodesExpected, nodes, rtol, atol);
    assert_almost_equal(weightsExpected, weights, rtol, atol);
}

// Test #2 : check hasUniformWeights
void test_2()
{
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform, with 3 nodes
    const Uniform distribution1(0.0, 1.0);
    const GaussProductExperiment marginalExperiment1(distribution1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform, with 5 nodes
    const Uniform distribution2(0.0, 1.0);
    const GaussProductExperiment marginalExperiment2(distribution2);  
    experimentCollection.add(marginalExperiment2);
    //
    const UnsignedInteger level = 3;
    SmolyakExperiment experiment(experimentCollection, level);
    const bool hasUniformWeights = experiment.hasUniformWeights();
    assert_equal(hasUniformWeights, false);
    
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
