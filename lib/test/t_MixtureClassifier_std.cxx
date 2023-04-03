//                                               -*- C++ -*-
/**
 *  @brief The test file of class MixtureClassifier for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    // Create a collection of distribution
    Mixture::DistributionCollection aCollection;
    aCollection.add( Normal(0., 4) );
    aCollection.add( Uniform(5., 7.) );
    aCollection.add( Triangular(7., 8., 9.) );

    // Instantiate one distribution object
    Mixture distribution(aCollection, Point(aCollection.getSize(), 1.0));
    fullprint << "mixture=" << distribution.__str__() << std::endl;
    Classifier classifier(MixtureClassifier(distribution).clone());
    Sample inS;
    inS.add(Point(1, 2.));
    inS.add(Point(1, 4.));
    inS.add(Point(1, 6.));
    inS.add(Point(1, 8.));
    for (UnsignedInteger i = 0; i < inS.getSize(); ++ i)
    {
      fullprint << "inP=" << Point(inS[i]).__str__() << " class=" << classifier.classify(inS[i]) << std::endl;
    }
    fullprint << "classes=" << classifier.classify(inS) << std::endl;

    for (UnsignedInteger i = 0; i < inS.getSize(); ++ i)
    {
      for (UnsignedInteger j = 0; j < aCollection.getSize(); ++ j )
      {
        const Scalar grade = classifier.grade(inS[i], j);
        // TODO JM: remove the check after the use of infs has been thoroughly tested
        fullprint << "inP=" << Point(inS[i]).__str__() << " grade|" << j << "=" << (grade > SpecFunc::LowestScalar ? grade : grade * 2.0) << std::endl;
      }
    }

    for (UnsignedInteger j = 0; j < aCollection.getSize(); ++ j )
    {
      Point grades(classifier.grade(inS, Indices(inS.getSize(), j)));
      for (UnsignedInteger k = 0; k < grades.getDimension(); ++ k)
      {
        // TODO JM: remove the check after the use of infs has been thoroughly tested
        if (grades[k] <= SpecFunc::LowestScalar) grades[k] *= 2.0;
      }
      fullprint << "grades|" << j << "=" << grades.__str__() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
