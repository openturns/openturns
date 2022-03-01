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

// Compare two points, according to lexicographic order
/** Returns -1 if node_1 < node_2, 
 *           0 if node1 == node_2,
 *           1 otherwise.
 */
UnsignedInteger comparePoints(const Point point1, const Point point2)
{
  const UnsignedInteger dimension(point1.getDimension());
  if (!(point2.getDimension() == dimension))
    throw InvalidArgumentException(HERE) << "Error: Dimension of point 1 is " << dimension << " but dimension of point 2 is " << point2.getDimension();
  UnsignedInteger comparison = 0;
  for (UnsignedInteger k = 0; k < dimension; ++k)
  {
    if (point1[k] < point2[k])
    {
      comparison = -1;
      break;
    }
    if (point1[k] > point2[k])
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
  fullprint << "sortNodesAndWeights()" << std::endl;
  const UnsignedInteger size(weights.getDimension());
  fullprint << "size = " << size << std::endl;
  UnsignedInteger signOfIMinusJ;
  if (size == 0)
  {
    return;      
  }
  for (UnsignedInteger i = 0; i < size - 1; ++i)
  {
    fullprint << "i = " << i << std::endl;
    for (UnsignedInteger j = i + 1; j < size; ++j)
    {
      fullprint << "j = " << j << std::endl;
      signOfIMinusJ = comparePoints(nodes[i], nodes[j]);
      if (signOfIMinusJ == 1)
      {
        const Point tempPoint(nodes[i]);
        nodes[i] = nodes[j];
        nodes[j] = tempPoint;
        const Scalar tempWeight(weights[i]);
        weights[i] = weights[j];
        weights[j] = tempWeight;
      }
    }
  }
}

// Test #1 : 2 experiments with dimensions 1
void test_1()
{
    Log::Show(Log::ALL);
    OStream fullprint(std::cout);
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform, with 3 nodes
    const Uniform distribution1(0.0, 1.0);
    Indices marginalSizes1(0);
    marginalSizes1.add(3);
    const GaussProductExperiment marginalExperiment1(distribution1, marginalSizes1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform, with 5 nodes
    const Uniform distribution2(0.0, 1.0);
    Indices marginalSizes2(0);
    marginalSizes2.add(5);
    const GaussProductExperiment marginalExperiment2(distribution2, marginalSizes2);  
    experimentCollection.add(marginalExperiment2);
    //
    const UnsignedInteger level = 3;
    SmolyakExperiment experiment(experimentCollection, level);
    Point weights(0);
    fullprint << "generateWithWeights()" << std::endl;
    Sample nodes(experiment.generateWithWeights(weights));
    //
    fullprint << "sortNodesAndWeights()" << std::endl;
    sortNodesAndWeights(nodes, weights);
    fullprint << "sort done." << std::endl;
    //
    const int size(weights.getDimension());
    const int dimension(nodes.getDimension());
    const int weightDimension(weights.getDimension());
    assert_equal(size, 15);
    assert_equal(dimension, 2);
    assert_equal(weightDimension, 15);
    //
    Point column_1 = {0.11270, 0.11270, 0.11270, 0.11270, 0.11270, 0.5, 0.5, 0.5, 0.5, 0.5, 0.88729, 0.88729, 0.88729, 0.88729, 0.88729};
    Point column_2 = {0.04691, 0.23076, 0.5, 0.76923, 0.95309, 0.04691, 0.23076, 0.5, 0.76923, 0.95309, 0.04691, 0.23076, 0.5, 0.76923, 0.95309};
    Sample nodesExpected(size, dimension);
    for (int i = 0; i < size; ++i)
    {
      nodesExpected(i, 0) = column_1[i];
      nodesExpected(i, 1) = column_2[i];
    }
    const Point weightsExpected = {0.03290, 0.06647, 0.07901, 0.06647, 0.03290, 0.05265, 0.10636, 0.12642, 0.10636, 0.05265, 0.03290, 0.06647, 0.07901, 0.06647, 0.03290};
    const Scalar rtol = 1.0e-5;
    const Scalar atol = 1.0e-5;
    assert_almost_equal(nodesExpected, nodes, rtol, atol);
    assert_almost_equal(weightsExpected, weights, rtol, atol);
}

// Test #3 : check hasUniformWeights
void test_3()
{
    SmolyakExperiment::WeightedExperimentCollection experimentCollection(0);
    // Marginal 0: Uniform, with 3 nodes
    const Uniform distribution1(0.0, 1.0);
    Indices marginalSizes1(0);
    marginalSizes1.add(3);
    const GaussProductExperiment marginalExperiment1(distribution1, marginalSizes1);  
    experimentCollection.add(marginalExperiment1);
    // Marginal 1: Uniform, with 5 nodes
    const Uniform distribution2(0.0, 1.0);
    Indices marginalSizes2(0);
    marginalSizes2.add(5);
    const GaussProductExperiment marginalExperiment2(distribution2, marginalSizes2);  
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
    test_3();
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
