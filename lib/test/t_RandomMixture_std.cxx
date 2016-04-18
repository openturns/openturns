//                                               -*- C++ -*-
/**
 *  @brief The test file of class RandomMixture for standard methods
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  ResourceMap::SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000);

  try
  {
    // Create a collection of test-cases and the associated references
    UnsignedInteger numberOfTests = 3;
    Collection< Collection< Distribution > > testCases(numberOfTests);
    Collection< Distribution > references(numberOfTests);
    testCases[0] = Collection<Distribution>(2);
    testCases[0][0] = Uniform(-1.0, 3.0);
    testCases[0][1] = Uniform(-1.0, 3.0);
    references[0] = Triangular(-2.0, 2.0, 6.0);
    testCases[1] = Collection<Distribution>(3);
    testCases[1][0] = Normal();
    testCases[1][1] = Normal(1.0, 2.0);
    testCases[1][2] = Normal(-2.0, 2.0);
    references[1] = Normal(-1.0, 3.0);
    testCases[2] = Collection<Distribution>(3);
    testCases[2][0] = Exponential();
    testCases[2][1] = Exponential();
    testCases[2][2] = Exponential();
    references[2] = Gamma(3.0, 1.0, 0.0);
    fullprint << "testCases=" << testCases << std::endl;
    fullprint << "references=" << references << std::endl;
    for (UnsignedInteger testIndex = 0; testIndex < testCases.getSize(); ++testIndex)
    {
      // Instanciate one distribution object
      RandomMixture distribution(testCases[testIndex]);
      distribution.setBlockMin(5);
      distribution.setBlockMax(20);
      Distribution distributionReference(references[testIndex]);
      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;

      // Is this distribution elliptical ?
      fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

      // Is this distribution continuous ?
      fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

      // Test for realization of distribution
      NumericalPoint oneRealization = distribution.getRealization();
      fullprint << "oneRealization=" << oneRealization << std::endl;

      // Test for sampling
      UnsignedInteger size = 10000;
      NumericalSample oneSample = distribution.getSample( size );
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;

      size = 100;
      for (UnsignedInteger i = 0; i < 2; ++i)
      {
        fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
        size *= 10;
      }

      // Define a point
      NumericalPoint point(distribution.getDimension(), 0.5);
      fullprint << "Point= " << point << std::endl;

      // Show PDF and CDF of point
      NumericalScalar eps = 1e-5;
      NumericalPoint DDF = distribution.computeDDF(point);
      fullprint << "ddf      =" << DDF << std::endl;
      fullprint << "ddf (ref)=" << distributionReference.computeDDF(point) << std::endl;
      NumericalScalar PDF = distribution.computePDF(point);
      fullprint << "pdf      =" << PDF << std::endl;
      fullprint << "pdf  (FD)=" << (distribution.computeCDF( point + NumericalPoint(1, eps) ) - distribution.computeCDF( point  + NumericalPoint(1, -eps) )) / (2.0 * eps) << std::endl;
      fullprint << "pdf (ref)=" << distributionReference.computePDF(point) << std::endl;
      NumericalScalar CDF = distribution.computeCDF( point );
      fullprint << "cdf      =" << CDF << std::endl;
      fullprint << "cdf (ref)=" << distributionReference.computeCDF(point) << std::endl;
      NumericalComplex CF = distribution.computeCharacteristicFunction( point[0] );
      fullprint << "characteristic function=" << CF << std::endl;
      NumericalComplex LCF = distribution.computeLogCharacteristicFunction( point[0] );
      fullprint << "log characteristic function=" << LCF << std::endl;
      NumericalPoint quantile = distribution.computeQuantile( 0.95 );
      fullprint << "quantile      =" << quantile << std::endl;
      fullprint << "quantile (ref)=" << distributionReference.computeQuantile(0.95) << std::endl;
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      NumericalPoint mean = distribution.getMean();
      fullprint << "mean      =" << mean << std::endl;
      fullprint << "mean (ref)=" << distributionReference.getMean() << std::endl;
      NumericalPoint standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation      =" << standardDeviation << std::endl;
      fullprint << "standard deviation (ref)=" << distributionReference.getStandardDeviation() << std::endl;
      NumericalPoint skewness = distribution.getSkewness();
      fullprint << "skewness      =" << skewness << std::endl;
      fullprint << "skewness (ref)=" << distributionReference.getSkewness() << std::endl;
      NumericalPoint kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis      =" << kurtosis << std::endl;
      fullprint << "kurtosis (ref)=" << distributionReference.getKurtosis() << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance      =" << covariance << std::endl;
      fullprint << "covariance (ref)=" << distributionReference.getCovariance() << std::endl;
      RandomMixture::NumericalPointWithDescriptionCollection parameters = distribution.getParametersCollection();
      fullprint << "parameters=" << parameters << std::endl;
      /*    distribution.setIntegrationNodesNumber(6);
            for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;*/
      fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;
      fullprint << "blockMin=" << distribution.getBlockMin() << std::endl;
      fullprint << "blockMax=" << distribution.getBlockMax() << std::endl;
      fullprint << "maxSize=" << distribution.getMaxSize() << std::endl;
      fullprint << "alpha=" << distribution.getAlpha() << std::endl;
      fullprint << "beta=" << distribution.getBeta() << std::endl;
    }
    // Tests of the simplification mechanism
    NumericalPoint weights(0);
    Collection< Distribution > coll(0);
    //      coll.add(Dirac(0.5));
    //      weights.add(1.0);
    coll.add(Normal(1.0, 2.0));
    weights.add(1.0);
    coll.add(Normal(2.0, 1.0));
    weights.add(1.0);
    coll.add(Uniform(-2.0, 2.0));
    weights.add(1.0);
    coll.add(Exponential(2.0, 0.0));
    weights.add(1.0);
    //      RandomMixture rm(coll, weights);
    //      coll.add(rm);
    //      weights.add(-2.5);
    coll.add(Gamma(3.0, 4.0, 0.0));
    weights.add(1.0);
    RandomMixture distribution(coll, weights);
    fullprint << "distribution=" << distribution << std::endl;
    fullprint << "distribution=" << distribution.__str__() << std::endl;
    for (UnsignedInteger i = 0; i < 30; ++i)
    {
      NumericalScalar x = -12.0 + i;
      fullprint << "pdf(" << x << ")=" << distribution.computePDF(x) << std::endl;
    }
    Graph graph(distribution.drawPDF());
    Distribution ks(KernelSmoothing().build(distribution.getSample(1000000)));
    graph.add(ks.drawPDF());
    Description colors(2);
    colors[0] = "red";
    colors[1] = "green";
    graph.setColors(colors);
    //     graph.draw("validation");
    // Test for the projection
    Collection<DistributionFactory> collFactories(0);
    collFactories.add(UniformFactory());
    collFactories.add(NormalFactory());
    collFactories.add(TriangularFactory());
    collFactories.add(ExponentialFactory());
    collFactories.add(GammaFactory());
