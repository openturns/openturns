//                                               -*- C++ -*-
/**
 *  @brief The test file of FunctionalChaosAlgoritm class
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
    UnsignedInteger dimension = 3;
    // Create the orthogonal basis
    OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomialCollection(dimension);
    polynomialCollection[0] = LegendreFactory();
    polynomialCollection[1] = LegendreFactory();
    polynomialCollection[2] = LegendreFactory();

    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);
    fullprint << productBasis.__str__() << std::endl;
    fullprint << productBasis.__repr_markdown__() << std::endl;
    
    // Heterogeneous collection
    OrthogonalProductPolynomialFactory::PolynomialFamilyCollection polynomCollection2(dimension);
    polynomCollection2[0] = LaguerreFactory(2.5);
    polynomCollection2[1] = LegendreFactory();
    polynomCollection2[2] = HermiteFactory();
    OrthogonalProductPolynomialFactory productBasis2(polynomCollection2);
    fullprint << productBasis2.__str__() << std::endl;
    fullprint << productBasis2.__repr_markdown__() << std::endl;
    
    // Collection based on distributions
    Collection<Distribution> marginals(dimension, Uniform(0.0, 1.0));
    OrthogonalProductPolynomialFactory productBasis3(marginals);
    fullprint << productBasis3.__str__() << std::endl;
    fullprint << productBasis3.__repr_markdown__() << std::endl;

    // Collection based on a complex collection of distributions
    ComposedDistribution::DistributionCollection aCollection4;
    Normal marginalN(0.0, 1.0);
    aCollection4.add( Distribution(marginalN) );
    Uniform marginalU(12345.6, 123456.7);
    aCollection4.add( Distribution(marginalU) );
    TruncatedDistribution marginalTN(Normal(2.0, 1.5), 1.0, 4.0);
    aCollection4.add( Distribution(marginalTN) );
    OrthogonalProductPolynomialFactory productBasis4(aCollection4);
    fullprint << productBasis4.__str__() << std::endl;
    fullprint << productBasis4.__repr_markdown__() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
