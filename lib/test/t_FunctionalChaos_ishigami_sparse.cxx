//                                               -*- C++ -*-
/**
 *  @brief The test file of class FunctionalChaosAlgorithm in interaction
 *         with the FittingAlgorithm class.
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
  //   Log::Show( Log::Flags() | Log::INFO );

  // Problem parameters
  UnsignedInteger dimension(3);
  NumericalScalar a(7.0);
  NumericalScalar b(0.1);
  // Reference analytical values
  NumericalScalar covTh((pow(b, 2.0) * pow(M_PI, 8.0)) / 18.0 + (b * pow(M_PI, 4.0)) / 5.0 + (pow(a, 2.0)) / 8.0 + 1.0 / 2.0);
  NumericalPoint sob_1(3);
  sob_1[0] = (b * pow(M_PI, 4.0) / 5.0 + pow(b, 2.0) * pow(M_PI, 8.0) / 50.0 + 1.0 / 2.0) / covTh;
  sob_1[1] = (pow(a, 2.0) / 8.0) / covTh;
  sob_1[2] = 0.0;
  NumericalPoint sob_2(3);
  sob_2[0] = 0.0;
  sob_2[1] = (pow(b, 2.0) * pow(M_PI, 8.0) / 18.0 - pow(b, 2.0) * pow(M_PI, 8.0) / 50.0) / covTh;
  sob_2[2] = 0.0;
  NumericalPoint sob_3(1, 0.0);
  NumericalPoint sob_T1(3);
  sob_T1[0] = sob_1[0] + sob_2[0] + sob_2[1] + sob_3[0];
  sob_T1[1] = sob_1[1] + sob_2[0] + sob_2[2] + sob_3[0];
  sob_T1[2] = sob_1[2] + sob_2[1] + sob_2[2] + sob_3[0];
  NumericalPoint sob_T2(3);
  sob_T2[0] = sob_2[0] + sob_2[1] + sob_3[0];
  sob_T2[1] = sob_2[0] + sob_2[2] + sob_3[0];
  sob_T2[2] = sob_2[1] + sob_2[2] + sob_3[0];
  NumericalPoint sob_T3(sob_3);
  // Create the Ishigami function
  Description inputVariables(dimension);
  inputVariables[0] = "xi1";
  inputVariables[1] = "xi2";
  inputVariables[2] = "xi3";
  Description outputVariables(1);
  outputVariables[0] = "y";
  Description formula(1);
  formula[0] = (OSS() << "sin(xi1) + (" << a << ") * (sin(xi2)) ^ 2 + (" << b << ") * xi3^4 * sin(xi1)");
  NumericalMathFunction model(inputVariables, outputVariables, formula);

  // Create the input distribution
  Collection<Distribution> marginalX(dimension);
  for ( UnsignedInteger i = 0; i < dimension; ++ i )
  {
    marginalX[i] = Uniform(-M_PI, M_PI);
  }
  ComposedDistribution distribution(marginalX);

  // Create the orthogonal basis
  Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
  for ( UnsignedInteger i = 0; i < dimension; ++ i )
  {
    polynomialCollection[i] = LegendreFactory();
  }
  LinearEnumerateFunction enumerateFunction(dimension);
  OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

  // design experiment
  UnsignedInteger samplingSize( 75 );

  // build basis
  UnsignedInteger degree( 10 );
  UnsignedInteger basisSize( enumerateFunction.getStrataCumulatedCardinal( degree ) );


  Collection<FittingAlgorithm> listFittingAlgorithm;
  listFittingAlgorithm.add( KFold() );
  listFittingAlgorithm.add( CorrectedLeaveOneOut() );

  for ( UnsignedInteger fittingAlgorithmIndex = 0; fittingAlgorithmIndex < listFittingAlgorithm.getSize(); ++ fittingAlgorithmIndex )
  {
    FixedStrategy adaptiveStrategy( productBasis, basisSize );
    FittingAlgorithm fittingAlgorithm( listFittingAlgorithm[fittingAlgorithmIndex] );
    LeastSquaresStrategy projectionStrategy( LowDiscrepancyExperiment(SobolSequence(dimension), samplingSize), LeastSquaresMetaModelSelectionFactory ( LAR(), fittingAlgorithm) );
    FunctionalChaosAlgorithm algo(model, distribution, adaptiveStrategy, projectionStrategy);
    RandomGenerator::SetSeed(0);
    algo.run();
    FunctionalChaosResult result(algo.getResult());
    fullprint << "coeffs = " << result.getCoefficients() << std::endl;
    fullprint << "residuals = " << result.getResiduals() << std::endl;
    fullprint << "relative errors = " << result.getRelativeErrors() << std::endl;
  }
  return ExitCode::Success;
}
