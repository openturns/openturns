#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test number zero : default constructor and string converter")

    # Default constructor
    tensor0 = ComplexTensor()

    # String converter
    print("tensor0 = ", repr(tensor0))

    # TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
    print(
        "test number one : constructor with size, operator() and string converter")

    # Constructor with size
    tensor1 = ComplexTensor(2, 2, 3)

    # Check operator() methods
    tensor1[0, 0, 0] = 1 + 0.1j
    tensor1[1, 0, 0] = 2 + 0.2j
    tensor1[0, 1, 0] = 3 + 0.3j
    tensor1[1, 1, 0] = 4 + 0.4j
    tensor1[0, 0, 1] = 5 + 0.5j
    tensor1[1, 0, 1] = 6 + 0.6j
    tensor1[0, 1, 1] = 7 + 0.7j
    tensor1[1, 1, 1] = 8 + 0.8j
    tensor1[0, 0, 2] = 9 + 0.9j
    tensor1[1, 0, 2] = 10 + 1.0j
    tensor1[0, 1, 2] = 11 + 1.1j
    tensor1[1, 1, 2] = 12 + 1.2j

    # String converter
    print("tensor1 = ", repr(tensor1))
    print("values = (%.6f, %.6fj)" % (tensor1[0, 0, 0].real, tensor1[0, 0, 0].imag), "  (%.6f, %.6fj)" % (tensor1[1, 0, 0].real, tensor1[1, 0, 0].imag), "  (%.6f, %.6fj)" % (tensor1[0, 1, 0].real, tensor1[0, 1, 0].imag), "  (%.6f, %.6fj)" % (tensor1[1, 1, 0].real, tensor1[1, 1, 0].imag), "  (%.6f, %.6fj)" % (tensor1[0, 0, 1].real, tensor1[0, 0, 1].imag), "  (%.6f, %.6fj)" % (tensor1[1, 0, 1].real, tensor1[
          1, 0, 1].imag), "  (%.6f, %.6fj)" % (tensor1[0, 1, 1].real, tensor1[0, 1, 1].imag), "  (%.6f, %.6fj)" % (tensor1[1, 1, 1].real, tensor1[1, 1, 1].imag), "  (%.6f, %.6fj)" % (tensor1[0, 0, 2].real, tensor1[0, 0, 2].imag), "  (%.6f, %.6fj)" % (tensor1[1, 0, 2].real, tensor1[1, 0, 2].imag), "  (%.6f, %.6fj)" % (tensor1[0, 1, 2].real, tensor1[0, 1, 2].imag), "  (%.6f, %.6fj)" % (tensor1[1, 1, 2].real, tensor1[1, 1, 2].imag))

    # TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER
    print("test number two : copy constructor and string converter")

    # Copy constructor
    tensor2 = (tensor1)

    # String converter
    print("tensor2 = ", repr(tensor2))

    # TEST NUMBER THREE : GET DIMENSIONS METHODS
    print("test number three : get dimensions methods")

    # Get dimension methods
    print("tensor1's nbRows = ", tensor1.getNbRows(), end=' ')
    print("tensor1's nbColumns = ", tensor1.getNbColumns())
    print("tensor1's nbSheets = ", tensor1.getNbSheets())

    # TEST NUMBER FOUR : ASSIGNMENT METHOD
    print("test number four : assignment method")

    # TEST NUMBER FIVE : ISEMPTY METHOD
    print("test number five : isEmpty method")

    # Check method isEmpty
    tensor5 = ComplexTensor()
    print("tensor1 is empty = ", tensor1.isEmpty())
    print("tensor0 is empty = ", tensor0.isEmpty())
    print("tensor5 is empty = ", tensor5.isEmpty())

    # TEST NUMBER SIX : GETSHEET AND SETSHEET METHODS
    print("tensor1 = ", repr(tensor1))

    #  ComplexMatrix sheet1(tensor1.getSheet(1))
    print("tensor1.getSheet(1) = ", repr(tensor1.getSheet(1)))
    sheet2 = ComplexMatrix(2, 2)
    sheet2[0, 0] = 0.5 - 0.5j
    sheet2[1, 0] = 0.6 - 0.5j
    sheet2[0, 1] = 0.7 - 0.5j
    sheet2[1, 1] = 0.8 - 0.5j
    print("sheet2 = ", repr(sheet2))
    tensor1.setSheet(1, sheet2)
    print("tensor1 = ", repr(tensor1))

except:
    import sys
    print("t_ComplexTensor_std.py", sys.exc_info()[0], sys.exc_info()[1])
