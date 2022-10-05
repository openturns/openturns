//                                               -*- C++ -*-
/**
 *  @brief The test file of the LeastSquaresExpansion class
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
    const Point expectedCoefficientsLinear = {3.5, 1.62542, 0, 0, 0, 0, 0, -0.594723, 0, 0, -1.29064, 0, 0, 0, 0, 1.37242, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1.95229, 0, 0, 0, 0, 0.194929, 0, 0, 0, 0, -1.08975, 0, 0, 0, 0, 0, 0, 0, 0, 0.409177, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1.35741, 0, 0, 0, 0, 0, 0, -0.0126684, 0, 0, 0, 0, 0.164588, 0, 0, 0, 0, 0, 0, 0, 0, -0.324901, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.33939, 0, 0, 0, 0, 0, 0, 0, 0, 0.00046142, 0, 0, 0, 0, -0.0106965, 0, 0, 0, 0, 0, 0, 0, 0, 0.0490707, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0459147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    const Point expectedCoefficientsCondensed = {3.5, 1.62542, -0.594723, -1.29064, 1.37242, -1.95229, 0.194929, -1.08975, 0.409177, 1.35741, -0.0126684, 0.164588, -0.324901, -0.33939, 0.00046142, -0.0106965, 0.0490707, 0.0459147};
    const Indices condensedIndices = {0, 1, 7, 10, 15, 30, 35, 40, 49, 77, 84, 89, 98, 156, 165, 170, 179, 275};
    const Point expectedCoefficientsHyper = {3.5, 1.62542, 0, 0, 0, -0.594723, 0, -1.29064, 0, 0, 0, 0, 0, 0, -1.95229, 0, 0.194929, 0, 0, 0, 0, 0, 0, 1.37242, 0, 0, 1.35741, 0, -0.0126684, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.33939, 0, 0, 0, 0, 0, 0, 0.409177, 0, 0.00046142, 0, 0, 0, -1.08975, 0, 0, 0, 0, 0, 0.0459147, 0, 0, 0, 0, 0, 0, 0};
    // Problem parameters
    UnsignedInteger dimension = 3;
    Scalar a = 7.0;
    Scalar b = 0.1;
    // Create the Ishigami function
    Description inputVariables(dimension);
    inputVariables[0] = "xi1";
    inputVariables[1] = "xi2";
    inputVariables[2] = "xi3";
    Description formula(1);
    formula[0] = (OSS() << "sin(xi1) + (" << a << ") * (sin(xi2)) ^ 2 + (" << b << ") * xi3^4 * sin(xi1)");
    SymbolicFunction model(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(dimension);
    marginals[0] = Uniform(-M_PI, M_PI);
    marginals[1] = Uniform(-M_PI, M_PI);
    marginals[2] = Uniform(-M_PI, M_PI);
    ComposedDistribution distribution(marginals);

    // Create the orthogonal basis
    Collection<OrthogonalUniVariatePolynomialFamily> polynomialCollection(dimension);
    polynomialCollection[0] = LegendreFactory();
    polynomialCollection[1] = LegendreFactory();
    polynomialCollection[2] = LegendreFactory();

    LinearEnumerateFunction enumerateFunction(dimension);
    OrthogonalProductPolynomialFactory productBasis(polynomialCollection, enumerateFunction);

    // Create the doe
    UnsignedInteger degree = 10;
    UnsignedInteger basisSize = enumerateFunction.getBasisSizeFromTotalDegree(degree);
    UnsignedInteger samplingSize = 8192;
    UnsignedInteger marginalSize = degree + 5;

    Collection<WeightedExperiment> doeList;
    RandomGenerator::SetSeed(0);
    doeList.add(LowDiscrepancyExperiment(LowDiscrepancySequence(SobolSequence()), distribution, samplingSize));
    doeList.add(GaussProductExperiment(distribution, Indices(distribution.getDimension(), marginalSize)));
    for (UnsignedInteger i = 0; i < doeList.getSize(); ++i)
    {
      // Sampling
      Point weights;
      Sample inputSample(doeList[i].generateWithWeights(weights));
      const Scalar wMin = *std::min_element(weights.begin(), weights.end());
      const Scalar wMax = *std::max_element(weights.begin(), weights.end());
      Sample outputSample(model(inputSample));
      // Check the full constructors
      Description methodNames = {"SVD", "QR", "Cholesky"};
      for (UnsignedInteger n = 0; n < methodNames.getSize(); ++n)
      {
        {
          // Create the polynomial chaos algorithm using the full constructor
          LeastSquaresExpansion algo(inputSample, weights, outputSample, distribution, productBasis, basisSize, methodNames[n]);
          fullprint << "algo=" << algo << std::endl;
          fullprint << "algo=" << algo.__str__() << std::endl;
          algo.run();
          // Check the coefficients
          FunctionalChaosResult result(algo.getResult());
          Point coeffs(result.getCoefficients().asPoint());
          Point ref(coeffs.getSize());
          std::copy(expectedCoefficientsLinear.begin(), expectedCoefficientsLinear.begin() + coeffs.getSize(), ref.begin());
          Scalar err = (coeffs - ref).norm();
          const Scalar rtol = 5.0e-2;
          const Scalar atol = 5.0e-2;
          assert_almost_equal(err, 0.0, rtol, atol);
          // Check the function restriction
          algo.setActiveFunctions(condensedIndices);
          fullprint << "algo=" << algo.__str__() << std::endl;
          algo.run();
          {
            FunctionalChaosResult result(algo.getResult());
            Point coeffs(result.getCoefficients().asPoint());
            Point ref(coeffs.getSize());
            std::copy(expectedCoefficientsCondensed.begin(), expectedCoefficientsCondensed.begin() + coeffs.getSize(), ref.begin());
            Scalar err = (coeffs - ref).norm();
            const Scalar rtol = 5.0e-2;
            const Scalar atol = 5.0e-2;
            assert_almost_equal(err, 0.0, rtol, atol);
          } // reduced set of functions
        } // full constructor with weights
        if (wMin == wMax)
        {
          // Create the polynomial chaos algorithm using the full constructor
          LeastSquaresExpansion algo(inputSample, outputSample, distribution, productBasis, basisSize, methodNames[n]);
          fullprint << "algo=" << algo << std::endl;
          fullprint << "algo=" << algo.__str__() << std::endl;
          algo.run();
          // Check the coefficients
          FunctionalChaosResult result(algo.getResult());
          Point coeffs(result.getCoefficients().asPoint());
          Point ref(coeffs.getSize());
          std::copy(expectedCoefficientsLinear.begin(), expectedCoefficientsLinear.begin() + coeffs.getSize(), ref.begin());
          Scalar err = (coeffs - ref).norm();
          const Scalar rtol = 1.0e-3;
          const Scalar atol = 1.0e-3;
          assert_almost_equal(err, 0.0, rtol, atol);
        } // full constructor without weights
      } // Method
      {
        // Create the polynomial chaos algorithm using the simplified constructor
        LeastSquaresExpansion algo(inputSample, weights, outputSample, distribution);
        fullprint << "algo=" << algo << std::endl;
        fullprint << "algo=" << algo.__str__() << std::endl;
        algo.run();
        // Check the coefficients
        FunctionalChaosResult result(algo.getResult());
        Point coeffs(result.getCoefficients().asPoint());
        Point ref(coeffs.getSize());
        std::copy(expectedCoefficientsHyper.begin(), expectedCoefficientsHyper.begin() + coeffs.getSize(), ref.begin());
        Scalar err = (coeffs - ref).norm();
        const Scalar rtol = 1.0e-2;
        const Scalar atol = 1.0e-2;
        assert_almost_equal(err, 0.0, rtol, atol);
      } // Simplified with weights
      // Check the constructors assuming uniform weights
      if (wMin == wMax)
      {
        // Create the polynomial chaos algorithm using the simplified constructor
        LeastSquaresExpansion algo(inputSample, outputSample, distribution);
        fullprint << "algo=" << algo << std::endl;
        fullprint << "algo=" << algo.__str__() << std::endl;
        algo.run();
        // Check the coefficients
        FunctionalChaosResult result(algo.getResult());
        Point coeffs(result.getCoefficients().asPoint());
        Point ref(coeffs.getSize());
        std::copy(expectedCoefficientsHyper.begin(), expectedCoefficientsHyper.begin() + coeffs.getSize(), ref.begin());
        Scalar err = (coeffs - ref).norm();
        const Scalar rtol = 1.0e-2;
        const Scalar atol = 1.0e-2;
        assert_almost_equal(err, 0.0, rtol, atol);
      } // Simplified without weights
    } // doe
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
