//                                               -*- C++ -*-
/**
 *  @brief The test file of class Dirichlet for standard methods
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

  try
  {
    // Instanciate one distribution object
    for (UnsignedInteger dim = 1; dim <= 2; dim++)
    {
      NumericalPoint theta(dim + 1);
      for (UnsignedInteger i = 0; i <= dim; i++) theta[i] = 1.0 + (i + 1.0) / 4.0;
      Dirichlet distribution(theta);
      Description description(dim);
      for (UnsignedInteger j = 1; j <= dim; j++)
      {
        OSS oss;
        oss << "Marginal " << j;
        description[j - 1] = oss;
      }
      distribution.setDescription(description);
      fullprint << std::setprecision(5);
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
      NumericalSample oneSample(distribution.getSample( size ));
      fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
      fullprint << "mean=" << oneSample.computeMean() << std::endl;
      fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
      if (distribution.getDimension() == 1)
      {
        size = 100;
        for (UnsignedInteger i = 0; i < 2; ++i)
        {
          RandomGenerator::SetSeed(2);
          fullprint << "Kolmogorov test for the generator, sample size=" << size << " is " << (FittingTest::Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure() ? "accepted" : "rejected") << std::endl;
          size *= 10;
        }
      }

      // Define a point
      NumericalPoint point( distribution.getDimension(), 0.5 / distribution.getDimension() );
      fullprint << "Point=" << point << std::endl;

      // Show PDF and CDF of point
      NumericalScalar LPDF = distribution.computeLogPDF( point );
      fullprint << "log pdf=" << LPDF << std::endl;
      NumericalScalar PDF = distribution.computePDF( point );
      fullprint << "pdf=" << PDF << std::endl;
      NumericalScalar CDF = distribution.computeCDF( point );
      fullprint << "cdf=" << CDF << std::endl;
      NumericalPoint quantile = distribution.computeQuantile( 0.95 );
      int oldPrecision = PlatformInfo::GetNumericalPrecision();
      PlatformInfo::SetNumericalPrecision( 4 );
      fullprint << "quantile=" << quantile << std::endl;
      PlatformInfo::SetNumericalPrecision( oldPrecision );
      fullprint << "cdf(quantile)=" << distribution.computeCDF(quantile) << std::endl;
      NumericalPoint mean = distribution.getMean();
      fullprint << "mean=" << mean << std::endl;
      NumericalPoint standardDeviation = distribution.getStandardDeviation();
      fullprint << "standard deviation=" << standardDeviation << std::endl;
      NumericalPoint skewness = distribution.getSkewness();
      fullprint << "skewness=" << skewness << std::endl;
      NumericalPoint kurtosis = distribution.getKurtosis();
      fullprint << "kurtosis=" << kurtosis << std::endl;
      CovarianceMatrix covariance = distribution.getCovariance();
      fullprint << "covariance=" << covariance << std::endl;

      // get/set parameters collection
      Dirichlet::NumericalPointWithDescriptionCollection parametersCollection(distribution.getParametersCollection());
      fullprint << "parameters collection=" << parametersCollection << std::endl;
      fullprint << "before set=" << distribution << std::endl;
      NumericalPoint theta2(dim + 1, 1.);
      Dirichlet tmp(theta2);
      tmp.setParametersCollection(parametersCollection);
      fullprint << "after set= " << tmp << std::endl;

      // Extract the marginals
      for (UnsignedInteger i = 0; i < dim; i++)
      {
        Distribution margin(distribution.getMarginal(i));
        fullprint << "margin=" << margin << std::endl;
        fullprint << "margin PDF=" << margin.computePDF(NumericalPoint(1, 0.5)) << std::endl;
        fullprint << "margin CDF=" << margin.computeCDF(NumericalPoint(1, 0.5)) << std::endl;
        fullprint << "margin quantile=" << margin.computeQuantile(0.95) << std::endl;
        fullprint << "margin realization=" << margin.getRealization() << std::endl;
      }
      if (dim >= 2)
      {
        // Extract a 2-D marginal
        Indices indices(2, 0);
        indices[0] = 1;
        indices[1] = 0;
        fullprint << "indices=" << indices << std::endl;
        Distribution margins(distribution.getMarginal(indices));
        fullprint << "margins=" << margins << std::endl;
        fullprint << "margins PDF=" << margins.computePDF(NumericalPoint(2, 0.5)) << std::endl;
        fullprint << "margins CDF=" << margins.computeCDF(NumericalPoint(2, 0.5)) << std::endl;
        NumericalPoint quantile = margins.computeQuantile(0.95);
        fullprint << "margins quantile=" << quantile << std::endl;
        fullprint << "margins CDF(quantile)=" << margins.computeCDF(quantile) << std::endl;
        fullprint << "margins realization=" << margins.getRealization() << std::endl;
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
