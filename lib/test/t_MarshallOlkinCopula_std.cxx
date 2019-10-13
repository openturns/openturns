//                                               -*- C++ -*-
/**
 *  @brief The test file of class MarshallOlkin for standard methods
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

// Check if point in [0,1]^2
void check2DPointInUnitSquare(Point point)
{
  const UnsignedInteger dim = 2;
  if (point.getDimension()!=2) throw TestFailed("Point is not dimension 2");
  for ( UnsignedInteger index = 0; index < dim; ++ index)
  {
    if (SpecFunc::IsNaN(point[index])) throw TestFailed("Point component is nan");
    if (point[index]>1.) throw TestFailed("Point component is >1");
    if (point[index]<0.) throw TestFailed("Point component is <0");
  }
}
// Check if bool is true
void assertTrue(bool condition)
{
  if (!condition) throw TestFailed("Boolean is not true");
}

// Check if bool is true
void assertEqual(Scalar value1, Scalar value2)
{
  if (value1!=value2) throw TestFailed("Values are not equal");
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instanciate one distribution object
    MarshallOlkinCopula copula(0.5,0.5);
    fullprint << "Copula " << copula << std::endl;
    std::cout << "Copula " << copula << std::endl;

    fullprint << "Dimension " << copula.getDimension() << std::endl;

    // Is this copula continuous ?
    fullprint << "Continuous = " << (copula.isContinuous() ? "true" : "false") << std::endl;

    // Is this copula independent ?
    fullprint << "Independent = " << copula.hasIndependentCopula()  << std::endl;

    // Compute PDF
    Point half(2,0.5);
    Scalar pointPDF = copula.computePDF( half );
    fullprint << "PDF at half = " << pointPDF << std::endl;

    // Compute Kendall's tau
    fullprint << "Kendall's tau = " << copula.getKendallTau() << std::endl;

    // Compute Spearman's rho
    fullprint << "Spearman's rho = " << copula.getSpearmanCorrelation() << std::endl;

    // Test for realization of copula
    Point oneRealization = copula.getRealization();
    fullprint << "oneRealization=" << oneRealization << std::endl;

    // Test for sampling
    UnsignedInteger size = 10000;
    Sample oneSample(copula.getSample( size ));
    fullprint << "oneSample first=" << oneSample[0] << " last=" << oneSample[size - 1] << std::endl;
    fullprint << "mean=" << oneSample.computeMean() << std::endl;
    fullprint << "covariance=" << oneSample.computeCovariance() << std::endl;
    fullprint << "Sample Kendall's tau =" << oneSample.computeKendallTau() << std::endl;
    fullprint << "Exact Kendall's tau =" << copula.getKendallTau() << std::endl;
    fullprint << "Sample Spearman's rho =" << oneSample.computeSpearmanCorrelation() << std::endl;
    fullprint << "Exact Spearman's rho =" << copula.getSpearmanCorrelation() << std::endl;

    // Compute CDF
    // x=[0.0,0.0]
    Point zero(2);
    Scalar pointCDFAtZero = copula.computeCDF( zero );
    assertEqual(pointCDFAtZero, 0.0);

    // x=[1.0,1.0]
    Point one(2);
    one[0] = 1.0;
    one[1] = 1.0;
    Scalar pointCDFAtOne = copula.computeCDF( one );
    assertEqual(pointCDFAtOne, 1.0);

    // x=[0.5,0.5]
    Scalar pointCDFAtHalf = copula.computeCDF( half );
    fullprint << "x=" << half << ", CDF at half = " << pointCDFAtHalf << std::endl;

    // copula.drawCDF().draw("pdf.png");
    
    // Special cases

    // Special case alpha=0
    copula = MarshallOlkinCopula(0.0,0.5);
    bool isIndependent;
    isIndependent = copula.hasIndependentCopula();
    assertTrue(isIndependent);
    Point random(2); 
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    CorrelationMatrix correlation;
    correlation = copula.getSpearmanCorrelation();
    assertEqual(correlation(0,1), 0.0);
    correlation = copula.getKendallTau();
    assertEqual(correlation(0,1), 0.0);

    // Special case beta=0
    copula = MarshallOlkinCopula(0.5,0.0);
    isIndependent = copula.hasIndependentCopula();
    assertTrue(isIndependent);
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    correlation = copula.getSpearmanCorrelation();
    assertEqual(correlation(0,1), 0.0);
    correlation = copula.getKendallTau();
    assertEqual(correlation(0,1), 0.0);

    // Special case alpha=beta=1
    copula = MarshallOlkinCopula(1.0,1.0);
    isIndependent = copula.hasIndependentCopula();
    assertTrue(!isIndependent);
    random = copula.getRealization();
    check2DPointInUnitSquare(random);
    correlation = copula.getSpearmanCorrelation();
    assertEqual(correlation(0,1), 1.0);
    correlation = copula.getKendallTau();
    assertEqual(correlation(0,1), 1.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
