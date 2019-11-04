//                                               -*- C++ -*-
/**
 *  @brief The test file of class Temporal Normal Process
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
  setRandomGenerator();

  try
  {

    /* Default dimension parameter to evaluate the model */
    const UnsignedInteger defaultDimension = 1;

    /* Spatial dimension of the model */
    const UnsignedInteger inputDimension = 1;

    /* Amplitude values */
    Point amplitude(defaultDimension, 1.0);
    /* Scale values */
    Point scale(inputDimension, 1.0);

    const Scalar tmin = 0.0;
    const Scalar step = 0.1;
    const UnsignedInteger n = 11;

    RegularGrid myTimeGrid(tmin, step, n);
    const UnsignedInteger size = 25;


    /* Second order model with parameters */
    ExponentialModel myCovModel(scale, amplitude);
    fullprint << "myCovModel=" << myCovModel << std::endl;

    GaussianProcess myProcess1(myCovModel, myTimeGrid);
    fullprint << "myProcess1=" << myProcess1 << std::endl;
    fullprint << "is stationary? " << myProcess1.isStationary() << std::endl;
    myProcess1.setSamplingMethod(0);
    fullprint << "mean over " << size << " realizations=" << myProcess1.getSample(size).computeMean() << std::endl;
    myProcess1.setSamplingMethod(2);
    fullprint << "mean over " << size << " realizations=" << myProcess1.getSample(size).computeMean() << std::endl;

    /* With constant trend */
    TrendTransform trend(SymbolicFunction("t", "4.0"), myTimeGrid);
    GaussianProcess myProcess2(trend, myCovModel, myTimeGrid);
    fullprint << "myProcess2=" << myProcess2 << std::endl;
    fullprint << "is stationary? " << myProcess2.isStationary() << std::endl;
    myProcess1.setSamplingMethod(0);
    fullprint << "mean over " << size << " realizations=" << myProcess1.getSample(size).computeMean() << std::endl;
    myProcess1.setSamplingMethod(2);
    fullprint << "mean over " << size << " realizations=" << myProcess1.getSample(size).computeMean() << std::endl;

    /* With varying trend */
    TrendTransform trend3(SymbolicFunction("t", "sin(t)"), myTimeGrid);
    GaussianProcess myProcess3(trend3, myCovModel, myTimeGrid);
    fullprint << "myProcess3=" << myProcess3 << std::endl;
    fullprint << "is stationary? " << myProcess3.isStationary() << std::endl;
    myProcess1.setSamplingMethod(0);
    fullprint << "mean over " << size << " realizations=" << myProcess1.getSample(size).computeMean() << std::endl;
    myProcess1.setSamplingMethod(2);
    fullprint << "mean over " << size << " realizations=" << myProcess1.getSample(size).computeMean() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
