//                                               -*- C++ -*-
/**
 *  @brief The test file of AnalyticalResult class
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

using namespace OT;
using namespace OT::Test;

String printPoint(const Point & point, const UnsignedInteger digits)
{
  OSS oss;
  oss << "[";
  Scalar eps = pow(0.1, 1.0 * digits);
  for (UnsignedInteger i = 0; i < point.getDimension(); i++)
  {
    oss << std::fixed << std::setprecision(digits) << (i == 0 ? "" : ",") << Bulk<double>((std::abs(point[i]) < eps) ? std::abs(point[i]) : point[i]);
  }
  oss << "]";
  return oss;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /* We create a numerical math function */
    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    SymbolicFunction myFunction(input, Description(1, "-F*L^3/(3*E*I)"));

    UnsignedInteger dim = myFunction.getInputDimension();
    /* We create a normal distribution point of dimension 1 */
    Point mean(dim, 0.0);
    mean[0] = 50.0; // E
    mean[1] =  1.0; // F
    mean[2] = 10.0; // L
    mean[3] =  5.0; // I
    Point sigma(dim, 1.0);
    IdentityMatrix R(dim);
    Normal myDistribution(mean, sigma, R);

    /* We create a 'usual' RandomVector from the Distribution */
    RandomVector vect(myDistribution);

    /* We create a composite random vector */
    CompositeRandomVector output(myFunction, vect);

    /* We create an Event from this RandomVector */
    ThresholdEvent myEvent(output, Less(), -3.0);

    /* We create an AnalyticalResult based on fictive results */
    AnalyticalResult result(sigma, myEvent, false);

    fullprint << "result=" << result << std::endl;

    UnsignedInteger digits = 5;
    fullprint << "standard space design point=" << printPoint(result.getStandardSpaceDesignPoint(), digits) << std::endl;
    fullprint << "physical space design point=" << printPoint(result.getPhysicalSpaceDesignPoint(), digits) << std::endl;
    fullprint << "is standard point origin in failure space? " << (result.getIsStandardPointOriginInFailureSpace() ? "true" : "false") << std::endl;
    fullprint << "importance factors=" << printPoint(result.getImportanceFactors(), digits) << std::endl;
    fullprint << "importance factors (classical)=" << printPoint(result.getImportanceFactors(AnalyticalResult::CLASSICAL), digits) << std::endl;
    fullprint << "importance factors (physical) =" << printPoint(result.getImportanceFactors(AnalyticalResult::PHYSICAL), digits) << std::endl;
    fullprint << "Hasofer reliability index=" << std::setprecision(digits) << result.getHasoferReliabilityIndex() << std::endl;
    fullprint << "graph importance factors=" << result.drawImportanceFactors() << std::endl;
    fullprint << "graph importance factors (classical)=" << result.drawImportanceFactors(AnalyticalResult::CLASSICAL) << std::endl;
    fullprint << "graph sensitivity=" << result.drawHasoferReliabilityIndexSensitivity() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
