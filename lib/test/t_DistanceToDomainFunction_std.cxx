//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for indicator
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
    // Distance function from an Interval of dimension 0
    const Interval singleton;
    const Point dimZeroPoint(0);
    const Point zero = {0.0};
    const DistanceToDomainFunction singletonDistance(singleton);
    assert_almost_equal(singletonDistance(dimZeroPoint), zero);

    // Distance function from an Interval
    const Point lower = {1.2, 0.7};
    const Point upper = {2.0, 1.0};
    Sample zero_dotone(2, 1);
    zero_dotone(0, 0) = 0.0;
    zero_dotone(1, 0) = 0.1;

    const Interval box(lower, upper);
    const DistanceToDomainFunction distance(box);

    const Point point1 = {1.5, 0.8};
    const Point distance_from_point1 = distance(point1);
    assert_almost_equal(distance_from_point1, zero);

    const Point point2 = {1.1, 0.8};
    const Point distance_from_point2 = distance(point2);
    assert_almost_equal(distance_from_point2, {0.1}, 1e-12);

    Sample sample(2, 2);
    sample[0] = point1;
    sample[1] = point2;
    assert_almost_equal(box.computeDistance(sample), zero_dotone, 1e-12, 0.0);

    const Sample distance_from_sample = distance(sample);
    assert_almost_equal(distance_from_sample, zero_dotone, 1e-12);

    // distance function from a DomainUnion of Intervals
    const Interval interval1(-0.5, 0.0); // [-0.5, 0]
    const Interval interval2(0.7, 1.3); // [0.7, 1.3]
    Interval interval3(0.9, 1.0);
    Interval::BoolCollection finite;
    finite.add(false);
    interval3.setFiniteUpperBound(finite); // [0.9, +infinity)

    const DomainUnion small_union(interval1, interval2); // [-0.5, 0] U [0.7, 1.3]
    const DomainUnion big_union(small_union, interval3); // [-0.5, 0] U [0.7, +infinity)

    Sample sample_dim1(2, 1);
    sample_dim1(0, 0) = 0.1;
    sample_dim1(1, 0) = 1.4;

    Sample dotone_dotone(2, 1);
    dotone_dotone(0, 0) = 0.1;
    dotone_dotone(1, 0) = 0.1;
    assert_almost_equal(small_union.computeDistance(sample_dim1), dotone_dotone, 1e-12, 0.0);

    Sample dotone_zero(2, 1);
    dotone_zero(0, 0) = 0.1;
    dotone_zero(1, 0) = 0.0;
    assert_almost_equal(big_union.computeDistance(sample_dim1), dotone_zero, 1e-12, 0.0);

    const DistanceToDomainFunction distance_to_small_union(small_union);
    assert_almost_equal(distance_to_small_union(sample_dim1), dotone_dotone, 1e-12, 0.0);
    const DistanceToDomainFunction distance_to_big_union(big_union);
    assert_almost_equal(distance_to_big_union(sample_dim1), dotone_zero, 1e-12, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
