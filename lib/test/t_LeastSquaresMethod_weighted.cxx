//                                               -*- C++ -*-
/**
 *  @brief The test file of class LeastSquaresMethod for the standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

void checkMethodResults(const String & name,
                        const Point & solve,
                        const Point & solveNormal,
                        const CovarianceMatrix & gramInverse,
                        const Point & hDiag,
                        Scalar gramInverseTrace,
                        const Point & gramInverseDiag,
                        const Point & refSolve,
                        const Point & refSolveNormal,
                        const CovarianceMatrix & refGramInverse,
                        const Point & refHDiag,
                        Scalar refGramInverseTrace,
                        const Point & refGramInverseDiag)
{
  OStream fullprint(std::cout);
  fullprint << name << std::endl;

  assert_almost_equal(solve, refSolve, 1e-10, 1e-10);
  assert_almost_equal(solveNormal, refSolveNormal, 1e-10, 1e-10);
  assert_almost_equal(gramInverse, refGramInverse, 1e-10, 1e-10);
  assert_almost_equal(hDiag, refHDiag, 1e-10, 1e-10);
  assert_almost_equal(gramInverseTrace, refGramInverseTrace, 1e-10, 1e-10);
  assert_almost_equal(gramInverseDiag, refGramInverseDiag, 1e-10, 1e-10);
}

template <typename Method>
void checkMethodH(Method & method, UnsignedInteger size)
{
  Point hFromH(size);
  SymmetricMatrix H(method.getH());
  for (UnsignedInteger k2 = 0; k2 < size; ++ k2) hFromH[k2] = H(k2, k2);
  assert_almost_equal(hFromH, method.getHDiag(), 1e-15, 1e-15);
  SquareMatrix H2(H * H);
  assert_almost_equal(H2, H, 1e-15, 1e-15);
}

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

    // Reference values extracted from QR with uniform weights
    Point refSolve;
    Point refSolveNormal;
    CovarianceMatrix refGramInverse;
    Point refHDiag;
    Scalar refGramInverseTrace = 0.0;
    Point refGramInverseDiag;

    // Test uniform weights: all three methods should agree
    fullprint << "Uniform weights" << std::endl << std::endl;
    {
      QRMethod qrMethod(proxy, weights, indices);
      qrMethod.update(Indices(0), indices, Indices(0));
      refSolve = qrMethod.solve(Point(size, 1.0));
      refSolveNormal = qrMethod.solveNormal(Point(dimension, 1.0));
      refGramInverse = qrMethod.getGramInverse();
      refHDiag = qrMethod.getHDiag();
      refGramInverseTrace = qrMethod.getGramInverseTrace();
      refGramInverseDiag = qrMethod.getGramInverseDiag();
    }
    {
      QRMethod qrMethod(proxy, weights, indices);
      qrMethod.update(Indices(0), indices, Indices(0));
      checkMethodResults("QR", qrMethod.solve(Point(size, 1.0)), qrMethod.solveNormal(Point(dimension, 1.0)), qrMethod.getGramInverse(), qrMethod.getHDiag(), qrMethod.getGramInverseTrace(), qrMethod.getGramInverseDiag(), refSolve, refSolveNormal, refGramInverse, refHDiag, refGramInverseTrace, refGramInverseDiag);
      checkMethodH(qrMethod, size);
    }
    {
      SVDMethod svdMethod(proxy, weights, indices);
      svdMethod.update(Indices(0), indices, Indices(0));
      checkMethodResults("SVD", svdMethod.solve(Point(size, 1.0)), svdMethod.solveNormal(Point(dimension, 1.0)), svdMethod.getGramInverse(), svdMethod.getHDiag(), svdMethod.getGramInverseTrace(), svdMethod.getGramInverseDiag(), refSolve, refSolveNormal, refGramInverse, refHDiag, refGramInverseTrace, refGramInverseDiag);
      checkMethodH(svdMethod, size);
    }
    {
      CholeskyMethod choleskyMethod(proxy, weights, indices);
      choleskyMethod.update(Indices(0), indices, Indices(0));
      checkMethodResults("Cholesky", choleskyMethod.solve(Point(size, 1.0)), choleskyMethod.solveNormal(Point(dimension, 1.0)), choleskyMethod.getGramInverse(), choleskyMethod.getHDiag(), choleskyMethod.getGramInverseTrace(), choleskyMethod.getGramInverseDiag(), refSolve, refSolveNormal, refGramInverse, refHDiag, refGramInverseTrace, refGramInverseDiag);
      checkMethodH(choleskyMethod, size);
    }

    // Save uniform-weight reference for cross-checking with non-uniform
    const Point uniformSolve(refSolve);
    const Point uniformSolveNormal(refSolveNormal);
    const CovarianceMatrix uniformGramInverse(refGramInverse);
    const Scalar uniformGramInverseTrace(refGramInverseTrace);

    // Test non-uniform weights: perturbation should barely change results
    fullprint << std::endl << "Non-uniform weights" << std::endl << std::endl;
    weights[0] += 1.e-10;
    {
      QRMethod qrMethod(proxy, weights, indices);
      qrMethod.update(Indices(0), indices, Indices(0));
      refSolve = qrMethod.solve(Point(size, 1.0));
      refSolveNormal = qrMethod.solveNormal(Point(dimension, 1.0));
      refGramInverse = qrMethod.getGramInverse();
      refHDiag = qrMethod.getHDiag();
      refGramInverseTrace = qrMethod.getGramInverseTrace();
      refGramInverseDiag = qrMethod.getGramInverseDiag();
    }
    {
      QRMethod qrMethod(proxy, weights, indices);
      qrMethod.update(Indices(0), indices, Indices(0));
      checkMethodResults("QR", qrMethod.solve(Point(size, 1.0)), qrMethod.solveNormal(Point(dimension, 1.0)), qrMethod.getGramInverse(), qrMethod.getHDiag(), qrMethod.getGramInverseTrace(), qrMethod.getGramInverseDiag(), refSolve, refSolveNormal, refGramInverse, refHDiag, refGramInverseTrace, refGramInverseDiag);
      checkMethodH(qrMethod, size);
    }
    {
      SVDMethod svdMethod(proxy, weights, indices);
      svdMethod.update(Indices(0), indices, Indices(0));
      checkMethodResults("SVD", svdMethod.solve(Point(size, 1.0)), svdMethod.solveNormal(Point(dimension, 1.0)), svdMethod.getGramInverse(), svdMethod.getHDiag(), svdMethod.getGramInverseTrace(), svdMethod.getGramInverseDiag(), refSolve, refSolveNormal, refGramInverse, refHDiag, refGramInverseTrace, refGramInverseDiag);
      checkMethodH(svdMethod, size);
    }
    {
      CholeskyMethod choleskyMethod(proxy, weights, indices);
      choleskyMethod.update(Indices(0), indices, Indices(0));
      checkMethodResults("Cholesky", choleskyMethod.solve(Point(size, 1.0)), choleskyMethod.solveNormal(Point(dimension, 1.0)), choleskyMethod.getGramInverse(), choleskyMethod.getHDiag(), choleskyMethod.getGramInverseTrace(), choleskyMethod.getGramInverseDiag(), refSolve, refSolveNormal, refGramInverse, refHDiag, refGramInverseTrace, refGramInverseDiag);
      checkMethodH(choleskyMethod, size);
    }

    // Non-uniform results should be nearly identical to uniform
    // (perturbation is only 1e-10)
    assert_almost_equal(refSolve, uniformSolve, 1e-4, 1e-4);
    assert_almost_equal(refSolveNormal, uniformSolveNormal, 1e-4, 1e-4);
    assert_almost_equal(refGramInverse, uniformGramInverse, 1e-4, 1e-4);
    assert_almost_equal(refGramInverseTrace, uniformGramInverseTrace, 1e-4, 1e-4);
  }

  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
