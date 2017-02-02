//                                               -*- C++ -*-
/**
 *  @brief The test file of class Mixture for standard methods
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  ResourceMap::Set("Distribution-parallel", "1");
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // First, 1D case
    {
      Mixture::DistributionCollection atoms(3);
      atoms[0] = Normal(-3.0, 1.0);
      atoms[0].setWeight(1.0);
      atoms[1] = Normal(1.0, 2.0);
      atoms[1].setWeight(2.0);
      atoms[2] = Normal(3.0, 3.0);
      atoms[2].setWeight(4.0);
      Mixture distribution(atoms);
      Mixture::NumericalPointWithDescriptionCollection parameters(distribution.getParametersCollection());
      fullprint << "parameters=" << parameters << std::endl;
      for (UnsignedInteger i = 0; i < 6; ++i) fullprint << "standard moment n=" << i << ", value=" << distribution.getStandardMoment(i) << std::endl;
      fullprint << "Standard representative=" << distribution.getStandardRepresentative()->__str__() << std::endl;
      // get weights used
      NumericalPoint weights(distribution.getWeights());
      fullprint << "weights=" << weights << std::endl;
      // Fix new weights
      weights[0] = 0.2;
      weights[1] = 0.5;
      //set item
      distribution.setWeights(weights);
      fullprint << "New weights=" << distribution.getWeights() << std::endl;
    }
    UnsignedInteger dimension = 3;
    NumericalPoint meanPoint(dimension, 1.0);
    meanPoint[0] = 0.5;
    meanPoint[1] = -0.5;
    NumericalPoint sigma(dimension, 1.0);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    CorrelationMatrix R(dimension);
    for (UnsignedInteger i = 1; i < dimension; i++)
    {
      R(i, i - 1) = 0.5;
    }
    // Create a collection of distribution
    Mixture::DistributionCollection aCollection;

    aCollection.add( Normal(meanPoint, sigma, R) );
    meanPoint += NumericalPoint(dimension, 1.0);
    aCollection.add( Normal(meanPoint, sigma, R) );
    meanPoint += NumericalPoint(dimension, 1.0);
    aCollection.add( Normal(meanPoint, sigma, R) );

    // Instanciate one distribution object
    Mixture distribution(aCollection, NumericalPoint(aCollection.getSize(), 2.0));
    fullprint << "Distribution " << distribution << std::endl;
    distribution = Mixture(aCollection);
    fullprint << "Distribution " << distribution.__str__() << std::endl;

    // Is this distribution elliptical ?
    fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Is this distribution continuous ?
    fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    NumericalPoint oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 1000;
    NumericalSample oneSample = distribution.getSample( size );
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
    if (distribution.getDimension() == 1)
    {
      size = 100;
      for (UnsignedInteger i = 0; i < 2; ++i)
      {
        fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
        size *= 10;
      }
    }
    // Define a point
    NumericalPoint point( dimension, 1.0 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    NumericalScalar eps = 1e-5;
    NumericalPoint DDF = distribution.computeDDF( point );
    fullprint << "ddf     =" << DDF << std::endl;
    NumericalPoint ddfFD(dimension);
    for (UnsignedInteger i = 0; i < dimension; i++)
    {
      NumericalPoint left(point);
      left[i] += eps;
      NumericalPoint right(point);
      right[i] -= eps;
      ddfFD[i] = (distribution.computePDF(left) - distribution.computePDF(right)) / (2.0 * eps);
    }
    fullprint << "ddf (FD)=" << ddfFD << std::endl;
    NumericalScalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf=" << LPDF << std::endl;
    NumericalScalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    if (dimension == 1)
    {
      fullprint << "pdf (FD)=" << (distribution.computeCDF( point + NumericalPoint(1, eps) ) - distribution.computeCDF( point  + NumericalPoint(1, -eps) )) / (2.0 * eps) << std::endl;
    }
    NumericalScalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    NumericalScalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf=" << CCDF << std::endl;
    NumericalScalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival =" << Survival << std::endl;
    NumericalPoint InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
    fullprint << "Inverse survival=" << InverseSurvival << std::endl;
    fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;
    if (dimension == 1)
    {
      NumericalComplex CF = distribution.computeCharacteristicFunction( point[0] );
      fullprint << "characteristic function=" << CF << std::endl;
      NumericalComplex LCF = distribution.computeLogCharacteristicFunction( point[0] );
      fullprint << "log characteristic function=" << LCF << std::endl;
    }
    NumericalPoint quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile=" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    if (distribution.getDimension() <= 2)
    {
      // Confidence regions
      NumericalScalar threshold;
      fullprint << "Minimum volume interval=" << distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95, threshold) << std::endl;
      fullprint << "threshold=" << threshold << std::endl;
      NumericalScalar beta;
      LevelSet levelSet(distribution.computeMinimumVolumeLevelSetWithThreshold(0.95, beta));
      fullprint << "Minimum volume level set=" << levelSet << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Bilateral confidence interval=" << distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (lower tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, false, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
      fullprint << "Unilateral confidence interval (upper tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, true, beta) << std::endl;
      fullprint << "beta=" << beta << std::endl;
    }
    NumericalPoint mean = distribution.getMean();
    fullprint << "mean=" << mean << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CovarianceMatrix correlation = distribution.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
//     CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
//     fullprint << "spearman=" << spearman << std::endl;
//     CovarianceMatrix kendall = distribution.getKendallTau();
//     fullprint << "kendall=" << kendall << std::endl;
    Mixture::NumericalPointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;

    // Constructor with separate weights. Also check small weights removal
    NumericalPoint weights;
    Mixture::DistributionCollection atoms;
    weights.add(1.0e-20);
    atoms.add(Normal(1.0, 1.0));
    weights.add(2.5);
    atoms.add(Normal(2.0, 2.0));
    weights.add(32.0);
    atoms.add(Normal(3.0, 3.0));
    Mixture newMixture(atoms, weights);
    fullprint << "newMixture pdf=" << newMixture.computePDF(2.5) << std::endl;
    fullprint << "atoms kept in mixture=" << newMixture.getDistributionCollection() << std::endl;
    fullprint << "newMixture=" << newMixture.__str__() << std::endl;



  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}

