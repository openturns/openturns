//                                               -*- C++ -*-
/**
 *  @brief Test file fo the correlation coefficients computation
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

typedef Collection<Distribution> DistributionCollection;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  try
  {
    UnsignedInteger dimension = 2;
    UnsignedInteger sampleSize = 100000;

    // we create an analytical function
    Description input(dimension);
    input[0] = "x0";
    input[1] = "x1";
    Description formulas(1, "10+3*x0+x1");
    SymbolicFunction analytical(input, formulas);

    // we create a collection of Normal centered distributions
    DistributionCollection aCollection;
    for(UnsignedInteger i = 0; i < dimension; ++i)
    {
      aCollection.add(Normal());
    }

    // we create one distribution object
    JointDistribution aDistribution(aCollection, IndependentCopula(dimension));

    RandomVector randomVector(aDistribution);
    CompositeRandomVector composite(analytical, randomVector);

    // we create two input samples for the function
    Sample inputSample(randomVector.getSample(sampleSize));
    Sample outputSample(analytical(inputSample));

    // Create the CorrelationAnalysis object
    CorrelationAnalysis corr_analysis(inputSample, outputSample);

    Point squared_src = corr_analysis.computeSquaredSRC();
    assert_almost_equal(squared_src, Point({0.9, 0.1}), 0.0, 1e-2); // theoretical value

    // Squared SRC with normalize
    Point squared_src_normalize(corr_analysis.computeSquaredSRC(true));
    assert_almost_equal(squared_src_normalize, Point({0.9, 0.1}), 0.0, 1e-2); // theoretical value

    Point src(corr_analysis.computeSRC());
    assert_almost_equal(src, Point({0.9486832980505138, 0.31622776601683794}), 0.0, 1e-2); // sqrt of squared_src

    Point srrc(corr_analysis.computeSRRC());
    assert_almost_equal(srrc, Point({0.94, 0.30}), 0.0, 1e-2); // approximate value

    Point pcc(corr_analysis.computePCC());
    assert_almost_equal(pcc, Point({1.0, 1.0}), 1e-5, 0.0); // theoretical value

    Point prcc(corr_analysis.computePRCC());
    assert_almost_equal(prcc, Point({0.99, 0.92}), 0.0, 1e-2); // approximate value

    Point pearson(corr_analysis.computeLinearCorrelation());
    assert_almost_equal(pearson, Point({0.95, 0.31}), 0.0, 1e-2); // approximate value

    Point spearman(corr_analysis.computeSpearmanCorrelation());
    assert_almost_equal(spearman, Point({0.94, 0.30}), 0.0, 1e-2); // approximate value

    Point kendalltau(corr_analysis.computeKendallTau());
    assert_almost_equal(kendalltau, Point({0.79, 0.20}), 0.0, 1e-2);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
