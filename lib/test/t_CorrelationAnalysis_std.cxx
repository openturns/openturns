//                                               -*- C++ -*-
/**
 *  @brief Test file fo the correlation coefficients computation
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

    Point src(CorrelationAnalysis::SRC(inputSample, outputSample));
    fullprint << "src=" << src.__str__() << std::endl;

    // Taking into account normalize
    Point src_normalize(CorrelationAnalysis::SRC(inputSample, outputSample, true));
    fullprint << "src with normalize=" << src_normalize.__str__() << std::endl;

    Point signed_src(CorrelationAnalysis::SignedSRC(inputSample, outputSample));
    fullprint << "signed src=" << signed_src.__str__() << std::endl;

    Point srrc(CorrelationAnalysis::SRRC(inputSample, outputSample));
    fullprint << "srrc=" << srrc.__str__() << std::endl;

    // Taking into account normalize
    Point srrc_normalize(CorrelationAnalysis::SRRC(inputSample, outputSample, true));
    fullprint << "srrc with normalize=" << src_normalize.__str__() << std::endl;

    Point pcc(CorrelationAnalysis::PCC(inputSample, outputSample));
    fullprint << "pcc=" << pcc.__str__() << std::endl;

    Point prcc(CorrelationAnalysis::PRCC(inputSample, outputSample));
    fullprint << "prcc=" << prcc.__str__() << std::endl;

    Point pearson(CorrelationAnalysis::PearsonCorrelation(inputSample, outputSample));
    fullprint << "pearson=" << pearson.__str__() << std::endl;

    Point spearman(CorrelationAnalysis::SpearmanCorrelation(inputSample, outputSample));
    fullprint << "spearman=" << spearman.__str__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
