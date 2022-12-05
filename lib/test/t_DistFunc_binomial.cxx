//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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
    // binomial + logdBinomial
    const Sample sample1(Sample::ImportFromCSVFile("t_DistFunc_binomial1.csv", ","));
    for (UnsignedInteger i = 0; i < sample1.getSize(); ++ i)
    {
      const UnsignedInteger n = sample1(i, 0);
      const Scalar p = sample1(i, 1);
      const UnsignedInteger k = sample1(i, 2);
      const Scalar ref = sample1(i, 3);
      const Scalar log_ref = sample1(i, 4);
        
      const Scalar val = DistFunc::dBinomial(n, p, k);
      std::cout << "dbinomial(" << n <<", " << p<<", "<<k<<") ref=" << ref << " val=" << val <<std::endl;
      assert_almost_equal(val, ref);

      const Scalar log_val = DistFunc::logdBinomial(n, p, k);
      std::cout << "logdbinomial(" << n <<", " << p<<", "<<k<<") ref=" << log_val << " val=" << log_val <<std::endl;
      assert_almost_equal(log_val, log_ref);
    }

    // rBinomial
    const Sample sample2(Sample::ImportFromCSVFile("t_DistFunc_binomial2.csv", ","));
    for (UnsignedInteger i = 0; i < sample2.getSize(); ++ i)
    {
      const UnsignedInteger n = sample2(i, 0);
      const Scalar p = sample2(i, 1);
      const UnsignedInteger iR = sample2(i, 2);
      const Scalar ref = sample2(i, 3);

      const Scalar val = DistFunc::rBinomial(n, p);
      std::cout << "rBinomial(" << n <<", " << p<<") iR="<<iR<<" ref=" << ref << " val=" << val <<std::endl;
      assert_almost_equal(val, ref);
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
