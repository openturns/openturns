//                                               -*- C++ -*-
/**
 *  @brief The test file of class HermiteFactory for standard methods
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    HermiteFactory hermite;
    fullprint << "hermite=" << hermite << std::endl;
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      fullprint << "hermite(" << i << ")=" << hermite.build(i).__str__() << std::endl;
      //        fullprint << "hermite(" << i << ")=" << UniVariatePolynomial(clean(hermite.build(i).getCoefficients())).__str__() << std::endl;
    }
    NumericalPoint roots(hermite.getRoots(10));
    fullprint << "hermite(10) roots=" << roots << std::endl;
    NumericalPoint weights;
    NumericalPoint nodes(hermite.getNodesAndWeights(10, weights));
    fullprint << "hermite(10) nodes=" << nodes << " and weights=" << weights <<  std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
