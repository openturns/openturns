//                                               -*- C++ -*-
/**
 *  @brief The test file of class WhittleFactory
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
    ResourceMap::SetAsUnsignedInteger("WhittleFactory-DefaultMaxFun", 2000);
    /* Reduce the precision output as the estimation is based on a lazy optimizer */
    PlatformInfo::SetNumericalPrecision(4);

    /* ARMA(p, q) */
    const UnsignedInteger p = 1;
    const UnsignedInteger q = 2;

    /* ARMACoefficients initializing */
    Point arCoefficients(p, 0.80);
    Point maCoefficients(q, 0.50);

    /* ARMA creation */
    ARMA myARMA(ARMACoefficients(arCoefficients), ARMACoefficients(maCoefficients), WhiteNoise(Normal(0.0, 0.05)));
    myARMA.setTimeGrid(RegularGrid(0.0, 0.1, 256));
    fullprint << "myARMA process = " << myARMA.__str__() << std::endl;

    /* Create a realization */
    TimeSeries timeSeries(myARMA.getRealization());

    /* Create a sample */
    ProcessSample sample(myARMA.getSample(100));

    /* First, build an ARMA based on a given order using the WhittleFactory */
    {
      WhittleFactory factory(p, q);
      // factory.setVerbose(true);
      fullprint << "factory=" << factory << std::endl;
      Point informationCriteria;
      Process result(factory.buildWithCriteria(timeSeries, informationCriteria));
      // Commented due to a bug in the cobyla algorithm
      //fullprint << "Estimated ARMA=" << result.__str__() << std::endl;
      //fullprint << "Information criteria=" << informationCriteria.__str__() << std::endl;
      Process result2(factory.buildWithCriteria(sample, informationCriteria));
      // Commented due to a bug in the cobyla algorithm
      //fullprint << "Estimated ARMA=" << result2.__str__() << std::endl;
      //fullprint << "Information criteria=" << informationCriteria.__str__() << std::endl;
    }

    /* Second, build the best ARMA based on a given range of order using the WhittleFactory */
    Indices pIndices(p + 1);
    pIndices.fill();
    Indices qIndices(q + 1);
    qIndices.fill();
    WhittleFactory factory(pIndices, qIndices);
    // factory.setVerbose(true);
    fullprint << "factory=" << factory << std::endl;
    Point informationCriteria;
    Process result(factory.buildWithCriteria(timeSeries, informationCriteria));
    // Commented due to a bug in the cobyla algorithm
    //fullprint << "Estimated ARMA=" << result.__str__() << std::endl;
    //fullprint << "Information criteria=" << informationCriteria.__str__() << std::endl;
    //fullprint << "History=" << factory.getHistory().__str__()  << std::endl;
    Process result2(factory.buildWithCriteria(sample, informationCriteria));
    // Commented due to a bug in the cobyla algorithm
    //fullprint << "Estimated ARMA=" << result2.__str__() << std::endl;
    //fullprint << "Information criteria=" << informationCriteria.__str__() << std::endl;
    //fullprint << "History=" << factory.getHistory().__str__()  << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
