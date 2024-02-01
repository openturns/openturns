//                                               -*- C++ -*-
/**
 *  @brief The test file of class GaussProductExperiment for standard methods
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
    Collection<Distribution> marginals(0);
    marginals.add(Exponential());
    marginals.add(Triangular(-1.0, -0.5, 1.0));
    ComposedDistribution distribution(marginals);
    Indices marginalSizes = {3, 6};
    // Test the constructor based on marginal degrees
    fullprint << "experiment1=" << GaussProductExperiment(marginalSizes) << std::endl;
    // Test the constructor based on distribution
    fullprint << "experiment2=" << GaussProductExperiment(distribution) << std::endl;
    // Test the constructor based on marginal degrees and distribution
    GaussProductExperiment experiment(distribution, marginalSizes);
    fullprint << "experiment = " << experiment << std::endl;
    Point weights(0);
    Sample sample(experiment.generateWithWeights(weights));
    fullprint << "sample = " << sample << std::endl;
    fullprint << "weights = " << weights << std::endl;
    // Test generate()
    Sample sampleGenerate(experiment.generate());
    fullprint << "sampleGenerate = " << sampleGenerate << std::endl;
    // Test set marginal degrees
    Indices marginalSizesBis = {7, 13};
    experiment.setMarginalSizes(marginalSizesBis);
    fullprint << "experiment = " << experiment << std::endl;
    Point weightsBis(0);
    Sample sampleBis(experiment.generateWithWeights(weightsBis));
    const int sizeBis(sampleBis.getSize());
    const int dimensionBis(sampleBis.getDimension());
    const int weightDimensionBis(weightsBis.getDimension());
    assert_equal(sizeBis, 91);
    assert_equal(dimensionBis, 2);
    assert_equal(weightDimensionBis, 91);
    // Test set size
    GaussProductExperiment experiment1(Normal(), Indices(1, 5));
    experiment1.setSize(47);
    fullprint << "experiment = " << experiment1 << std::endl;
    Point weightsTer(0);
    Sample sampleTer(experiment1.generateWithWeights(weightsTer));
    const int sizeTer(sampleTer.getSize());
    const int dimensionTer(sampleTer.getDimension());
    const int weightDimensionTer(weightsTer.getDimension());
    assert_equal(sizeTer, 47);
    assert_equal(dimensionTer, 1);
    assert_equal(weightDimensionTer, 47);
    // Test set distribution
    Collection<Distribution> marginalsBis(0);
    marginalsBis.add(Normal());
    marginalsBis.add(Uniform());
    ComposedDistribution distributionBis(marginalsBis);
    experiment.setDistribution(distributionBis);
    fullprint << "experiment = " << experiment << std::endl;
    Point weightsQuater(0);
    Sample sampleQuater(experiment.generateWithWeights(weightsQuater));
    const int sizeQuater(sampleQuater.getSize());
    const int dimensionQuater(sampleQuater.getDimension());
    const int weightDimensionQuater(weightsQuater.getDimension());
    assert_equal(sizeQuater, 91);
    assert_equal(dimensionQuater, 2);
    assert_equal(weightDimensionQuater, 91);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
