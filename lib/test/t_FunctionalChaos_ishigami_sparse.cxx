//                                               -*- C++ -*-
/**
 *  @brief The test file of class FunctionalChaosAlgorithm in interaction
 *         with the FittingAlgorithm class.
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IshigamiUseCase.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  // Problem parameters
  IshigamiUseCase ishigami;
  UnsignedInteger dimension = 3;
  Function model(ishigami.getModel());  // Create the Ishigami function
  JointDistribution distribution(ishigami.getInputDistribution());  // Create the input distribution

  // Create the orthogonal basis
  Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
  for ( UnsignedInteger i = 0; i < dimension; ++ i )
  {
    polynomialCollection[i] = LegendreFactory();
  }
  LinearEnumerateFunction enumerateFunction(dimension);
  OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

  // design experiment
  UnsignedInteger samplingSize = 75;

  // build basis
  UnsignedInteger degree = 10;
  UnsignedInteger basisSize = enumerateFunction.getStrataCumulatedCardinal( degree );


  Collection<FittingAlgorithm> listFittingAlgorithm;
  listFittingAlgorithm.add( KFold() );
  listFittingAlgorithm.add( CorrectedLeaveOneOut() );

  for (UnsignedInteger fittingAlgorithmIndex = 0; fittingAlgorithmIndex < listFittingAlgorithm.getSize(); ++ fittingAlgorithmIndex)
  {
    FixedStrategy adaptiveStrategy( productBasis, basisSize );
    FittingAlgorithm fittingAlgorithm( listFittingAlgorithm[fittingAlgorithmIndex] );
    const LeastSquaresStrategy projectionStrategy(LeastSquaresMetaModelSelectionFactory(LARS(), fittingAlgorithm));
    const LowDiscrepancyExperiment experiment(SobolSequence(), distribution, samplingSize);
    RandomGenerator::SetSeed(0);
    const Sample X(experiment.generate());
    const Sample Y(model(X));
    FunctionalChaosAlgorithm algo(X, Y, distribution, adaptiveStrategy, projectionStrategy);
    algo.run();
    FunctionalChaosResult result(algo.getResult());
    fullprint << "coeffs = " << result.getCoefficients() << std::endl;
    fullprint << "residuals = " << result.getResiduals() << std::endl;
    fullprint << "relative errors = " << result.getRelativeErrors() << std::endl;
  }
  return ExitCode::Success;
}
