#! /usr/bin/env python

from __future__ import print_function
from openturns import *

  {
    // First, build two functions from R^3->R
    Description inVar(3);
    inVar[0] = "x1";
    inVar[1] = "x2";
    inVar[2] = "x3";
    Description outVar(1);
    outVar[0] = "y";
    Description formula(1);
    formula[0] = "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)";
    LinearCombinationEvaluationImplementation::NumericalMathFunctionCollection functions(2);
    functions[0] = NumericalMathFunction(inVar, outVar, formula);
    formula[0] = "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)";
    functions[1] = NumericalMathFunction(inVar, outVar, formula);
    // Second, build the weights
    NumericalPoint coefficients(2);
    coefficients[0] = 0.3;
    coefficients[1] = 2.9;
    LinearCombinationEvaluationImplementation myEvaluation(functions, coefficients);
    NumericalPoint inPoint(3);
    inPoint[0] = 1.2;
    inPoint[1] = 2.3;
    inPoint[2] = 3.4;
    fullprint << "myEvaluation=" << myEvaluation << std::endl;
    fullprint << "Value at " << inPoint << "=" << myEvaluation(inPoint) << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
