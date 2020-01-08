//                                               -*- C++ -*-
/**
 *  @brief The test file of class ConditionedGaussianProcess for standard methods
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

  // Set Numerical precision to 3
  PlatformInfo::SetNumericalPrecision(3);

  // Process with empty constructor
  ConditionedGaussianProcess myProcess;
  fullprint << "process = " << myProcess << std::endl;

  // Learning data
  Point levels(2);
  levels[0] = 8;
  levels[1] = 5;
  // Define the Box
  Box box(levels);
  // Get the input sample
  Sample inputSample( box.generate() );
  // Scale each direction
  inputSample *= 10;

  // Define model
  Description inputDescription(2);
  inputDescription[0] = "x";
  inputDescription[1] = "y";

  Description formula(1);
  formula[0] = "cos(0.5*x) + sin(y)" ;
  const SymbolicFunction model(inputDescription, formula);

  // Build the output sample
  const Sample  outputSample( model(inputSample) );

  // 2) Definition of exponential model
  Point scale(2);
  scale[0] = 1.988;
  scale[1] = 0.924;
  Point amplitude(1, 3.153);
  SquaredExponential covarianceModel(scale, amplitude);

  // 3) Basis definition
  Basis basis(ConstantBasisFactory(2).build());

  // Kriging algorithm
  KrigingAlgorithm algo(inputSample, outputSample, covarianceModel, basis, true);
  algo.run();
  // Get result
  KrigingResult result(algo.getResult());
  // Build a mesh
  // Start with vertices
  Sample vertices(0, 2);
  Point p(2);

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


  IndicesCollection simplices(4, 3);
  simplices(0, 0) = 0;
  simplices(0, 1) = 1;
  simplices(0, 2) = 4;

  simplices(1, 0) = 1;
  simplices(1, 1) = 2;
  simplices(1, 2) = 4;

  simplices(2, 0) = 2;
  simplices(2, 1) = 3;
  simplices(2, 2) = 4;

  simplices(3, 0) = 3;
  simplices(3, 1) = 0;
  simplices(3, 2) = 4;

  Mesh mesh2D(vertices, simplices);


  // Conditioned process
  ConditionedGaussianProcess process(result, mesh2D);
  // Get a realization of the process
  Field realization( process.getRealization() );
  fullprint << "realization = " << realization << std::endl;

  //  Get a sample & compare it to expectation
  ProcessSample sample(process.getSample(5000));
  fullprint << "Mean over 5000 realizations = " << sample.computeMean() << std::endl;

  return ExitCode::Success;
}
