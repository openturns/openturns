//                                               -*- C++ -*-
/**
 *  @brief The test file of class TranslationFunction for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  UnsignedInteger inputDimension = 3;
  Point constant(inputDimension);
  constant[0] = 2.0;
  constant[1] = 3.0;
  constant[2] = -4.0;

  TranslationFunction myFunction(constant);
  myFunction.setName("translationFunction");
  Point inPoint(inputDimension);
  inPoint[0] = 7.0;
  inPoint[1] = 8.0;
  inPoint[2] = 9.0;
  fullprint << "myFunction=" << myFunction << std::endl;
  fullprint << myFunction.getName() << "( " << inPoint << " ) = " <<  myFunction(inPoint) << std::endl;
  fullprint << myFunction.getName() << ".gradient( " << inPoint << " ) = " << myFunction.gradient(inPoint) << std::endl;
  fullprint << myFunction.getName() << ".hessian( " << inPoint << " ) = " << myFunction.hessian(inPoint) << std::endl;
  return ExitCode::Success;
}
