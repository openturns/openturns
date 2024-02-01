//                                               -*- C++ -*-
/**
 *  @brief The test file of class MarshallOlkin for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

// Check if point in [0,1]^2
void check2DPointInUnitSquare(Point point)
{
  const UnsignedInteger dim = 2;
  if (point.getDimension() != 2) throw TestFailed("Point is not dimension 2");
  for ( UnsignedInteger index = 0; index < dim; ++ index)
  {
    if (std::isnan(point[index])) throw TestFailed("Point component is nan");
    if (point[index] > 1.) throw TestFailed(OSS() << "Point's component " << index << "=" << point[index] << " is greater than 1");
    if (point[index] < 0.) throw TestFailed(OSS() << "Point's component " << index << "=" << point[index] << " is lower than 0");
  }
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instantiate one distribution object
    MarshallOlkinCopula copula(0.5, 0.5);
    fullprint << "Copula " << copula << std::endl;
    std::cout << "Copula " << copula << std::endl;

    fullprint << "Dimension" << std::endl;
    int dimension = copula.getDimension();
    assert_equal(dimension, 2);

    // Is this copula continuous ?
    fullprint << "isContinuous" << std::endl;
    bool isContinuous = copula.isContinuous();
    assert_equal(isContinuous, true);

    // Is this copula independent ?
    fullprint << "hasIndependentCopula" << std::endl;
    bool isIndependent = copula.hasIndependentCopula();
    assert_equal(isIndependent, false);

    // Compute Kendall's tau
    fullprint << "getKendallTau" << std::endl;
    CorrelationMatrix correlation;
    Scalar rtol = 1.0e-14;
    correlation = copula.getKendallTau();
    assert_almost_equal(correlation(1, 0), 0.333333333333333333, rtol);

    // Compute Spearman's rho
    fullprint << "getSpearmanCorrelation" << std::endl;
    correlation = copula.getSpearmanCorrelation();
    assert_almost_equal(correlation(1, 0), 0.42857142857142855, rtol);

    // Test for realization of copula
    fullprint << "getRealization" << std::endl;
    Point oneRealization = copula.getRealization();
    check2DPointInUnitSquare(oneRealization);

    // Test for sampling
    fullprint << "getSample" << std::endl;
    UnsignedInteger size = 10000;
    Sample oneSample(copula.getSample( size ));
    check2DPointInUnitSquare(oneSample[0]);
    check2DPointInUnitSquare(oneSample[size - 1]);
    fullprint << "computeMean" << std::endl;
    Point samplemean = oneSample.computeMean();
    Scalar atol = 0.1;
    assert_almost_equal(samplemean[0], 0.5, rtol, atol);
    assert_almost_equal(samplemean[1], 0.5, rtol, atol);
    fullprint << "computeCovariance" << std::endl;
    CovarianceMatrix samplecovariance = oneSample.computeCovariance();
    assert_almost_equal(samplecovariance(0, 0), 0.0, rtol, atol);
    assert_almost_equal(samplecovariance(1, 0), 0.0, rtol, atol);
    assert_almost_equal(samplecovariance(0, 1), 0.0, rtol, atol);
    assert_almost_equal(samplecovariance(1, 1), 0.0, rtol, atol);
    fullprint << "Sample Kendall's tau vs copula" << std::endl;
    Scalar exact;
    Scalar estimator;
    exact = copula.getKendallTau()(1, 0);
    estimator = oneSample.computeKendallTau()(1, 0);
    assert_almost_equal(exact, estimator, rtol, atol);
    fullprint << "Sample Kendall's tau vs copula" << std::endl;
    exact = copula.getSpearmanCorrelation()(1, 0);
    estimator = oneSample.computeSpearmanCorrelation()(1, 0);
    assert_almost_equal(exact, estimator, rtol, atol);

    // Compute CDF
    // x=[0.0,0.0]
    Point zero(2);
    Scalar pointCDFAtZero = copula.computeCDF( zero );
    assert_equal(pointCDFAtZero, 0.0);

    // x=[1.0,1.0]
    Point one(2);
    one[0] = 1.0;
    one[1] = 1.0;
    Scalar pointCDFAtOne = copula.computeCDF( one );
    assert_equal(pointCDFAtOne, 1.0);

    // x=[0.5,0.5]
    Point half(2, 0.5);
    Scalar pointCDFAtHalf = copula.computeCDF( half );
    assert_almost_equal(pointCDFAtHalf, 0.3535533905932738, rtol);

    // Special cases

    // Special case alpha=0
    fullprint << "Special case alpha=0" << std::endl;
    copula = MarshallOlkinCopula(0.0, 0.5);
    isIndependent = copula.hasIndependentCopula();
    assert_equal(isIndependent, true);
    Point random(2);
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    correlation = copula.getSpearmanCorrelation();
    assert_equal(correlation(0, 1), 0.0);
    correlation = copula.getKendallTau();
    assert_equal(correlation(0, 1), 0.0);
    pointCDFAtHalf = copula.computeCDF( half );
    assert_equal(pointCDFAtHalf, 0.25);
    pointCDFAtZero = copula.computeCDF( zero );
    assert_equal(pointCDFAtZero, 0.);
    pointCDFAtOne = copula.computeCDF( one );
    assert_equal(pointCDFAtOne, 1.);

    // Special case beta=0
    fullprint << "Special case beta=0" << std::endl;
    copula = MarshallOlkinCopula(0.5, 0.0);
    isIndependent = copula.hasIndependentCopula();
    assert_equal(isIndependent, true);
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    correlation = copula.getSpearmanCorrelation();
    assert_equal(correlation(0, 1), 0.0);
    correlation = copula.getKendallTau();
    assert_equal(correlation(0, 1), 0.0);
    pointCDFAtHalf = copula.computeCDF( half );
    assert_equal(pointCDFAtHalf, 0.25);
    pointCDFAtZero = copula.computeCDF( zero );
    assert_equal(pointCDFAtZero, 0.);
    pointCDFAtOne = copula.computeCDF( one );
    assert_equal(pointCDFAtOne, 1.);

    // Special case alpha=beta=1
    fullprint << "Special case alpha=beta=1" << std::endl;
    copula = MarshallOlkinCopula(1.0, 1.0);
    isIndependent = copula.hasIndependentCopula();
    assert_equal(isIndependent, false);
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    correlation = copula.getSpearmanCorrelation();
    assert_equal(correlation(0, 1), 1.0);
    correlation = copula.getKendallTau();
    assert_equal(correlation(0, 1), 1.0);
    pointCDFAtHalf = copula.computeCDF( half );
    assert_equal(pointCDFAtHalf, 0.5);
    pointCDFAtZero = copula.computeCDF( zero );
    assert_equal(pointCDFAtZero, 0.);
    pointCDFAtOne = copula.computeCDF( one );
    assert_equal(pointCDFAtOne, 1.);

    // Special case alpha=beta=0
    fullprint << "Special case alpha=beta=0" << std::endl;
    copula = MarshallOlkinCopula(0.0, 0.0);
    isIndependent = copula.hasIndependentCopula();
    assert_equal(isIndependent, true);
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    correlation = copula.getSpearmanCorrelation();
    assert_equal(correlation(0, 1), 0.0);
    correlation = copula.getKendallTau();
    assert_equal(correlation(0, 1), 0.0);
    pointCDFAtHalf = copula.computeCDF( half );
    assert_equal(pointCDFAtHalf, 0.25);
    pointCDFAtZero = copula.computeCDF( zero );
    assert_equal(pointCDFAtZero, 0.);
    pointCDFAtOne = copula.computeCDF( one );
    assert_equal(pointCDFAtOne, 1.);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
