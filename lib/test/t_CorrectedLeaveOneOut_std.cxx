//                                               -*- C++ -*-
/**
 *  @brief The test file of class CorrectedLeaveOneOut for standard methods
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  UnsignedInteger size = 100;
  Uniform xuniform(0.9, 1.1);
  NumericalSample x( xuniform.getSample(size) );
  Uniform yuniform(1.9, 2.1);
  NumericalSample y( yuniform.getSample(size) );
  AnalyticalFunction f("x", "2.0*x");
  Basis basis;
  basis.add(f);
  Indices indices(basis.getSize());
  indices.fill();
  FittingAlgorithm fittingAlgo = CorrectedLeaveOneOut();

  NumericalScalar result = fittingAlgo.run(x, y, basis, indices);

  fullprint << "result = " << result << std::endl;


  try
  {
    size = 2;
    x = xuniform.getSample(size);
    y = yuniform.getSample(size);
    AnalyticalFunction f2("x", "x^2");
    basis.add(f2);
    AnalyticalFunction f3("x", "x^3");
    basis.add(f3);
    indices = Indices(basis.getSize());
    indices.fill();
    result = fittingAlgo.run( x, y, basis, indices );
    fullprint << "result=" << result << std::endl;
  }
  catch (InvalidArgumentException & ex)
  {
    fullprint << "caught exception: " << ex.what() << std::endl;
  }

  return ExitCode::Success;
}
