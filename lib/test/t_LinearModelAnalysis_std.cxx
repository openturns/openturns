//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearModelAlgorithm
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

  {
    fullprint << "Fit y ~ 3 - 2 x + 0.05 * sin(x) model using 20 points (sin(x) ~ noise)" << std::endl;
    UnsignedInteger size = 20;
    Sample oneSample(size, 1);
    Sample twoSample(size, 1);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      oneSample[i][0] = 7.0 * sin(-3.5 + (6.5 * i) / (size - 1.0)) + 2.0;
      twoSample[i][0] = -2.0 * oneSample[i][0] + 3.0 + 0.05 * sin(oneSample[i][0]);
    }
    LinearModelAlgorithm test(oneSample, twoSample);
    LinearModelResult result(test.getResult());
    LinearModelAnalysis analysis(result);
    fullprint << analysis.__str__() << std::endl;
    // Compute confidence level (95%) for coefficients estimate
    const Scalar alpha = 0.95;
    const Interval interval(analysis.getCoefficientsConfidenceInterval(alpha));
    fullprint << "Confidence intervals with level=" << alpha << " : "  << interval << std::endl;
  }

  {
    fullprint << std::endl;
    fullprint << std::endl;
    fullprint << std::endl;
    fullprint << "Fit y ~ 1 + 0.1 x + 10 x^2 model using 100 points" << std::endl;
    UnsignedInteger size = 100;
    // Define a linespace from 0 to 10 with size points
    // We use a Box experiment ==> remove 0 & 1 points
    const Box experiment(Indices(1, size - 2));
    Sample X(experiment.generate());
    // X is defined in [0,1]
    const Point scale(1, 10.0);
    X *= scale;
    // Stack X2
    Sample X2(X);
    for (UnsignedInteger i = 0; i < size; ++i)
      X2(i, 0) = X(i, 0) * X(i, 0);
    // Stack
    X.stack(X2);
    // Define y = 1 + 0.1 * x + 10  x^2 + e with e a gaussian noise
    Sample Y(size, 1);
    for (UnsignedInteger i = 0; i < size; ++i)
      Y(i, 0) = 1.0 +  0.1 * X(i, 0) + 10.0 * X(i, 0) * X(i, 0) + 0.1 * DistFunc::rNormal() ;
    LinearModelAlgorithm test(X, Y);
    LinearModelResult result(test.getResult());
    LinearModelAnalysis analysis(result);
    fullprint << analysis.__str__() << std::endl;
    // Compute confidence level (95%) for coefficients estimate
    const Scalar alpha = 0.95;
    // Finally define the interval
    const Interval interval(analysis.getCoefficientsConfidenceInterval(alpha));
    fullprint << "Confidence intervals with level=" << alpha << " : "  << interval << std::endl;
  }

  {
    // Tests for asymptotic distributions

    RandomGenerator::SetSeed(0);
    const UnsignedInteger sample_size = 1000;
    const Scalar true_standard_deviation = 0.1;
    const Point coefficients{3.0, 2.0, -1.0};
    const Description input_variables{"x1", "x2", "a0", "a1", "a2"};
    const Description formula(1, "a0 + a1 * x1 + a2 * x2");
    const SymbolicFunction f(input_variables, formula);
    const ParametricFunction model(f, {2, 3, 4}, coefficients);
    const UnsignedInteger number_of_parameters = coefficients.getDimension();
    const UnsignedInteger input_dimension = number_of_parameters - 1;
    const Normal distribution(input_dimension);
    const Normal error_distribution(0.0, true_standard_deviation);
    const Sample input_sample(distribution.getSample(sample_size));
    const Sample output_sample(model(input_sample));
    const Sample error_sample(error_distribution.getSample(sample_size));
    const Sample noisy_output_sample(output_sample + error_sample);
    LinearModelAlgorithm algo(input_sample, noisy_output_sample);
    const LinearModelResult result(algo.getResult());
    const LinearModelAnalysis analysis(result);

    // Asymptotic coefficients distribution
    const Normal coefficients_distribution(analysis.getCoefficientsDistribution());
    assert_equal(coefficients_distribution.getDimension(), (UnsignedInteger)3);
    const Point computed_parameters0(coefficients_distribution.getMarginal(0).getParameter());
    const Point computed_parameters1(coefficients_distribution.getMarginal(1).getParameter());
    const Point computed_parameters2(coefficients_distribution.getMarginal(2).getParameter());
    const Point expected_parameters0{3.0, 0.00316346};
    const Point expected_parameters1{2.0, 0.00320948};
    const Point expected_parameters2{-1.0, 0.00316354};
    Scalar atol = 2.0e-1 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters0, expected_parameters0, 0.0, atol);
    atol = 2.0e-3 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters1, expected_parameters1, 0.0, atol);
    atol = 7.0e-2 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters2, expected_parameters2, 0.0, atol);

    // Asymptotic variance distribution (Gaussian noise)
    Distribution variance_distribution(analysis.getVarianceDistribution(true));
    assert_equal(variance_distribution.getImplementation()->getClassName(), std::string("Gamma"));
    Point computed_parameters(variance_distribution.getParameter());
    Point expected_parameters{498.5, 49850.0, 0.0};
    atol = 5.0e4 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol);

    // Asymptotic variance distribution (arbitrary noise)
    variance_distribution = analysis.getVarianceDistribution(false);
    assert_equal(variance_distribution.getImplementation()->getClassName(), std::string("Normal"));
    computed_parameters = variance_distribution.getParameter();
    expected_parameters = {0.01, 0.000447886};
    atol = 9.0e-3 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol);

    // Asymptotic prediction distribution
    Point x0{1.5, 2.5};
    const Normal prediction_distribution(analysis.getPredictionDistribution(x0));
    computed_parameters = prediction_distribution.getParameter();
    expected_parameters = {3.5, 0.00978034};
    atol = 4.0e-1 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol);

    // Asymptotic observation distribution
    x0 = {1.5, 2.5};
    const Normal observation_distribution(analysis.getOutputObservationDistribution(x0));
    computed_parameters = observation_distribution.getParameter();
    expected_parameters = {3.5, 0.100477};
    atol = 4.0e-1 / std::sqrt(sample_size);
    assert_almost_equal(computed_parameters, expected_parameters, 0.0, atol);
  }

  return ExitCode::Success;

}
