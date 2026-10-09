//                                               -*- C++ -*-
/**
 *  @brief The test file of class LevelSetMesher for standard methods
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
#include <cmath>
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
    ResourceMap::SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber", 1000 );
    ResourceMap::SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumCallsNumber", 100000);
    ResourceMap::SetAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-7 );
    ResourceMap::SetAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-7 );
    ResourceMap::SetAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-7 );
    ResourceMap::SetAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-7 );
    PlatformInfo::SetNumericalPrecision(2);

    // The 1D mesher
    LevelSetMesher mesher1D(Indices(1, 7));
    fullprint << "mesher1D=" << mesher1D << std::endl;

    Scalar level = 0.5;
    SymbolicFunction function1D("x", "cos(x)/(1+0.1*x^2)");
    LevelSet levelSet1D(function1D, LessOrEqual(), level);
    // Manual bounding box
    Mesh mesh1D = mesher1D.build(levelSet1D, Interval(Point(1, -10.0), Point(1, 10.0)));
    fullprint << "mesh1D=" << mesh1D << std::endl;

    // The 2D mesher
    LevelSetMesher mesher2D(Indices(2, 5));
    fullprint << "mesher2D=" << mesher2D << std::endl;

    SymbolicFunction function2D(Description::BuildDefault(2, "x"), Description(1, "cos(x0 * x1)/(1 + 0.1 * (x0^2 + x1^2))"));
    LevelSet levelSet2D(function2D, LessOrEqual(), level);
    // Manual bounding box, linear interpolation
    {
      Mesh mesh2D = mesher2D.build(levelSet2D, Interval(Point(2, -10.0), Point(2, 10.0)), false);
      fullprint << "mesh2D=" << mesh2D << std::endl;
    }
    // Manual bounding box, solve the equation projection
    {
      ResourceMap::SetAsBool("LevelSetMesher-SolveEquation", true);
      Mesh mesh2D = mesher2D.build(levelSet2D, Interval(Point(2, -10.0), Point(2, 10.0)), true);
      fullprint << "mesh2D=" << mesh2D << std::endl;
    }
    // Manual bounding box, optimization projection
    {
      ResourceMap::SetAsBool("LevelSetMesher-SolveEquation", false);
      Mesh mesh2D = mesher2D.build(levelSet2D, Interval(Point(2, -10.0), Point(2, 10.0)), true);
      fullprint << "mesh2D=" << mesh2D << std::endl;
    }
    // The 3D mesher
    LevelSetMesher mesher3D(Indices(3, 3));
    fullprint << "mesher3D=" << mesher3D << std::endl;

    SymbolicFunction function3D(Description::BuildDefault(3, "x"), Description(1, "cos(x0 * x1 + x2)/(1 + 0.1*(x0^2 + x1^2 + x2^2))"));
    LevelSet levelSet3D(function3D, LessOrEqual(), level);
    // Manual bounding box
    ResourceMap::SetAsBool("LevelSetMesher-SolveEquation", true);
    Mesh mesh3D = mesher3D.build(levelSet3D, Interval(Point(3, -10.0), Point(3, 10.0)));
    fullprint << "mesh3D=" << mesh3D << std::endl;

    // The 4D mesher
    LevelSetMesher mesher4D(Indices(4, 5));
    fullprint << "mesher4D=" << mesher4D << std::endl;

    SymbolicFunction function4D(Description::BuildDefault(4, "x"), Description(1, "sqrt(x0^2+x1^2+x2^2+x3^2)"));
    LevelSet levelSet4D(function4D, LessOrEqual(), level);

    // Manual bounding box
    ResourceMap::SetAsBool("LevelSetMesher-SolveEquation", true);
    Mesh mesh4D = mesher4D.build(levelSet4D, Interval(Point(4, -0.5), Point(4, 0.5)));
    fullprint << "mesh4D=" << mesh4D << std::endl;

    // Collection of level sets = intersection: lens of two unit disks
    SymbolicFunction disk1(Description::BuildDefault(2, "x"), Description(1, "(x0+0.5)^2+x1^2"));
    SymbolicFunction disk2(Description::BuildDefault(2, "x"), Description(1, "(x0-0.5)^2+x1^2"));
    Collection<LevelSet> lens(0);
    lens.add(LevelSet(disk1, LessOrEqual(), 1.0));
    lens.add(LevelSet(disk2, LessOrEqual(), 1.0));
    LevelSetMesher lensMesher(Indices(2, 16));
    Interval lensBox(Point(2, -1.6), Point(2, 1.6));
    if (lensMesher.getUseQEF())
      throw TestFailed("QEF must be off by default");
    Mesh lensLegacy = lensMesher.build(lens, lensBox);
    lensMesher.setUseQEF(true);
    if (!lensMesher.getUseQEF())
      throw TestFailed("accessor round-trip failed");
    Mesh lensQEF = lensMesher.build(lens, lensBox);
    lensMesher.setUseQEF(false);
    const Scalar lensExact = 2.0 * std::acos(0.5) - 0.5 * std::sqrt(3.0);
    assert_almost_equal(lensQEF.getVolume(), lensExact, 1.0e-2, 1.0e-2, "wrong QEF lens volume");
    if (!(std::abs(lensQEF.getVolume() - lensExact) <= std::abs(lensLegacy.getVolume() - lensExact)))
      throw TestFailed("QEF lens must be at least as accurate as legacy lens");
    // 1-element collection parity with single build
    Collection<LevelSet> singleton(0);
    singleton.add(levelSet2D);
    Mesh singleMesh = mesher2D.build(levelSet2D, Interval(Point(2, -10.0), Point(2, 10.0)), false);
    Mesh collectionMesh = mesher2D.build(singleton, Interval(Point(2, -10.0), Point(2, 10.0)), false);
    assert_almost_equal(singleMesh.getVolume(), collectionMesh.getVolume(), 1.0e-12, 1.0e-12, "wrong singleton volume");
    assert_almost_equal(singleMesh.getVertices(), collectionMesh.getVertices(), 1.0e-12, 1.0e-12, "wrong singleton vertices");
    // Default value read in ResourceMap: on enables QEF at construction
    ResourceMap::SetAsBool("LevelSetMesher-UseQEF", true);
    LevelSetMesher defaultOnMesher(Indices(2, 16));
    if (!defaultOnMesher.getUseQEF())
      throw TestFailed("default must follow the ResourceMap key");
    ResourceMap::SetAsBool("LevelSetMesher-UseQEF", false);
    LevelSetMesher defaultOffMesher(Indices(2, 16));
    if (defaultOffMesher.getUseQEF())
      throw TestFailed("default must follow the ResourceMap key");
    // Empty collection raises
    try
    {
      mesher2D.build(Collection<LevelSet>(0), Interval(Point(2, -10.0), Point(2, 10.0)));
      throw TestFailed("empty collection must raise");
    }
    catch (const InvalidArgumentException &)
    {
      // expected
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
