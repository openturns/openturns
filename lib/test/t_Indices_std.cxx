//                                               -*- C++ -*-
/**
 *  @brief The test file of class Indices for standard methods
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
    UnsignedInteger size = 4;
    Indices indices(size, 0);
    indices.fill();
    fullprint << "indices=" << indices << std::endl;
    fullprint << "are indices valid with bound=" << size << "? " << (indices.check(size) ? "true" : "false") << std::endl;
    fullprint << "are indices valid with bound=" << size / 2 << "? " << (indices.check(size / 2) ? "true" : "false") << std::endl;
    indices[0] = indices[size - 1];
    fullprint << "indices after transformation=" << indices << std::endl;
    fullprint << "are indices valid with bound=" << size << "? " << (indices.check(size) ? "true" : "false") << std::endl;
    fullprint << "complement wrt 7=" << indices.complement(7) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
