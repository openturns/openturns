//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for execution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    /** Instance creation */
    NumericalMathFunction deviation("poutre_sample");

    Description input(4);
    input[0] = "E";
    input[1] = "F";
    input[2] = "L";
    input[3] = "I";
    Description output(1);
    output[0] = "d";
    Description formula(output.getSize());
    formula[0] = "-F*L^3/(3*E*I)";
    NumericalMathFunction deviation_analytical(input, output, formula);

    UnsignedInteger size(10);
    NumericalSample inSample(size, 4);
    for(UnsignedInteger i = 0; i < size; i++)
    {
      NumericalScalar fact(1.0 + NumericalScalar(i) / size);
      inSample[i][0] = 210.e9 * fact;
      inSample[i][1] = 1000.0 * fact;
      inSample[i][2] = 1.5 * fact;
      inSample[i][3] = 2.e-6 * fact;
    }
    inSample[2] = inSample[3];

    NumericalSample outSample1(deviation(inSample));
    NumericalSample outSample2(deviation.getEvaluation()->NumericalMathEvaluationImplementation::operator()(inSample));
    NumericalSample outSample3(deviation_analytical(inSample));
    fullprint << "outSample by sample evaluation=" << outSample1 << std::endl;
    fullprint << "outSample by point evaluation=" << outSample2 << std::endl;
    fullprint << "outSample by analytical function=" << outSample3 << std::endl;

    // Test cache behavior
    deviation.enableCache();
    fullprint << "calls = " << deviation.getEvaluationCallsNumber() << " hits = " << deviation.getCacheHits() << std::endl;
    outSample1 = deviation(inSample);
    fullprint << "deviation =" << outSample1 << std::endl;
    fullprint << "calls = " << deviation.getEvaluationCallsNumber() << " hits = " << deviation.getCacheHits() << std::endl;
    outSample1 = deviation(inSample);
    fullprint << "deviation =" << outSample1 << std::endl;
    fullprint << "calls = " << deviation.getEvaluationCallsNumber() << " hits = " << deviation.getCacheHits() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
