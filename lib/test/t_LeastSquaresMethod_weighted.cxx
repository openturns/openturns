//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresMethod for the standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    Point weights(size, 10.0);
    fullprint << "Uniform weights" << std::endl << std::endl;
    {
      QRMethod qrMethod(proxy, weights, indices);
      qrMethod.update(Indices(0), indices, Indices(0));

      fullprint << "QR" << std::endl;
      fullprint << "Solve=" << qrMethod.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << qrMethod.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << qrMethod.getGramInverse() << std::endl;
      fullprint << "HDiag=" << qrMethod.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << qrMethod.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << qrMethod.getGramInverseDiag() << std::endl;
    }
    {
      SVDMethod svdMethod(proxy, weights, indices);
      svdMethod.update(Indices(0), indices, Indices(0));

      fullprint << "SVD" << std::endl;
      fullprint << "Solve=" << svdMethod.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << svdMethod.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << svdMethod.getGramInverse() << std::endl;
      fullprint << "HDiag=" << svdMethod.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << svdMethod.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << svdMethod.getGramInverseDiag() << std::endl;
    }
    {
      CholeskyMethod choleskyMethod(proxy, weights, indices);
      choleskyMethod.update(Indices(0), indices, Indices(0));

      fullprint << "Cholesky" << std::endl;
      fullprint << "Solve=" << choleskyMethod.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << choleskyMethod.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << choleskyMethod.getGramInverse() << std::endl;
      fullprint << "HDiag=" << choleskyMethod.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << choleskyMethod.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << choleskyMethod.getGramInverseDiag() << std::endl;
    }

    fullprint << std::endl << "Non-uniform weights" << std::endl << std::endl;
    weights[0] += 1.e-10;
    {
      QRMethod qrMethod(proxy, weights, indices);
      qrMethod.update(Indices(0), indices, Indices(0));

      fullprint << "QR" << std::endl;
      fullprint << "Solve=" << qrMethod.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << qrMethod.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << qrMethod.getGramInverse() << std::endl;
      fullprint << "HDiag=" << qrMethod.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << qrMethod.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << qrMethod.getGramInverseDiag() << std::endl;
    }
    {
      SVDMethod svdMethod(proxy, weights, indices);
      svdMethod.update(Indices(0), indices, Indices(0));

      fullprint << "SVD" << std::endl;
      fullprint << "Solve=" << svdMethod.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << svdMethod.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << svdMethod.getGramInverse() << std::endl;
      fullprint << "HDiag=" << svdMethod.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << svdMethod.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << svdMethod.getGramInverseDiag() << std::endl;
    }
    {
      CholeskyMethod choleskyMethod(proxy, weights, indices);
      choleskyMethod.update(Indices(0), indices, Indices(0));

      fullprint << "Cholesky" << std::endl;
      fullprint << "Solve=" << choleskyMethod.solve(Point(size, 1.0)) << std::endl;
      fullprint << "SolveNormal=" << choleskyMethod.solveNormal(Point(dimension, 1.0)) << std::endl;
      fullprint << "GramInverse=" << choleskyMethod.getGramInverse() << std::endl;
      fullprint << "HDiag=" << choleskyMethod.getHDiag() << std::endl;
      fullprint << "GramInverseTrace=" << choleskyMethod.getGramInverseTrace() << std::endl;
      fullprint << "GramInverseDiag=" << choleskyMethod.getGramInverseDiag() << std::endl;
    }
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
