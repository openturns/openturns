//                                               -*- C++ -*-
/**
 *  @brief The test file of class NumericalMathFunction for analytical
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
  setRandomGenerator();

  try
  {
    /* Check the elementary functions */
    Collection<String> elementaryFunctions(0);
    elementaryFunctions.add("sin");
    elementaryFunctions.add("cos");
    elementaryFunctions.add("tan");
    elementaryFunctions.add("asin");
    elementaryFunctions.add("acos");
    elementaryFunctions.add("atan");
    elementaryFunctions.add("sinh");
    elementaryFunctions.add("cosh");
    elementaryFunctions.add("tanh");
    elementaryFunctions.add("asinh");
    elementaryFunctions.add("acosh");
    elementaryFunctions.add("atanh");
    elementaryFunctions.add("log2");
    elementaryFunctions.add("log10");
    elementaryFunctions.add("log");
    elementaryFunctions.add("ln");
    elementaryFunctions.add("lngamma");
    elementaryFunctions.add("gamma");
    elementaryFunctions.add("exp");
    elementaryFunctions.add("erf");
    elementaryFunctions.add("erfc");
    elementaryFunctions.add("sqrt");
    elementaryFunctions.add("cbrt");
    elementaryFunctions.add("besselJ0");
    elementaryFunctions.add("besselJ1");
    elementaryFunctions.add("besselY0");
    elementaryFunctions.add("besselY1");
    elementaryFunctions.add("sign");
    elementaryFunctions.add("rint");
    elementaryFunctions.add("abs");
    /* Check the creation of the elementary functions */
    for (UnsignedInteger i = 0; i < elementaryFunctions.getSize(); ++i)
    {
      NumericalPoint x(1, 0.4 / 3);

      // acosh only defined for 1 <= x <= pi
      if ( elementaryFunctions[i] == String("acosh") )
      {
        x[0] = 1.4 / 3;
      }

      NumericalMathFunction f("x", "2*" + elementaryFunctions[i] + "(3*x)", "y");
      fullprint << "f=" << f.__str__() << std::endl;
      fullprint << "f(" << x[0] << ")=" << std::scientific << std::setprecision(4) << f(x)[0] << std::endl;
      NumericalScalar df(CenteredFiniteDifferenceGradient(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), f.getEvaluation()).gradient(x)(0, 0));
      NumericalScalar grad_f(0.0);
      try
      {
        grad_f = f.gradient(x)(0, 0);
      }
      catch(...)
      {
        fullprint << "finite difference" << std::endl;
        f.setGradient(new CenteredFiniteDifferenceGradient(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), f.getEvaluation()));
        grad_f = f.gradient(x)(0, 0);
      }
      fullprint << "df(" << x[0] << ")=" << std::scientific << std::setprecision(4) << grad_f << std::endl;
      NumericalScalar error(std::abs(grad_f) > 1.0e-5 ? std::abs(df / grad_f - 1.0) : std::abs(df - grad_f));
      if (error > 1e-5) std::cout << "GRADIENT ERROR! error=" << error << ", check " + elementaryFunctions[i] << std::endl;
      NumericalScalar d2f(CenteredFiniteDifferenceHessian(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), f.getEvaluation()).hessian(x)(0, 0, 0));
      NumericalScalar hess_f(0.0);
      try
      {
        hess_f = f.hessian(x)(0, 0, 0);
      }
      catch(...)
      {
        f.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsNumericalScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), f.getEvaluation()));
        hess_f = f.hessian(x)(0, 0, 0);
      }
      std::cout << "d2f(" << x[0] << ")=" << std::scientific << std::setprecision(4) << hess_f << std::endl;
      error = (std::abs(hess_f) > 1.0e-5 ? std::abs(d2f / hess_f - 1.0) : std::abs(d2f - hess_f));
      if (error > 1e-4) std::cout << "HESSIAN ERROR! error=" << error << ", check " + elementaryFunctions[i] << std::endl;
    }
    Description inp(2);
    inp[0] = "x0";
    inp[1] = "x1";
    Description out(2);
    out[0] = "y0";
    out[1] = "y1";
    Description form(2);
    form[0] = "x0+x1";
    form[1] = "x0-x1";

    NumericalMathFunction nmf(inp, out, form);
    NumericalMathFunction marginal0(nmf.getMarginal(0));
    NumericalMathFunction marginal1(nmf.getMarginal(1));
    fullprint << "marginal 0=" << marginal0.__str__() << std::endl;
    fullprint << "marginal 1=" << marginal1.__str__() << std::endl;

    // test sample as input of a function
    Description inputVariables(3);
    inputVariables[0] = "xi1";
    inputVariables[1] = "xi2";
    inputVariables[2] = "xi3";
    Description outputVariables(1);
    outputVariables[0] = "y";
    Description formula(1);
    formula[0] = (OSS() << "sin(xi1) + 7. * (sin(xi2)) ^ 2 + 0.1 * xi3^4 * sin(xi1)");
    NumericalMathFunction model(inputVariables, outputVariables, formula);

    // Create an input distribution to calculate reference values
    Collection<Distribution> marginals(3);
    marginals[0] = Uniform(-M_PI, M_PI);
    marginals[1] = Uniform(-M_PI, M_PI);
    marginals[2] = Uniform(-M_PI, M_PI);
    ComposedDistribution distribution(marginals);
    NumericalSample inputsSample(distribution.getSample(100));

    NumericalPoint refResultValues(100, 0.);
    for (UnsignedInteger i = 0; i < 100; ++ i)
    {
      refResultValues[i] = model(inputsSample[i])[0];
    }

    NumericalSample resultSample(model(inputsSample));

    fullprint << "First reference value : " << refResultValues[0] << std::endl;
    fullprint << "First result calculated : " << resultSample[0][0] << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
