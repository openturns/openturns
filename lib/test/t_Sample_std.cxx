//                                               -*- C++ -*-
/**
 *  @brief The test file of class Sample for standard methods
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public Sample
{
public:
  TestObject() : Sample(1, 1) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test basic functionnalities
    checkClassWithClassName<TestObject>();

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

    // We get the second element of the sample
    Point secondElement = sample1[1];
    fullprint << "second element=" << secondElement << std::endl;

    // We set the third element to a valid new element
    Point newPoint(2);
    newPoint[0] = 1000;
    newPoint[1] = 2000;
    sample1[2] = newPoint;
    fullprint << "sample1=" << sample1 << std::endl;

    Point translation(2, 5.0);
    sample1 += translation;
    fullprint << "after a translation of vector=" << translation << " sample1=" << sample1 << std::endl;

    Point scaling(2, 2.0);
    sample1 *= scaling;
    fullprint << "after a scaling of vector=" << scaling << " sample1=" << sample1 << std::endl;

    Sample sample2(10, point1);
    fullprint << "sample2=" << sample2 << std::endl;

    sample2[5] = point2;
    fullprint << "sample2=" << sample2 << std::endl;


    try
    {
      // We get the tenth element of the sample
      // THIS SHOULD NORMALY FAIL
      Point tenthElement = sample1.at(9);

      // Normally, we should never go here
      throw TestFailed("Exception has NOT been thrown or catched !");

    }
    catch (OutOfBoundException & ex)
    {
      // Nothing to do
    }




    // We try to create a sample with 5 times the same point
    Point samePoint(3);
    samePoint[0] = 1000.;
    samePoint[1] = 2000.;
    samePoint[2] = 3000.;
    Sample sample3(5, samePoint);
    fullprint << "sample3=" << sample3 << std::endl;

    // We append a new point to the sample...
    Point anotherNewPoint(3);
    anotherNewPoint[0] = -1000.;
    anotherNewPoint[1] = -2000.;
    anotherNewPoint[2] = -3000.;
    sample3.add(anotherNewPoint);
    fullprint << "sample3=" << sample3 << std::endl;

    // ... then the first point again
    sample3.add(samePoint);
    fullprint << "sample3=" << sample3 << std::endl;

    // We find the new point into the sample
    UnsignedInteger pos = sample3.find(anotherNewPoint);
    //fullprint << "pos=" << pos << std::endl;
    if (pos != 5)
    {
      OSS oss;
      oss << "Error in finding point into sample. Expected 5. Got " << pos;
      throw TestFailed(oss);
    }

    //for(UnsignedInteger i=0; i<sample3.getSize(); ++i)
    //  fullprint << "sample3[" << i << "]=" << sample3[i] << std::endl;

    // We erase the new point from the sample
    sample3.erase(pos);
    fullprint << "sample3=" << sample3 << std::endl;

    // Partial copy constructor containing sample2[4:7]
    Sample sample4(sample2, 4, 8);
    fullprint << "sample4=" << sample4 << std::endl;

    // Internal storage
    Sample sample5(3, 1);
    sample5[0][0] = 1.0;
    sample5[1][0] = 3.0;
    sample5[2][0] = 2.0;
    fullprint << "sample5=" << sample5.asPoint() << std::endl;
    fullprint << "sample5 (linear)=" << sample5.asPoint() << std::endl;

    // Selection
    Indices indices(5);
    indices[0] = 1;
    indices[1] = 0;
    indices[2] = 2;
    indices[3] = 1;
    indices[4] = 2;
    fullprint << "selection=" << sample5.select(indices) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
