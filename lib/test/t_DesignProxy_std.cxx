//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresMethod for the standard methods
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

  try
  {
    UnsignedInteger basisSize = 3;
    UnsignedInteger sampleSize = 3;

    Sample X(sampleSize, 1);
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
      X(i, 0) = i + 1.0;

    Sample Y(sampleSize, 1);

    Collection<Function> phis;
    for (UnsignedInteger j = 0; j < basisSize; ++j)
      phis.add(SymbolicFunction("x", String(OSS() << "x^" << j + 1)));
    Basis basis(phis);
    for (UnsignedInteger i = 0; i < basisSize; ++i)
      fullprint << Collection<Function>(basis)[i](X) << std::endl;

    DesignProxy proxy(X, basis);
    Indices full(basisSize);
    full.fill();

    Matrix design = proxy.computeDesign(full);
    fullprint << design << std::endl;

    Indices partial = {0, 2};
    fullprint << proxy.getBasis(partial) << std::endl;

    proxy = DesignProxy(design);
    design = proxy.computeDesign(full);
    fullprint << design << std::endl;

  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
