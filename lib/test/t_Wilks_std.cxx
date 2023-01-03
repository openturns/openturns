//                                               -*- C++ -*-
/**
 *  @brief The test file of class Wilks for standard methods
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
  setRandomGenerator();

  try
  {
    Distribution distribution(Normal(0.0, 1.0));
    std::cout << "distribution=" << distribution << std::endl;
    RandomVector vector(distribution);
    std::cout << "vector=" << vector << std::endl;
    Wilks algo(vector);
    Point alpha(2);
    alpha[0] = 0.9;
    alpha[1] = 0.95;
    Point beta(2);
    beta[0] = 0.9;
    beta[1] = 0.95;
    for (UnsignedInteger a = 0; a < 2; a++)
    {
      fullprint << "alpha=" << alpha[a] << std::endl;
      for (UnsignedInteger b = 0; b < 2; b++)
      {
        fullprint << "beta=" << beta[b] << std::endl;
        for (UnsignedInteger i = 0; i < 5; i++)
        {
          // Sample size for various parameters set
          fullprint << "n(" << alpha[a] << ", " << beta[b] << ", " << i << ")=" << Wilks::ComputeSampleSize(alpha[a], beta[b], i) << std::endl;
          // Quantile estimation using Wilks method
          fullprint << "Quantile of level=" << alpha[a] << " with confidence=" << beta[b] << " using upper statistics=" << i << " gives estimate=" << algo.computeQuantileBound(alpha[a], beta[b], i) << std::endl;
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
