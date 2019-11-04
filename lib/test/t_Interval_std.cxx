//                                               -*- C++ -*-
/**
 *  @brief The test file of class Interval for standard methods
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

using namespace OT;
using namespace OT::Test;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // The 1D interface
    Interval interval1D(-3, 5);
    fullprint << "interval1D=" << interval1D << std::endl;
    // The default interface
    Interval defaultInterval(2);
    fullprint << "defaultInterval=" << defaultInterval << std::endl;
    // Construction from 2 Point
    Point p1(2, -1.0);
    Point p2(2, 2.0);
    fullprint << "interval from 2 points=" << Interval(p1, p2) << std::endl;
    // Construction from 2 points and 2 flags
    Interval::BoolCollection flag1(2, false);
    Interval::BoolCollection flag2(2, true);
    Interval interval(p1, p2, flag1, flag2);
    fullprint << "interval from 2 points and 2 flags=" << interval << std::endl;
    // Accessors
    fullprint << "lower bound=" << interval.getLowerBound() << std::endl;
    fullprint << "upper bound=" << interval.getUpperBound() << std::endl;
    fullprint << "lower bound flags=" << interval.getFiniteLowerBound() << std::endl;
    fullprint << "upper bound flags=" << interval.getFiniteUpperBound() << std::endl;
    // Check if a given interval is empty
    fullprint << "interval [p1, p2] empty? " << (Interval(p1, p2).isEmpty() ? "true" : "false") << std::endl;
    fullprint << "interval [p2, p1] empty? " << (Interval(p2, p1).isEmpty() ? "true" : "false") << std::endl;
    // Intersection
    Interval interval1(p1, p2);
    Point p3(2);
    p3[0] = 0.5;
    p3[1] = -1.5;
    Point p4(2);
    p4[0] = 1.5;
    p4[1] = 2.5;
    Interval interval2(p3, p4);
    fullprint << "intersection of " << interval1 << " and " << interval2 << " equals " << interval1.intersect(interval2) << std::endl;
    // Inside
    Point p5((p1 + p2) * 0.5);
    fullprint << "is point " << p5 << " inside " << interval1 << "? " << (interval1.contains(p5) ? "yes" : "no") << std::endl;
    Point p6(2.0 * p2);
    fullprint << "is point " << p6 << " inside " << interval1 << "? " << (interval1.contains(p6) ? "yes" : "no") << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
