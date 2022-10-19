#! /usr/bin/env python

import openturns as ot
import os

ot.TESTPREAMBLE()

# 1st sample
with open("sample.csv", "w") as f:
    f.write("X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;5.1;7.5\n")
aSample = ot.Sample.ImportFromCSVFile("sample.csv", ";")
aSample.setName("a good sample")
print("aSample=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile("sample.csv", ";")
aSample.setName("a good sample")
print("aSample=", repr(aSample))

# 2nd sample
with open("sample.csv", "w") as f:
    f.write("X1,X2,X3,X4\n-1.2,2.3,3.4,-4.5\n5.6,-6.7,7.8,8.9\n-0.1,3.2,5.1,7.5\n")
aSample = ot.Sample.ImportFromCSVFile("sample.csv", ",")
aSample.setName("a good coma separated sample")
print("aSample=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile("sample.csv", ",")
aSample.setName("a good coma separated sample")
print("aSample=", repr(aSample))

# export
aSample.exportToCSVFile("sample.csv")
with open("sample.csv") as f:
    print((f.read()))

# import with quotes in description
aSample = ot.Sample.ImportFromTextFile("sample.csv", ";")
print("import with quotes:", repr(aSample))

# 3rd sample
with open("sample.csv", "w") as f:
    f.write("X1 X2 X3 X4\n-1.2 2.3 3.4 -4.5\n5.6 -6.7 7.8 8.9\n-0.1 3.2 5.1 7.5\n")

aSample = ot.Sample.ImportFromTextFile("sample.csv")
aSample.setName("a good sample with spaces")
print("aSample=", repr(aSample))

# 4th sample
with open("sample.csv", "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\n5.6;-xxx;7.8;8.9\n-0.1;3.2;5..1;7.5\n0.9;9.8;8.4;5.4\n")
aSample = ot.Sample.ImportFromCSVFile("sample.csv", ";")
aSample.setName("a sample with bad entries")
print("aSample with bad entries (see log)=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile("sample.csv", ";")
aSample.setName("a sample with bad entries")
print("aSample with bad entries (see log)=", repr(aSample))

# 5th sample
with open("sample.csv", "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8\n-0.1;3.2;;7.5\n6.5;9.0;7.3;-3.7\n")
aSample = ot.Sample.ImportFromCSVFile("sample.csv", ";")
aSample.setName("a sample with missing entries")
print("aSample with missing entries (see log)=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile("sample.csv", ";")
aSample.setName("a sample with missing entries")
print("aSample with missing entries (see log)=", repr(aSample))

# 6th sample
with open("sample.csv", "w") as f:
    f.write(
        '"X1;";"X2!()#{}%&<=>^$+-*./:\\|`?";"X3 [unit]"\n5.6;-6.7;7.8\n-0.1;3.2;7.5 \n'
    )
aSample = ot.Sample.ImportFromCSVFile("sample.csv", ";")
aSample.setName("a sample with special chars")
print("aSample with special chars (see log)=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile("sample.csv", ";")
aSample.setName("a sample with special chars")
print("aSample with special chars (see log)=", repr(aSample))

# text file with comments
with open("sample.csv", "w") as f:
    f.write(
        "# hello\n\nh1 h2 h3 h4 \n-1.2 2.3 3.4 -4.5 \n#spock\n5.6 -6.7 7.8 8.9 \n-0.1 3.2 5.1 7.5 "
    )
aSample = ot.Sample.ImportFromTextFile("sample.csv")
print("aSample with comments=", repr(aSample))

# DOS EOLs
with open("sample.csv", "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\r\n5.6;-6.7;7.8;8.9\r\n-0.1;3.2;7.5;4.6\r\n")
aSample = ot.Sample.ImportFromTextFile("sample.csv", ";")
print("aSample from CRLF file=", repr(aSample))

# skip lines
aSample = ot.Sample.ImportFromTextFile("sample.csv", ";", 1)
print("import with skipped lines=", repr(aSample))

# description with an empty component
with open("sample.csv", "w") as f:
    f.write("a,,c\n1.0,2.0,3.0")
aSample = ot.Sample.ImportFromCSVFile("sample.csv", ",")
print("aSample description empty component=", aSample)
aSample = ot.Sample.ImportFromTextFile("sample.csv", ",")
print("aSample description empty component=", aSample)

# check export with nan/inf
values = ["-2.4e-08", "nan", "NaN", "+nan", "inf", "Inf", "+inf", "-inf"]
aSample = ot.Sample([[float(v)] for v in values])
aSample.exportToCSVFile("sample.csv")
with open("sample.csv") as f:
    print(f.read())

# check import with nan/inf
with open("sample.csv", "w") as f:
    for v in values:
        f.write(v + "\n")
aSample = ot.Sample.ImportFromTextFile("sample.csv")
print("aSample with nan/inf (text)=", repr(aSample))
aSample = ot.Sample.ImportFromCSVFile("sample.csv")
print("aSample with nan/inf  (csv)=", repr(aSample))

os.remove("sample.csv")
