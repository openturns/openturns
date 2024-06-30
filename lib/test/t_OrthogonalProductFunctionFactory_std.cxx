//                                               -*- C++ -*-
/**
 *  @brief The test file of OrthogonalProductFunctionFactory class
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
Point computeFunctionValue(const UnsignedInteger & index, const Point & point) 
{
    if (point.getDimension() != 3)
      throw InvalidArgumentException(HERE) << "Expected a dimension 3 point, but dimension is " << point.getDimension();
    const UnsignedInteger dimension = 3;
    TensorizedUniVariateFunctionFactory::FunctionFamilyCollection functionCollection(dimension);
    functionCollection[0] = HaarWaveletFactory();
    functionCollection[1] = HaarWaveletFactory();
    functionCollection[2] = HaarWaveletFactory();
    const LinearEnumerateFunction enumerate(dimension);
    const TensorizedUniVariateFunctionFactory factory(functionCollection, enumerate);
    const Function referenceFunction(factory.build(index));
    const Point value(referenceFunction(point));
    return value;
}

// Compute reference function value from multi-index and point
Point computeFunctionValue(const Indices & indices, const Point & point) 
{    
    if (point.getDimension() != 3)
      throw InvalidArgumentException(HERE) << "Expected a dimension 3 point, but dimension is " << point.getDimension();
    const UnsignedInteger dimension = 3;
    const LinearEnumerateFunction enumerate(dimension);
    const UnsignedInteger index = enumerate.inverse(indices);
    const Point value(computeFunctionValue(index, point));
    return value;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();


  try
  {
    // Create the orthogonal basis
    fullprint << "Create the orthogonal basis" << std::endl;
    UnsignedInteger dimension = 3;
    OrthogonalProductFunctionFactory::FunctionFamilyCollection functionCollection(dimension);
    functionCollection[0] = HaarWaveletFactory();
    functionCollection[1] = HaarWaveletFactory();
    functionCollection[2] = HaarWaveletFactory();
    
    // Create linear enumerate function
    fullprint << "Create linear enumerate function" << std::endl;
    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductFunctionFactory productBasis(functionCollection, enumerateFunction);
    fullprint << productBasis.__str__() << std::endl;
    fullprint << productBasis.__repr_markdown__() << std::endl;
    // Test the build() method on a collection of functions
    const Point center({0.5, 0.5, 0.5});
    for (UnsignedInteger i = 0; i < 10; ++ i)
    {
      // Test build from index
      const Function function(productBasis.build(i));
      assert_almost_equal(function(center), computeFunctionValue(i, center));
      // Test build from multi-index
      const Indices indices(enumerateFunction(i));
      const Function function2(productBasis.build(indices));
      assert_almost_equal(function2(center), computeFunctionValue(indices, center));
    }

    // Heterogeneous collection
    fullprint << "Heterogeneous collection" << std::endl;
    OrthogonalProductFunctionFactory::FunctionFamilyCollection functionCollection2(dimension);
    functionCollection2[0] = HaarWaveletFactory();
    functionCollection2[1] = FourierSeriesFactory();
    functionCollection2[2] = HaarWaveletFactory();
    OrthogonalProductFunctionFactory productBasis2(functionCollection2);
    fullprint << productBasis2.__str__() << std::endl;
    fullprint << productBasis2.__repr_markdown__() << std::endl;
    OrthogonalProductFunctionFactory::FunctionFamilyCollection functionCollection4(productBasis2.getFunctionFamilyCollection());  
    assert_equal((int) functionCollection4.getSize(), 3);

    // Test getMarginal
    fullprint << "Test getMarginal" << std::endl;
    UnsignedInteger dimension2 = 5;
    OrthogonalProductFunctionFactory::FunctionFamilyCollection functionCollection3(dimension2);
    functionCollection3[0] = HaarWaveletFactory();
    functionCollection3[1] = FourierSeriesFactory();
    functionCollection3[2] = HaarWaveletFactory();
    functionCollection3[3] = HaarWaveletFactory();
    functionCollection3[4] = FourierSeriesFactory();
    OrthogonalProductFunctionFactory productBasis5(functionCollection3);
    Indices indices({0, 2, 4});
    OrthogonalFunctionFactory productBasis6(productBasis5.getMarginal(indices));
    fullprint << productBasis6.__str__() << std::endl;
    // Test the build() method on a collection of functions
    const Point center2({0.5, 0.5, 0.5});
    for (UnsignedInteger i = 0; i < 10; ++ i)
    {
      // Test build from index
      const Function function(productBasis6.build(i));
      assert_almost_equal(function(center2), computeFunctionValue(i, center2));
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
