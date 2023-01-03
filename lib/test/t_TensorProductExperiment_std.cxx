//                         -*- C++ -*-
/**
 *  @brief The test file of class TensorProductExperiment for standard methods
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
    const UnsignedInteger index(order[i]);
    weights[i] = weightsUnordered[index];
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      nodes(i, j) = nodesUnordered(index, j);
    } // loop over dimensions
  } // loop over points
}

// Test #1 : 2 experiments with dimensions 1
void test_1()
{
  Log::Show(Log::ALL);
  TensorProductExperiment::WeightedExperimentCollection experimentCollection(0);
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
  TensorProductExperiment experiment(experimentCollection);
  Point weights(0);
  Sample nodes(experiment.generateWithWeights(weights));
  //
  sortNodesAndWeights(nodes, weights);
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
  //
  const int sizeExperiment(experiment.getSize());
  assert_equal(sizeExperiment, 15);
  //
  Distribution distribution(experiment.getDistribution());
  BlockIndependentDistribution::DistributionCollection collection(2);
  collection[0] = distribution1;
  collection[1] = distribution2;
  Distribution expected_distribution = BlockIndependentDistribution(collection);
  assert_equal(distribution, expected_distribution);
}

// Test #2 : 2 experiments with dimensions [2, 3]
void test_2()
{
  Log::Show(Log::ALL);
  TensorProductExperiment::WeightedExperimentCollection experimentCollection(0);
  // Marginal 0: [Uniform * 2] with sizes [3, 2]
  const Uniform uniform_1(-1.0, 1.0);
  const Uniform uniform_2(-1.0, 1.0);
  ComposedDistribution::DistributionCollection distributionCollection1;
  distributionCollection1.add(Distribution(uniform_1));
  distributionCollection1.add(Distribution(uniform_2));
  ComposedDistribution distribution3(distributionCollection1);
  Indices marginalSizes3(0);
  marginalSizes3.add(3);
  marginalSizes3.add(2);
  const GaussProductExperiment marginalExperiment3(distribution3, marginalSizes3);
  experimentCollection.add(marginalExperiment3);
  // Marginal 1: [Normal * 2, with sizes [2, 2, 1]
  const Normal normal_1(0.0, 1.0);
  const Normal normal_2(0.0, 1.0);
  const Normal normal_3(0.0, 1.0);
  ComposedDistribution::DistributionCollection distributionCollection2;
  distributionCollection2.add(Distribution(normal_1));
  distributionCollection2.add(Distribution(normal_2));
  distributionCollection2.add(Distribution(normal_3));
  ComposedDistribution distribution4(distributionCollection2);
  Indices marginalSizes4(0);
  marginalSizes4.add(2);
  marginalSizes4.add(2);
  marginalSizes4.add(1);
  const GaussProductExperiment marginalExperiment4(distribution4, marginalSizes4);
  experimentCollection.add(marginalExperiment4);
  //
  TensorProductExperiment experiment(experimentCollection);
  Point weights(0);
  Sample nodes(experiment.generateWithWeights(weights));
  //
  sortNodesAndWeights(nodes, weights);
  //
  const int size(weights.getDimension());
  const int dimension(nodes.getDimension());
  const int weightDimension(weights.getDimension());
  assert_equal(size, 24);
  assert_equal(dimension, 5);
  assert_equal(weightDimension, 24);
  //
  Point column1 = {-0.77459, -0.77459, -0.77459, -0.77459, -0.77459, -0.77459, -0.77459, -0.77459, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.77459, 0.77459, 0.77459, 0.77459, 0.77459, 0.77459, 0.77459, 0.77459};
  Point column2 = {-0.57735, -0.57735, -0.57735, -0.57735, 0.57735, 0.57735, 0.57735, 0.57735, -0.57735, -0.57735, -0.57735, -0.57735, 0.57735, 0.57735, 0.57735, 0.57735, -0.57735, -0.57735, -0.57735, -0.57735, 0.57735, 0.57735, 0.57735, 0.57735};
  Point column3 = {-1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0};
  Point column4 = {-1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0};
  Point column5 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Sample nodesExpected(size, dimension);
  for (int i = 0; i < size; ++i)
  {
    nodesExpected(i, 0) = column1[i];
    nodesExpected(i, 1) = column2[i];
    nodesExpected(i, 2) = column3[i];
    nodesExpected(i, 3) = column4[i];
    nodesExpected(i, 4) = column5[i];
  }
  const Point weightsExpected = {0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222, 0.0347222};
  const Scalar rtol = 1.0e-5;
  const Scalar atol = 1.0e-5;
  assert_almost_equal(nodesExpected, nodes, rtol, atol);
  assert_almost_equal(weightsExpected, weights, rtol, atol);
}
// Test #3 : check hasUniformWeights
void test_3()
{
  TensorProductExperiment::WeightedExperimentCollection experimentCollection(0);
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
  TensorProductExperiment experiment(experimentCollection);
  const bool hasUniformWeights = experiment.hasUniformWeights();
  assert_equal(hasUniformWeights, false);

}

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    test_1();
    test_2();
    test_3();
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
