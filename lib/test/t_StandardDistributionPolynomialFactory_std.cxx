//                                               -*- C++ -*-
/**
 *  @brief The test file of class StandardDistributionPolynomialFactory for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

UniVariatePolynomial clean(const UniVariatePolynomial & polynomial)
{
  Point coefficients(polynomial.getCoefficients());
  for (UnsignedInteger i = 0; i < coefficients.getDimension(); ++i)
    if (std::abs(coefficients[i]) < 1.0e-10) coefficients[i] = 0.0;
  return coefficients;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    const UnsignedInteger iMax = 5;
    Collection<Distribution> distributionCollection;
    distributionCollection.add(Laplace(0.0, 1.0));
    distributionCollection.add(Logistic(0.0, 1.0));
    distributionCollection.add(Normal(0.0, 1.0));
    distributionCollection.add(Normal(1.0, 1.0));
    distributionCollection.add(Rayleigh(1.0));
    distributionCollection.add(Student(22.0));
    distributionCollection.add(Triangular(-1.0, 0.3, 1.0));
    distributionCollection.add(Uniform(-1.0, 1.0));
    distributionCollection.add(Uniform(-1.0, 3.0));
    distributionCollection.add(WeibullMin(1.0, 3.0));
    distributionCollection.add(Beta(1.0, 2.0, -1.0, 1.0));
    distributionCollection.add(Beta(0.5, 0.5, -1.0, 1.0));
    distributionCollection.add(Beta(0.5, 0.5, -2.0, 3.0));
    distributionCollection.add(Gamma(1.0, 3.0));
    distributionCollection.add(Arcsine());
    for (UnsignedInteger n = 0; n < distributionCollection.getSize(); ++n)
    {
      const Distribution distribution(distributionCollection[n]);
      const String name(distribution.getImplementation()->getClassName());
      // Must use this construction for some compilers
      StandardDistributionPolynomialFactory polynomialFactory = StandardDistributionPolynomialFactory(AdaptiveStieltjesAlgorithm(distribution));
      fullprint << "polynomialFactory(" << name << "=" << polynomialFactory << std::endl;
      for (UnsignedInteger i = 0; i < iMax; ++i)
        fullprint << name << " polynomial(" << i << ")=" << clean(polynomialFactory.build(i)).__str__() << std::endl;
      Point roots(polynomialFactory.getRoots(iMax - 1));
      fullprint << name << " polynomial(" << iMax - 1 << ") roots=" << roots << std::endl;
      Point weights;
      Point nodes(polynomialFactory.getNodesAndWeights(iMax - 1, weights));
      fullprint << name << " polynomial(" << iMax - 1 << ") nodes=" << nodes << " and weights=" << weights <<  std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
