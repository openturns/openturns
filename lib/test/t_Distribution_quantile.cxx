//                                               -*- C++ -*-
/**
 *  @brief The test file of class Distribution for quantile continuity
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

  try
  {
    Collection< DistributionFactory > factories = DistributionFactory::GetUniVariateFactories();
    for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
    {
      Distribution dist = factories[i].build();
      Scalar q0 = dist.computeQuantile(0.0)[0];
      Scalar qm1 = dist.computeQuantile(-1.0)[0];
      if (q0 != qm1)
        fullprint << dist.getClassName() << " <0" << ", q0=" << q0 << ", qm1=" << qm1 << std::endl;
      Scalar q1 = dist.computeQuantile(1.0)[0];
      Scalar q0p = dist.computeQuantile(SpecFunc::MinScalar)[0];
      if (q0 != q0p)
        fullprint << dist.getClassName() << " 0+" << ", q0=" << q0 << ", q0p=" << q0p << std::endl;
      Scalar q2 = dist.computeQuantile(2.0)[0];
      if (q1 != q2)
        fullprint << dist.getClassName() << " >1" << ", q1=" << q1 << ", q2=" << q2 << std::endl;
      Scalar q1m = dist.computeQuantile(1.0 - SpecFunc::ScalarEpsilon)[0];
      if (q1m != q1)
        fullprint << dist.getClassName() << " 1-" << ", q1m=" << q1m << ", q1=" << q1 << std::endl;
    } // i
  } // try
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
