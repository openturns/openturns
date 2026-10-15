//                                               -*- C++ -*-
/**
 *  @brief The test file of class HMatrixImplementation
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

class TestHMatrixRealAssemblyFunction : public HMatrixRealAssemblyFunction
{
  const Sample& vertices_;
  const Scalar scaling_;
public:
  TestHMatrixRealAssemblyFunction(const Sample& vertices, Scalar scaling)
    : vertices_(vertices)
    , scaling_(scaling)
  {}
  inline Scalar operator() (Point pt1, Point pt2) const
  {
    Point difference(pt1 - pt2);
    return exp(-std::abs(difference.norm()) / scaling_);
  }
  Scalar operator() (UnsignedInteger i, UnsignedInteger j) const
  {
    return operator()(vertices_[i], vertices_[j]);
  }
};

Scalar computeNormError(char side, char transB, char transA, Scalar alpha, HMatrix& hmat, const TriangularMatrix& cholDense, const Matrix& randomSampleMat)
{
  OStream fullprint(std::cout);
  bool trans_a, trans_b;
  bool transpose_result = false;
  Matrix epsilonHmat(hmat.gemm_dense(transB, transA, side, alpha, randomSampleMat));
  // Reference product
  Matrix epsilonMat;

  if (side == 'R')
  {
    transpose_result = true;
    if (transA == 'T')
      trans_a = false;
    else
      trans_a = true;
    if (transB == 'T')
      trans_b = false;
    else
      trans_b = true;
  }
  else
  {
    if (transB == 'T')
      trans_b = true;
    else
      trans_b = false;
    if (transA == 'T')
      trans_a = true;
    else
      trans_a = false;
  }
  Matrix left, right;
  if (trans_b)
   left = cholDense.transpose();
  else
    left = cholDense;
  if (trans_a)
    right = randomSampleMat.transpose();
  else
    right = randomSampleMat;
  epsilonMat = left * right;
  if (transpose_result)
    epsilonMat = epsilonMat.transpose();
  Matrix diff(epsilonMat - epsilonHmat);
  diff.squareElements();
  Scalar normDiff = std::sqrt(diff.computeSumElements() / randomSampleMat.getNbRows());
  return normDiff;
}


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    ResourceMap::SetAsBool("HMatrix-ForceSequential", true);
    ResourceMap::SetAsUnsignedInteger("HMatrix-MaxLeafSize", 10);
    ResourceMap::SetAsScalar("HMatrix-AssemblyEpsilon", 1e-12);
    ResourceMap::SetAsScalar("HMatrix-RecompressionEpsilon", 1e-12);

    HMatrixFactory hmatrixFactory;

    const UnsignedInteger n = 10;

    const Indices indices = {n, n};
    const IntervalMesher intervalMesher(indices);
    const Point lowerBound = {0.0, 0.0};
    const Point upperBound = {1.0, 1.0};
    const Mesh mesh2D(intervalMesher.build(Interval(lowerBound, upperBound)));
    const Sample vertices(mesh2D.getVertices());
    TestHMatrixRealAssemblyFunction simpleAssembly(vertices, 0.05);
    // Symmetric HMatrix
    HMatrix hmat(hmatrixFactory.build(vertices, 1, true));
    hmat.assemble(simpleAssembly, 'L');

    HMatrix hmatRef(hmat);
    const Scalar refNorm = hmatRef.norm();

    hmat.factorize("LLt");

    // Create a full dense matrix
    CovarianceMatrix fullDense(mesh2D.getVertices().getSize());
    for (UnsignedInteger j=0; j < fullDense.getNbColumns(); ++j)
      for (UnsignedInteger i=0; i < fullDense.getNbRows(); ++i)
        fullDense(i, j) = simpleAssembly(i, j);
    // Reference cholsesky
    TriangularMatrix cholDense(fullDense.computeCholesky());

    // First check with identity
    Matrix randomSampleMat(IdentityMatrix(hmat.getNbRows()));
    // All values should be the same
    const Scalar alpha = 1.0;
    Point errors;
    Scalar normError;

    char vals_trans[2] = {'N', 'T'};
    char vals_side[2] = {'L', 'R'};

    // Iterate through all 2x2x2 = 8 combinations
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) 
        {
            for (int k = 0; k < 2; k++) 
            {
                char side = vals_side[i];
                char trans_b = vals_trans[j];
                char trans_a = vals_trans[k];
                // Case 1 : side, transB, transA
               normError = computeNormError(side, trans_b, trans_a, alpha, hmat, cholDense, randomSampleMat);
               assert_almost_equal(normError, 0.000199239, 1e-8, 1e-8);
               errors.add(normError);
            }
        }
    }
    // errors should have a 0 standard deviation
    Sample errorAsSample(Sample::BuildFromPoint(errors));
    Scalar sigma = errorAsSample.computeStandardDeviation()[0];
    assert_almost_equal(sigma, 0, 1e-12, 1e-12);

    // Check with rectangular matrix
    // Generate a random sample of size nRows x nSamples 
    const UnsignedInteger nSamples = 100;
    randomSampleMat = Matrix(hmat.getNbRows(), nSamples);
    Point randomSample(DistFunc::rNormal(nSamples * hmat.getNbColumns()));
    randomSampleMat = Matrix(hmat.getNbColumns(), nSamples, randomSample);
    errors = Point(0);
    // Case a : side = 'L', transA = 'N', transB = 'N'
    normError = computeNormError('L', 'N', 'N', alpha, hmat, cholDense, randomSampleMat);
    assert_almost_equal(normError, 0.00199516, 1e-8, 1e-8);
    errors.add(normError);
    // Case b : side = 'R', transA = 'T', transB = 'T' (same result as case a expected)
    normError = computeNormError('R', 'T', 'T', alpha, hmat, cholDense, randomSampleMat);
    assert_almost_equal(normError, 0.00199516, 1e-8, 1e-8);
    errors.add(normError);
    // check the results are the same
    errorAsSample = Sample::BuildFromPoint(errors);
    sigma = errorAsSample.computeStandardDeviation()[0];
    assert_almost_equal(sigma, 0, 1e-12, 1e-12);
    // 
    errors = Point(0);
    // Case c : side = 'L', transA = 'N', transB = 'T'
    normError = computeNormError('L', 'T', 'N', alpha, hmat, cholDense, randomSampleMat);
    assert_almost_equal(normError, 0.00199509, 1e-8, 1e-8);
    errors.add(normError);
    // Case d : side = 'R', transA = 'T', transB = 'N' (same result as case c expected)
    normError = computeNormError('R', 'N', 'T', alpha, hmat, cholDense, randomSampleMat);
    assert_almost_equal(normError, 0.00199509, 1e-8, 1e-8);
    errors.add(normError);
    errorAsSample = Sample::BuildFromPoint(errors);
    sigma = errorAsSample.computeStandardDeviation()[0];
    assert_almost_equal(sigma, 0, 1e-12, 1e-12);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
