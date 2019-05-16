//                                               -*- C++ -*-
/**
 *  @brief The test file of class SquaredNormal
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    SquaredNormal sqn;
    std::cout << "distribution = " << sqn << std::endl;
    std::cout << "range = " << sqn.getRange() << std::endl;
    std::cout << "cdf = " << sqn.computeCDF(1.0) << std::endl;
    std::cout << "pdf = " << sqn.computePDF(1.0) << std::endl;
    std::cout << "samples(5) = " << sqn.getSample(5) << std::endl;
    std::cout << "mean = " << sqn.getMean() << std::endl;
    std::cout << "covariance = " << sqn.getCovariance() << std::endl;
    
    sqn.drawPDF().draw("sqn.png");
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
