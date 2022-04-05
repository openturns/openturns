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

// Test #3 : 3 experiments with dimensions 1
void test_3()
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
    // Marginal 2: Uniform, with 3 nodes
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

/* Comparison class with std:map interface. */
class NodeWeightCompare
{
public:
    NodeWeightCompare(const Scalar absoluteEpsilon, 
                      const Scalar relativeEpsilon):
                      absoluteEpsilon_(absoluteEpsilon)
                      , relativeEpsilon_(relativeEpsilon)
    {
        // Nothing to do
    };

    /* Compare two points, according to lexicographic order
    * Returns true if x < y, false otherwise.
    */
    bool operator()(const Point x, const Point y) const {
        const UnsignedInteger dimension = x.getDimension();
        if (y.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the two points must have the same dimension. Here x has dimension " << dimension << " while y has dimension " << y.getDimension();
        bool comparison = false;
        for (UnsignedInteger k = 0; k < dimension; ++k)
        {
            if (x[k] < y[k])
            {
                comparison = true;
                break;
            }
        }
        // std::cout << "Compare(" << x << ", " << y << ") = " << comparison << std::endl;
        return comparison;
    }
private:
    // Absolute tolerance for comparison
    Scalar absoluteEpsilon_;
    // Relative tolerance for comparison
    Scalar relativeEpsilon_;
};


void print_NodeWeightMap(std::map<Point, Scalar, NodeWeightCompare> nodeWeightMap)
{
    std::cout << "print_NodeWeightMap. Size = " << nodeWeightMap.size() << std::endl;
    UnsignedInteger index = 0;
    for (std::map<Point, Scalar>::iterator it = nodeWeightMap.begin(); it != nodeWeightMap.end(); ++ it)
    {
        std::cout << "[" << index << "] : " << it->first << " = " << it->second << std::endl;
        ++ index;
    }
}

// Implement merge with std::map
void mergeNodesAndWeights(const Sample duplicatedNodes, const Point duplicatedWeights,
                          const Scalar absoluteEpsilon, const Scalar relativeEpsilon, 
                          Sample & nodes, Point & weights)
{
    UnsignedInteger duplicatedSize = duplicatedNodes.getSize();
    if (duplicatedWeights.getDimension() != duplicatedSize) throw InvalidArgumentException(HERE) << "Error: the weights must have dimension " << duplicatedSize << " but have dimension " << duplicatedWeights.getDimension();
    UnsignedInteger dimension = duplicatedNodes.getDimension();
    // Fill the map
    std::map<Point, Scalar, NodeWeightCompare> nodeWeightMap(NodeWeightCompare(absoluteEpsilon, relativeEpsilon));
    for (UnsignedInteger i = 0; i < duplicatedSize; ++i)
    {
        std::map<Point, Scalar>::iterator search = nodeWeightMap.find(duplicatedNodes[i]);
        if (search != nodeWeightMap.end()) {
            std::cout << "[" << i << "], found     : " << search->first << " = " << search->second << std::endl;
            search->second += duplicatedWeights[i];
        } else {
            std::cout << "[" << i << "], not found : " << duplicatedNodes[i] << std::endl;
            nodeWeightMap[duplicatedNodes[i]] = duplicatedWeights[i];
        }
    }
    print_NodeWeightMap(nodeWeightMap);
    // Extract the map
    UnsignedInteger size = nodeWeightMap.size();
    Sample uniqueNodes(size, dimension);
    Point uniqueWeights(size);
    UnsignedInteger index = 0;
    for (std::map<Point, Scalar>::iterator it = nodeWeightMap.begin(); it != nodeWeightMap.end(); ++ it)
    {
        std::cout << "[" << index << "], add " << it->first << " = " << it->second << std::endl;
        uniqueNodes[index] = it->first;
        uniqueWeights[index] = it->second;
        ++ index;
    }
    nodes = uniqueNodes;
    weights = uniqueWeights;
}

// Test simplified merged operation
void test_5()
{
    std::cout << "test_5" << std::endl;
    const UnsignedInteger dimension = 2;
    Point column_1 = {0.1, 0.1, 0.2, 0.2, 0.2, 0.2, 0.20001, 0.3, 0.3, 0.3, 0.30001};
    Point column_2 = {0.5, 0.5, 0.2, 0.5, 0.7, 0.7, 0.70001, 0.6, 0.6, 0.6, 0.60001};
    const UnsignedInteger duplicatedSize = column_1.getSize();
    Sample duplicatedNodes(duplicatedSize, dimension);
    for (UnsignedInteger i = 0; i < duplicatedSize; ++i)
    {
      duplicatedNodes(i, 0) = column_1[i];
      duplicatedNodes(i, 1) = column_2[i];
    }
    Point duplicatedWeights = {0.2, 0.2, 0.3, -0.5, 0.3, 0.2, -0.5, 0.8, -0.5, 0.2, 0.3};
    printNodesAndWeights(duplicatedNodes, duplicatedWeights);
    Scalar absoluteEpsilon = 1.e-2;
    Scalar relativeEpsilon = 1.e-2;
    Sample nodes(0, 0);
    Point weights(0);
    mergeNodesAndWeights(duplicatedNodes, duplicatedWeights, 
                         absoluteEpsilon, relativeEpsilon, 
                         nodes, weights);
    sortNodesAndWeights(nodes, weights);
    printNodesAndWeights(nodes, weights);    
}

// Test Point comparison
void test_6()
{
    std::cout << "test_6" << std::endl;
    Scalar absoluteEpsilon = 1.e-2;
    Scalar relativeEpsilon = 1.e-2;
    NodeWeightCompare comparison(absoluteEpsilon, relativeEpsilon);
    std::cout << "true = " << true << ", false = " << false << std::endl;
    comparison(Point({0.1, 0.2}), Point({0.3, 0.4}));
    comparison(Point({0.3, 0.4}), Point({0.1, 0.2}));
    comparison(Point({0.1, 0.2}), Point({0.1, 0.2}));
    comparison(Point({0.1001, 0.2001}), Point({0.1, 0.2}));
    comparison(Point({0.1, 0.2}), Point({0.1001, 0.2001}));
    comparison(Point({0.0001, 0.0001}), Point({0.0, 0.0}));
    comparison(Point({0.0, 0.0}), Point({0.0001, 0.0001}));
}

// Test realistic Smolyak quadrature
void test_7()
{
    std::cout << "test_7" << std::endl;
    const UnsignedInteger dimension = 2;
    Point column_1 = {0.5, 0.788675, 0.788675, 0.788675, 0.887298, 0.5};
    Point column_2 = {0.887298, 0.211325, 0.5, 0.788675, 0.5, 0.5};
    const UnsignedInteger duplicatedSize = column_1.getSize();
    Sample duplicatedNodes(duplicatedSize, dimension);
    for (UnsignedInteger i = 0; i < duplicatedSize; ++i)
    {
      duplicatedNodes(i, 0) = column_1[i];
      duplicatedNodes(i, 1) = column_2[i];
    }
    Point duplicatedWeights = {0.277778, 0.25, -0.5, 0.25, 0.277778, 0.277778};
    printNodesAndWeights(duplicatedNodes, duplicatedWeights);
    Scalar absoluteEpsilon = 1.e-8;
    Scalar relativeEpsilon = 1.e-8;
    Sample nodes(0, 0);
    Point weights(0);
    mergeNodesAndWeights(duplicatedNodes, duplicatedWeights, 
                         absoluteEpsilon, relativeEpsilon, 
                         nodes, weights);
    sortNodesAndWeights(nodes, weights);
    printNodesAndWeights(nodes, weights);    
}

int main(int, char *[])
{
  TESTPREAMBLE;

  try
  {
    // test_1();
    // test_2();
    // test_3();
    //test_4();
    //test_5();
    //test_6();
    test_7();
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
