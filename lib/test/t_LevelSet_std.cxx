//                                               -*- C++ -*-
/**
 *  @brief The test file of class LevelSet for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // The function defining the level-set
    Description inVars(0);
    inVars.add("x");
    inVars.add("y");
    Description formula1(1, "x^4 + y^4");
    SymbolicFunction function1(inVars, formula1);
    LevelSet levelSet1(function1, LessOrEqual(), 1.0);
    fullprint << "levelSet1=" << levelSet1 << std::endl;
    Point p0(2);
    p0[0] = -0.5;
    p0[1] = -0.5;
    Point p1(2);
    p1[0] = 0.5;
    p1[1] = 0.0;
    Point p2(2);
    p2[0] = 1.5;
    p2[1] = 0.0;
    fullprint << "levelSet1 contains " << p0 << "? " << (levelSet1.contains(p0) ? "true" : "false") << std::endl;
    fullprint << "levelSet1 contains " << p1 << "? " << (levelSet1.contains(p1) ? "true" : "false") << std::endl;
    fullprint << "levelSet1 contains " << p2 << "? " << (levelSet1.contains(p2) ? "true" : "false") << std::endl;
    // Intersection
    Description formula2(1, "(x-1)^2 + y^2");
    SymbolicFunction function2(inVars, formula2);
    LevelSet levelSet2(function2, LessOrEqual(), 1.0);
    fullprint << "levelSet2=" << levelSet2 << std::endl;
    fullprint << "levelSet2 contains " << p0 << "? " << (levelSet2.contains(p0) ? "true" : "false") << std::endl;
    fullprint << "levelSet2 contains " << p1 << "? " << (levelSet2.contains(p1) ? "true" : "false") << std::endl;
    fullprint << "levelSet2 contains " << p2 << "? " << (levelSet2.contains(p2) ? "true" : "false") << std::endl;
    LevelSet intersection(levelSet1.intersect(levelSet2));
    fullprint << "intersection of " << levelSet1 << " and " << levelSet2 << " equals " << intersection << std::endl;
    fullprint << "intersection contains " << p0 << "? " << (intersection.contains(p0) ? "true" : "false") << std::endl;
    fullprint << "intersection contains " << p1 << "? " << (intersection.contains(p1) ? "true" : "false") << std::endl;
    fullprint << "intersection contains " << p2 << "? " << (intersection.contains(p2) ? "true" : "false") << std::endl;
    // Union
    LevelSet join(levelSet1.join(levelSet2));
    fullprint << "join of " << levelSet1 << " and " << levelSet2 << " equals " << join << std::endl;
    fullprint << "join contains " << p0 << "? " << (join.contains(p0) ? "true" : "false") << std::endl;
    fullprint << "join contains " << p1 << "? " << (join.contains(p1) ? "true" : "false") << std::endl;
    fullprint << "join contains " << p2 << "? " << (join.contains(p2) ? "true" : "false") << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
