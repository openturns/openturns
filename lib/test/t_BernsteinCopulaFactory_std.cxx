//                                               -*- C++ -*-
/**
 *  @brief The test file of class BernsteinCopulaFactory for standard methods
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
    Collection<Distribution> coll;
    coll.add(GumbelCopula(3.0));
    coll.add(ClaytonCopula(3.0));
    coll.add(FrankCopula(3.0));
    Point point(2);
    UnsignedInteger size = 100;
    for (UnsignedInteger i = 0; i < coll.getSize(); ++i)
    {
      Distribution ref_copula(coll[i]);
      fullprint << "Reference copula " << ref_copula.__str__() << std::endl;
      Sample sample(ref_copula.getSample(size));
      // Default method: log-likelihood
      {
        UnsignedInteger m = BernsteinCopulaFactory::ComputeLogLikelihoodBinNumber(sample);
        fullprint << "Log-likelihood bin number=" << m << std::endl;
        Distribution est_copula(BernsteinCopulaFactory().build(sample, m));
        Scalar max_error = 0.0;
        for (UnsignedInteger n = 0; n < 11; ++n)
        {
          point[0] = 0.1 * n;
          for (UnsignedInteger p = 0; p < 11; ++p)
          {
            point[1] = 0.1 * p;
            max_error = std::max(max_error, std::abs(ref_copula.computeCDF(point) - est_copula.computeCDF(point)));
          }
        }
        fullprint << "Max. error=" << max_error << std::endl;
      }
      // AMISE method
      {
        UnsignedInteger m = BernsteinCopulaFactory::ComputeAMISEBinNumber(sample);
        fullprint << "AMISE bin number=" << m << std::endl;
        Distribution est_copula(BernsteinCopulaFactory().build(sample, m));
        Scalar max_error = 0.0;
        for (UnsignedInteger n = 0; n < 11; ++n)
        {
          point[0] = 0.1 * n;
          for (UnsignedInteger p = 0; p < 11; ++p)
          {
            point[1] = 0.1 * p;
            max_error = std::max(max_error, std::abs(ref_copula.computeCDF(point) - est_copula.computeCDF(point)));
          }
        }
        fullprint << "Max. error=" << max_error << std::endl;
      }
      // Penalized Csiszar divergence method
      {
        SymbolicFunction f("t", "-log(t)");
        UnsignedInteger m = BernsteinCopulaFactory::ComputePenalizedCsiszarDivergenceBinNumber(sample, f);
        fullprint << "Penalized Csiszar divergence bin number=" << m << std::endl;
        Distribution est_copula(BernsteinCopulaFactory().build(sample, m));
        Scalar max_error = 0.0;
        for (UnsignedInteger n = 0; n < 11; ++n)
        {
          point[0] = 0.1 * n;
          for (UnsignedInteger p = 0; p < 11; ++p)
          {
            point[1] = 0.1 * p;
            max_error = std::max(max_error, std::abs(ref_copula.computeCDF(point) - est_copula.computeCDF(point)));
          }
        }
        fullprint << "Max. error=" << max_error << std::endl;
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
