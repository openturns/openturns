//                                               -*- C++ -*-
/**
 *  @brief The test file of class SymmetricTensor for standard methods
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    /** TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number zero : default constructor and string converter" << std::endl;

    /* Default constructor */
    SymmetricTensor symtensor0;

    /* String converter */
    fullprint << "symtensor0 = " << symtensor0 << std::endl;


    /** TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test number one : constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    SymmetricTensor symtensor1(2, 3);
    symtensor1.setName("symtensor1");

    /* Check operator() methods */
    symtensor1(0, 0, 0) = 1. ;
    symtensor1(1, 0, 0) = 2. ;
    symtensor1(1, 1, 0) = 3. ;
    symtensor1(0, 0, 1) = 4. ;
    symtensor1(1, 0, 1) = 5. ;
    symtensor1(1, 1, 1) = 6. ;
    symtensor1(0, 0, 2) = 7. ;
    symtensor1(1, 0, 2) = 8. ;
    symtensor1(1, 1, 2) = 9. ;

    /* String converter */
    fullprint << "symtensor1 = " << symtensor1 << std::endl;
    fullprint << "symtensor1 = " << symtensor1.__str__() << std::endl;

    /* Reading all the values */
    fullprint << "symtensor1's values = "
              << symtensor1(0, 0, 0) << "," << symtensor1(0, 1, 0) << ","
              << symtensor1(1, 0, 0) << "," << symtensor1(1, 1, 0) << ","
              << symtensor1(0, 0, 1) << "," << symtensor1(0, 1, 1) << ","
              << symtensor1(1, 0, 1) << "," << symtensor1(1, 1, 1) << ","
              << symtensor1(0, 0, 2) << "," << symtensor1(0, 1, 2) << ","
              << symtensor1(1, 0, 2) << "," << symtensor1(1, 1, 2) << std::endl;

    /** TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number two : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    SymmetricTensor symtensor2(symtensor1);

    /* String converter */
    fullprint << "symtensor2 = " << symtensor2 << std::endl;
    fullprint << "symtensor2 = " << symtensor2.__str__() << std::endl;


    /** TEST NUMBER THREE : GET DIMENSIONS METHODS */
    fullprint << "test number three : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "symtensor1's nbRows = " << symtensor1.getNbRows() << std::endl
              << "symtensor1's nbColumns = " << symtensor1.getNbColumns() << std::endl
              << "symtensor1's nbSheets = " << symtensor1.getNbSheets() << std::endl;


    /** TEST NUMBER FOUR : ASSIGNMENT METHOD */
    fullprint << "test number four : assignment method" << std::endl;

    /* Assignment method */
    SymmetricTensor symtensor3 ;
    symtensor3 = symtensor1 ;
    fullprint << "symtensor3 = " << symtensor3 << std::endl;
    fullprint << "symtensor3 = " << symtensor3.__str__() << std::endl;


    /** TEST NUMBER FIVE : ISEMPTY METHOD */
    fullprint << "test number five : isEmpty method" << std::endl;

    /* Check method isEmpty */
    SymmetricTensor symtensor5;
    SymmetricTensor symtensor6;
    fullprint << "symtensor1 is empty = " << symtensor1.isEmpty() << std::endl
              << "symtensor0 is empty = " << symtensor0.isEmpty() << std::endl
              << "symtensor5 is empty = " << symtensor5.isEmpty() << std::endl
              << "symtensor6 is empty = " << symtensor6.isEmpty() << std::endl;

    /** TEST NUMBER SIX : GETSHEET AND SETSHEET METHODS */
    fullprint << "symtensor1 = " << symtensor1 << std::endl;
    SymmetricMatrix sheet1 = symtensor1.getSheet(1);
    fullprint << "symtensor1.getSheet(1) = " << sheet1 << std::endl;
    SymmetricMatrix sheet2(2);
    sheet2(0, 0) = 0.4;
    sheet2(1, 0) = 0.5;
    sheet2(1, 1) = 0.6;
    fullprint << "sheet2 = " << sheet2 << std::endl;
    symtensor1.setSheet(1, sheet2);
    fullprint << "symtensor1 = " << symtensor1 << std::endl;
    fullprint << "symtensor1 = " << symtensor1.__str__() << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }

  return ExitCode::Success;
}
