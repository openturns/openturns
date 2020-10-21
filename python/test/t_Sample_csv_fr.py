#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import *
import os

ot.TESTPREAMBLE()

# "Le French way"
fname = 'sample_fr.csv'
with open(fname, 'w') as f:
    f.write(
        "X1;X2;X3;X4\n-1,2;2,3;3,4;-4,5\n5,6;-6,7;7,8;8,9\n-0,1;3,2;5,1;7,5\n")
aSample = ot.Sample.ImportFromTextFile(fname, ';', 0, ',')
print('aSample from fr_FR.utf-8 file=', repr(aSample))

os.remove(fname)
