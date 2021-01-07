//                                               -*- C++ -*-
/**
 *  @brief The test file of BasisFactory class
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

String printPoint(const Point & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  Scalar eps = pow(0.1, 1.0 * digits);
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((std::abs(point[i]) < eps) ? std::abs(point[i]) : point[i]);
  }
  oss << "]";
  return oss;
}

int main(int, char *[])
{
  try
  {
    UnsignedInteger inputDimension = 2;

    Point x(inputDimension);
    for (UnsignedInteger i = 0; i < inputDimension; ++ i)
    {
      x[i] = 2. + i;
    }
    std::cout << "x=" << x << std::endl;
    {
      ConstantBasisFactory factory(inputDimension);
      std::cout << "factory=" << factory << std::endl;
      Basis basis(factory.build());
      std::cout << "basis=" << basis << std::endl;

      Indices indices(basis.getSize());
      indices.fill();
      AggregatedFunction f(basis.getSubBasis(indices));
      Point y(f(x));
      std::cout << "y=" << y << std::endl;
    }
    {
      LinearBasisFactory factory(inputDimension);
      std::cout << "factory=" << factory << std::endl;
      Basis basis(factory.build());
      std::cout << "basis=" << basis << std::endl;

      Indices indices(basis.getSize());
      indices.fill();
      AggregatedFunction f(basis.getSubBasis(indices));
      Point y(f(x));
      std::cout << "y=" << y << std::endl;
    }
    {
      QuadraticBasisFactory factory(inputDimension);
      std::cout << "factory=" << factory << std::endl;
      Basis basis(factory.build());
      std::cout << "basis=" << basis << std::endl;

      Indices indices(basis.getSize());
      indices.fill();
      AggregatedFunction f(basis.getSubBasis(indices));
      Point y(f(x));
      std::cout << "y=" << y << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
