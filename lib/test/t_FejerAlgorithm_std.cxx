//                                               -*- C++ -*-
/**
 *  @brief The test file of class FejerAlgorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>

using namespace OT;
using namespace OT::Test;

typedef Collection<Complex> ComplexCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {

    {
      const SymbolicFunction f("x", "sin(x)");
      const Scalar a = -2.5;
      const Scalar b = 4.5;
      const Interval interval(a, b);

      Collection<FejerAlgorithm> algos(0);
      algos.add(FejerAlgorithm(Indices(1, 20), FejerAlgorithm::FEJERTYPE1) );
      algos.add(FejerAlgorithm(Indices(1, 20), FejerAlgorithm::FEJERTYPE2));
      algos.add(FejerAlgorithm(Indices(1, 20), FejerAlgorithm::CLENSHAWCURTIS));

      FejerAlgorithm algo;
      Sample adaptedNodes;
      Scalar value;
      const Scalar referenceValue = cos(a) - cos(b);
      for (UnsignedInteger testCase = 0; testCase < algos.getSize(); ++ testCase)
      {
        algo = algos[testCase];
        value = algo.integrate(f, interval)[0];
        assert_almost_equal(value, referenceValue, 1e-10, 1e-10);
        // Low-level interface
        value = algo.integrateWithNodes(f, interval, adaptedNodes)[0];
        assert_almost_equal(value, referenceValue, 1e-10, 1e-10);
      }
    }

    {
      const Description inputVariables = {"x", "y"};
      const Description outputVariables = {"cos(pi_ * x / 2) * sin(pi_ * y)"};
      const SymbolicFunction f(inputVariables, outputVariables);
      const Scalar referenceValue = 8 / (M_PI * M_PI);
      const Point lowerBounds = {-1.0, 0.0};
      const Point upperBounds = {1.0, 1.0};
      const Interval interval(lowerBounds, upperBounds);

      Collection<FejerAlgorithm> algos(0);
      algos.add(FejerAlgorithm(Indices(2, 20), FejerAlgorithm::FEJERTYPE1));
      algos.add(FejerAlgorithm(Indices(2, 20), FejerAlgorithm::FEJERTYPE2));
      algos.add(FejerAlgorithm(Indices(2, 20), FejerAlgorithm::CLENSHAWCURTIS));

      FejerAlgorithm algo;
      Scalar value;
      Sample adaptedNodes;
      for (UnsignedInteger testCase = 0; testCase < algos.getSize(); ++testCase)
      {
        algo = algos[testCase];
        value = algo.integrate(f, interval)[0];
        assert_almost_equal(value, referenceValue, 1e-10, 1e-10);
        // Low-level interface
        value = algo.integrateWithNodes(f, interval, adaptedNodes)[0];
        assert_almost_equal(value, referenceValue, 1e-10, 1e-10);
      }
    }

  {
    const Description inputVariables = {"x", "y"};
    const Description outputVariables = {"cos(pi_ * x / 2) * sin(pi_ * y / 2 + pi_/2 ) / 2"};
    const SymbolicFunction f(inputVariables, outputVariables);
    const Scalar referenceValue = 8 / (M_PI * M_PI);
    const Point lowerBounds = {-1.0, -1.0};
    const Point upperBounds = {1.0, 1.0};
    const Interval interval(lowerBounds, upperBounds);

    Collection<FejerAlgorithm> algos(0);
    algos.add(FejerAlgorithm(Indices(2, 20), FejerAlgorithm::FEJERTYPE1));
    algos.add(FejerAlgorithm(Indices(2, 20), FejerAlgorithm::FEJERTYPE2));
    algos.add(FejerAlgorithm(Indices(2, 20), FejerAlgorithm::CLENSHAWCURTIS));

    FejerAlgorithm algo;
    Sample adaptedNodes;
    Scalar value;
    for (UnsignedInteger testCase = 0; testCase < algos.getSize(); ++testCase)
    {
      algo = algos[testCase];
      value = algo.integrate(f, interval)[0];
      assert_almost_equal(value, referenceValue, 1e-10, 1e-10);
      // Low-level interface
      value = algo.integrateWithNodes(f, interval, adaptedNodes)[0];
      assert_almost_equal(value, referenceValue, 1e-10, 1e-10);
    }
  }
}
catch (TestFailed &ex)
{
  std::cerr << ex << std::endl;
  return ExitCode::Error;
  }


  return ExitCode::Success;
}