//     collFactories.add(TrapezoidalFactory());
    NumericalPoint norms;
    Collection< Distribution > result(distribution.project(collFactories, norms));
    fullprint << "projections=" << result << std::endl;
    fullprint << "norms=" << norms << std::endl;
    //------------------------------ Multivariate tests ------------------------------//
    // 2D RandomMixture
    Collection< Distribution > collection(0);
    collection.add(Normal(0.0, 1.0));
    collection.add(Normal(0.0, 1.0));
    collection.add(Normal(0.0, 1.0));

    Matrix weightMatrix(2, 3);
    weightMatrix(0, 0) = 1.0;
    weightMatrix(0, 1) = -2.0;
    weightMatrix(0, 2) = 1.0;
    weightMatrix(1, 0) = 1.0;
    weightMatrix(1, 1) = 1.0;
    weightMatrix(1, 2) = -3.0;

    // Build the RandomMixture
    RandomMixture distribution2D(collection, weightMatrix);
    fullprint << "distribution = " << distribution2D << std::endl;
    fullprint << "range = " << distribution2D.getRange() << std::endl;
    fullprint << "mean = " <<  distribution2D.getMean() << std::endl;
    fullprint << "cov = " << distribution2D.getCovariance() << std::endl;
    fullprint << "sigma = " << distribution2D.getStandardDeviation() << std::endl;
    distribution2D.setBlockMin(3);
    distribution2D.setBlockMax(10);

    // Build a grid for validation
    const NumericalScalar xMin = distribution2D.getRange().getLowerBound()[0];
    const NumericalScalar xMax = distribution2D.getRange().getUpperBound()[0];
    const NumericalScalar yMin = distribution2D.getRange().getLowerBound()[1];
    const NumericalScalar yMax = distribution2D.getRange().getUpperBound()[1];
    // Number of points of discretization
    const UnsignedInteger nx = 4;
    const UnsignedInteger ny = 4;
    NumericalPoint boxParameters(2);
    boxParameters[0] = nx;
    boxParameters[1] = ny;
    Box boxGrid(boxParameters);
    NumericalSample grid(boxGrid.generate());
    // scaling box grid
    NumericalPoint scaleFactor(2);
    scaleFactor[0] = 0.25 * (xMax - xMin);
    scaleFactor[1] = 0.25 * (yMax - yMin);
    grid *= scaleFactor;
    //translating
    NumericalPoint translateFactor(2);
    translateFactor[0] = distribution2D.getMean()[0];
    translateFactor[1] = distribution2D.getMean()[1];
    grid += translateFactor;
    // Compute PDF
    // parameters for theoritical PDF, obtained thanks to Maple
    const NumericalScalar factor = sqrt(2.0) / (20 * M_PI);
    for (UnsignedInteger index = 0; index < grid.getSize(); ++ index)
    {
      const NumericalPoint point(grid[index]);
      const NumericalScalar PDF = distribution2D.computePDF(point);
      // Very small values are not very accurate on x86, skip them
      if (PDF < 1.e-12) continue;
      fullprint << "pdf      =" << PDF << std::endl;
      const NumericalScalar y = point[1];
      const NumericalScalar x = point[0];
      fullprint << "pdf (ref)=" << factor * exp(-3.0 / 50.0 * y * y - 2.0 / 25 * x * y - 11.0 / 100 * x * x) << std::endl;
    }
    // 2D test, but too much CPU consuming

    Collection< Distribution > collUniforme(0);
    collUniforme.add(Uniform(0, 1));
    collUniforme.add(Uniform(0, 1));
    collUniforme.add(Uniform(0, 1));
    // Build the RandomMixture
    RandomMixture dist_2D(collUniforme, weightMatrix);
    dist_2D.setBlockMin(3);
    dist_2D.setBlockMax(8);

    fullprint << "new distribution = " << dist_2D << std::endl;
    fullprint << "range = " << dist_2D.getRange() << std::endl;
    fullprint << "mean = " <<  dist_2D.getMean() << std::endl;
    fullprint << "cov = " << dist_2D.getCovariance() << std::endl;
    fullprint << "sigma = " << dist_2D.getStandardDeviation() << std::endl;

    // Discretization on 2D grid [mu, mu+\sigma]
    NumericalSample newGrid(boxGrid.generate());
    // scaling box grid
    newGrid *= dist_2D.getStandardDeviation();
    //translating
    newGrid += dist_2D.getMean();
    // Compute PDF
    for (UnsignedInteger index = 0; index < newGrid.getSize(); ++ index)
    {
      const NumericalPoint point(newGrid[index]);
      const NumericalScalar PDF = dist_2D.computePDF(point);
      fullprint << "pdf      =" << PDF << std::endl;
    }
    // 3D test
    ResourceMap::SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 8290688);
    Collection<Distribution> collectionMixture(0);
    collectionMixture.add(Normal(2.0, 1.0));
    collectionMixture.add(Normal(-2.0, 1.0));
    const Mixture mixture(collectionMixture);
    Collection<Distribution> collection3D(0);
    collection3D.add(Normal(0.0, 1.0));
    collection3D.add(mixture);
    collection3D.add(Uniform(0, 1));
    collection3D.add(Uniform(0, 1));
    // Set weights
    weightMatrix = Matrix(3, 4);
    weightMatrix(0, 0) = 1.0;
    weightMatrix(0, 1) = -0.05;
    weightMatrix(0, 2) = 1.0;
    weightMatrix(0, 3) = -0.5;

    weightMatrix(1, 0) = 0.5;
    weightMatrix(1, 1) = 1.0;
    weightMatrix(1, 2) = -0.05;
    weightMatrix(1, 3) = 0.3;

    weightMatrix(2, 0) = -0.5;
    weightMatrix(2, 1) = -0.1;
    weightMatrix(2, 2) = 1.2;
    weightMatrix(2, 3) = -0.8;

    RandomMixture dist_3D(collection3D, weightMatrix);
    dist_3D.setBlockMin(3);
    dist_3D.setBlockMax(6);

    fullprint << "3D distribution = " << dist_3D << std::endl;
    fullprint << "range = " << dist_3D.getRange() << std::endl;
    fullprint << "mean = " <<  dist_3D.getMean() << std::endl;
    fullprint << "cov = " << dist_3D.getCovariance() << std::endl;
    fullprint << "sigma = " << dist_3D.getStandardDeviation() << std::endl;

    // Total number of points (is (2+2)**3)
    // Test is CPU consuming
    // Number of points of discretization
    const UnsignedInteger N = 2;
    NumericalPoint box3DParameters(3, N);
    Box box3D(box3DParameters);
    // Grid ==> (mu, mu+sigma)
    NumericalSample grid3D(box3D.generate());
    //scaling
    grid3D *= dist_3D.getStandardDeviation();
    // translating
    grid3D += dist_3D.getMean();
    for (UnsignedInteger index = 0; index < grid3D.getSize(); ++ index)
    {
      const NumericalPoint point(grid3D[index]);
      const NumericalScalar PDF = dist_3D.computePDF(point);
      fullprint << "pdf      =" << PDF << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
