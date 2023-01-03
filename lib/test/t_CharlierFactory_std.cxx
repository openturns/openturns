//                                               -*- C++ -*-
/**
 *  @brief The test file of class CharlierFactory for standard methods
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
    CharlierFactory charlier(2.5);
    fullprint << "charlier=" << charlier << std::endl;
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      fullprint << "charlier(" << i << ")=" << charlier.build(i).__str__() << std::endl;
    }
    Point roots(charlier.getRoots(10));
    fullprint << "charlier(10) roots=" << roots << std::endl;
    Point weights;
    roots = charlier.getNodesAndWeights(10, weights);
    fullprint << "charlier(10) roots=" << roots << " and weights=" << weights << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
