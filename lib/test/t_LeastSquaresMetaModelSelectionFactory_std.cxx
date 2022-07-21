//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresMetaModelSelectionFactory for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

  Sample x(1, 1);
  Sample y(1, 1);
  Point weight(1, 1.0);
  Collection<Function> coll;
  coll.add(SymbolicFunction("x", "x"));
  Indices indices(coll.getSize());
  indices.fill();

  LeastSquaresMetaModelSelectionFactory factory = LeastSquaresMetaModelSelectionFactory(LARS(), CorrectedLeaveOneOut());
  ApproximationAlgorithm algo(factory.build(x, y, weight, coll, indices));
  fullprint << "algo = " << algo << std::endl;

  return ExitCode::Success;
}
