//                                               -*- C++ -*-
/**
 *  @brief The test file of class DomainComplement
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
    // First domain: [0,2]x[0,2]
    Interval cube(Point(2, 0.0), Point(2, 2.0));

    // Second domain: sphere center=(0,0) r=1
    Description inVars(0);
    inVars.add("x");
    inVars.add("y");
    Description formula(1, "x^2 + y^2");
    SymbolicFunction function(inVars, formula);
    LevelSet sphere(function, LessOrEqual(), 1.0);

    Point p0(2); // inside sphere but not cube
    p0[0] = -0.2;
    p0[1] = 0.2;
    Point p1(2, 0.2); // inside cube and sphere
    Point p2(2, 0.8); // inside cube but not sphere
    Point p3(2, 4.0); // outside

    DomainIntersection domain(DomainIntersection::DomainCollection({cube, sphere}));
    fullprint << "cube=" << cube << std::endl;
    fullprint << "sphere=" << sphere << std::endl;
    fullprint << "union=" << domain << std::endl;
    fullprint << "union contains " << p0 << "? " << (domain.contains(p0) ? "true" : "false") << std::endl;
    fullprint << "union contains " << p1 << "? " << (domain.contains(p1) ? "true" : "false") << std::endl;
    fullprint << "union contains " << p2 << "? " << (domain.contains(p2) ? "true" : "false") << std::endl;
    fullprint << "union contains " << p3 << "? " << (domain.contains(p3) ? "true" : "false") << std::endl;

    Sample sample(4, 2);
    sample[0] = p0;
    sample[1] = p1;
    sample[2] = p2;
    sample[3] = p3;
    fullprint << "union contains " << sample << "? " << (domain.contains(sample)) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
