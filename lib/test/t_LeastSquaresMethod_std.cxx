//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresMethod for the standard methods
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

    const UnsignedInteger dimension = 10;
    const UnsignedInteger size = 20;

    Sample x(Normal(dimension).getSample(size));
    Collection<Function> coll;
    const Description description(Description::BuildDefault(dimension, "x"));
    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      coll.add(SymbolicFunction(description, Description(1, OSS() << "x" << i)));
    }
    Indices indices(dimension);
    indices.fill();
    DesignProxy proxy(x, coll);

    Description methods(3);
    methods[0] = "QR";
    methods[1] = "SVD";
    methods[2] = "Cholesky";

    Point hFromH(size);

    for (UnsignedInteger k = 0; k < 3; ++k)
    {
      LeastSquaresMethod algo(LeastSquaresMethod::Build(methods[k], proxy, indices));
      algo.update(Indices(0), indices, Indices(0));

      fullprint << methods[k] << std::endl;
      fullprint << "Solve=" << algo.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << algo.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << algo.getGramInverse() << std::endl;
      fullprint << "HDiag=" << algo.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << algo.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << algo.getGramInverseDiag() << std::endl;

      // Validation of H
      SymmetricMatrix H(algo.getH());

      // Get the Diagonal of H matrix and compare it to the getHDiag
      // Second method is already validated
      // Note also that H^n = H so we could add this test
      for (UnsignedInteger k2 = 0; k2 < size; ++ k2) hFromH[k2] = H(k2, k2);
      assert_almost_equal(hFromH,  algo.getHDiag(), 1e-15, 1e-15);
      SquareMatrix H2(H * H);
      assert_almost_equal(H2, H, 1e-15, 1e-15);

    }
    Matrix design(proxy.computeDesign(indices));
    for (UnsignedInteger k = 0; k < 3; ++k)
    {
      LeastSquaresMethod algo(LeastSquaresMethod::Build(methods[k], design));
      algo.update(Indices(0), indices, Indices(0));

      fullprint << methods[k] << std::endl;
      fullprint << "Solve=" << algo.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << algo.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << algo.getGramInverse() << std::endl;
      fullprint << "HDiag=" << algo.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << algo.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << algo.getGramInverseDiag() << std::endl;

      // Validation of H
      SymmetricMatrix H(algo.getH());

      // Get the Diagonal of H matrix and compare it to the getHDiag
      // Second method is already validated
      // Note also that H^n = H so we could add this test
      for (UnsignedInteger k2 = 0; k2 < size; ++ k2) hFromH[k2] = H(k2, k2);
      assert_almost_equal(hFromH,  algo.getHDiag(), 1e-15, 1e-15);
      SquareMatrix H2(H * H);
      assert_almost_equal(H2,  H, 1e-15, 1e-15);

    }

  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
