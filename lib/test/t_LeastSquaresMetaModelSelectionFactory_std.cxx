//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresMetaModelSelectionFactory for standard methods
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

  NumericalSample x(1, 1);
  NumericalSample y(1, 1);
  NumericalPoint weight(1, 1.0);
  Collection<NumericalMathFunction> coll;
  coll.add(NumericalMathFunction("x", "x"));
  Basis psi(coll);
  Indices indices(coll.getSize());
  indices.fill();

  LeastSquaresMetaModelSelectionFactory factory = LeastSquaresMetaModelSelectionFactory( LAR(), CorrectedLeaveOneOut() );
  ApproximationAlgorithm algo(factory.build(x, y, weight, psi, indices));
  fullprint << "algo = " << algo << std::endl;

  return ExitCode::Success;
}
