//                                               -*- C++ -*-
/**
 *  @brief The test file of class HMatrixImplementation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
  setRandomGenerator();

  try
  {
    ResourceMap::SetAsBool("HMatrix-ForceSequential", true);
    ResourceMap::SetAsUnsignedInteger("HMatrix-MaxLeafSize", 10);

    HMatrixFactory hmatrixFactory;

    const UnsignedInteger n = 30;

    const Indices indices = {n, n};
    const IntervalMesher intervalMesher(indices);
    const Point lowerBound = {0.0, 0.0};
    const Point upperBound = {1.0, 1.0};
    const Mesh mesh2D(intervalMesher.build(Interval(lowerBound, upperBound)));
    const Sample vertices(mesh2D.getVertices());
    TestHMatrixRealAssemblyFunction simpleAssembly(vertices, 0.1);
    // Symmetric HMatrix
    HMatrix hmat(hmatrixFactory.build(vertices, 1, true));
    hmat.assemble(simpleAssembly, 'L');

    hmat.factorize("LDLt");

    Point mean(vertices.computeMean());
    Point rhs(vertices.getSize());
    for (UnsignedInteger i = 0; i < vertices.getSize(); ++i)
    {
      rhs[i] = simpleAssembly(vertices[i], mean);
    }
    Point rhsCopy(rhs);
    Scalar rhsCopyNorm = rhsCopy.norm();

    Point result(hmat.solve(rhs));

    for (UnsignedInteger i = 0; i < vertices.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < vertices.getSize(); ++j)
      {
        rhsCopy[i] -= simpleAssembly(i, j) * result[j];
      }
    }
    Scalar diffNorm = rhsCopy.norm();
    Scalar threshold = 1.e-2;
    fullprint << "|| M X - b || / || b ||" << ((diffNorm < threshold * rhsCopyNorm) ? " < " : " > ") << threshold << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
