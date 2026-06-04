//                                               -*- C++ -*-
/**
 *  @brief The test file of class Indices for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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


    // braced-init-list
    Indices indices2 = {1, 2, 3};
    fullprint << "indices2=" << indices2 << std::endl;
    // copy-list-initialization
    indices2 = {4, 5};
    fullprint << "indices2=" << indices2 << std::endl;

    // Test contains()
    Indices indices3 = {};
    if(indices3.contains(0)) throw InvalidArgumentException(HERE) << "contains(0)";
    if(indices3.contains(1)) throw InvalidArgumentException(HERE) << "contains(1)";
    Indices indices4 = {1, 2, 3};
    if(!indices4.contains(1)) throw InvalidArgumentException(HERE) << "contains(1)";
    if(indices4.contains(4)) throw InvalidArgumentException(HERE) << "contains(4)";

    // Test normInf() and norm1()
    Indices indices6 = {};
    if(indices6.normInf() != 0.0) throw InvalidArgumentException(HERE) << "norm";
    if(indices6.norm1() != 0.0) throw InvalidArgumentException(HERE) << "norm";
    Indices indices7 = {1, 2, 3};
    if(indices7.normInf() != 3.0) throw InvalidArgumentException(HERE) << "norm";
    if(indices7.norm1() != 6.0) throw InvalidArgumentException(HERE) << "norm";

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
