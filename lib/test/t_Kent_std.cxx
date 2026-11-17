//                                               -*- C++ -*-
/**
 *  @brief The test file of class Kent for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

  try
  {
    // Default constructor
    Kent defaultDist;
    fullprint << "Default distribution=" << defaultDist << std::endl;
    assert_almost_equal(defaultDist.getKappa(), 1.0, 0.0, 0.0);
    assert_almost_equal(defaultDist.getBeta(), 0.25, 0.0, 0.0);
    SquareMatrix defaultGamma(defaultDist.getGamma());
    assert_almost_equal(defaultGamma(0, 0), 1.0, 0.0, 0.0);
    assert_almost_equal(defaultGamma(1, 1), 1.0, 0.0, 0.0);
    assert_almost_equal(defaultGamma(2, 2), 1.0, 0.0, 0.0);

    // Parameterized constructor
    SquareMatrix gamma(3);
    gamma(0, 0) = 1.0;
    gamma(0, 1) = 0.0;
    gamma(0, 2) = 0.0;
    gamma(1, 0) = 0.0;
    gamma(1, 1) = 1.0;
    gamma(1, 2) = 0.0;
    gamma(2, 0) = 0.0;
    gamma(2, 1) = 0.0;
    gamma(2, 2) = 1.0;
    Kent dist(25.0, 0.5, gamma);
    fullprint << "Kent(25, 0.5, I)=" << dist << std::endl;
    assert_almost_equal(dist.getKappa(), 25.0, 0.0, 0.0);
    assert_almost_equal(dist.getBeta(), 0.5, 0.0, 0.0);
    assert(dist.isContinuous());

    // Dimension
    assert_equal(dist.getDimension(), static_cast<UnsignedInteger>(3));

    // getEpsilon
    assert_almost_equal(dist.getEpsilon(), ResourceMap::GetAsScalar("Kent-OrthogonalityThreshold"), 0.0, 0.0);

    // Range
    Interval range(dist.getRange());
    assert_almost_equal(range.getLowerBound()[0], -1.0, 0.0, 0.0);
    assert_almost_equal(range.getUpperBound()[0], 1.0, 0.0, 0.0);

    // PDF and log-PDF at a point on the sphere
    Point point(3);
    point[0] = 1.0;
    point[1] = 0.0;
    point[2] = 0.0;
    Scalar pdf = dist.computePDF(point);
    Scalar logpdf = dist.computeLogPDF(point);
    fullprint << "PDF(1,0,0)=" << pdf << std::endl;
    fullprint << "logPDF(1,0,0)=" << logpdf << std::endl;
    assert(pdf > 0.0);

    // PDF at off-sphere point should be 0
    Point offSphere(3);
    offSphere[0] = 0.5;
    offSphere[1] = 0.0;
    offSphere[2] = 0.0;
    assert_almost_equal(dist.computePDF(offSphere), 0.0, 0.0, 0.0);

    // getRealization
    Point realization(dist.getRealization());
    assert_equal(realization.getDimension(), static_cast<UnsignedInteger>(3));
    assert_almost_equal(realization.norm(), 1.0, 0.0, 1.0e-14);

    // getParameter / setParameter
    Point param(dist.getParameter());
    assert_equal(param.getDimension(), static_cast<UnsignedInteger>(11));
    assert_almost_equal(param[0], 25.0, 0.0, 0.0);
    assert_almost_equal(param[1], 0.5, 0.0, 0.0);

    // getParameterDescription
    Description paramDesc(dist.getParameterDescription());
    assert_equal(paramDesc.getSize(), static_cast<UnsignedInteger>(11));
    assert_equal(paramDesc[0], String("kappa"));
    assert_equal(paramDesc[1], String("beta"));

    // Clone
    Kent * cloneDist = dist.clone();
    fullprint << "Cloned=" << *cloneDist << std::endl;
    assert_almost_equal(cloneDist->getKappa(), dist.getKappa(), 0.0, 0.0);
    assert_almost_equal(cloneDist->getBeta(), dist.getBeta(), 0.0, 0.0);
    delete cloneDist;

    // setKappa
    Kent dist2(10.0, 0.5, gamma);
    dist2.setKappa(15.0);
    assert_almost_equal(dist2.getKappa(), 15.0, 0.0, 0.0);
    // setKappa to same value (no-op)
    dist2.setKappa(15.0);
    assert_almost_equal(dist2.getKappa(), 15.0, 0.0, 0.0);

    // setBeta
    dist2.setBeta(1.0);
    assert_almost_equal(dist2.getBeta(), 1.0, 0.0, 0.0);
    // setBeta to same value (no-op)
    dist2.setBeta(1.0);
    assert_almost_equal(dist2.getBeta(), 1.0, 0.0, 0.0);
    // setBeta to 0 (von Mises-Fisher)
    dist2.setBeta(0.0);
    assert_almost_equal(dist2.getBeta(), 0.0, 0.0, 0.0);

    // setGamma
    Scalar theta = 0.5;
    SquareMatrix rotGamma(3);
    rotGamma(0, 0) = std::cos(theta);
    rotGamma(0, 1) = -std::sin(theta);
    rotGamma(0, 2) = 0.0;
    rotGamma(1, 0) = std::sin(theta);
    rotGamma(1, 1) = std::cos(theta);
    rotGamma(1, 2) = 0.0;
    rotGamma(2, 0) = 0.0;
    rotGamma(2, 1) = 0.0;
    rotGamma(2, 2) = 1.0;
    dist2.setGamma(rotGamma);
    SquareMatrix gotGamma(dist2.getGamma());
    assert_almost_equal(gotGamma(0, 0), std::cos(theta), 0.0, 0.0);
    assert_almost_equal(gotGamma(0, 1), -std::sin(theta), 0.0, 0.0);

    // setParameter
    Kent dist3;
    dist3.setParameter(param);
    assert_almost_equal(dist3.getKappa(), 25.0, 0.0, 0.0);
    assert_almost_equal(dist3.getBeta(), 0.5, 0.0, 0.0);

    // Mean
    Point mean(dist.getMean());
    fullprint << "Mean=" << mean << std::endl;
    assert_equal(mean.getDimension(), static_cast<UnsignedInteger>(3));

    // Validation: kappa <= 0 should throw
    try
    {
      Kent badDist(-1.0, 0.5, gamma);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for kappa <= 0" << std::endl;
    }

    // Validation: 2*beta >= kappa should throw
    try
    {
      Kent badDist(1.0, 1.0, gamma);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for 2*beta >= kappa" << std::endl;
    }

    // Validation: non-orthogonal gamma should throw
    try
    {
      SquareMatrix badGamma(3);
      badGamma(0, 0) = 2.0;
      badGamma(0, 1) = 0.0;
      badGamma(0, 2) = 0.0;
      badGamma(1, 0) = 0.0;
      badGamma(1, 1) = 1.0;
      badGamma(1, 2) = 0.0;
      badGamma(2, 0) = 0.0;
      badGamma(2, 1) = 0.0;
      badGamma(2, 2) = 1.0;
      Kent badDist(1.0, 0.25, badGamma);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for non-orthogonal gamma" << std::endl;
    }

    // Validation: wrong dimension gamma should throw
    try
    {
      SquareMatrix badGamma(2);
      badGamma(0, 0) = 1.0;
      badGamma(0, 1) = 0.0;
      badGamma(1, 0) = 0.0;
      badGamma(1, 1) = 1.0;
      Kent badDist(1.0, 0.25, badGamma);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for wrong dimension gamma" << std::endl;
    }

    // setKappa with invalid values should throw
    try
    {
      dist.setKappa(0.0);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setKappa(0)" << std::endl;
    }

    // setKappa violating 2*beta < kappa should throw
    try
    {
      dist.setKappa(0.5);  // beta=0.5, 2*beta=1.0 > 0.5
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setKappa violating constraint" << std::endl;
    }

    // setBeta with invalid value should throw
    try
    {
      dist.setBeta(-0.1);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setBeta(-0.1)" << std::endl;
    }

    // setBeta violating 2*beta < kappa should throw
    try
    {
      dist.setBeta(20.0);  // kappa=25, 2*20=40 > 25
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setBeta violating constraint" << std::endl;
    }

    // setGamma with wrong dimension should throw
    try
    {
      SquareMatrix badGamma(2);
      dist.setGamma(badGamma);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setGamma wrong dimension" << std::endl;
    }

    // setGamma with non-orthogonal should throw
    try
    {
      SquareMatrix badGamma(3);
      badGamma(0, 0) = 2.0;
      badGamma(0, 1) = 0.0;
      badGamma(0, 2) = 0.0;
      badGamma(1, 0) = 0.0;
      badGamma(1, 1) = 1.0;
      badGamma(1, 2) = 0.0;
      badGamma(2, 0) = 0.0;
      badGamma(2, 1) = 0.0;
      badGamma(2, 2) = 1.0;
      dist.setGamma(badGamma);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setGamma non-orthogonal" << std::endl;
    }

    // setParameter with wrong size should throw
    try
    {
      Point badParam(3);
      dist.setParameter(badParam);
      return ExitCode::Error;
    }
    catch (InvalidArgumentException &)
    {
      fullprint << "Caught expected exception for setParameter wrong size" << std::endl;
    }

    // Sampling quality: von Mises-Fisher case
    {
      Kent vmf(10.0, 0.0, gamma);
      Sample sample(vmf.getSample(5000));
      Point sampleMean(sample.computeMean());
      fullprint << "vMF sample mean=" << sampleMean << std::endl;
      assert(sampleMean[0] > 0.8);
    }

    // PDF consistency: computePDF should equal exp(computeLogPDF)
    {
      Scalar pdfVal = dist.computePDF(point);
      Scalar logPdfVal = dist.computeLogPDF(point);
      assert_almost_equal(pdfVal, std::exp(logPdfVal), 0.0, 1.0e-14);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
