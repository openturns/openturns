//                                               -*- C++ -*-
/**
 *  @brief The test file of class LAR for standard methods
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
  //   Log::Show( Log::Flags() | Log::INFO );

  // Problem parameters
  UnsignedInteger dimension = 3;
  Scalar a = 7.0;
  Scalar b = 0.1;
  // Reference analytical values
  Scalar covTh = (pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0;
  Point sob_1(3);
  sob_1[0] = (b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0 + 1.0 / 2.0) / covTh;
  sob_1[1] = (pow(a, 2.0) / 8.0) / covTh;
  sob_1[2] = 0.0;
  Point sob_2(3);
  sob_2[0] = 0.0;
  sob_2[1] = (pow(b, 2.0) * pow(M_PI, 8.0) / 18.0 - pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / covTh;
  sob_2[2] = 0.0;
  Point sob_3(1, 0.0);
  Point sob_T1(3);
  sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0];
  sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0];
  sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0];
  Point sob_T2(3);
  sob_T2[0] = sob_2[0] + sob_2[1] + sob_3[0];
  sob_T2[1] = sob_2[0] + sob_2[2] + sob_3[0];
  sob_T2[2] = sob_2[1] + sob_2[2] + sob_3[0];
  Point sob_T3(sob_3);
  // Create the Ishigami function
  Description inputVariables(dimension);
  inputVariables[0] = "xi1";
  inputVariables[1] = "xi2";
  inputVariables[2] = "xi3";
  Description formula(1);
  formula[0] = (OSS() << "sin(xi1) + (" << a << ") * (sin(xi2)) ^ 2 + (" << b << ") * xi3^4 * sin(xi1)");
  SymbolicFunction model(inputVariables, formula);

  // Create the input distribution
  Collection<Distribution> marginalX(dimension);
  marginalX[0] = Uniform(-M_PI, M_PI);
  marginalX[1] = Uniform(-M_PI, M_PI);
  marginalX[2] = Uniform(-M_PI, M_PI);
  ComposedDistribution distribution(marginalX);

  // Create the orthogonal basis
  Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
  polynomialCollection[0] = LegendreFactory();
  polynomialCollection[1] = LegendreFactory();
  polynomialCollection[2] = LegendreFactory();
  LinearEnumerateFunction enumerateFunction(dimension);
  OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

  // design experiment
  UnsignedInteger samplingSize = 75;
  Experiment experiment(LowDiscrepancyExperiment(SobolSequence(dimension), distribution, samplingSize));

  // build the isoprobabilistic transformation
  Collection<Distribution> marginalZ;
  for (UnsignedInteger i = 0; i < dimension; ++ i )
  {
    marginalZ.add(productBasis.getMeasure().getMarginal(i));
  }
  MarginalTransformationEvaluation evaluationT(MarginalTransformationEvaluation(marginalX, marginalZ));
  MarginalTransformationGradient gradientT(evaluationT);
  MarginalTransformationHessian hessianT(evaluationT);
  Function xToU(evaluationT.clone(), gradientT.clone(), hessianT.clone());

  // generate samples
  Sample x( experiment.generate() );
  Sample u( xToU(x) );
  Sample y( model(x) );

  // build basis
  UnsignedInteger degree = 10;
  UnsignedInteger basisSize = enumerateFunction.getStrataCumulatedCardinal(degree);
  Collection<Function> coll;
  for ( UnsignedInteger i = 0; i < basisSize; ++ i )
  {
    coll.add(productBasis.build(i));
  }
  Indices indices(basisSize);
  indices.fill();

  // run algorithm
  LARS factory;
  factory.setVerbose(true);
  fullprint << "factory = " << factory << std::endl;

  BasisSequence seq = factory.build( u, y, coll, indices);

  UnsignedInteger first = 20;
  if ( seq.getSize() >= first )
    fullprint << "first " << first << " indices = " << seq.getIndices(first - 1) << std::endl;
  else
    fullprint << " size=" << seq.getSize() << std::endl;
  return ExitCode::Success;
}
