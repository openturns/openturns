//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
    Sample sample(4, 3);
    sample[0][0] = 1;
    sample[0][1] = 0;
    sample[0][2] = 9;
    sample[1][0] = 2;
    sample[1][1] = 3;
    sample[1][2] = 5;
    sample[2][0] = 5;
    sample[2][1] = 1;
    sample[2][2] = 8;
    sample[3][0] = 6;
    sample[3][1] = 7;
    sample[3][2] = 2;

    fullprint << "sample=" << sample << std::endl;
    fullprint << "min=" << sample.getMin() << std::endl;
    fullprint << "max=" << sample.getMax() << std::endl;
    fullprint << "mean=" << sample.computeMean() << std::endl;
    fullprint << "covariance=" << sample.computeCovariance() << std::endl;
    fullprint << "standard deviation=" << sample.computeStandardDeviation() << std::endl;
    fullprint << "standard deviation per component=" << sample.computeStandardDeviationPerComponent() << std::endl;
    fullprint << "Pearson correlation=" << sample.computePearsonCorrelation() << std::endl;
    fullprint << "Spearman correlation=" << sample.computeSpearmanCorrelation() << std::endl;
    fullprint << "Kendall tau=" << sample.computeKendallTau() << std::endl;
    fullprint << "range per component=" << sample.computeRange() << std::endl;
    fullprint << "median per component=" << sample.computeMedian() << std::endl;
    fullprint << "Variance=" << sample.computeVariance() << std::endl;
    fullprint << "Skewness=" << sample.computeSkewness() << std::endl;
    fullprint << "Kurtosis=" << sample.computeKurtosis() << std::endl;
    for (UnsignedInteger i = 0; i < 10; ++i)
      fullprint << "Centered moment of order " << i << "=" << sample.computeCenteredMoment(i) << std::endl;

    fullprint << "Marginal 1=" << sample.getMarginal(1) << std::endl;
    Indices indices(2);
    indices[0] = 2;
    indices[1] = 0;
    fullprint << "Marginal [2, 0]=" << sample.getMarginal(indices) << std::endl;
    fullprint << "Rank           =" << sample.rank() << std::endl;
    Scalar prob = 0.25;
    fullprint << "Quantile per component(" << prob << ")=" << sample.computeQuantilePerComponent(prob) << std::endl;
    //    fullprint << "Quantile(" << prob << ")=" << sample.computeQuantile(prob) << std::endl;
    Point probs(2);
    probs[0] = 0.25;
    probs[1] = 0.75;
    fullprint << "Quantile per component(" << probs << ")=" << sample.computeQuantilePerComponent(probs) << std::endl;
    probs[0] = 0.75;
    probs[1] = 0.25;
    fullprint << "Quantile per component(" << probs << ")=" << sample.computeQuantilePerComponent(probs) << std::endl;
    Point pointCDF(sample.getDimension(), 0.25);
    fullprint << "Empirical CDF(" << pointCDF << "=" << sample.computeEmpiricalCDF(pointCDF) << std::endl;
    UnsignedInteger dim = 3;
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.25;
    }
    CorrelationMatrix Rtmp(dim);
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      for (UnsignedInteger j = 0; j < i; j++)
      {
        Rtmp(i, j) = 6.0 * asin(R(i, j) / 2.0) / M_PI;
      }
    }
    fullprint << "Pearson correlation (exact)=" << R << std::endl;
    fullprint << "Spearman correlation (exact)=" << Rtmp << std::endl;
    // Add some ties
    sample[0][0] = 1;
    sample[0][1] = 0;
    sample[0][2] = 9;
    sample[1][0] = 2;
    sample[1][1] = 3;
    sample[1][2] = 9;
    sample[2][0] = 5;
    sample[2][1] = 1;
    sample[2][2] = 9;
    sample[3][0] = 2;
    sample[3][1] = 2;
    sample[3][2] = 9;
    fullprint << "sample=" << sample << std::endl;
    fullprint << "Rank=" << sample.rank() << std::endl;
    fullprint << "Rank component 0=" << sample.rank(0) << std::endl;
    fullprint << "Sort           =" << sample.sort() << std::endl;
    Sample sampleTMP(sample);
    sampleTMP.sortInPlace();
    fullprint << "Sort (in place)=" << sampleTMP << std::endl;    
    fullprint << "Sort according to component 0           =" << sample.sortAccordingToAComponent(0) << std::endl;
    sampleTMP = sample;
    sampleTMP.sortAccordingToAComponentInPlace(0);
    fullprint << "Sort according to component 0 (in place)=" << sampleTMP << std::endl;
    fullprint << "Spearman correlation=" << sample.computeSpearmanCorrelation() << std::endl;
    fullprint << "Kendall tau=" << sample.computeKendallTau() << std::endl;
    UnsignedInteger size = 10000;
    Point p1(dim, 0.0);
    Point p2(dim, 1.0);
    Normal normal(Normal(p1, p2, R));
    fullprint << "Normal=" << normal << std::endl;
    fullprint << "covariance=" << normal.getCovariance() << std::endl;
    Sample normalSample(normal.getSample(size));
    fullprint << "Empirical covariance=" << normalSample.computeCovariance() << std::endl;
    CorrelationMatrix RPearson(normalSample.computePearsonCorrelation());
    fullprint << "Pearson correlation=" << RPearson << std::endl;
    CorrelationMatrix RSpearman(normalSample.computeSpearmanCorrelation());
    fullprint << "Spearman correlation=" << RSpearman << std::endl;

    // Check sortUnique()
    fullprint << "Unique           =" << sample.sortUnique() << std::endl;
    sampleTMP = sample;
    sampleTMP.sortUniqueInPlace();
    fullprint << "Unique (in place)=" << sampleTMP << std::endl;
    sample[2][0] = 1;
    sample[2][1] = 0;
    sample[2][2] = 9;
    fullprint << "Unique           =" << sample.sortUnique() << std::endl;
    sampleTMP = sample;
    sampleTMP.sortUniqueInPlace();
    fullprint << "Unique (in place)=" << sampleTMP << std::endl;    
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
