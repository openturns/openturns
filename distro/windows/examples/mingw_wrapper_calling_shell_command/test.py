#                                               -*- Python -*-
#
#  @file  test_wcode.py
#  @brief A test file for the wrapper code
#
#  Copyright (C) 2005-2013 EDF-EADS-Phimeca
#
#  This library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#  This script imports the OpenTURNS environment in Python, loads
#  ths wrapper and the wrapped code, and then calls it.
#
from openturns import *

# set number of thread manually
#ResourceMap.Set("parallel-threads", "1")
#print "Nb of thread", ResourceMap.Get("parallel-threads")

f = NumericalMathFunction("wcode")
print "f = ", f

inP = NumericalPoint(f.getInputDimension())
for i in range(inP.getDimension()):
    inP[i] = 10. * (i + 1)
print "inP = ", inP

outP = f(inP)
print "outP = ", outP
