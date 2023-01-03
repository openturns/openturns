//                                               -*- C++ -*-
/**
 *  @brief The test file of class LevelSetMesher for standard methods
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

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    ResourceMap::SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber", 1000 );
    ResourceMap::SetAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 100000);
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
    // Manual bounding box
    Mesh mesh2D = mesher2D.build(levelSet2D, Interval(Point(2, -10.0), Point(2, 10.0)));
    fullprint << "mesh2D=" << mesh2D << std::endl;

    // The 3D mesher
    LevelSetMesher mesher3D(Indices(3, 3));
    fullprint << "mesher3D=" << mesher3D << std::endl;

    SymbolicFunction function3D(Description::BuildDefault(3, "x"), Description(1, "cos(x0 * x1 + x2)/(1 + 0.1*(x0^2 + x1^2 + x2^2))"));
    LevelSet levelSet3D(function3D, LessOrEqual(), level);
    // Manual bounding box
    Mesh mesh3D = mesher3D.build(levelSet3D, Interval(Point(3, -10.0), Point(3, 10.0)));
    fullprint << "mesh3D=" << mesh3D << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
