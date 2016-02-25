//                                               -*- C++ -*-
/**
 *  @brief The test file of class HMatrixImplementation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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
  const NumericalSample& vertices_;
  const NumericalScalar scaling_;
public:
  TestHMatrixRealAssemblyFunction(const NumericalSample& vertices, NumericalScalar scaling)
    : vertices_(vertices)
    , scaling_(scaling)
  {}
  inline NumericalScalar operator() (NumericalPoint pt1, NumericalPoint pt2) const
  {
    NumericalPoint difference(pt1 - pt2);
    return exp(-std::abs(difference.norm()) / scaling_);
  }
  NumericalScalar operator() (UnsignedInteger i, UnsignedInteger j) const
  {
    return operator()(vertices_[i], vertices_[j]);
  }
};


int main(int argc, char *argv[])
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

    const UnsignedInteger n(30);

    Indices indices(0);
    indices.add(n);
    indices.add(n);
    const IntervalMesher intervalMesher(indices);
    const NumericalPoint lowerBound(2, 0.0);
    const NumericalPoint upperBound(2, 1.0);
    const Mesh mesh2D(intervalMesher.build(Interval(lowerBound, upperBound)));
    const NumericalSample vertices2D(mesh2D.getVertices());
    NumericalSample vertices(vertices2D.getSize(), 3);
    for (UnsignedInteger i = 0; i < vertices2D.getSize(); ++i)
    {
      vertices[i][0] = vertices2D[i][0];
      vertices[i][1] = vertices2D[i][1];
    }
    TestHMatrixRealAssemblyFunction simpleAssembly(vertices, 0.1);
    // Symmetric HMatrix
    HMatrix hmat(hmatrixFactory.build(vertices, 1, true));
    hmat.assemble(simpleAssembly, 'L');

    HMatrix hmatRef(hmat);
    NumericalScalar refNorm = hmatRef.norm();

    hmat.factorize("LLt");

    hmatRef.gemm('N', 'T', -1., hmat, hmat, 1.);
    NumericalScalar threshold(1.e-10);
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
