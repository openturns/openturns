//                                               -*- C++ -*-
/**
 *  @brief The test file of class DiscreteCompoundDistribution for standard methods
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
    // Instanciate one distribution object
    DiscreteCompoundDistribution distribution(Bernoulli(0.5), Poisson(20.0));
    fullprint << "Distribution " << distribution << std::endl;
    std::cout << "Distribution " << distribution << std::endl;
    UnsignedInteger upper_bound(distribution.computeIntegerUpperBound());
    std::cout << "Upper bound : " << upper_bound << std::endl;
    
    for (UnsignedInteger i=0; i<upper_bound; i++)
    {
	    fullprint << "Probability of "<< i << " = " << distribution.computePDF(Point(1, i)) << std::endl;
    }

  
    Sample points(upper_bound, 1);

    for(UnsignedInteger i=0;i<upper_bound;i++)
    {
	  points(i,0) = i;
    }

    Sample pdf(distribution.computePDF(points));

    Poisson poisson_distribution(Poisson(10.0));
    Sample poisson_pdf(poisson_distribution.computePDF(points));

    assert_almost_equal(pdf, poisson_pdf, 1e-10, 1e-10);

//    Graph g = distribution.drawPDF(0.0, 20.0);
//    Drawable curve = Poisson(10.0).drawPDF(0.0, 20.0).getDrawable(0);
//    curve.setColor("green");
//    g.add(curve);
//    g.draw("~/discrete_integral_compound.png");

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
