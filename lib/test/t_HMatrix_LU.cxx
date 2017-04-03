//                                               -*- C++ -*-
/**
 *  @brief The test file of class HMatrixImplementation
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  const Sample& vertices_;
  const NumericalScalar scaling_;
public:
  TestHMatrixRealAssemblyFunction(const Sample& vertices, NumericalScalar scaling)
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

    HMatrixParameters hmatrixParameters;
    hmatrixParameters.setAssemblyEpsilon(1.e-6);
    hmatrixParameters.setRecompressionEpsilon(1.e-6);

    const UnsignedInteger n = 30;

    Indices indices(0);
    indices.add(n);
    indices.add(n);
    const IntervalMesher intervalMesher(indices);
    const NumericalPoint lowerBound(2, 0.0);
    const NumericalPoint upperBound(2, 1.0);
    const Mesh mesh2D(intervalMesher.build(Interval(lowerBound, upperBound)));
    const Sample vertices(mesh2D.getVertices());

    TestHMatrixRealAssemblyFunction simpleAssembly(vertices, 10.0);
    // Non-symmetric HMatrix
    HMatrix hmat(hmatrixFactory.build(vertices, 1, false, hmatrixParameters));
    hmat.assemble(simpleAssembly, 'N');

    hmat.factorize("LU");

    NumericalPoint mean(vertices.computeMean());
    NumericalPoint rhs(vertices.getSize());
    for (UnsignedInteger i = 0; i < vertices.getSize(); ++i)
    {
      rhs[i] = simpleAssembly(vertices[i], mean);
    }
    NumericalPoint rhsCopy(rhs);
    NumericalScalar rhsCopyNorm = rhsCopy.norm();

    NumericalPoint result(hmat.solve(rhs));

    for (UnsignedInteger i = 0; i < vertices.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < vertices.getSize(); ++j)
      {
        rhsCopy[i] -= simpleAssembly(i, j) * result[j];
      }
    }
    NumericalScalar diffNorm = rhsCopy.norm();
    NumericalScalar threshold = 1.e-4;

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
