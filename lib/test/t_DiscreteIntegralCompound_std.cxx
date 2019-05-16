//                                               -*- C++ -*-
/**
 *  @brief The test file of class Geometric for standard methods
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


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    // Instanciate one distribution object
    DiscreteIntegralCompound distribution(Bernoulli(0.5), Poisson(20.0));
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;
    std::cout << "Upper bound : " << distribution.computeIntegerUpperBound() << std::endl;
    
    for (UnsignedInteger i=0; i<distribution.computeIntegerUpperBound(); i++)
    {
	    std::cout << "Probability of "<< i << " = " << distribution.computePDF(Point(1, i)) << std::endl;
    }

    Graph g = distribution.drawPDF(0.0, 20.0);
    Drawable curve = Poisson(10.0).drawPDF(0.0, 20.0).getDrawable(0);
    curve.setColor("green");
    g.add(curve);
    g.draw("~/discrete_integral_compound.png");

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
