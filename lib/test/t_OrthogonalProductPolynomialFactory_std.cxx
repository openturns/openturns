//                                               -*- C++ -*-
/**
 *  @brief The test file of OrthogonalProductPolynomialFactory class
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

// Compute reference function value from index and point
Point computePolynomialValue(const UnsignedInteger & index, const Point & point) 
{    
    if (point.getDimension() != 3)
      throw InvalidArgumentException(HERE) << "Expected a dimension 3 point, but dimension is " << point.getDimension();
    const UnsignedInteger dimension = 3;
    const LinearEnumerateFunction enumerate(dimension);
    // Compute the multi-indices using the EnumerateFunction
    Indices indices(enumerate(index));
    // Then build the collection of polynomials using the collection of factories
    ProductPolynomialEvaluation::PolynomialCollection polynomials(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      polynomials[i] = LegendreFactory().build(indices[i]);
    }
    const ProductPolynomialEvaluation product(polynomials);
    const Point value(product(point));
    return value;
}

// Compute reference function value from multi-index and point
Point computePolynomialValue(const Indices & indices, const Point & point) 
{    
    if (point.getDimension() != 3)
      throw InvalidArgumentException(HERE) << "Expected a dimension 3 point, but dimension is " << point.getDimension();
    const UnsignedInteger dimension = 3;
    const LinearEnumerateFunction enumerate(dimension);
    const UnsignedInteger index = enumerate.inverse(indices);
    const Point value(computePolynomialValue(index, point));
    return value;
}

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
    // Test the build() method on a collection of functions
    const Point center({0.5, 0.5, 0.5});
    for (UnsignedInteger i = 0; i < 10; ++ i)
    {
      // Test build from index
      const Function polynomial(productBasis.build(i));
      assert_almost_equal(polynomial(center), computePolynomialValue(i, center));
      // Test build from multi-index
      const Indices indices(enumerateFunction(i));
      const Function polynomial2(productBasis.build(indices));
      assert_almost_equal(polynomial2(center), computePolynomialValue(indices, center));
    }

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
    JointDistribution::DistributionCollection aCollection4;
    Normal marginalN(0.0, 1.0);
    aCollection4.add( Distribution(marginalN) );
    Uniform marginalU(12345.6, 123456.7);
    aCollection4.add( Distribution(marginalU) );
    TruncatedDistribution marginalTN(Normal(2.0, 1.5), 1.0, 4.0);
    aCollection4.add( Distribution(marginalTN) );
    OrthogonalProductPolynomialFactory productBasis4(aCollection4);
    fullprint << productBasis4.__str__() << std::endl;
    fullprint << productBasis4.__repr_markdown__() << std::endl;

    // Test getMarginal
    fullprint << "Test getMarginal" << std::endl;
    UnsignedInteger dimension2 = 5;
    Collection<Distribution> marginals4(dimension2, Uniform(0.0, 1.0));
    OrthogonalProductPolynomialFactory productBasis5(marginals4);
    Indices indices({0, 2, 4});
    OrthogonalBasis productBasis6(productBasis5.getMarginal(indices));
    fullprint << productBasis6.__str__() << std::endl;
    // Test the build() method on a collection of functions
    const Point center2({0.5, 0.5, 0.5});
    for (UnsignedInteger i = 0; i < 10; ++ i)
    {
      // Test build from index
      const Function polynomial(productBasis6.build(i));
      assert_almost_equal(polynomial(center2), computePolynomialValue(i, center2));
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
