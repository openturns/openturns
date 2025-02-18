#! /usr/bin/env python

import openturns as ot
import os

ot.TESTPREAMBLE()

fname = "sample.csv"

# 1st sample
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;5.1;7.5\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a good sample")
print("aSample=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a good sample")
print("aSample=", repr(aSample))

# 2nd sample
with open(fname, "w") as f:
    f.write("X1,X2,X3,X4\n-1.2,2.3,3.4,-4.5\n5.6,-6.7,7.8,8.9\n-0.1,3.2,5.1,7.5\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ",")
aSample.setName("a good coma separated sample")
print("aSample=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile(fname, ",")
aSample.setName("a good coma separated sample")
print("aSample=", repr(aSample))

# export
aSample.exportToCSVFile(fname)
with open(fname) as f:
    print((f.read()))

# import with quotes in description
aSample = ot.Sample.ImportFromTextFile(fname, ";")
print("import with quotes:", repr(aSample))

# spaces in description
with open(fname, "w") as f:
    f.write(
        'nos sbef saft  sbeaf  mid "qnos"  "qsbef" "qsaft"  "qsbeaf"  "mid mid" \n1 2 3 4 5 6 7 8 9 10\n'
    )
aSample = ot.Sample.ImportFromTextFile(fname)
print("import with spaces:", repr(aSample))

# spaces in description / semicolon
with open(fname, "w") as f:
    f.write(
        'nos; sbef;saft ; sbeaf ;mid mid;"qnos"; "qsbef";"qsaft"; "qsbeaf" ;"mid mid"\n1;2;3;4;5;6;7;8;9;10\n'
    )
aSample = ot.Sample.ImportFromTextFile(fname, ";")
print("import with spaces semicolon:", repr(aSample))

# 3rd sample
with open(fname, "w") as f:
    f.write("X1 X2 X3 X4\n-1.2 2.3 3.4 -4.5\n5.6 -6.7 7.8 8.9\n-0.1 3.2 5.1 7.5\n")

aSample = ot.Sample.ImportFromTextFile(fname)
aSample.setName("a good sample with spaces")
print("aSample=", repr(aSample))

# 4th sample
with open(fname, "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\n5.6;-xxx;7.8;8.9\n-0.1;3.2;5..1;7.5\n0.9;9.8;8.4;5.4\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a sample with bad entries")
print("aSample with bad entries (see log)=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a sample with bad entries")
print("aSample with bad entries (see log)=", repr(aSample))

# 5th sample
with open(fname, "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8\n-0.1;3.2;;7.5\n6.5;9.0;7.3;-3.7\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a sample with missing entries")
print("aSample with missing entries (see log)=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a sample with missing entries")
print("aSample with missing entries (see log)=", repr(aSample))

# 6th sample
with open(fname, "w") as f:
    f.write(
        '"X1;";"X2!()#{}%&<=>^$+-*./:\\|`?";"X3 [unit]"\n5.6;-6.7;7.8\n-0.1;3.2;7.5 \n'
    )
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a sample with special chars")
print("aSample with special chars (see log)=", repr(aSample))

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a sample with special chars")
print("aSample with special chars (see log)=", repr(aSample))

# text file with comments
with open(fname, "w") as f:
    f.write(
        "# hello\n\nh1 h2 h3 h4 \n-1.2 2.3 3.4 -4.5 \n#spock\n5.6 -6.7 7.8 8.9 \n-0.1 3.2 5.1 7.5 "
    )
aSample = ot.Sample.ImportFromTextFile(fname)
print("aSample with comments=", repr(aSample))

# DOS EOLs
with open(fname, "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\r\n5.6;-6.7;7.8;8.9\r\n-0.1;3.2;7.5;4.6\r\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";")
print("aSample from CRLF file=", repr(aSample))

# skip lines
aSample = ot.Sample.ImportFromTextFile(fname, ";", 1)
print("import with skipped lines=", repr(aSample))

# skip lines, with desc
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;7.5;4.6\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";", 1)
print("import with skipped lines, desc=", repr(aSample))

# description with an empty component
with open(fname, "w") as f:
    f.write("a,,c\n1.0,2.0,3.0")
aSample = ot.Sample.ImportFromCSVFile(fname, ",")
print("aSample description empty component=", aSample)
aSample = ot.Sample.ImportFromTextFile(fname, ",")
print("aSample description empty component=", aSample)

# check export with nan/inf
values = ["-2.4e-08", "nan", "NaN", "+nan", "inf", "Inf", "+inf", "-inf"]
aSample = ot.Sample([[float(v)] for v in values])
aSample.exportToCSVFile(fname)
with open(fname) as f:
    print(f.read())

# check import with nan/inf
with open(fname, "w") as f:
    for v in values:
        f.write(v + "\n")
aSample = ot.Sample.ImportFromTextFile(fname)
print("aSample with nan/inf (text)=", repr(aSample))
aSample = ot.Sample.ImportFromCSVFile(fname)
print("aSample with nan/inf  (csv)=", repr(aSample).replace("(ind)", ""))

# "Le French way"
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1,2;2,3;3,4;-4,5\n5,6;-6,7;7,8;8,9\n-0,1;3,2;5,1;7,5\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";", 0, ",")
print("aSample from fr_FR.utf-8 file=", repr(aSample))

# export in fr too
aSample.exportToCSVFile(fname, ";", ",")
with open(fname, "r") as f:
    print("export fr=", f.readlines()[1])

# unicode description
with open(fname, "w", encoding="utf-8") as f:
    f.write("Pmax (bar);Tmax (°C)\n1.4138771410089075e+02;6.1158681591754430e+01\n3.3836092350059448e+02;1.0205155574810637e+02\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
assert aSample.getDescription()[1] == "Tmax (°C)"

os.remove(fname)

# unicode file name
fname = 'Données'
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;5.1;7.5\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
assert aSample.getSize() == 3

os.remove(fname)
