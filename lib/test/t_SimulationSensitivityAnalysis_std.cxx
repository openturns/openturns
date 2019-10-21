//                                               -*- C++ -*-
/**
 *  @brief The test file of class SimulationSensitivityAnalysis for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
    /* Uncertain parameters*/
    Normal distribution(Point(3, 1.0), Point(3, 2.0), CorrelationMatrix(3));
    distribution.setName("Unnamed");
    /* Model */
    Description input(3);
    input[0] = "x";
    input[1] = "y";
    input[2] = "z";
    Description formulas(1);
    formulas[0] = "x-1.5*y+2*z";
    SymbolicFunction f(input, formulas);
    /* Sampling */
    UnsignedInteger size = 100;
    Sample inputSample(distribution.getSample(size));
    Sample outputSample(f(inputSample));
    Collection<ComparisonOperator> comparisonOperators(4);
    comparisonOperators[0] = Less();
    comparisonOperators[1] = LessOrEqual();
    comparisonOperators[2] = Greater();
    comparisonOperators[3] = GreaterOrEqual();
    ResourceMap::SetAsUnsignedInteger("SimulationSensitivityAnalysis-DefaultSampleMargin", 10);
    Scalar threshold = 3.0;
    for (UnsignedInteger i = 0; i < 4; ++i)
    {
      {
        SimulationSensitivityAnalysis algo(inputSample, outputSample, distribution.getIsoProbabilisticTransformation(), comparisonOperators[i], threshold);
        fullprint << "algo=" << algo << std::endl;
        /* Perform the analysis */
        fullprint << "Mean point in event domain=" << algo.computeMeanPointInEventDomain() << std::endl;
        fullprint << "Importance factors at " << threshold << " =" << algo.computeImportanceFactors() << std::endl;
        fullprint << "Importance factors at " << threshold / 2 << " =" << algo.computeImportanceFactors(threshold / 2) << std::endl;
        Graph importanceFactorsGraph(algo.drawImportanceFactors());
        fullprint << "importanceFactorsGraph=" << importanceFactorsGraph << std::endl;

        /* Importance factors evolution on probability scale */
        Graph importanceFactorsRangeGraphProbability(algo.drawImportanceFactorsRange());
        fullprint << "importanceFactorsRangeGraphProbability=" << importanceFactorsRangeGraphProbability << std::endl;

        /* Importance factors evolution on threshold scale */
        Graph importanceFactorsRangeGraphThreshold(algo.drawImportanceFactorsRange(false));
        fullprint << "importanceFactorsRangeGraphThreshold=" << importanceFactorsRangeGraphThreshold << std::endl;
      }

      /* Must activate the history mechanism if one want to perform sensitivity analysis */
      MemoizeFunction fh(f);

      /* Analysis based on an event */
      RandomVector X(distribution);
      CompositeRandomVector Y(fh, X);
      ThresholdEvent event(Y, comparisonOperators[i], threshold);
      /* Get a sample of the event to simulate a Monte Carlo analysis. We don't care
         of the result as the interesting values are stored in the model history */
      event.getSample(size);
      {
        SimulationSensitivityAnalysis algo(event);
        fullprint << "algo=" << algo << std::endl;
        /* Perform the analysis */
        fullprint << "Mean point in event domain=" << algo.computeMeanPointInEventDomain() << std::endl;
        fullprint << "Importance factors at threshold " << threshold << " =" << algo.computeImportanceFactors() << std::endl;
        fullprint << "Importance factors at threshold/2 " << threshold / 2 << " =" << algo.computeImportanceFactors(threshold / 2) << std::endl;
        Graph importanceFactorsGraph(algo.drawImportanceFactors());
        fullprint << "importanceFactorsGraph=" << importanceFactorsGraph << std::endl;

        /* Importance factors evolution on probability scale */
        Graph importanceFactorsRangeGraphProbability(algo.drawImportanceFactorsRange());
        fullprint << "importanceFactorsRangeGraphProbability=" << importanceFactorsRangeGraphProbability << std::endl;

        /* Importance factors evolution on threshold scale */
        Graph importanceFactorsRangeGraphThreshold(algo.drawImportanceFactorsRange(false));
        fullprint << "importanceFactorsRangeGraphThreshold=" << importanceFactorsRangeGraphThreshold << std::endl;
      }
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
