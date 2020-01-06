//                                               -*- C++ -*-
/**
 *  @brief The test file of class ComparisonOperator for standard methods
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

  try
  {

    //     Pointer<ComparisonOperator> p_operator;

    //     p_operator.reset(new Less);
    //     fullprint << "Less.compare(10,20) = " << p_operator->compare(10,20) << std::endl;
    //     fullprint << "Less.compare(20,20) = " << p_operator->compare(20,20) << std::endl;
    //     fullprint << "Less.compare(30,20) = " << p_operator->compare(30,20) << std::endl;

    //     p_operator.reset(new LessOrEqual);
    //     fullprint << "LessOrEqual.compare(10,20) = " << p_operator->compare(10,20) << std::endl;
    //     fullprint << "LessOrEqual.compare(20,20) = " << p_operator->compare(20,20) << std::endl;
    //     fullprint << "LessOrEqual.compare(30,20) = " << p_operator->compare(30,20) << std::endl;

    //     p_operator.reset(new Equal);
    //     fullprint << "Equal.compare(10,20) = " << p_operator->compare(10,20) << std::endl;
    //     fullprint << "Equal.compare(20,20) = " << p_operator->compare(20,20) << std::endl;
    //     fullprint << "Equal.compare(30,20) = " << p_operator->compare(30,20) << std::endl;

    //     p_operator.reset(new GreaterOrEqual);
    //     fullprint << "GreaterOrEqual.compare(10,20) = " << p_operator->compare(10,20) << std::endl;
    //     fullprint << "GreaterOrEqual.compare(20,20) = " << p_operator->compare(20,20) << std::endl;
    //     fullprint << "GreaterOrEqual.compare(30,20) = " << p_operator->compare(30,20) << std::endl;

    //     p_operator.reset(new Greater);
    //     fullprint << "Greater.compare(10,20) = " << p_operator->compare(10,20) << std::endl;
    //     fullprint << "Greater.compare(20,20) = " << p_operator->compare(20,20) << std::endl;
    //     fullprint << "Greater.compare(30,20) = " << p_operator->compare(30,20) << std::endl;

    ComparisonOperator comparisonOperator;
    ComparisonOperator less = Less();
    fullprint << "Less(10,20) = " << less(10, 20) << std::endl;
    fullprint << "Less(20,20) = " << less(20, 20) << std::endl;
    fullprint << "Less(30,20) = " << less(30, 20) << std::endl;

    ComparisonOperator lessOrEqual = LessOrEqual();
    fullprint << "LessOrEqual(10,20) = " << lessOrEqual(10, 20) << std::endl;
    fullprint << "LessOrEqual(20,20) = " << lessOrEqual(20, 20) << std::endl;
    fullprint << "LessOrEqual(30,20) = " << lessOrEqual(30, 20) << std::endl;

    ComparisonOperator equal = Equal();
    fullprint << "Equal(10,20) = " << equal(10, 20) << std::endl;
    fullprint << "Equal(20,20) = " << equal(20, 20) << std::endl;
    fullprint << "Equal(30,20) = " << equal(30, 20) << std::endl;

    ComparisonOperator greaterOrEqual = GreaterOrEqual();
    fullprint << "GreaterOrEqual(10,20) = " << greaterOrEqual(10, 20) << std::endl;
    fullprint << "GreaterOrEqual(20,20) = " << greaterOrEqual(20, 20) << std::endl;
    fullprint << "GreaterOrEqual(30,20) = " << greaterOrEqual(30, 20) << std::endl;

    ComparisonOperator greater = Greater();
    fullprint << "Greater(10,20) = " << greater(10, 20) << std::endl;
    fullprint << "Greater(20,20) = " << greater(20, 20) << std::endl;
    fullprint << "Greater(30,20) = " << greater(30, 20) << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
