//                                               -*- C++ -*-
/**
 *  @brief The test file of StrongMaximumTest class
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

    /* dimension n=2  */
    Collection<UnsignedInteger> dimension(3);
    dimension[0] = 2;
    dimension[1] = 5;
    dimension[2] = 10;


    for (UnsignedInteger indexDimension = 0; indexDimension < dimension.getSize(); indexDimension++)
    {
      UnsignedInteger dim = dimension[indexDimension];
      fullprint << "dimension = " << dim << std::endl;

      /* We create a numerical math function */
      Description inputVar(dim);
      for (UnsignedInteger i = 0; i < dim; ++i)
      {
        OSS oss;
        oss << "x" << i + 1;
        inputVar[i] = oss;
      }
      SymbolicFunction myFunction(inputVar, Description(1, "0"));

      /* We create a normal distribution point of dimension 1 */
      Point mean(dim, 0.0);
      Point sigma(dim, 1.0);
      IdentityMatrix R(dim);
      Normal myDistribution(mean, sigma, R);

      /* We create a 'usual' RandomVector from the Distribution */
      RandomVector vect(myDistribution);

      /* We create a composite random vector */
      CompositeRandomVector output(myFunction, vect);

      /* We create a StandardEvent from this RandomVector */
      StandardEvent myStandardEvent(output, Less(), 2.0);

      Normal std(Point(1, 0.0), Point(1, 1.0), IdentityMatrix(1));

      Point beta(3);
      beta[0] = round(-std.computeQuantile(1e-3)[0]);
      beta[1] = round(-std.computeQuantile(1e-5)[0]);
      beta[2] = round(-std.computeQuantile(1e-7)[0]);

      Point importanceLevel(3);
      importanceLevel[0] = 0.01;
      importanceLevel[1] = 0.05;
      importanceLevel[2] = 0.10;

      Point accuracyLevel(3);
      accuracyLevel[0] = 1.5;
      accuracyLevel[1] = 2.0;
      accuracyLevel[2] = 4.0;

      Point confidenceLevel(3);
      confidenceLevel[0] = 0.90;
      confidenceLevel[1] = 0.95;
      confidenceLevel[2] = 0.99;

      Collection<UnsignedInteger> pointNumber(3);
      pointNumber[0] = 10;
      pointNumber[1] = 100;
      pointNumber[2] = 1000;

      /* TABLE 1 : we impose beta, the importance level, the accuracy level, tne confidence level and we calculate the corresponding deltaEpsilon and pointNumber N */
      fullprint << std::right
                << std::setw(10) << "beta "
                << std::setw(16) << "importanceLevel "
                << std::setw(14) << "accuracyLevel "
                << std::setw(16) << "confidenceLevel "
                << std::setw(13) << "deltaEpsilon "
                << std::setw(11) << "pointNumber"
                << std::endl;

      /* loop on beta */
      for (UnsignedInteger indexBeta = 0; indexBeta < beta.getDimension(); indexBeta++)
      {

        /* We create the design point */
        Point designPoint(dim, 0.0);
        designPoint[0] = beta[indexBeta];

        /* loop on the importance level epsilon */
        for (UnsignedInteger indexImportanceLevel = 0; indexImportanceLevel < importanceLevel.getDimension(); indexImportanceLevel++)
        {

          /* loop on the accuracy level tau */
          for (UnsignedInteger indexAccuracyLevel = 0; indexAccuracyLevel < accuracyLevel.getDimension(); indexAccuracyLevel++)
          {

            /* loop on the confidence level (1-q) */
            for (UnsignedInteger indexConfidenceLevel = 0; indexConfidenceLevel < confidenceLevel.getDimension(); indexConfidenceLevel++)
            {
              /* we calculate the corresponding deltaEpsilon and pointNumber N */
              StrongMaximumTest myTest(myStandardEvent, designPoint, importanceLevel[indexImportanceLevel], accuracyLevel[indexAccuracyLevel], confidenceLevel[indexConfidenceLevel]);
              fullprint << std::setprecision(3) //<< std::scientific
                        << std::setw(7)  << beta[indexBeta] << " "
                        << std::setw(15) << importanceLevel[indexImportanceLevel] << " "
                        << std::setw(13) << accuracyLevel[indexAccuracyLevel] <<  " "
                        << std::setw(15) << confidenceLevel[indexConfidenceLevel] << " "
                        << std::setw(12) << myTest.getDeltaEpsilon() << " "
                        << std::setw(11) << std::right << myTest.getPointNumber()
                        << std::endl;
            }

          }
        }
      }


      /* TABLE 2 : we impose beta, the importance level, the accuracy level, the pointNumber N  and we calculate the corresponding deltaEpsilon and confidence level */
      //      fullprint << std::right
      //                << std::setw(10) << "beta "
      //                << std::setw(16) << "importanceLevel "
      //                <<  "accuracyLevel " << "pointNumber " << "deltaEpsilon " << "confidenceLevel" << std::endl;
      fullprint << std::right
                << std::setw(10) << "beta "
                << std::setw(16) << "importanceLevel "
                << std::setw(14) << "accuracyLevel "
                << std::setw(12) << "pointNumber "
                << std::setw(13) << "deltaEpsilon "
                << std::setw(15) << "confidenceLevel"
                << std::endl;

      /* loop on beta */
      for (UnsignedInteger indexBeta = 0; indexBeta < beta.getDimension(); indexBeta++)
      {

        /* We create the design point */
        Point designPoint(dim, 0.0);
        designPoint[0] = beta[indexBeta];

        /* loop on the importance level epsilon */
        for (UnsignedInteger indexImportanceLevel = 0; indexImportanceLevel < importanceLevel.getDimension(); indexImportanceLevel++)
        {

          /* loop on the accuracy level tau */
          for (UnsignedInteger indexAccuracyLevel = 0; indexAccuracyLevel < accuracyLevel.getDimension(); indexAccuracyLevel++)
          {

            /* loop on the pointNumber N */
            for (UnsignedInteger indexPointNumber = 0; indexPointNumber < pointNumber.getSize(); indexPointNumber++)
            {
              /* we calculate the corresponding deltaEpsilon and confidenceLevel */
              StrongMaximumTest myTest(myStandardEvent, designPoint, importanceLevel[indexImportanceLevel], accuracyLevel[indexAccuracyLevel], pointNumber[indexPointNumber]);
              //                      fullprint << std::setprecision(3) << std::scientific << std::right
              //                                << std::setw(7) << beta[indexBeta] << " "
              //                                << importanceLevel[indexImportanceLevel] << " "
              //                                << accuracyLevel[indexAccuracyLevel] <<  "   "
              //                                << std::setw(8)
              //                                << pointNumber[indexPointNumber] <<  "   "
              //                                << myTest.getDeltaEpsilon() <<  "   "
              //                                << myTest.getConfidenceLevel()
              //                                << std::endl;
              fullprint << std::setprecision(3) //<< std::scientific
                        << std::setw(7)  << beta[indexBeta] << " "
                        << std::setw(15) << importanceLevel[indexImportanceLevel] << " "
                        << std::setw(13) << accuracyLevel[indexAccuracyLevel] <<  " "
                        << std::setw(11) << std::right << pointNumber[indexPointNumber] <<  " "
                        << std::setw(12) << myTest.getDeltaEpsilon() << " "
                        << std::setw(15) << myTest.getConfidenceLevel()
                        << std::endl;
            }

          }
        }
      }

    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
