//                                               -*- C++ -*-
/**
 *  @brief The test file of class TensorApproximationAlgorithm for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
  setRandomGenerator();
  Log::Show(Log::ALL);

  try
  {
    UnsignedInteger dim = 3;
    Description input(dim);
    input[0] = "x0";
    input[1] = "x1";
    input[2] = "x2";
    SymbolicFunction model(input, Description(1, "sin(x0)*cos(x1)*exp(x2)"));
    ComposedDistribution distribution(ComposedDistribution::DistributionCollection(dim, Uniform()));

    Collection<OrthogonalUniVariateFunctionFamily> factoryCollection(dim, OrthogonalUniVariatePolynomialFunctionFactory(LegendreFactory()));
    OrthogonalProductFunctionFactory functionFactory(factoryCollection);

    UnsignedInteger size = 1000;
    Sample X = distribution.getSample(size);
    Sample Y = model(X);
    Indices nk(dim, 10);
    UnsignedInteger maxRank = 5;
    TensorApproximationAlgorithm algo(X, Y, distribution, functionFactory, nk, maxRank);
    algo.run();
    TensorApproximationResult result(algo.getResult());
    fullprint << "residuals=" << result.getResiduals() << std::endl;
    Function metamodel = result.getMetaModel();
    Point x(dim, 0.4);
    fullprint << "x=" << x << " f(x)=" << model(x).__str__() << " f^(x)=" << metamodel(x).__str__() << std::endl;
    fullprint << "rank=" << result.getTensor(0).getRank() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  return ExitCode::Success;
}
