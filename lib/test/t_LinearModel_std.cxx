//                                               -*- C++ -*-
/**
 *  @brief The test file of class LinearModel
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  setRandomGenerator();

  /** TEST NUMBER ZERO : DEFAULT & COPY CONSTRUCTORs AND STRING CONVERTER */
  fullprint << "test number zero : default & copy constructors and string converter" << std::endl;

  Point vectR(1, 12.0);
  Sample testSample(1, 1);

  Scalar begin = -1.0;
  Scalar end = 36.92;
  Interval testInterval(Point(1, begin), Point(1, end));
  testInterval.setName("testInterval");

  OT::Collection<Scalar> valColl;
  valColl.add(1.0);

  /* Default constructor */
  LinearModel LM(vectR, testInterval, valColl);
  LinearModel LMcopy(LM);

  /* Constructor from Point */
  LinearModel emptyLM(vectR);

  /* String converter */
  fullprint << "LM = " << LM << std::endl;
  fullprint << "LMcopy = " << LMcopy << std::endl;
  fullprint << "emptyLM = " << emptyLM << std::endl;


  /** TEST NUMBER ONE : GET ELEMENTS */
  fullprint << "test number one : get elements" << std::endl;

  fullprint << "LM.getRegression = " << LM.getRegression() << std::endl;
  fullprint << "LM.getConfidenceIntervals = " << LM.getConfidenceIntervals() << std::endl;
  fullprint << "LM.getPValues = " << LM.getPValues() << std::endl;


  /** TEST NUMBER TWO : GET PREDICT/RESIDUAL */
  fullprint << "test number two : get predicted/residual" << std::endl;
  LinearModelFactory lmfact;
  UnsignedInteger size = 20;
  Sample oneSample(size, 1);
  Sample twoSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    oneSample[i][0] = 7.0 * sin(-3.5 + (6.5 * i) / (size - 1.0)) + 2.0;
    twoSample[i][0] = -2.0 * oneSample[i][0] + 3.0 + 0.05 * sin(oneSample[i][0]);
  }
  LinearModel test(lmfact.build(oneSample, twoSample));
  fullprint << "LM.getPredicted = " << test.getPredicted(oneSample) << std::endl;
  fullprint << "LM.getResidual = " << test.getResidual(oneSample, twoSample) << std::endl;


  return ExitCode::Success;
}
