#                                               -*- Python -*-
#
#  @brief A test file for the wrapper code
#
#  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#
#  This script imports the OpenTURNS environment in Python, loads
#  ths wrapper and the wrapped code, and then calls it.
#
from openturns import *

# If you encounter difficulties with wrapper, uncomment the following command and look at the log
#Log.Show( Log.Flags() + Log.WRAPPER )

f = NumericalMathFunction("wcode")
print "f = ", f

inP = NumericalPoint(f.getInputDimension())
for j in range(inP.getDimension()):
    inP[j] = 10. * (j + 1)
print "inP = ", inP

outP = f(inP)
print "outP = ", outP

N = 10
inS = NumericalSample(N, f.getInputDimension())
for i in range(N):
    for j in range(inP.getDimension()):
        inS[i,j] = 1. * i + 10. * (j + 1)
print "inS = ", inS

outS = f(inS)
print "outS = ", outS


