//                                               -*- C++ -*-
/**
 *  @brief The test file of class BasisSequence for standard methods
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
  Indices cumulBasis;
  Indices partialBasis[dimension];
  for ( UnsignedInteger j = 0; j < dimension; ++ j )
  {
    SymbolicFunction ei(input, output[j]);
    coll.add(ei);
    cumulBasis.add(j);
    partialBasis[j] = cumulBasis;
  }

  Basis masterBasis(coll);
  BasisSequence seq(masterBasis);
  for ( UnsignedInteger j = 0; j < dimension; ++ j )
  {
    seq.add(partialBasis[j]);
  }
  fullprint << "sequence = " << seq << std::endl;
  for ( UnsignedInteger j = 0; j < dimension; ++ j )
  {
    fullprint << "indices of subBasis[" << j << "] = " << seq.getIndices(j) << std::endl;
  }

  return ExitCode::Success;
}
