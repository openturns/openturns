//                                               -*- C++ -*-
/**
 *  @brief The test file of class MeixnerFactory for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    MeixnerFactory meixner(10, 0.3);
    fullprint << "meixner=" << meixner << std::endl;
    for (UnsignedInteger i = 0; i < 10; ++i)
    {
      fullprint << "meixner(" << i << ")=" << meixner.build(i).__str__() << std::endl;
    }
    Point roots(meixner.getRoots(10));
    fullprint << "meixner(10) roots=" << roots << std::endl;
    Point weights;
    roots = meixner.getNodesAndWeights(10, weights);
    fullprint << "meixner(10) roots=" << roots << " and weights=" << weights << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
