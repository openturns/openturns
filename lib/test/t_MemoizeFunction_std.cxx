//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for history mechanism
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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


class FooEvaluation : public EvaluationImplementation
{
public:
  FooEvaluation() : EvaluationImplementation() {}

  FooEvaluation * clone() const override
  {
    return new FooEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 2;
  }
  UnsignedInteger getOutputDimension() const override
  {
    return 3;
  }

  Point operator()(const Point & inP) const override
  {
    const Scalar x0 = inP[0];
    const Scalar x1 = inP[1];
    if (x1 < 0.0)
      throw InvalidArgumentException(HERE) << "x1<0 at " << inP.__str__();
    const Scalar y0 = x0 + x1;
    const Scalar y1 = x0 * x1;
    const Scalar y2 = y0 + y1;
    return {y0, y1, y2};
  }

  Sample operator()(const Sample & inS) const override
  {
    const UnsignedInteger size = inS.getSize();
    Sample result(size, getOutputDimension());
    Indices okIndices;
    Indices failedIndices;
    Description errors;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      try
      {
        result[i] = operator()(inS[i]);
        okIndices.add(i);
      }
      catch (const Exception & exc)
      {
        errors.add(exc.what());
        failedIndices.add(i);
      }
    }
    if (failedIndices.getSize())
      throw BatchFailedException(HERE, failedIndices, errors, okIndices, result.select(okIndices)) << "operator(Sample) partial fail";
    return result;
  }
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    SymbolicFunction g("x", "x^2");
    MemoizeFunction f(g);
    f.disableHistory();
    fullprint << "default gradient=" << f.getUseDefaultGradientImplementation() << std::endl;
    fullprint << f << std::endl;
    UnsignedInteger size = 4;
    Sample input(size, 1);
    for (UnsignedInteger i = 0; i < size; ++i) input(i, 0) = i;
    Sample output(f(input));
    fullprint << "Is history enabled for f? " << (f.isHistoryEnabled() ? "true" : "false") << std::endl;
    fullprint << "input history=" << f.getInputHistory() << std::endl;
    fullprint << "output history=" << f.getOutputHistory() << std::endl;
    f.enableHistory();
    output = f(input);
    fullprint << "Is history enabled for f? " << (f.isHistoryEnabled() ? "true" : "false") << std::endl;
    fullprint << "input history=" << f.getInputHistory() << std::endl;
    fullprint << "output history=" << f.getOutputHistory() << std::endl;
    f.clearHistory();
    fullprint << "Is history enabled for f? " << (f.isHistoryEnabled() ? "true" : "false") << std::endl;
    fullprint << "input history=" << f.getInputHistory() << std::endl;
    fullprint << "output history=" << f.getOutputHistory() << std::endl;
    // Perform the computation twice
    output = f(input);
    output = f(input);
    fullprint << "input history=" << f.getInputHistory() << std::endl;
    fullprint << "output history=" << f.getOutputHistory() << std::endl;
    // Marginal
    Description inputVariables;
    inputVariables.add("x");
    Description formulas;
    formulas.add("x");
    formulas.add("x^2");
    formulas.add("x^3");
    formulas.add("x^4");
    formulas.add("x^5");
    SymbolicFunction multi(inputVariables, formulas);
    MemoizeFunction memoMulti(multi);
    Sample output5(memoMulti(input));
    Indices indices;
    indices.add(3);
    indices.add(1);
    Function marginal(memoMulti.getMarginal(indices));
    fullprint << "memoized marginal=" << marginal << std::endl;
    Function g2(new SymbolicEvaluation(Description(1, "x"), Description(1, "y"), Description(1, "x^3")));
    MemoizeFunction f2(g2);
    fullprint << "default gradient=" << f2.getUseDefaultGradientImplementation() << std::endl;

    // test Batch exception
    const Function g3(new FooEvaluation);
    const MemoizeFunction f3(g3);
    Sample X(Normal(2).getSample(10));
    X.add(X.select(Indices({0, 1, 3, 8, 9})));
    std::cout << X << std::endl;
    try
    {
      f3(X);
    }
    catch (const BatchFailedException & exc)
    {
      std::cout << "i_fail=" << exc.getFailedIndices() << std::endl;
      std::cout << "X_fail=" << X.select(exc.getFailedIndices()) << std::endl;
      for (UnsignedInteger i = 0; i < exc.getFailedIndices().getSize(); ++ i)
        std::cout << "i_fail=" << exc.getFailedIndices()[i] << " error=" << exc.getErrorDescription()[i] << std::endl;
      std::cout << "i_ok=" << exc.getSucceededIndices() << std::endl;
      std::cout << "X_ok=" << X.select(exc.getSucceededIndices()) << std::endl;
      std::cout << "Y_ok=" << exc.getOutputSample() << std::endl;
      std::cout << "f(X_ok)=" << f3(X.select(exc.getSucceededIndices())) << std::endl;
      std::cout << "what=" << exc.what() << std::endl;
    }
  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
