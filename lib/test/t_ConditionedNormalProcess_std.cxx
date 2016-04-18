//                                               -*- C++ -*-
/**
 *  @brief The test file of class ConditionedNormalProcess for standard methods
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  // Set Numerical precision to 3
  UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
  PlatformInfo::SetNumericalPrecision(3);

  // Process with empty constructor
  ConditionedNormalProcess myProcess;
  fullprint << "process = " << myProcess << std::endl;

  // Learning data
  NumericalPoint levels(2);
  levels[0] = 8;
  levels[1] = 5;
  // Define the Box
  Box box(levels);
  // Get the input sample
  NumericalSample inputSample( box.generate() );
  // Scale each direction
  inputSample *= 10;

  // Define model
  Description inputDescription(2);
  inputDescription[0] = "x";
  inputDescription[1] = "y";

  Description formula(1);
  formula[0] = "cos(0.5*x) + sin(y)" ;
  const NumericalMathFunction model(inputDescription, formula);

  // Build the output sample
  const NumericalSample  outputSample( model(inputSample) );

  // 2) Definition of exponential model
  NumericalPoint scale(2);
  scale[0] = 1.98824;
  scale[1] = 0.924731;
  NumericalPoint amplitude(1, 3.15352);
  SquaredExponential covarianceModel(scale, amplitude);

  // 3) Basis definition
  Basis basis(ConstantBasisFactory(2).build());

  // Kriring algorithm
  KrigingAlgorithm algo(inputSample, outputSample, basis, covarianceModel);
  algo.run();

  // Get result
  KrigingResult result(algo.getResult());

  // Build a mesh
  // Start with vertices
  NumericalSample vertices(0, 2);
  NumericalPoint p(2);

  p[0] = 1.0;
  p[1] = 0.0;
  vertices.add(p);

  p[0] = 2.0;
  p[1] = 0.0;
  vertices.add(p);

  p[0] = 2.0;
  p[1] = 1.0;
  vertices.add(p);

  p[0] = 1.0;
  p[1] = 1.0;
  vertices.add(p);

  p[0] = 1.5;
  p[1] = 0.5;
  vertices.add(p);


  Mesh::IndicesCollection simplicies(4, Indices(3));
  simplicies[0][0] = 0;
  simplicies[0][1] = 1;
  simplicies[0][2] = 4;

  simplicies[1][0] = 1;
  simplicies[1][1] = 2;
  simplicies[1][2] = 4;

  simplicies[2][0] = 2;
  simplicies[2][1] = 3;
  simplicies[2][2] = 4;

  simplicies[3][0] = 3;
  simplicies[3][1] = 0;
  simplicies[3][2] = 4;

  Mesh mesh2D(vertices, simplicies);


  // Conditioned process
  ConditionedNormalProcess process(result, mesh2D);

  // Get a realization of the process
  Field realization( process.getRealization() );
  fullprint << "realization = " << realization << std::endl;

  //  Get a sample & compare it to expectation
  ProcessSample sample(process.getSample(5000));
  fullprint << "Mean over 5000 realizations = " << sample.computeMean() << std::endl;

  return ExitCode::Success;
}
