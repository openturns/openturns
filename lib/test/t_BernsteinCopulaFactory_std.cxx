//                                               -*- C++ -*-
/**
 *  @brief The test file of class BernsteinCopulaFactory for standard methods
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
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();
  try
  {
    Collection<Copula> coll;
    coll.add(GumbelCopula(3.0));
    coll.add(ClaytonCopula(3.0));
    coll.add(FrankCopula(3.0));
    NumericalPoint point(2);
    UnsignedInteger size(1000);
    for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
      {
	Copula ref_copula(coll[i]);
	fullprint << "Reference copula" << ref_copula << std::endl;
	NumericalSample sample(ref_copula.getSample(size));
	Distribution est_copula(BernsteinCopulaFactory().build(sample));
	NumericalScalar max_error(0.0);
	for (UnsignedInteger m = 0; m < 11; ++m)
	  {
	    point[0] = 0.1 * m;
	  for (UnsignedInteger n = 0; n < 11; ++n)
	    {
	      point[1] = 0.1 * n;
	      max_error = std::max(max_error, std::abs(ref_copula.computeCDF(point) - est_copula.computeCDF(point)));
	    }
	  }
	fullprint << "Max. error=" << max_error << std::endl;
      }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
