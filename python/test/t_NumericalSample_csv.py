#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from os import getenv

TESTPREAMBLE()

try:
    Log.Show(Log.INFO)

    # 1st sample
    f = open('sample.csv', 'w')
    f.write(
        "X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;5.1;7.5\n")
    f.close()
    aSample = NumericalSample.ImportFromCSVFile("sample.csv", ';')
    aSample.setName("a good sample")
    print("aSample=", repr(aSample))

    aSample = NumericalSample.ImportFromTextFile("sample.csv", ";")
    aSample.setName("a good sample")
    print("aSample=", repr(aSample))

    # 2nd sample
    f = open('sample.csv', 'w')
    f.write(
        "X1,X2,X3,X4\n-1.2,2.3,3.4,-4.5\n5.6,-6.7,7.8,8.9\n-0.1,3.2,5.1,7.5\n")
    f.close()
    aSample = NumericalSample.ImportFromCSVFile("sample.csv", ",")
    aSample.setName("a good coma separated sample")
    print("aSample=", repr(aSample))

    aSample = NumericalSample.ImportFromTextFile("sample.csv", ",")
    aSample.setName("a good coma separated sample")
    print("aSample=", repr(aSample))

    # export
    aSample.exportToCSVFile('sample.csv')
    with open('sample.csv') as f:
        print((f.read()))

    # 3rd sample
    f = open('sample.csv', 'w')
    f.write(
        "X1 X2 X3 X4\n-1.2 2.3 3.4 -4.5\n5.6 -6.7 7.8 8.9\n-0.1 3.2 5.1 7.5\n")
    f.close()

    aSample = NumericalSample.ImportFromTextFile("sample.csv")
    aSample.setName("a good sample with spaces")
    print("aSample=", repr(aSample))

    # 4th sample
    f = open('sample.csv', 'w')
    f.write(
        "-1.2;2.3;3.4;-4.5\n5.6;-xxx;7.8;8.9\n-0.1;3.2;5..1;7.5\n0.9;9.8;8.4;5.4\n")
    f.close()
    aSample = NumericalSample.ImportFromCSVFile("sample.csv", ';')
    aSample.setName("a sample with bad entries")
    print("aSample with bad entries (see log)=", repr(aSample))

    aSample = NumericalSample.ImportFromTextFile("sample.csv", ";")
    aSample.setName("a sample with bad entries")
    print("aSample with bad entries (see log)=", repr(aSample))

    # 5th sample
    f = open('sample.csv', 'w')
    f.write(
        "-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8\n-0.1;3.2;;7.5\n6.5;9.0;7.3;-3.7\n")
    f.close()
    aSample = NumericalSample.ImportFromCSVFile("sample.csv", ';')
    aSample.setName("a sample with missing entries")
    print("aSample with missing entries (see log)=", repr(aSample))

    aSample = NumericalSample.ImportFromTextFile("sample.csv", ";")
    aSample.setName("a sample with missing entries")
    print("aSample with missing entries (see log)=", repr(aSample))

    os.remove('sample.csv')

except:
    import sys
    print("t_NumericalSample_csv.py", sys.exc_info()[0], sys.exc_info()[1])
