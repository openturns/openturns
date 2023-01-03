//                                               -*- C++ -*-
/**
 *  @brief The test file of class Histogram for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    Scalar x0 = -1.5;
    UnsignedInteger collectionSize = 4;
    Point l(collectionSize);
    l[0] = 1.0;
    l[1] = 0.7;
    l[2] = 1.2;
    l[3] = 0.9;
    Point h(collectionSize);
    h[0] = 0.5;
    h[1] = 1.5;
    h[2] = 3.5;
    h[3] = 2.5;
    Point ticks(collectionSize + 1);
    ticks[0] = x0;
    ticks[1] = ticks[0] + l[0];
    ticks[2] = ticks[1] + l[1];
    ticks[3] = ticks[2] + l[2];
    ticks[4] = ticks[3] + l[3];
    Point frequencies(collectionSize);
    frequencies[0] = h[0] * l[0];
    frequencies[1] = h[1] * l[1];
    frequencies[2] = h[2] * l[2];
    frequencies[3] = h[3] * l[3];
    // Instantiate one distribution object using ticks/frequencies
    {
      Histogram distribution(ticks, frequencies);
      fullprint << "ticks=" << ticks << " frequencies=" << frequencies << std::endl;

      fullprint << "Distribution " << distribution << std::endl;
      std::cout << "Distribution " << distribution << std::endl;
    }
    Histogram distribution(x0, l, h);
    fullprint << "l=" << l << " h=" << h << std::endl;

    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;

    // Is this distribution elliptical ?
    fullprint << "Elliptical = " << (distribution.isElliptical() ? "true" : "false") << std::endl;

    // Is this distribution continuous ?
    fullprint << "Continuous = " << (distribution.isContinuous() ? "true" : "false") << std::endl;

    // Test for realization of distribution
    Point oneRealization = distribution.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10000;
    Sample oneSample = distribution.getSample( size );
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
    Point point( distribution.getDimension(), 1.0 );
    fullprint << "Point= " << point << std::endl;

    // Show PDF and CDF of point
    Scalar eps = 1e-5;
    Point DDF = distribution.computeDDF( point );
    fullprint << "ddf     =" << DDF << std::endl;
    Scalar LPDF = distribution.computeLogPDF( point );
    fullprint << "log pdf=" << LPDF << std::endl;
    Scalar PDF = distribution.computePDF( point );
    fullprint << "pdf     =" << PDF << std::endl;
    fullprint << "pdf (FD)=" << (distribution.computeCDF( point + Point(1, eps) ) - distribution.computeCDF( point  + Point(1, -eps) )) / (2.0 * eps) << std::endl;
    Scalar CDF = distribution.computeCDF( point );
    fullprint << "cdf=" << CDF << std::endl;
    Scalar CCDF = distribution.computeComplementaryCDF( point );
    fullprint << "ccdf=" << CCDF << std::endl;
    Scalar Survival = distribution.computeSurvivalFunction( point );
    fullprint << "survival=" << Survival << std::endl;
    Point InverseSurvival = distribution.computeInverseSurvivalFunction(0.95);
    fullprint << "Inverse survival=" << InverseSurvival << std::endl;
    fullprint << "Survival(inverse survival)=" << distribution.computeSurvivalFunction(InverseSurvival) << std::endl;
    //    Point PDFgr = distribution.computePDFGradient( point );
    //    fullprint << "pdf gradient     =" << PDFgr << std::endl;
    //    Point PDFgrFD(1 + 2 * collectionSize);
    //    PDFgrFD[0] = (Histogram(distribution.getFirst() + eps, distribution.getPairCollection()).computePDF(point) -
    //                  Histogram(distribution.getFirst() - eps, distribution.getPairCollection()).computePDF(point)) / (2.0 * eps);
    //    for (UnsignedInteger i = 0; i < collectionSize; i++)
    //      {
    //  Histogram::PairCollection collectionLeft = distribution.getPairCollection();
    //  Histogram::PairCollection collectionRight = distribution.getPairCollection();
    //  collectionLeft[i].h_  += eps;
    //  collectionRight[i].h_ -= eps;
    //  PDFgrFD[2 * i + 1] = (Histogram(distribution.getFirst(), collectionLeft).computePDF(point) -
    //                        Histogram(distribution.getFirst(), collectionRight).computePDF(point)) / (2.0 * eps);
    //  collectionLeft = distribution.getPairCollection();
    //  collectionRight = collectionLeft;
    //  collectionLeft[i].l_  += eps;
    //  collectionRight[i].l_ -= eps;
    //  PDFgrFD[2 * i + 2] = (Histogram(distribution.getFirst(), collectionLeft).computePDF(point) -
    //                        Histogram(distribution.getFirst(), collectionRight).computePDF(point)) / (2.0 * eps);
    //      }
    //    fullprint << "pdf gradient (FD)=" << PDFgrFD << std::endl;
    //    Point CDFgr = distribution.computeCDFGradient( point );
    //    fullprint << "cdf gradient     =" << CDFgr << std::endl;
    //    Point CDFgrFD(1 + 2 * collectionSize);
    //    CDFgrFD[0] = (Histogram(distribution.getFirst() + eps, distribution.getPairCollection()).computeCDF(point) -
    //                  Histogram(distribution.getFirst() - eps, distribution.getPairCollection()).computeCDF(point)) / (2.0 * eps);
    //    for (UnsignedInteger i = 0; i < collectionSize; i++)
    //      {
    //  Histogram::PairCollection collectionLeft = distribution.getPairCollection();
    //  Histogram::PairCollection collectionRight = distribution.getPairCollection();
    //  collectionLeft[i].h_  += eps;
    //  collectionRight[i].h_ -= eps;
    //  CDFgrFD[2 * i + 1] = (Histogram(distribution.getFirst(), collectionLeft).computeCDF(point) -
    //                        Histogram(distribution.getFirst(), collectionRight).computeCDF(point)) / (2.0 * eps);
    //  collectionLeft = distribution.getPairCollection();
    //  collectionRight = collectionLeft;
    //  collectionLeft[i].l_  += eps;
    //  collectionRight[i].l_ -= eps;
    //  CDFgrFD[2 * i + 2] = (Histogram(distribution.getFirst(), collectionLeft).computeCDF(point) -
    //                        Histogram(distribution.getFirst(), collectionRight).computeCDF(point)) / (2.0 * eps);
    //      }
    //    fullprint << "cdf gradient (FD)=" << CDFgrFD << std::endl;
    Point quantile = distribution.computeQuantile( 0.95 );
    fullprint << "quantile=" << quantile << std::endl;
    fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
    // Confidence regions
    Scalar threshold;
    fullprint << "Minimum volume interval=" << distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95, threshold) << std::endl;
    fullprint << "threshold=" << threshold << std::endl;
    Scalar beta;
    LevelSet levelSet(distribution.computeMinimumVolumeLevelSetWithThreshold(0.95, beta));
    fullprint << "Minimum volume level set=" << levelSet << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "Bilateral confidence interval=" << distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95, beta) << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "Unilateral confidence interval (lower tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, false, beta) << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "Unilateral confidence interval (upper tail)=" << distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, true, beta) << std::endl;
    fullprint << "beta=" << beta << std::endl;
    fullprint << "entropy=" << distribution.computeEntropy() << std::endl;
    fullprint << "entropy (MC)=" << -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0] << std::endl;
    Point mean = distribution.getMean();
    fullprint << "mean=" << mean << std::endl;
    CovarianceMatrix covariance = distribution.getCovariance();
    fullprint << "covariance=" << covariance << std::endl;
    CovarianceMatrix correlation = distribution.getCorrelation();
    fullprint << "correlation=" << correlation << std::endl;
    CovarianceMatrix spearman = distribution.getSpearmanCorrelation();
    fullprint << "spearman=" << spearman << std::endl;
    CovarianceMatrix kendall = distribution.getKendallTau();
    fullprint << "kendall=" << kendall << std::endl;
    Histogram::PointWithDescriptionCollection parameters = distribution.getParametersCollection();
    fullprint << "parameters=" << parameters << std::endl;
    fullprint << "Standard representative=" << distribution.getStandardRepresentative().__str__() << std::endl;

    UnsignedInteger testSize = 0;
    for (UnsignedInteger i = 0; i < testSize; i++)
    {
      Scalar q = RandomGenerator::Generate();
      if (std::abs(q - distribution.computeCDF(distribution.computeQuantile(q))) > eps)
      {
        fullprint << "q=" << q << " quantile=" << distribution.computeQuantile(q)[0] << " CDF(quantile)=" << distribution.computeCDF(distribution.computeQuantile(q)) << std::endl;
      }
    }

    // non-regression for #599
    {
      l = Point(2);
      l[0] = 1.0;
      l[1] = 2.0;
      h = Point(2);
      h[0] = 0.6;
      h[1] = 0.3;
      distribution = Histogram(2.0, l, h);
      for ( UnsignedInteger i = 0; i < 10; ++ i )
      {
        const Scalar p = 1.0 * i / 10.0;
        fullprint << "q@" << p << "=" << distribution.computeQuantile(p)[0] << std::endl;
      }
    }
    // Specific to this distribution
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
