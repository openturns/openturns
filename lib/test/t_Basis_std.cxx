//                                               -*- C++ -*-
/**
 *  @brief The test file of class Basis for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  const UnsignedInteger dimension = 3;
  Description input;
  Description output[dimension];
  for ( UnsignedInteger j = 0; j < dimension; ++ j )
  {
    OSS oss;
    oss << "x" << j;
    String in(oss);
    input.add(in);
    output[j].add(in);
  }
  Collection<Function> coll;
  for ( UnsignedInteger j = 0; j < dimension; ++ j )
  {
    SymbolicFunction ei(input, output[j]);
    coll.add(ei);
  }


  Basis myBasis(coll);

  fullprint << "basis = " << myBasis << std::endl;

  fullprint << "dimension = " << myBasis.getDimension() << std::endl;

  return ExitCode::Success;
}
