//                                               -*- C++ -*-
/**
 *  @brief The test file of class Normal for large dimension vector
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Big test case for correlated components
    // Instanciate one distribution object
    UnsignedInteger dim = 4;
    Point meanPoint(dim, 1.0);
    Point sigma(dim, 1.0);
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.5;
    }

    Normal distribution(meanPoint, sigma, R);

    // Test for sampling
    UnsignedInteger size = 1000;
    Sample oneSample = distribution.getSample( size );
    fullprint << "sample of size " << size << " first=" << oneSample[0] << " last=" << oneSample[oneSample.getSize() - 1] << std::endl;
    Point mean(oneSample.computeMean());
    fullprint << "mean error (relative)=" << (mean - meanPoint).norm() / meanPoint.norm() << std::endl;
    CovarianceMatrix covariance(oneSample.computeCovariance());
    Scalar errorCovariance = 0.0;
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      for (UnsignedInteger j = 0; j < dim; j++)
      {
        errorCovariance += std::abs(covariance(i, j) - sigma[i] * sigma[j] * R(i, j));
      }
    }
    fullprint << "covariance error (absolute, normalized)=" << errorCovariance / (dim * dim) << std::endl;

    // Define a point
    Point zero(dim, 0.0);

    // Show PDF of zero point
    Scalar zeroPDF = distribution.computePDF( zero );
    Scalar zeroCDF = distribution.computeCDF( zero );
    fullprint << "Zero point= " << zero
              << " pdf=" << zeroPDF
              << " cdf=" << zeroCDF
              << " density generator=" << distribution.computeDensityGenerator(0.0)
              << std::endl;
    // Extract the marginals
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      Distribution margin(distribution.getMarginal(i));
      fullprint << "margin=" << margin << std::endl;
      fullprint << "margin PDF=" << margin.computePDF(Point(1)) << std::endl;
      fullprint << "margin CDF=" << margin.computeCDF(Point(1)) << std::endl;
      fullprint << "margin quantile=" << margin.computeQuantile(0.5) << std::endl;
      fullprint << "margin realization=" << margin.getRealization() << std::endl;
    }

    // Extract a 2-D marginal
    Indices indices(2, 0);
    indices[0] = 1;
    indices[1] = 0;
    fullprint << "indices=" << indices << std::endl;
    Distribution margins(distribution.getMarginal(indices));
    fullprint << "margins=" << margins << std::endl;
    fullprint << "margins PDF=" << margins.computePDF(Point(2)) << std::endl;
    fullprint << "margins CDF=" << margins.computeCDF(Point(2)) << std::endl;
    Point quantile = margins.computeQuantile(0.5);
    fullprint << "margins quantile=" << quantile << std::endl;
    fullprint << "margins CDF(quantile)=" << margins.computeCDF(quantile) << std::endl;
    fullprint << "margins realization=" << margins.getRealization() << std::endl;


    // Very big test case for independent components
    dim = 200;
    meanPoint = Point(dim, 0.1);
    sigma = Point(dim, 1.0);

    distribution = Normal(meanPoint, sigma, IdentityMatrix(dim));
    fullprint << "Has independent copula? " << (distribution.hasIndependentCopula() ? "true" : "false") << std::endl;
    // Test for sampling
    oneSample = distribution.getSample( size / 10 );
    fullprint << "sample of size " << size << " first=" << oneSample[0] << " last=" << oneSample[oneSample.getSize() - 1] << std::endl;
    mean = oneSample.computeMean();
    fullprint << "mean error (relative)=" << (mean - meanPoint).norm() / meanPoint.norm() << std::endl;
    covariance = oneSample.computeCovariance();
    errorCovariance = 0.0;
    for (UnsignedInteger i = 0; i < dim; i++)
    {
      for (UnsignedInteger j = 0; j < dim; j++)
      {
        errorCovariance += std::abs(covariance(i, j) - (i == j ? sigma[i] * sigma[j] : 0));
      }
    }
    fullprint << "covariance error (absolute, normalized)=" << errorCovariance / (dim * dim) << std::endl;

    // Define a point
    zero = Point(dim, 0.0);

    // Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF( zero );
    zeroCDF = distribution.computeCDF( zero );
    fullprint << "Zero point= " << zero
              << " pdf=" << zeroPDF
              << " cdf=" << zeroCDF
              << " density generator=" << distribution.computeDensityGenerator(0.0)
              << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
