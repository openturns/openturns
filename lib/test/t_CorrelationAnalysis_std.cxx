//                                               -*- C++ -*-
/**
 *  @brief Test file fo the correlation coefficients computation
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
    UnsignedInteger sampleSize = 1000;

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
    ComposedDistribution aDistribution(aCollection, IndependentCopula(dimension));

    RandomVector randomVector(aDistribution);
    CompositeRandomVector composite(analytical, randomVector);

    // we create two input samples for the function
    Sample inputSample(randomVector.getSample(sampleSize));
    Sample outputSample(analytical(inputSample));

    // Create the CorrelationAnalysis object
    CorrelationAnalysis corr_analysis(inputSample, outputSample);

    Point squared_src = corr_analysis.computeSquaredSRC();
    assert_almost_equal(squared_src, Point({0.901238,0.105409}), 1e-5, 0.0);

    // Squared SRC with normalize
    Point squared_src_normalize(corr_analysis.computeSquaredSRC(true));
    assert_almost_equal(squared_src_normalize, Point({0.895287,0.104713}), 1e-5, 0.0);

    Point src(corr_analysis.computeSRC());
    assert_almost_equal(src, Point({0.949336,0.324667}), 1e-5, 0.0);

    Point srrc(corr_analysis.computeSRRC());
    assert_almost_equal(srrc, Point({0.946444,0.304684}), 1e-5, 0.0);

    Point pcc(corr_analysis.computePCC());
    assert_almost_equal(pcc, Point({1.0, 1.0}), 1e-5, 0.0);

    Point prcc(corr_analysis.computePRCC());
    assert_almost_equal(prcc, Point({0.98879,0.905344}), 1e-5, 0.0);

    Point pearson(corr_analysis.computePearsonCorrelation());
    assert_almost_equal(pearson, Point({0.945835,0.314431}), 1e-5, 0.0);

    Point spearman(corr_analysis.computeSpearmanCorrelation());
    assert_almost_equal(spearman, Point({0.941684,0.289898}), 1e-5, 0.0);

    Point kendalltau(corr_analysis.computeKendallTau());
    assert_almost_equal(kendalltau, Point({0.7935375375375376,0.1965765765765766}), 1e-16, 0.0);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
