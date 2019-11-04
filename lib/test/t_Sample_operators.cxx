//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample for standard methods
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

    // We create an empty Sample
    Sample sample1(0, 2);
    sample1.setName("Sample1");

    // We populate the empty sample
    Point point1(2);
    point1[0] = 10.;
    point1[1] = 20.;
    Point point2(2);
    point2[0] = 11.;
    point2[1] = 21.;
    Point point3(2);
    point3[0] = 12.;
    point3[1] = 22.;
    sample1.add(point1);
    sample1.add(point2);
    sample1.add(point3);
    fullprint << "sample1=" << sample1 << std::endl;


    // We create a new empty Sample
    Sample sample2(0, 2);
    sample2.setName("Sample2");

    // We populate the empty sample
    Point point4(2);
    point4[0] = 30.;
    point4[1] = 40.;
    Point point5(2);
    point5[0] = 31.;
    point5[1] = 41.;
    Point point6(2);
    point6[0] = 32.;
    point6[1] = 42.;
    sample2.add(point4);
    sample2.add(point5);
    sample2.add(point6);
    fullprint << "sample2=" << sample2 << std::endl;

    // Operator +/-
    const Sample sum(sample1 + sample2);
    const Sample diff(sample2 - sample1);
    fullprint << "sample1 + sample2=" << sum << std::endl;
    fullprint << "sample2 - sample1=" << diff << std::endl;

    // Operator +=/-=
    Sample sample3(sample2);
    Sample sample4(sample2);
    sample3 += sample1;
    sample4 -= sample1;
    fullprint << "sample3=" << sample3 << std::endl;
    fullprint << "sample4=" << sample4 << std::endl;

    // translation ==> += operator
    Point translation(2, 5.0);
    sample1 += translation;
    fullprint << "after a translation of vector=" << translation << " sample1=" << sample1 << std::endl;

    // scaling ==> *= operator
    Point scaling(2, 2.0);
    sample1 *= scaling;
    fullprint << "after a scaling of vector=" << scaling << " sample1=" << sample1 << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
