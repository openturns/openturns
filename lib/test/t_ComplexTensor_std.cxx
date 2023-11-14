//                                               -*- C++ -*-
/**
 *  @brief The test file of class ComplexTensor for standard methods
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
    ComplexTensor tensor0;

    /* String converter */
    fullprint << "tensor0 = " << tensor0 << std::endl;


    /** TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER */
    fullprint << "test number one : constructor with size, operator() and string converter" << std::endl;

    /* Constructor with size */
    ComplexTensor tensor1(2, 2, 3);
    tensor1.setName("tensor1");

    /* Check operator() methods */
    tensor1(0, 0, 0) = Complex(1., 0.1);
    tensor1(1, 0, 0) = Complex(2., 0.2);
    tensor1(0, 1, 0) = Complex(3., 0.3);
    tensor1(1, 1, 0) = Complex(4., 0.4);
    tensor1(0, 0, 1) = Complex(5., 0.5);
    tensor1(1, 0, 1) = Complex(6., 0.6);
    tensor1(0, 1, 1) = Complex(7., 0.7);
    tensor1(1, 1, 1) = Complex(8., 0.8);
    tensor1(0, 0, 2) = Complex(9., 0.9);
    tensor1(1, 0, 2) = Complex(10., 1.0) ;
    tensor1(0, 1, 2) = Complex(11., 1.1) ;
    tensor1(1, 1, 2) = Complex(12., 1.2) ;

    /* String converter */
    fullprint << "tensor1 = " << tensor1 << std::endl;
    fullprint << "values = " << tensor1(0, 0, 0) << " ; "
              << tensor1(1, 0, 0) << " ; "
              << tensor1(0, 1, 0) << " ; "
              << tensor1(1, 1, 0) << " ; "
              << tensor1(0, 0, 1) << " ; "
              << tensor1(1, 0, 1) << " ; "
              << tensor1(0, 1, 1) << " ; "
              << tensor1(1, 1, 1) << " ; "
              << tensor1(0, 0, 2) << " ; "
              << tensor1(1, 0, 2) << " ; "
              << tensor1(0, 1, 2) << " ; "
              << tensor1(1, 1, 2) << std::endl;


    /** TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
    fullprint << "test number two : copy constructor and string converter" << std::endl;

    /* Copy constructor */
    ComplexTensor tensor2(tensor1);

    /* String converter */
    fullprint << "tensor2 = " << tensor2 << std::endl;


    /** TEST NUMBER THREE : GET DIMENSIONS METHODS */
    fullprint << "test number three : get dimensions methods" << std::endl;

    /* Get dimension methods */
    fullprint << "tensor1's nbRows = " << tensor1.getNbRows() << std::endl
              << "tensor1's nbColumns = " << tensor1.getNbColumns() << std::endl
              << "tensor1's nbSheets = " << tensor1.getNbSheets() << std::endl;



    /** TEST NUMBER FOUR : ASSIGNMENT METHOD */
    fullprint << "test number four : assignment method" << std::endl;

    /* Assignment method */
    ComplexTensor tensor3 ;
    tensor3 = tensor1 ;
    fullprint << "tensor3 = " << tensor3 << std::endl;


    /** TEST NUMBER FIVE : ISEMPTY METHOD */
    fullprint << "test number five : isEmpty method" << std::endl;

    /* Check method isEmpty */
    ComplexTensor tensor5;
    ComplexTensor tensor6;
    fullprint << "tensor1 is empty = " << tensor1.isEmpty() << std::endl
              << "tensor0 is empty = " << tensor0.isEmpty() << std::endl
              << "tensor5 is empty = " << tensor5.isEmpty() << std::endl
              << "tensor6 is empty = " << tensor6.isEmpty() << std::endl;

    /** TEST NUMBER SIX : GETSHEET AND SETSHEET METHODS */
    fullprint << "tensor1 = " << tensor1 << std::endl;
    //  Matrix sheet1(tensor1.getSheet(1));
    fullprint << "tensor1.getSheet(1) = " << tensor1.getSheet(1) << std::endl;
    ComplexMatrix sheet2(2, 2);
    sheet2(0, 0) = 0.5;
    sheet2(1, 0) = 0.6;
    sheet2(0, 1) = 0.7;
    sheet2(1, 1) = 0.8;
    fullprint << "sheet2 = " << sheet2 << std::endl;
    tensor1.setSheet(1, sheet2);
    fullprint << "tensor1 = " << tensor1 << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
