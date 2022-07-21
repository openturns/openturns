//                                               -*- C++ -*-
/**
 *  @brief The test file of distribution-based RandomVector class
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

    /* We create a distribution */
    Point meanPoint(1);
    meanPoint[0] = 1.0;
    Point sigma(1);
    sigma[0] = 1.0;
    CorrelationMatrix R(1);
    R(0, 0) = 1.0;
    Normal distribution(meanPoint, sigma, R);

    const Normal & ref_distribution(distribution);

    fullprint << "distribution = " << ref_distribution << std::endl;



    /* We create a distribution-based RandomVector */
    RandomVector vect = UsualRandomVector(distribution);
    fullprint << "vect=" << vect << std::endl;

    /* Check standard methods of class RandomVector */
    fullprint << "vect dimension=" << vect.getDimension() << std::endl;
    fullprint << "vect realization (first )=" << vect.getRealization() << std::endl;
    fullprint << "vect realization (second)=" << vect.getRealization() << std::endl;
    fullprint << "vect realization (third )=" << vect.getRealization() << std::endl;
    fullprint << "vect sample =" << vect.getSample(5) << std::endl;


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
