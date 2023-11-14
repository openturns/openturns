//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for indicator
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

  try
  {
    // Indicator function of an Interval of dimension 0
    const Interval singleton;
    const Point dimZeroPoint(0);
    const Point one = {1.0};
    const IndicatorFunction singletonIndicator(singleton);
    assert_almost_equal(singletonIndicator(dimZeroPoint), one);

    // Indicator function of an Interval
    const Point lower = {1.2, 0.7};
    const Point upper = {2.0, 1.0};
    const Point zero = {0.0};
    Sample one_zero(2, 1);
    one_zero(0, 0) = 1.0;
    one_zero(1, 0) = 0.0;

    const Interval box(lower, upper);
    const IndicatorFunction indicator(box);

    const Point point1 = {1.5, 0.8};
    const Point indicator_at_point1 = indicator(point1);
    assert_almost_equal(indicator_at_point1, one);

    const Point point2 = {1.1, 0.8};
    const Point indicator_at_point2 = indicator(point2);
    assert_almost_equal(indicator_at_point2, zero);

    Sample sample(2, 2);
    sample[0] = point1;
    sample[1] = point2;
    const Sample indicator_at_sample = indicator(sample);
    assert_almost_equal(indicator_at_sample, one_zero);

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
