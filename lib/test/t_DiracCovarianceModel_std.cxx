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


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    UnsignedInteger precision(PlatformInfo::GetNumericalPrecision());
    PlatformInfo::SetNumericalPrecision(3);

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
    for (UnsignedInteger j = j; j < dimension; ++j)
      for (UnsignedInteger i = j + 1; i < dimension; ++i)
        spatialCorrelation(i , j) = (i + 1.0)  / dimension - (j + 1.0) / dimension;
    DiracCovarianceModel myModel2(spatialDimension, amplitude, spatialCorrelation);
    fullprint << "myModel2 = " << myModel2 << std::endl;
    fullprint << "Pretty print of myModel2 = " << myModel2.__str__() << std::endl;

     // Evaluation on some points
    Indices levels(spatialDimension, 1);
    Box box(levels);
    const NumericalSample vertices = box.generate();
    // Two first points of vertices
    // First is 0, second one different from 0
    const NumericalPoint tau1 = vertices[0];
    const NumericalPoint tau2 = vertices[1];
    fullprint << "Evaluation of the models on the point"  << vertices[0] << std::endl;
    fullprint << "myModel1("  << tau1.__str__() << ") = " << myModel1.operator()(vertices[0]) << std::endl;
    fullprint << "myModel2("  << tau2.__str__() << ") = " << myModel2.operator()(vertices[0]) << std::endl;

    fullprint << "Evaluation of the models on the point"  << vertices[1] << std::endl;
    fullprint << "myModel1("  << tau1.__str__() << ") = " << myModel1.operator()(vertices[1]) << std::endl;
    fullprint << "myModel2("  << tau2.__str__() << ") = " << myModel2.operator()(vertices[1]) << std::endl;

    fullprint << "Discretization on a grid of vertices" << std::endl;
    fullprint << "Discretization of myModel1 = " << myModel1.discretize(vertices).__str__() << std::endl;
    fullprint << "Discretization of myModel2 = " << myModel2.discretize(vertices).__str__() << std::endl;


    PlatformInfo::SetNumericalPrecision(precision);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
