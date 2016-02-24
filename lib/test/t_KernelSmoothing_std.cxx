//                                               -*- C++ -*-
/**
 *  @brief The test file of class KernelSmoothing for standard methods
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
    UnsignedInteger dim(2);
    NumericalPoint meanPoint(dim, 1.0);
    meanPoint[0] = 0.5;
    meanPoint[1] = -0.5;
    NumericalPoint sigma(dim, 1.0);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.5;
    }
    // Instanciate one distribution object
    Normal distribution(meanPoint, sigma, R);
    UnsignedInteger discretization(300);
    NumericalSample sample(distribution.getSample(discretization));
    Collection<Distribution> kernels;
    kernels.add(Normal());
    kernels.add(Epanechnikov());
    kernels.add(Uniform());
    kernels.add(Triangular());
    kernels.add(Logistic());
    kernels.add(Beta(2.0, 4.0, -1.0, 1.0));
    kernels.add(Beta(3.0, 6.0, -1.0, 1.0));
    for (UnsignedInteger i = 0; i < kernels.getSize(); ++i)
    {
      Distribution kernel(kernels[i]);
      fullprint << "kernel=" << kernel.getName() << std::endl;
      KernelSmoothing smoother(kernel);
      Distribution smoothed(smoother.build(sample));
      fullprint << "kernel bandwidth=[" << smoother.getBandwidth()[0] << ", " << smoother.getBandwidth()[1] << "]" << std::endl;
      // Check moments
      fullprint << "mean(smoothed)=[" << smoothed.getMean()[0] << ", " << smoothed.getMean()[1] << "] mean(exact)=[" << distribution.getMean()[0] << ", " << distribution.getMean()[1] << "]" << std::endl;
      fullprint << "covariance(smoothed)=" << smoothed.getCovariance() << " covariance(exact)=" << distribution.getCovariance() << std::endl;
      // Define a point
      NumericalPoint point( smoothed.getDimension(), 0.0 );

      // Show PDF and CDF of point point
      NumericalScalar pointPDF = smoothed.computePDF( point );
      NumericalScalar pointCDF = smoothed.computeCDF( point );
      fullprint << "Point= " << point << std::endl;
      fullprint << " pdf(smoothed)= " << pointPDF << " pdf(exact)=" << distribution.computePDF( point ) << std::endl;
      fullprint << " cdf(smoothed)= " << pointCDF << " cdf(exact)=" << distribution.computeCDF( point ) << std::endl;
    }
    // Test for boundary correction
    Collection<Distribution> distributionCollection(2);
    distributionCollection[0] = Normal(0.0, 1.0);
    distributionCollection[1] = Beta(0.7, 1.6, -1.0, 2.0);
    Collection<NumericalSample> sampleCollection(2);
    sampleCollection[0] = distributionCollection[0].getSample(discretization);
    sampleCollection[1] = distributionCollection[1].getSample(discretization);
    Collection<UnsignedInteger> bounded(2);
    bounded[0] = 0;
    bounded[1] = 1;
    for (UnsignedInteger i = 0; i < kernels.getSize(); ++i)
    {
      Distribution kernel(kernels[i]);
      fullprint << "kernel=" << kernel.getName() << std::endl;
      KernelSmoothing smoother(kernel);
      for (UnsignedInteger j = 0; j < 2; ++j)
      {
        NumericalScalar hSilverman(smoother.computeSilvermanBandwidth(sampleCollection[j])[0]);
        NumericalScalar hPlugin(smoother.computePluginBandwidth(sampleCollection[j])[0]);
        NumericalScalar hMixed(smoother.computeMixedBandwidth(sampleCollection[j])[0]);
        fullprint << "Silverman's bandwidth=" << hSilverman << " plugin bandwidth=" << hPlugin << " mixed bandwidth=" << hMixed << std::endl;
        for (UnsignedInteger k = 0; k < 2; ++k)
        {
          Distribution smoothed(smoother.build(sampleCollection[j], bounded[k]));
          fullprint << "Bounded underlying distribution? " << (j == 0 ? "False" : "True") << " bounded reconstruction? " << (k == 0 ? "False" : "True") << std::endl;
          // Define a point
          NumericalPoint point( smoothed.getDimension(), -0.9 );

          // Show PDF and CDF of point point
          NumericalScalar pointPDF = smoothed.computePDF( point );
          NumericalScalar pointCDF = smoothed.computeCDF( point );
          fullprint << " pdf(smoothed)= " << pointPDF << " pdf(exact)=" << distributionCollection[j].computePDF( point ) << std::endl;
          fullprint << " cdf(smoothed)= " << pointCDF << " cdf(exact)=" << distributionCollection[j].computeCDF( point ) << std::endl;
        }
      }
    }
    // Test with varying binning
    {
      NumericalSample sample(Normal().getSample(5000));
      Distribution ks1(KernelSmoothing(Normal(), true, 64).build(sample));
      Distribution ks2(KernelSmoothing(Normal(), true, 1024).build(sample));
      Distribution ks3(KernelSmoothing(Normal(), false).build(sample));
      NumericalPoint point(1, 0.3);
      fullprint << "with low  bin count, pdf=" << ks1.computePDF(point) << std::endl;
      fullprint << "with high bin count, pdf=" << ks2.computePDF(point) << std::endl;
      fullprint << "without   binning,   pdf=" << ks3.computePDF(point) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
