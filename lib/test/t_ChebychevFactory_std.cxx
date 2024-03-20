//                                               -*- C++ -*-
/**
 *  @brief The test file of class ChebychevFactory for standard methods
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

  try
  {
    ChebychevFactory chebychev;
    fullprint << "chebychev=" << chebychev << std::endl;
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      fullprint << "chebychev(" << i << ")=" << chebychev.build(i).__str__() << std::endl;
    }
    Point roots(chebychev.getRoots(10));
    fullprint << "chebychev(10) roots=" << roots << std::endl;
    Point weights;
    Point nodes(chebychev.getNodesAndWeights(10, weights));
    fullprint << "chebychev(10) nodes=" << nodes << " and weights=" << weights <<  std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
