//                                               -*- C++ -*-
/**
 *  @brief The test file of SORM class
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

    /* We create a NearestPoint algorithm */
    AbdoRackwitz myAbdoRackwitz;
    myAbdoRackwitz.setMaximumIterationNumber(100);
    myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10);
    myAbdoRackwitz.setMaximumRelativeError(1.0e-10);
    myAbdoRackwitz.setMaximumResidualError(1.0e-10);
    myAbdoRackwitz.setMaximumConstraintError(1.0e-10);
    fullprint << "myAbdoRackwitz=" << myAbdoRackwitz << std::endl;

    /* We create a SORM algorithm */
    /* The first parameter is an OptimizationAlgorithm */
    /* The second parameter is an event */
    /* The third parameter is a starting point for the design point research */
    SORM myAlgo(myAbdoRackwitz, myEvent, mean);

    /* Perform the simulation */
    myAlgo.run();

    /* Stream out the result */
    SORMResult result(myAlgo.getResult());
    UnsignedInteger digits = 4;
    fullprint << "Breitung event probability=" << result.getEventProbabilityBreitung() << std::endl;
    fullprint << "Breitung generalized reliability index=" << std::setprecision(digits) << result.getGeneralisedReliabilityIndexBreitung() << std::endl;
    fullprint << "Hohenbichler event probability=" << std::setprecision(digits) << result.getEventProbabilityHohenbichler() << std::endl;
    fullprint << "Hohenbichler generalized reliability index=" << std::setprecision(digits) << result.getGeneralisedReliabilityIndexHohenbichler() << std::endl;
    fullprint << "Tvedt event probability=" << std::setprecision(digits) << result.getEventProbabilityTvedt() << std::endl;
    fullprint << "Tvedt generalized reliability index=" << std::setprecision(digits) << result.getGeneralisedReliabilityIndexTvedt() << std::endl;
    fullprint << "sorted curvatures=" << printPoint(result.getSortedCurvatures(), digits) << std::endl;
    fullprint << "standard space design point=" << printPoint(result.getStandardSpaceDesignPoint(), digits) << std::endl;
    fullprint << "physical space design point=" << printPoint(result.getPhysicalSpaceDesignPoint(), digits) << std::endl;
    fullprint << "is standard point origin in failure space? " << (result.getIsStandardPointOriginInFailureSpace() ? "true" : "false") << std::endl;
    fullprint << "importance factors=" << printPoint(result.getImportanceFactors(), digits) << std::endl;
    fullprint << "importance factors (classical)=" << printPoint(result.getImportanceFactors(AnalyticalResult::CLASSICAL), digits) << std::endl;
    fullprint << "Hasofer reliability index=" << std::setprecision(digits) << result.getHasoferReliabilityIndex() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
