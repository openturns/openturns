//                                               -*- C++ -*-
/**
 *  @brief The test file of class DiracCovarianceModel
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  Id:      $Id$
 */
#include <iostream>
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

static std::string
hmatrix__str__(const HMatrix & hmat)
{
  if (hmat.getNbRows() == 0 || hmat.getNbColumns() == 0) return "[]";

  NumericalSample res(hmat.getNbRows(), hmat.getNbColumns());
  for ( UnsignedInteger i = 0; i < hmat.getNbRows(); ++i)
  {
    NumericalPoint x(hmat.getNbColumns());
    x[i] = 1.0;
    NumericalPoint y(hmat.getNbRows());
    hmat.gemv('N', 1.0, x, 0.0, y);
    res[i] = y;
  }
  return res.__str__();
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    UnsignedInteger precision(PlatformInfo::GetNumericalPrecision());
    PlatformInfo::SetNumericalPrecision(3);

    ResourceMap::SetAsUnsignedInteger("HMatrix-MaxLeafSize", 6);

    // Spatial dimension
    const UnsignedInteger spatialDimension = 2;
    // Dimension
    const UnsignedInteger dimension = 2;

    DiracCovarianceModel myDefautModel;
    fullprint << "myDefautModel = " << myDefautModel << std::endl;
    // Amplitude of dimension 2
    NumericalPoint amplitude(dimension);
    for (UnsignedInteger k = 0; k < dimension; ++k) amplitude[k] = 1.5 + 2.0 * k;
    // Model of dimension 2
    DiracCovarianceModel myModel1(spatialDimension, amplitude);
    fullprint << "myModel1 = " << myModel1 << std::endl;
    fullprint << "Pretty print of myModel1 = " << myModel1.__str__() << std::endl;

    // Definition of spatial correlation
    CorrelationMatrix spatialCorrelation(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      for (UnsignedInteger i = j + 1; i < dimension; ++i)
        spatialCorrelation(i , j) = (i + 1.0)  / dimension - (j + 1.0) / dimension;
    DiracCovarianceModel myModel2(spatialDimension, amplitude, spatialCorrelation);
    fullprint << "myModel2 = " << myModel2 << std::endl;
    fullprint << "Pretty print of myModel2 = " << myModel2.__str__() << std::endl;

     // Evaluation on some points
    Indices levels(spatialDimension, 1);
    Box box(levels);
    const NumericalSample vertices = box.generate();
    const NumericalScalar nuggetFactor(0.0);
    const HMatrixParameters parameters;

    fullprint << "Discretization on a grid of vertices" << std::endl;
    fullprint << "Discretization of myModel1 = " << hmatrix__str__(myModel1.discretizeHMatrix(vertices, nuggetFactor, parameters)) << std::endl;
    fullprint << "Discretization of myModel2 = " << hmatrix__str__(myModel2.discretizeHMatrix(vertices, nuggetFactor, parameters)) << std::endl;

    PlatformInfo::SetNumericalPrecision(precision);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
