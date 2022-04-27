//                                               -*- C++ -*-
/**
 *  @brief The test file of class DomainComplement
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
    {
      // Interval
      Point p1(2, -1.0);
      Point p2(2, 2.0);
      Interval interval(p1, p2);
      // Inside
      Point p3((p1 + p2) * 0.5);
      // Outside
      Point p4(2.0 * p2);
      DomainComplement complement(interval);
      fullprint << "interval contains " << p3 << "? " << (interval.contains(p3) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p3 << "? " << (complement.contains(p3) ? "true" : "false") << std::endl;
      fullprint << "interval contains " << p4 << "? " << (interval.contains(p4) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p4 << "? " << (complement.contains(p4) ? "true" : "false") << std::endl;
      Sample sample(2, 2);
      sample[0] = p3;
      sample[1] = p4;
      fullprint << "interval contains " << sample << "? " << (interval.contains(sample)) << std::endl;
      fullprint << "complement contains " << sample << "? " << (complement.contains(sample)) << std::endl;
    }
    {
      // Mesh
      Sample vertices(0, 2);
      Point p(2);

      p[0] = 0.0;
      p[1] = 0.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 0.0;
      vertices.add(p);

      p[0] = 1.0;
      p[1] = 1.0;
      vertices.add(p);

      p[0] = 1.5;
      p[1] = 1.0;
      vertices.add(p);

      p[0] = 2.0;
      p[1] = 1.5;
      vertices.add(p);

      p[0] = 0.5;
      p[1] = 1.5;
      vertices.add(p);
      IndicesCollection simplices(5, 3);
      simplices(0, 0) = 0;
      simplices(0, 1) = 1;
      simplices(0, 2) = 2;

      simplices(1, 0) = 1;
      simplices(1, 1) = 2;
      simplices(1, 2) = 3;

      simplices(2, 0) = 2;
      simplices(2, 1) = 3;
      simplices(2, 2) = 4;

      simplices(3, 0) = 2;
      simplices(3, 1) = 4;
      simplices(3, 2) = 5;

      simplices(4, 0) = 0;
      simplices(4, 1) = 2;
      simplices(4, 2) = 5;
      Mesh mesh2D(vertices, simplices);
      MeshDomain mesh2Ddomain(mesh2D);
      const Point p0(2, -0.25);
      const Point p1(2, 1.25);
      DomainComplement complement(mesh2Ddomain);
      fullprint << "mesh contains " << p0 << "? " << (mesh2Ddomain.contains(p0) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p0 << "? " << (complement.contains(p0) ? "true" : "false") << std::endl;
      fullprint << "mesh contains " << p1 << "? " << (mesh2Ddomain.contains(p1) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p1 << "? " << (complement.contains(p1) ? "true" : "false") << std::endl;
      Sample sample(2, 2);
      sample[0] = p0;
      sample[1] = p1;
      fullprint << "mesh contains " << sample << "? " << (mesh2Ddomain.contains(sample)) << std::endl;
      fullprint << "complement contains " << sample << "? " << (complement.contains(sample)) << std::endl;
    }
    {
      // Level set
      Description inVars(0);
      inVars.add("x");
      inVars.add("y");
      Description formula(1, "x^4 + y^4");
      SymbolicFunction function(inVars, formula);
      LevelSet levelSet(function, LessOrEqual(), 1.0);
      fullprint << "levelSet=" << levelSet << std::endl;
      Point p0(2);
      p0[0] = -0.5;
      p0[1] = -0.5;
      Point p1(2);
      p1[0] = 0.5;
      p1[1] = 0.0;
      Point p2(2);
      p2[0] = 1.5;
      p2[1] = 0.0;
      DomainComplement complement(levelSet);
      fullprint << "levelSet contains " << p0 << "? " << (levelSet.contains(p0) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p0 << "? " << (complement.contains(p0) ? "true" : "false") << std::endl;
      fullprint << "levelSet contains " << p1 << "? " << (levelSet.contains(p1) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p1 << "? " << (complement.contains(p1) ? "true" : "false") << std::endl;
      fullprint << "levelSet contains " << p2 << "? " << (levelSet.contains(p2) ? "true" : "false") << std::endl;
      fullprint << "complement contains " << p2 << "? " << (complement.contains(p2) ? "true" : "false") << std::endl;
      Sample sample(3, 2);
      sample[0] = p0;
      sample[1] = p1;
      sample[2] = p2;
      fullprint << "levelSet contains " << sample << "? " << (levelSet.contains(sample)) << std::endl;
      fullprint << "complement contains " << sample << "? " << (complement.contains(sample)) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
