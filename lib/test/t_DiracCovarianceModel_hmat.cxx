//                                               -*- C++ -*-
/**
 *  @brief The test file of class DiracCovarianceModel
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
#include <iostream>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

static std::string
hmatrix__str__(const HMatrix & hmat)
{
  if (hmat.getNbRows() == 0 || hmat.getNbColumns() == 0) return "[]";

  Sample res(hmat.getNbRows(), hmat.getNbColumns());
  for ( UnsignedInteger i = 0; i < hmat.getNbRows(); ++i)
  {
    Point x(hmat.getNbColumns());
    x[i] = 1.0;
    Point y(hmat.getNbRows());
    hmat.gemv('N', 1.0, x, 0.0, y);
    res[i] = y;
  }
  return res.__str__();
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    PlatformInfo::SetNumericalPrecision(3);

    ResourceMap::SetAsUnsignedInteger("HMatrix-MaxLeafSize", 6);

    // Spatial dimension
    const UnsignedInteger inputDimension = 2;
    // Dimension
    const UnsignedInteger dimension = 2;

    DiracCovarianceModel myDefaultModel;
    fullprint << "myDefaultModel = " << myDefaultModel << std::endl;
    // Amplitude of dimension 2
    Point amplitude(dimension);
    for (UnsignedInteger k = 0; k < dimension; ++k) amplitude[k] = 1.5 + 2.0 * k;
    // Model of dimension 2
    DiracCovarianceModel myModel1(inputDimension, amplitude);
    fullprint << "myModel1 = " << myModel1 << std::endl;
    fullprint << "Pretty print of myModel1 = " << myModel1.__str__() << std::endl;

    // Definition of spatial correlation
    CorrelationMatrix spatialCorrelation(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      for (UnsignedInteger i = j + 1; i < dimension; ++i)
        spatialCorrelation(i, j) = (i + 1.0)  / dimension - (j + 1.0) / dimension;
    DiracCovarianceModel myModel2(inputDimension, amplitude, spatialCorrelation);
    fullprint << "myModel2 = " << myModel2 << std::endl;
    fullprint << "Pretty print of myModel2 = " << myModel2.__str__() << std::endl;

    // Evaluation on some points
    Indices levels(inputDimension, 1);
    Box box(levels);
    const Sample vertices = box.generate();
    const Scalar nuggetFactor = ResourceMap::GetAsScalar("CovarianceModel-DefaultNuggetFactor");
    myModel1.setNuggetFactor(nuggetFactor);
    myModel2.setNuggetFactor(nuggetFactor);
    const HMatrixParameters parameters;

    fullprint << "Discretization on a grid of vertices" << std::endl;
    fullprint << "Discretization of myModel1 = " << hmatrix__str__(myModel1.discretizeHMatrix(vertices, parameters)) << std::endl;
    fullprint << "Discretization of myModel2 = " << hmatrix__str__(myModel2.discretizeHMatrix(vertices, parameters)) << std::endl;

    // Evaluation of the Cholesky factor
    fullprint << "Discretization & factorization on a grid of vertices" << std::endl;
    fullprint << "Discretization & factorization of myModel1 = " <<  hmatrix__str__(myModel1.discretizeAndFactorizeHMatrix(vertices, parameters)) << std::endl;
    fullprint << "Discretization  & factorizationof myModel2 = " <<  hmatrix__str__(myModel2.discretizeAndFactorizeHMatrix(vertices, parameters)) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
