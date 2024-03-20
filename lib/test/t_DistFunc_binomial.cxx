//                                               -*- C++ -*-
/**
 *  @brief The test file of class DistFunc for standard methods
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
      std::cout << "dbinomial(" << n << ", " << p << ", " << k << ") ref=" << ref << " val=" << val << std::endl;
      assert_almost_equal(val, ref);

      const Scalar log_val = DistFunc::logdBinomial(n, p, k);
      std::cout << "logdbinomial(" << n << ", " << p << ", " << k << ") ref=" << log_val << " val=" << log_val << std::endl;
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
      std::cout << "rBinomial(" << n << ", " << p << ") iR=" << iR << " ref=" << ref << " val=" << val << std::endl;
      assert_almost_equal(val, ref);
    }

    // Corner cases
    const Sample dataset(Sample::ImportFromTextFile("t_binomial_dataset.csv", ",", 13));

    const Scalar precision = 1e-11;
    Binomial binomial;
    for (UnsignedInteger i = 0; i < dataset.getSize(); ++ i)
    {
        const UnsignedInteger N = (UnsignedInteger)dataset(i, 1);
        binomial.setN(N);
        binomial.setP(dataset(i, 2));
        const Scalar x = dataset(i, 0);
        const Scalar pdf = dataset(i, 3);
        const Scalar cdf = dataset(i, 4);
        const Scalar surv = dataset(i, 5);
        std::cout << "i = " << i << " x = " << x << " N = " << N << " p = " << binomial.getP() << std::endl;
        assert_almost_equal(binomial.computePDF(x), pdf, precision, 0.0);
        assert_almost_equal(binomial.computeCDF(x), cdf, precision, 0.0);
        assert_almost_equal(binomial.computeSurvivalFunction(x), surv, precision, 0.0);
        if (i > 0)  // FIXME: test fails for i = 0 (x=0, N=10, P=0)
        {
          #if 0 // FIXME: tests fail for i = 7 (x=400, N=1030, P=0.5)
            assert_almost_equal(binomial.computeQuantile(cdf), Point({x}), 0.0, 1.0); // Can be off by 1 unit
          #endif
        }
    }

    // 2147483647 is the maximum integer.
    // Values greater than this are not doubles anymore.
    const UnsignedInteger N = 2147483647;
    binomial.setN(N);
    binomial.setP(1.0 / N);
    Scalar computed = binomial.computePDF(1.0);
    Scalar expected = 0.3678794;
    assert_almost_equal(computed, expected, 1.0e-6, 0.0);

    computed = binomial.computePDF(2.0);
    expected = 0.1839397;
    assert_almost_equal(computed, expected, 1.0e-6, 0.0);

    // Extreme inputs
    binomial.setN(9999);
    binomial.setP(0.5);
    computed = binomial.computePDF(4999.0);
    expected = 0.0079786461393821558191;
    assert_almost_equal(computed, expected, 1.0e-7, 0.0);

    // Check pdf for values of P closer to 1
    binomial.setN(2);
    binomial.setP(0.9999);
    computed = binomial.computePDF(1.0);
    expected = 1.999799999999779835e-04;
    assert_almost_equal(computed, expected, 1e-12, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
