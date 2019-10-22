//                                               -*- C++ -*-
/**
 *  @brief The test file of class HMatrixImplementation
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

class TestHMatrixTensorRealAssemblyFunction : public HMatrixTensorRealAssemblyFunction
{
private:
  const CovarianceModel & covarianceModel_;
  const Sample & vertices_;

public:
  TestHMatrixTensorRealAssemblyFunction(const CovarianceModel & covarianceModel, const Sample & vertices)
    : HMatrixTensorRealAssemblyFunction(covarianceModel.getOutputDimension())
    , covarianceModel_(covarianceModel)
    , vertices_(vertices)
  {}

  void compute(UnsignedInteger i, UnsignedInteger j, Matrix* result) const
  {
    CovarianceMatrix localResult(covarianceModel_( vertices_[i] - vertices_[j] ));
    std::copy(&localResult.getImplementation()->operator[](0), &localResult.getImplementation()->operator[](0) + dimension_ * dimension_, &result->getImplementation()->operator[](0));
  }
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    ResourceMap::SetAsBool("HMatrix-ForceSequential", true);
    ResourceMap::SetAsUnsignedInteger("HMatrix-MaxLeafSize", 10);

    HMatrixFactory hmatrixFactory;
    if (!hmatrixFactory.IsAvailable()) throw NotYetImplementedException(HERE);

    const UnsignedInteger n = 20;

    Indices indices(0);
    indices.add(n);
    indices.add(n);
    const IntervalMesher intervalMesher(indices);
    const Point lowerBound(2, 0.0);
    const Point upperBound(2, 1.0);
    const Mesh mesh2D(intervalMesher.build(Interval(lowerBound, upperBound)));
    const Sample vertices(mesh2D.getVertices());
    Point xMin(vertices.getMin());
    Point xMax(vertices.getMax());
    Point scale(2, 0.1);
    CovarianceModel covarianceModel(ExponentialModel(scale, Point(2, 1.0)));

    TestHMatrixTensorRealAssemblyFunction blockAssembly(covarianceModel, vertices);
    // Non-symmetric HMatrix
    HMatrix hmat(hmatrixFactory.build(vertices, covarianceModel.getOutputDimension(), true));
    hmat.assemble(blockAssembly, 'L');

    HMatrix hmatRef(hmat);
    hmatRef.assemble(blockAssembly, 'L');
    Scalar refNorm = hmatRef.norm();

    hmat.factorize("LLt");

    hmatRef.gemm('N', 'T', -1., hmat, hmat, 1.);
    Scalar threshold = 5.e-3;
    fullprint << "|| M - L Lt || / || M ||" << ((hmatRef.norm() < threshold * refNorm) ? " < " : " > ") << threshold << std::endl;
  }
  catch (NotYetImplementedException & ex)
  {
    std::cerr << "Compiled without HMat" << std::endl;
    return ExitCode::Success;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
