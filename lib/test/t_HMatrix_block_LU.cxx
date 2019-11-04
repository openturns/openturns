//                                               -*- C++ -*-
/**
 *  @brief The test file of class HMatrixImplementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    const Sample vertices2D(mesh2D.getVertices());
    Sample vertices(vertices2D.getSize(), 3);
    for (UnsignedInteger i = 0; i < vertices2D.getSize(); ++i)
    {
      vertices[i][0] = vertices2D[i][0];
      vertices[i][1] = vertices2D[i][1];
    }
    Point xMin(vertices.getMin());
    Point xMax(vertices.getMax());
    Point scale(3, 0.1);
    CovarianceModel covarianceModel(ExponentialModel(scale, Point(3, 1.0)));

    TestHMatrixTensorRealAssemblyFunction blockAssembly(covarianceModel, vertices);
    // Non-symmetric HMatrix
    HMatrix hmat(hmatrixFactory.build(vertices, covarianceModel.getOutputDimension(), false));
    hmat.assemble(blockAssembly, 'N');

    hmat.factorize("LU");

    Point rhs(covarianceModel.getOutputDimension() * vertices.getSize());
    CovarianceMatrix local(covarianceModel.getOutputDimension());
    for (UnsignedInteger i = 0; i < vertices.getSize(); ++i)
    {
      blockAssembly.compute(i, 0, &local);
      for (UnsignedInteger dim = 0; dim < covarianceModel.getOutputDimension(); ++dim)
      {
        rhs[covarianceModel.getOutputDimension() * i + dim] = local(dim, 0);
      }
    }
    Point rhsCopy(rhs);
    Scalar rhsCopyNorm = rhsCopy.norm();

    Point result(hmat.solve(rhs));

    for (UnsignedInteger i = 0; i < vertices.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < vertices.getSize(); ++j)
      {
        blockAssembly.compute(i, j, &local);
        for (UnsignedInteger dimI = 0; dimI < covarianceModel.getOutputDimension(); ++dimI)
        {
          for (UnsignedInteger dimJ = 0; dimJ < covarianceModel.getOutputDimension(); ++dimJ)
          {
            rhsCopy[covarianceModel.getOutputDimension() * i + dimI] -= local(dimI, dimJ) * result[covarianceModel.getOutputDimension() * j + dimJ];
          }
        }
      }
    }
    Scalar diffNorm = rhsCopy.norm();
    Scalar threshold = 5.e-3;
    fullprint << "|| M X - b || / || b ||" << ((diffNorm < threshold * rhsCopyNorm) ? " < " : " > ") << threshold << std::endl;
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
