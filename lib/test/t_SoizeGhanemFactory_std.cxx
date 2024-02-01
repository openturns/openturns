//                                               -*- C++ -*-
/**
 *  @brief The test file of class HermiteFactory for standard methods
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

struct KernelWrapper
{
  KernelWrapper(const Function & left,
                const Function & right,
                const Distribution & weight)
    : left_(left)
    , right_(right)
    , weight_(weight)
  {};

  Point operator()(const Point & point) const
  {
    return left_(point) * (right_(point)[0] * weight_.computePDF(point));
  };

  const Function left_;
  const Function right_;
  const Distribution weight_;
};

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    Collection<Distribution> marginals(2);
    marginals[0] = Normal();
    marginals[1] = Uniform();
    CorrelationMatrix R(2);
    R(0, 1) = 0.5;
    NormalCopula copula(R);
    Collection<SoizeGhanemFactory> factories(3);
    factories[0] = SoizeGhanemFactory(ComposedDistribution(marginals));
    factories[1] = SoizeGhanemFactory(ComposedDistribution(marginals, copula), false);
    factories[2] = SoizeGhanemFactory(ComposedDistribution(marginals, copula), true);
    Point x(2, 0.5);
    UnsignedInteger kMax = 5;
    ResourceMap::SetAsUnsignedInteger( "IteratedQuadrature-MaximumSubIntervals", 2048 );
    ResourceMap::SetAsScalar( "IteratedQuadrature-MaximumError",    1.0e-6 );
    for (UnsignedInteger i = 0; i < factories.getSize(); ++i)
    {
      SoizeGhanemFactory soize(factories[i]);
      Distribution distribution(soize.getMeasure());
      fullprint << "SoizeGhanem=" << soize << std::endl;
      Collection<Function> functions(kMax);
      for (UnsignedInteger k = 0; k < kMax; ++k)
      {
        functions[k] = soize.build(k);
        fullprint << "SoizeGhanem(" << k << ")=" << functions[k].getEvaluation().__str__() << std::endl;
        fullprint << "SoizeGhanem(" << k << ")(" << x.__str__() << "=" << functions[k](x).__str__() << std::endl;
      }
      SymmetricMatrix M(kMax);
      for (UnsignedInteger m = 0; m < kMax; ++m)
        for (UnsignedInteger n = 0; n <= m; ++n)
        {
          KernelWrapper wrapper(functions[m], functions[n], distribution);
          Function kernel(bindMethod<KernelWrapper, Point, Point>(wrapper, &KernelWrapper::operator(), distribution.getDimension(), 1));
          Scalar value = IteratedQuadrature().integrate(kernel, distribution.getRange())[0];
          M(m, n) = (std::abs(value) < 1e-6 ? 0.0 : value);
        }
      fullprint << "M=\n" << M.__str__() << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
