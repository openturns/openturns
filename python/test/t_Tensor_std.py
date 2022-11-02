#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER
print("test number zero : default constructor and string converter")

# Default constructor
tensor0 = ot.Tensor()

# String converter
print("tensor0 = ", repr(tensor0))

# TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
print("test number one : constructor with size, operator() and string converter")

# Constructor with size
tensor1 = ot.Tensor(2, 2, 3)

# Check operator() methods
tensor1[0, 0, 0] = 1.0
tensor1[1, 0, 0] = 2.0
tensor1[0, 1, 0] = 3.0
tensor1[1, 1, 0] = 4.0
tensor1[0, 0, 1] = 5.0
tensor1[1, 0, 1] = 6.0
tensor1[0, 1, 1] = 7.0
tensor1[1, 1, 1] = 8.0
tensor1[0, 0, 2] = 9.0
tensor1[1, 0, 2] = 10.0
tensor1[0, 1, 2] = 11.0
tensor1[1, 1, 2] = 12.0

# String converter
print("tensor1 = ", repr(tensor1))
print(
    "values = %.6f" % tensor1[0, 0, 0],
    "  %.6f" % tensor1[1, 0, 0],
    "  %.6f" % tensor1[0, 1, 0],
    "  %.6f" % tensor1[1, 1, 0],
    "  %.6f" % tensor1[0, 0, 1],
    "  %.6f" % tensor1[1, 0, 1],
    "  %.6f" % tensor1[0, 1, 1],
    "  %.6f" % tensor1[1, 1, 1],
    "  %.6f" % tensor1[0, 0, 2],
    "  %.6f" % tensor1[1, 0, 2],
    "  %.6f" % tensor1[0, 1, 2],
    "  %.6f" % tensor1[1, 1, 2],
)

# TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER
print("test number two : copy constructor and string converter")

# Copy constructor
tensor2 = tensor1

# String converter
print("tensor2 = ", repr(tensor2))

# TEST NUMBER THREE : GET DIMENSIONS METHODS
print("test number three : get dimensions methods")

# Get dimension methods
print("tensor1's nbRows = ", tensor1.getNbRows(), end=" ")
print("tensor1's nbColumns = ", tensor1.getNbColumns())
print("tensor1's nbSheets = ", tensor1.getNbSheets())

# TEST NUMBER FOUR : ASSIGNMENT METHOD
print("test number four : assignment method")

# Assignment method
# no sense with python

# TEST NUMBER FIVE : ISEMPTY METHOD
print("test number five : isEmpty method")

# Check method isEmpty
tensor5 = ot.Tensor()
tensor6 = ot.Tensor()
print("tensor1 is empty = ", tensor1.isEmpty())
print("tensor0 is empty = ", tensor0.isEmpty())
print("tensor5 is empty = ", tensor5.isEmpty())
print("tensor6 is empty = ", tensor6.isEmpty())

# TEST NUMBER SIX : GETSHEET AND SETSHEET METHODS
print("tensor1 = ", repr(tensor1))

#  Matrix sheet1(tensor1.getSheet(1))
print("tensor1.getSheet(1) = ", repr(tensor1.getSheet(1)))
sheet2 = ot.Matrix(2, 2)
sheet2[0, 0] = 0.5
sheet2[1, 0] = 0.6
sheet2[0, 1] = 0.7
sheet2[1, 1] = 0.8
print("sheet2 = ", repr(sheet2))
tensor1.setSheet(1, sheet2)
print("tensor1 = ", repr(tensor1))
