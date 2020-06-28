//                                               -*- C++ -*-
/**
 *  @brief The test file of class NLopt for standard methods
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

  try
  {
    PlatformInfo::SetNumericalPrecision(3);
    UnsignedInteger m = 10;
    Sample x(m, 1);
    for (UnsignedInteger i = 0; i < m; ++i) x(i, 0) = 0.5 + i;

    Description inVars(0);
    inVars.add("a");
    inVars.add("b");
    inVars.add("c");
    inVars.add("x");
    Description formulas(1, "a + b * exp(c * x)");
    formulas.add("(a * x^2 + b) / (c + x^2)");
    SymbolicFunction model(inVars, formulas);
    Point p_ref(0);
    p_ref.add(2.8);
    p_ref.add(1.2);
    p_ref.add(0.5);
    Indices params(3);
    params.fill();
    ParametricFunction modelX(model, params, p_ref);
    Sample y = modelX(x);
    y += Normal(Point(2), Point(2, 0.05), IdentityMatrix(2)).getSample(y.getSize());
    Point candidate(3, 1.0);
    Indices bootstrapSizes(0);
    bootstrapSizes.add(0);
    bootstrapSizes.add(100);
    for (UnsignedInteger n = 0; n < bootstrapSizes.getSize(); ++n)
    {
      NonLinearLeastSquaresCalibration algo(modelX, x, y, candidate);
      algo.setBootstrapSize(bootstrapSizes[n]);
      algo.run();
      // To avoid discrepance between the plaforms with or without CMinpack
      fullprint << "result (Auto)=" << algo.getResult().getParameterMAP() << std::endl;
      algo.setOptimizationAlgorithm(MultiStart(TNC(), LowDiscrepancyExperiment(SobolSequence(), Normal(candidate, CovarianceMatrix(candidate.getDimension())), ResourceMap::GetAsUnsignedInteger("NonLinearLeastSquaresCalibration-MultiStartSize")).generate()));
      algo.run();
      fullprint << "result  (TNC)=" << algo.getResult().getParameterMAP() << std::endl;
      fullprint << "error=" << algo.getResult().getObservationsError() << std::endl;
    } // n
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
