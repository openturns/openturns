//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for history mechanism
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
    SymbolicFunction g("x", "x^2");
    MemoizeFunction f(g);
    f.disableHistory();
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
    // marginal is in fact a MemoizeFunction; to access its history, it
    // must be wrapped.
    Sample output2(marginal(input));
    MemoizeFunction memoMarginal(marginal);
    fullprint << "input history=" << memoMarginal.getInputHistory() << std::endl;
    fullprint << "output history=" << memoMarginal.getOutputHistory() << std::endl;
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
