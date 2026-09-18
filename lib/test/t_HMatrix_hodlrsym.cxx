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


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    ResourceMap::SetAsBool("HMatrix-ForceSequential", true);
    ResourceMap::SetAsUnsignedInteger("HMatrix-MaxLeafSize", 400);
    ResourceMap::SetAsUnsignedInteger("HMatrix-ClusteringMaxDof", 400);

    HMatrixFactory hmatrixFactory;
    const UnsignedInteger n = 39;

    const Indices indices = {n, n};
    const IntervalMesher intervalMesher(indices);
    const Point lowerBound = {0.0, 0.0};
    const Point upperBound = {1.0, 1.0};
    const Mesh mesh(intervalMesher.build(Interval(lowerBound, upperBound)));
    const Sample vertices(mesh.getVertices());
    TestHMatrixRealAssemblyFunction simpleAssembly(vertices, 0.1);
    // Symmetric HMatrix
    HMatrixParameters parameters;
    parameters.setAdmissibility("hodlr");
    parameters.setCompressionMethod("AcaRandom");
    parameters.setFactorizationMethod("hodlrsym");
    HMatrix hmat(hmatrixFactory.build(vertices, 1, true, parameters));
    hmat.assemble(simpleAssembly, 'L');
    // Ref = Assembled matrix
    HMatrix hmatRef(hmat);
    hmat.factorize("hodlrsym");

    const Scalar refNorm = hmatRef.norm();
    // Compute the norm of the difference between the original matrix and the product of the factors using a stochastic method.
    UnsignedInteger nSamples=1000;
    // Generate a random sample of vectors (Normal distribution)
    Point randomSample(hmat.getNbRows());
    Point error(hmat.getNbRows());
    Scalar normDiff = 0.0;
    for (UnsignedInteger i = 0; i < nSamples; ++i)
    { 
      // DistFunc generate
      randomSample = DistFunc::rNormal(hmat.getNbRows());
      // hmat * (hmat^T * randomSample[i]) -->  first (hmat^T * randomSample[i])
      hmat.gemv('T', 1., randomSample, 0., error);
      // Then (hmat * error) --> error
      hmat.gemv('N', 1., error, 0., error);
      // hmatRef * randomSample[i]
      hmatRef.gemv('N', 1., randomSample, -1.0, error);
      normDiff += error.normSquare();
    }
    normDiff = std::sqrt(normDiff / nSamples);
    Scalar threshold = 0.00342328;
    assert_almost_equal(std::abs(normDiff / refNorm), threshold, 1e-8, 1e-6);
    const Scalar normL(hmat.norm());
    const Scalar alpha(0.1);
    hmat.scale(alpha);
    assert_almost_equal(std::abs(normL - hmat.norm() / alpha), 0.0, 1e-8, 1e-6);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
