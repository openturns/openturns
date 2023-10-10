//                                               -*- C++ -*-
/**
 *  @brief The test file of class Distribution for quantile continuity
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

  try
  {
    Collection< DistributionFactory > factories = DistributionFactory::GetUniVariateFactories();
    for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
    {
      // test comparison via interface
      const DistributionFactory factory(factories[i]);
      Distribution distribution = factory.build();
      Distribution distribution2 = factory.build();
      assert_equal(distribution == distribution2, true);
      assert_equal(distribution != distribution2, false);
      const Dirac dirac(5.0);
      assert_equal(distribution == dirac, false);
      assert_equal(distribution != dirac, true);
    }

    // via implementation
    assert_equal(Arcsine() == Arcsine(), true);
    assert_equal(Exponential(2.0) != Exponential(2.3), true);
    assert_equal(Triangular() != Gumbel(), true);
    assert_equal(Gumbel() != Triangular(), true);
    assert_equal(Normal(2) == Normal(2), true);
    assert_equal(Normal(2) != Normal(3), true);
    assert_equal(Normal(2) == ComposedDistribution(Collection<Distribution>(2, Normal())), true);
    assert_equal(Normal(2) != ComposedDistribution(Collection<Distribution>(2, Normal())), false);
    assert_equal(Normal(3) == ComposedDistribution(Collection<Distribution>(2, Normal())), false);
    assert_equal(Normal(3) != ComposedDistribution(Collection<Distribution>(2, Normal())), true);
    assert_equal(ComposedDistribution(Collection<Distribution>(2, Normal())) == Normal(2), true);
    assert_equal(ComposedDistribution(Collection<Distribution>(2, Normal())) != Normal(2), false);
    assert_equal(ComposedDistribution(Collection<Distribution>(2, Normal())) == Normal(3), false);
    assert_equal(ComposedDistribution(Collection<Distribution>(2, Normal())) != Normal(3), true);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  return ExitCode::Success;
}
