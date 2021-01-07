//                                               -*- C++ -*-
/**
 *  @brief The test file of class KernelSmoothing for standard methods
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
  PlatformInfo::SetNumericalPrecision(4);
  try
  {
    UnsignedInteger dim = 2;
    Point meanPoint(dim, 1.0);
    meanPoint[0] = 0.5;
    meanPoint[1] = -0.5;
    Point sigma(dim, 1.0);
    sigma[0] = 2.0;
    sigma[1] = 3.0;
    CorrelationMatrix R(dim);
    for (UnsignedInteger i = 1; i < dim; i++)
    {
      R(i, i - 1) = 0.5;
    }
    // Instanciate one distribution object
    Normal distribution(meanPoint, sigma, R);
    UnsignedInteger discretization = 300;
    Sample sample(distribution.getSample(discretization));
    Collection<Distribution> kernels;
    kernels.add(Normal());
    kernels.add(Epanechnikov());
    kernels.add(Uniform());
    kernels.add(Triangular());
    kernels.add(Logistic());
    kernels.add(Beta(2.0, 2.0, -1.0, 1.0));
    kernels.add(Beta(3.0, 3.0, -1.0, 1.0));
    for (UnsignedInteger i = 0; i < kernels.getSize(); ++i)
    {
      Distribution kernel(kernels[i]);
      fullprint << "kernel=" << kernel.getName() << std::endl;
      KernelSmoothing smoother(kernel);
      Distribution smoothed(smoother.build(sample));
      fullprint << "kernel bandwidth=[" << std::setprecision(4) << smoother.getBandwidth()[0] << ", " << std::setprecision(4) << smoother.getBandwidth()[1] << "]" << std::endl;
      // Check moments
      fullprint << "mean(smoothed)=[" << std::setprecision(4) << smoothed.getMean()[0] << ", " << std::setprecision(4) << smoothed.getMean()[1] << "] mean(exact)=[" << distribution.getMean()[0] << ", " << distribution.getMean()[1] << "]" << std::endl;
      fullprint << "covariance(smoothed)=" << smoothed.getCovariance() << " covariance(exact)=" << distribution.getCovariance() << std::endl;
      // Define a point
      Point point( smoothed.getDimension(), 0.0 );

      // Show PDF and CDF of point point
      Scalar pointPDF = smoothed.computePDF( point );
      Scalar pointCDF = smoothed.computeCDF( point );
      fullprint << "Point= " << point << std::endl;
      fullprint << " pdf(smoothed)= " << std::setprecision(4) << pointPDF << " pdf(exact)=" << distribution.computePDF( point ) << std::endl;
      fullprint << " cdf(smoothed)= " << std::setprecision(4) << pointCDF << " cdf(exact)=" << distribution.computeCDF( point ) << std::endl;
    }
    // Test for boundary correction
    Collection<Distribution> distributionCollection(2);
    distributionCollection[0] = Normal(0.0, 1.0);
    distributionCollection[1] = Beta(0.7, 0.9, -1.0, 2.0);
    Collection<Sample> sampleCollection(2);
    sampleCollection[0] = distributionCollection[0].getSample(discretization);
    sampleCollection[1] = distributionCollection[1].getSample(discretization);
    for (UnsignedInteger i = 0; i < kernels.getSize(); ++i)
    {
      Distribution kernel(kernels[i]);
      fullprint << "kernel=" << kernel.getName() << std::endl;
      KernelSmoothing smoother(kernel);
      for (UnsignedInteger j = 0; j < 2; ++j)
      {
        Scalar hSilverman = smoother.computeSilvermanBandwidth(sampleCollection[j])[0];
        Scalar hPlugin = smoother.computePluginBandwidth(sampleCollection[j])[0];
        Scalar hMixed = smoother.computeMixedBandwidth(sampleCollection[j])[0];
        fullprint << "Silverman's bandwidth=" << std::setprecision(4) << hSilverman << " plugin bandwidth=" << std::setprecision(4) << hPlugin << " mixed bandwidth=" << std::setprecision(4) << hMixed << std::endl;
        for (UnsignedInteger k = 0; k < 2; ++k)
        {
          smoother.setBoundaryCorrection(k == 1);
          Distribution smoothed(smoother.build(sampleCollection[j]));
          fullprint << "Bounded underlying distribution? " << (j == 0 ? "False" : "True") << " bounded reconstruction? " << (k == 0 ? "False" : "True") << std::endl;
          // Define a point
          Point point( smoothed.getDimension(), -0.9 );

          // Show PDF and CDF of point point
          Scalar pointPDF = smoothed.computePDF( point );
          Scalar pointCDF = smoothed.computeCDF( point );
          fullprint << " pdf(smoothed)= " << std::setprecision(4) << pointPDF << " pdf(exact)=" << distributionCollection[j].computePDF( point ) << std::endl;
          fullprint << " cdf(smoothed)= " << std::setprecision(4) << pointCDF << " cdf(exact)=" << distributionCollection[j].computeCDF( point ) << std::endl;
        }
      }
    }
    // Test with varying binning
    {
      sample = Normal().getSample(5000);
      Distribution ks1(KernelSmoothing(Normal(), true, 64).build(sample));
      Distribution ks2(KernelSmoothing(Normal(), true, 1024).build(sample));
      Distribution ks3(KernelSmoothing(Normal(), false).build(sample));
      Point point(1, 0.3);
      fullprint << "with low  bin count, pdf=" << std::setprecision(4) << ks1.computePDF(point) << std::endl;
      fullprint << "with high bin count, pdf=" << std::setprecision(4) << ks2.computePDF(point) << std::endl;
      fullprint << "without   binning,   pdf=" << std::setprecision(4) << ks3.computePDF(point) << std::endl;
    }
    // Test with varying boundary corrections
    {
      Point left(1, -0.9);
      Point right(1, 0.9);
      sample = Uniform().getSample(500);
      KernelSmoothing algo1(Normal(), false);
      algo1.setBoundingOption(KernelSmoothing::NONE);
      Distribution ks1 = algo1.build(sample);
      fullprint << "with no boundary correction, pdf(left)=" << std::setprecision(4) << ks1.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks1.computePDF(right) << std::endl;

      KernelSmoothing algo2(Normal(), false);
      algo2.setBoundingOption(KernelSmoothing::LOWER);
      algo2.setAutomaticLowerBound(true);
      Distribution ks2 = algo2.build(sample);
      fullprint << "with automatic lower boundary correction, pdf(left)=" << std::setprecision(4) << ks2.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks2.computePDF(right) << std::endl;

      KernelSmoothing algo3(Normal(), false);
      algo3.setBoundingOption(KernelSmoothing::LOWER);
      algo3.setLowerBound(-1.0);
      algo3.setAutomaticLowerBound(false);
      Distribution ks3 = algo3.build(sample);
      fullprint << "with user defined lower boundary correction, pdf(left)=" << std::setprecision(4) << ks3.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks3.computePDF(right) << std::endl;

      KernelSmoothing algo4(Normal(), false);
      algo4.setBoundingOption(KernelSmoothing::UPPER);
      algo4.setAutomaticUpperBound(true);
      Distribution ks4 = algo4.build(sample);
      fullprint << "with automatic upper boundary correction, pdf(left)=" << std::setprecision(4) << ks4.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks4.computePDF(right) << std::endl;

      KernelSmoothing algo5(Normal(), false);
      algo5.setBoundingOption(KernelSmoothing::UPPER);
      algo5.setUpperBound(1.0);
      algo5.setAutomaticLowerBound(false);
      Distribution ks5 = algo5.build(sample);
      fullprint << "with user defined upper boundary correction, pdf(left)=" << std::setprecision(4) << ks5.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks5.computePDF(right) << std::endl;

      KernelSmoothing algo6(Normal(), false);
      algo6.setBoundingOption(KernelSmoothing::BOTH);
      Distribution ks6 = algo6.build(sample);
      fullprint << "with automatic boundaries correction, pdf(left)=" << std::setprecision(4) << ks6.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks6.computePDF(right) << std::endl;

      KernelSmoothing algo7(Normal(), false);
      algo7.setBoundingOption(KernelSmoothing::BOTH);
      algo7.setLowerBound(-1.0);
      Distribution ks7 = algo7.build(sample);
      fullprint << "with user defined lower/automatic upper boundaries correction, pdf(left)=" << std::setprecision(4) << ks7.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks7.computePDF(right) << std::endl;

      KernelSmoothing algo8(Normal(), false);
      algo8.setBoundingOption(KernelSmoothing::BOTH);
      algo8.setUpperBound(1.0);
      Distribution ks8 = algo8.build(sample);
      fullprint << "with automatic lower/user defined upper boundaries correction, pdf(left)=" << std::setprecision(4) << ks8.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks8.computePDF(right) << std::endl;

      KernelSmoothing algo9(Normal(), false);
      algo9.setBoundingOption(KernelSmoothing::BOTH);
      algo9.setLowerBound(-1.0);
      algo9.setUpperBound(1.0);
      Distribution ks9 = algo9.build(sample);
      fullprint << "with user defined boundaries correction, pdf(left)=" << std::setprecision(4) << ks9.computePDF(left) << ", pdf(right)=" << std::setprecision(4) << ks9.computePDF(right) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
