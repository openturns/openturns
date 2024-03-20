//                                               -*- C++ -*-
/**
 *  @brief The test file of class TimeSeries for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestObject : public TimeSeries
{
public:
  TestObject() : TimeSeries(1, 1) {}
  virtual ~TestObject() {}
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    // Test basic functionnalities
    // TODO: requires IndicesCollection::operator==
    // checkClassWithClassName<TestObject>();

    const UnsignedInteger DIM = 2;

    // We create an empty TimeSeries
    TimeSeries ts1(0, DIM);
    ts1.setName("Ts1");

    // We populate the empty ts
    for (UnsignedInteger p = 0; p < 3; ++p)
    {
      Point pt(DIM);
      for (UnsignedInteger i = 0; i < DIM; ++i) pt[i] = 10. * (p + 1) + i;
      ts1.add( pt );
    }
    fullprint << "ts1=" << ts1 << std::endl;

    // We get the second element of the ts
    Point secondElement = ts1[1];
    fullprint << "second element=" << secondElement << std::endl;

    // We set the third element to a valid new element
    Point newPoint(DIM + 1);
    for (UnsignedInteger i = 0; i < DIM; ++i) newPoint[i + 1] = 1000. * (i + 1);
    ts1[2] = newPoint;
    fullprint << "ts1=" << ts1 << std::endl;

    // We get the values of the second element of the ts
    Point values1 = ts1.getValueAtIndex(1);
    fullprint << "values of the second element=" << values1 << std::endl;

    // We set new values for the second element of the ts
    Point newValues = values1 * 2.;
    ts1.setValueAtIndex(1, newValues);
    Point values2 = ts1.getValueAtIndex(1);
    fullprint << "ts1=" << ts1 << std::endl;
    fullprint << "modified values of the second element=" << values2 << " should be " << newValues << std::endl;

    // We get the values of the second element of the ts
    KDTree tree(ts1.getMesh().getVertices());
    Point values3 = ts1.getValueAtIndex(tree.query( Point(1, -1.0) ));
    fullprint << "values at t=-1.0 =" << values3 << std::endl;
    Point values4 = ts1.getValueAtIndex(tree.query( Point(1, 1.45) ));
    fullprint << "values at t=1.45 =" << values4 << std::endl;
    Point values5 = ts1.getValueAtIndex(tree.query( Point(1, 1.54) ));
    fullprint << "values at t=1.54 =" << values5 << std::endl;
    Point values6 = ts1.getValueAtIndex(tree.query( Point(1, 14.5) ));
    fullprint << "values at t=14.5 =" << values6 << std::endl;

    // We set new values for the third element of the ts
    ts1.setValueAtIndex(tree.query( Point(1, 1.54) ), values6 * -1.0 );
    fullprint << "ts1=" << ts1 << std::endl;

    TimeSeries ts2(10, DIM);
    fullprint << "ts2=" << ts2 << std::endl;


    // try
    // {
    //   // We get the tenth element of the ts
    //   // THIS SHOULD NORMALLY FAIL
    //   Point tenthElement = ts1.at(9);

    //   // Normally, we should never go here
    //   throw TestFailed("Exception has NOT been thrown or caught !");

    // }
    // catch (OutOfBoundException & ex)
    // {
    //   // Nothing to do
    // }

    RegularGrid tg1( 0.0, 0.1, 11 );
    TimeSeries ts3( tg1, DIM );
    fullprint << "ts3=" << ts3 << std::endl;

    RegularGrid tg2( 0.0, 0.2, 6 );
    TimeSeries ts4( tg2, DIM );
    fullprint << "ts4=" << ts4 << std::endl;


    // We append a sample to a time series
    TimeSeries ts5(3, DIM);
    Sample ns1(3, Point(DIM, 99.9));
    fullprint << "ts5=" << ts5 << std::endl;
    ts5.add( ns1 );
    fullprint << "ts5=" << ts5 << std::endl;

    // We retrieve the values of the time series as a sample
    Sample ns2 = ts5.getValues();
    fullprint << "ns2=" << ns2 << std::endl;

    // We build a time series from a time grid and a sample
    Sample ns3 = ts4.getValues();
    TimeSeries ts6( tg2, ns3 );
    fullprint << "ts6=" << ts6 << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
