//                                               -*- C++ -*-
/**
 *  @brief The test file of StrongMaximumTest class
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

void printSample(String name, NumericalSample sample)
{
  OStream fullprint(std::cout);

  fullprint << name << "=[" ;
  for (UnsignedInteger counter = 0; counter < sample.getSize(); counter++)
  {
    if (counter != 0)  fullprint << ";";
    fullprint << "[" ;
    NumericalPoint point(sample[counter]);
    for (UnsignedInteger coordinate = 0; coordinate < point.getDimension(); coordinate++)
    {
      if (coordinate != 0)  fullprint << ",";
      fullprint <<  point[coordinate];
    }
    fullprint << "]" ;
  }
  fullprint << "];" << std::endl;
}

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    /* We create a numerical math function */
    Description input(2);
    input[0] = "x1";
    input[1] = "x2";
    NumericalMathFunction myFunction(input, Description(1, "y1"), Description(1, "(x1-0.3)*(x1-0.3)-x2*x2"));

    UnsignedInteger dim = myFunction.getInputDimension();
    /* We create a normal distribution point of dimension 1 */
    NumericalPoint mean(dim, 0.0);
    NumericalPoint sigma(dim, 1.0);
    IdentityMatrix R(dim);
    Normal myDistribution(mean, sigma, R);

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    RandomVector output(myFunction, vect);

    /* We create a StandardEvent from this RandomVector  : RandomVector > 0.0 */
    double seuil(10);
    StandardEvent myStandardEvent(output, Greater(), seuil);

    /* We create the design point */
    NumericalPoint designPoint(dim, 0.0);
    double C(0.3);
    designPoint[0] = - sqrt(seuil) + C;

    /* We create the "second" design point */
    NumericalPoint pseudoDesignPoint(dim, 0.0);
    pseudoDesignPoint[0] = sqrt(seuil) + C;

    NumericalScalar importanceLevel = 0.01;
    NumericalScalar accuracyLevel = 2;
    NumericalScalar confidenceLevel = 0.999999;

    StrongMaximumTest myTest(myStandardEvent, designPoint, importanceLevel, accuracyLevel, confidenceLevel);
    fullprint << "myTest=" << myTest << std::endl;
    myTest.run();
    fullprint << "Beta = " << designPoint.norm() << std::endl;
    fullprint << "Discretised sphere radius = "  << designPoint.norm() * (1 + myTest.getAccuracyLevel() * myTest.getDeltaEpsilon()) << std::endl;
    fullprint << "PointNumber = " << myTest.getPointNumber() << std::endl;
    fullprint << "DesignPointVicinity Angle (rad)= " << acos(myTest.getDesignPointVicinity()) << std::endl;
    fullprint << "DesignPointVicinity Angle (deg)= " << acos(myTest.getDesignPointVicinity()) * 180 / M_PI << std::endl;
    fullprint << "Near Design Point Verifying Event Points Number = " << (myTest.getNearDesignPointVerifyingEventPoints()).getSize() << std::endl;
    fullprint << "Near Design Point Violating Event Points Number = " << (myTest.getNearDesignPointViolatingEventPoints()).getSize() << std::endl;
    fullprint << "Far Design Point Verifying Event Points Number = " << (myTest.getFarDesignPointVerifyingEventPoints()).getSize() << std::endl;
    fullprint << "Far Design Point Violating Event Points Number = " << (myTest.getFarDesignPointViolatingEventPoints()).getSize() << std::endl;
    fullprint << "//////" << std::endl;
    // parameters of the test
    fullprint << "importanceLevel=" << myTest.getImportanceLevel() << std::endl;
    fullprint << "accuracyLevel=" << myTest.getAccuracyLevel() << std::endl;
    fullprint << "confidenceLevel=" << myTest.getConfidenceLevel() << std::endl;

    // design point coordinates
    fullprint << "standardSpaceDesignPoint=[" << myTest.getStandardSpaceDesignPoint()[0] << "," << myTest.getStandardSpaceDesignPoint()[1] << "]" << std::endl;

    // pseudo design point coordinates
    fullprint << "pseudoStandardSpaceDesignPoint=[" << pseudoDesignPoint[0] << "," << pseudoDesignPoint[1] << "]" << std::endl;

    // cout of the coordinates of the points of the 4 samples
    /* NearDesignPointVerifyingEventPoints */
    printSample("NearDesignPointVerifyingEventPointsSample", myTest.getNearDesignPointVerifyingEventPoints());
    /* NearDesignPointViolatingEventPoints */
    printSample("NearDesignPointViolatingEventPoints", myTest.getNearDesignPointViolatingEventPoints());
    /* FarDesignPointVerifyingEventPoints */
    printSample("FarDesignPointVerifyingEventPoints", myTest.getFarDesignPointVerifyingEventPoints());
    /* FarDesignPointViolatingEventPoints */
    printSample("FarDesignPointViolatingEventPoints", myTest.getFarDesignPointViolatingEventPoints());

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
