//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for analytical
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
    ResourceMap::Set("SymbolicParser-Backend", "ExprTk");
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
      Point x(1, 0.4 / 3);

      // acosh only defined for 1 <= x <= pi
      if ( elementaryFunctions[i] == String("acosh") )
      {
        x[0] = 1.4 / 3;
      }

      SymbolicFunction f("x", "2*" + elementaryFunctions[i] + "(3*x)");
      fullprint << "f=" << f.__str__() << std::endl;
      fullprint << "f(" << x[0] << ")=" << std::scientific << std::setprecision(4) << f(x)[0] << std::endl;
      Scalar df = CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), f.getEvaluation()).gradient(x)(0, 0);
      Scalar grad_f = 0.0;
      try
      {
        grad_f = f.gradient(x)(0, 0);
      }
      catch(...)
      {
        fullprint << "finite difference" << std::endl;
        f.setGradient(new CenteredFiniteDifferenceGradient(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon" ), f.getEvaluation()));
        grad_f = f.gradient(x)(0, 0);
      }
      fullprint << "df(" << x[0] << ")=" << std::scientific << std::setprecision(4) << grad_f << std::endl;
      Scalar error = std::abs(grad_f) > 1.0e-5 ? std::abs(df / grad_f - 1.0) : std::abs(df - grad_f);
      if (error > 1e-5) std::cout << "GRADIENT ERROR! error=" << error << ", check " + elementaryFunctions[i] << std::endl;
      Scalar d2f = CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), f.getEvaluation()).hessian(x)(0, 0, 0);
      Scalar hess_f = 0.0;
      try
      {
        hess_f = f.hessian(x)(0, 0, 0);
      }
      catch(...)
      {
        f.setHessian(new CenteredFiniteDifferenceHessian(ResourceMap::GetAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon" ), f.getEvaluation()));
        hess_f = f.hessian(x)(0, 0, 0);
      }
      std::cout << "d2f(" << x[0] << ")=" << std::scientific << std::setprecision(4) << hess_f << std::endl;
      error = (std::abs(hess_f) > 1.0e-5 ? std::abs(d2f / hess_f - 1.0) : std::abs(d2f - hess_f));
      if (error > 1e-4) std::cout << "HESSIAN ERROR! error=" << error << ", check " + elementaryFunctions[i] << std::endl;
    }
    Description inp(2);
    inp[0] = "x0";
    inp[1] = "x1";
    Description form(2);
    form[0] = "x0+x1";
    form[1] = "x0-x1";

    SymbolicFunction nmf(inp, form);
    Function marginal0(nmf.getMarginal(0));
    Function marginal1(nmf.getMarginal(1));
    fullprint << "marginal 0=" << marginal0.__str__() << std::endl;
    fullprint << "marginal 1=" << marginal1.__str__() << std::endl;

    // test sample as input of a function
    Description inputVariables(3);
    inputVariables[0] = "xi1";
    inputVariables[1] = "xi2";
    inputVariables[2] = "xi3";
    Description formula(1);
    formula[0] = (OSS() << "sin(xi1) + 7. * (sin(xi2)) ^ 2 + 0.1 * xi3^4 * sin(xi1)");
    SymbolicFunction model(inputVariables, formula);

    // Create an input distribution to calculate reference values
    Collection<Distribution> marginals(3);
    marginals[0] = Uniform(-M_PI, M_PI);
    marginals[1] = Uniform(-M_PI, M_PI);
    marginals[2] = Uniform(-M_PI, M_PI);
    ComposedDistribution distribution(marginals);
    Sample inputsSample(distribution.getSample(100));

    Point refResultValues(100, 0.);
    for (UnsignedInteger i = 0; i < 100; ++ i)
    {
      refResultValues[i] = model(inputsSample[i])[0];
    }

    Sample resultSample(model(inputsSample));

    fullprint << "First reference value : " << refResultValues[0] << std::endl;
    fullprint << "First result calculated : " << resultSample[0][0] << std::endl;
    // Check constants
    {
      SymbolicFunction f("x", "e_");
      fullprint << f.__str__() << ", e_=" << f(Point(1)) << std::endl;
    }
    {
      SymbolicFunction f("x", "pi_");
      fullprint << f.__str__() << ", pi_=" << f(Point(1)) << std::endl;
    }
    // Check unary operators
    {
      SymbolicFunction f("x", "-x");
      fullprint << f.__str__() << ", f([1])=" << f(Point(1, 1.0)) << std::endl;
    }
    {
      SymbolicFunction f("x", "(x:=2.0)*x");
      fullprint << f.__str__() << ", f([1])=" << f(Point(1, 1.0)) << std::endl;
    }
    // Check binary operators
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x <= y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([1, 1])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([2, 1])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x >= y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([1, 1])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([2, 1])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x != y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([1, 1])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([2, 1])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x == y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([1, 1])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([2, 1])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x > y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([1, 1])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([2, 1])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x < y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([1, 1])=" << f(x) << std::endl;
      x[1] = 1.0;
      fullprint << f.__str__() << ", f([2, 1])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x + y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x - y"));
      Point x(2);
      x[0] = 1.0;
      x[1] = 2.0;
      fullprint << f.__str__() << ", f([1, 2])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x * y"));
      Point x(2);
      x[0] = 2.0;
      x[1] = 3.0;
      fullprint << f.__str__() << ", f([2, 3])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x / y"));
      Point x(2);
      x[0] = 2.0;
      x[1] = 3.0;
      fullprint << f.__str__() << ", f([2, 3])=" << f(x) << std::endl;
    }
    {
      Description input(2);
      input[0] = "x";
      input[1] = "y";
      SymbolicFunction f(input, Description(1, "x ^ y"));
      Point x(2);
      x[0] = 2.0;
      x[1] = 3.0;
      fullprint << f.__str__() << ", f([2, 3])=" << f(x) << std::endl;
    }
    // Check functions with multiple return values
    {
      Description input(4);
      input[0] = "x";
      input[1] = "y";
      input[2] = "z";
      input[3] = "t";
      Description output(3);
      output[0] = "a";
      output[1] = "b";
      output[2] = "c";
      SymbolicFunction f(input, output, "a := t; b := z-y; c := x");
      Point x(4);
      x[0] = 2.0;
      x[1] = 3.0;
      x[2] = 4.0;
      x[3] = 5.0;
      fullprint << f.__str__() << ", f([2, 3, 4, 5])=" << f(x) << std::endl;
    }
    // Check exceptional values
    {
      SymbolicFunction f("x", "sqrt(x)");
      fullprint << "Trying to evaluate f=" << f.__str__() << " at x=-1.0" << std::endl;
      Point x(1, -1.0);
      fullprint << "Result is ";
      try
      {
        fullprint << f(x) << std::endl;
      }
      catch(...)
      {
        fullprint << "undefined" << std::endl;
      }
    }
    // Disable check for exceptional values
    {
      SymbolicFunction f("x", "sqrt(x)");
      f.getEvaluation().getImplementation()->setCheckOutput(false);
      fullprint << "Trying to evaluate f=" << f.__str__() << " at x=-1.0" << std::endl;
      Point x(1, -1.0);
      fullprint << "Result is normal? " << SpecFunc::IsNormal(f(x)[0]) << std::endl;
    }
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
