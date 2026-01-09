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
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "semi/csv"

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a good sample")
print("aSample=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "semi/txt"

# 2nd sample
with open(fname, "w") as f:
    f.write("X1,X2,X3,X4\n-1.2,2.3,3.4,-4.5\n5.6,-6.7,7.8,8.9\n-0.1,3.2,5.1,7.5\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ",")
aSample.setName("a good comma separated sample")
print("aSample=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "comma/csv"

aSample = ot.Sample.ImportFromTextFile(fname, ",")
aSample.setName("a good comma separated sample")
print("aSample=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "comma/txt"

# export
aSample.exportToCSVFile(fname)
with open(fname) as f:
    print((f.read()))

# import with quotes in description
aSample = ot.Sample.ImportFromTextFile(fname, ";")
print("import with quotes:", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "quotes"

# spaces in description
with open(fname, "w") as f:
    f.write(
        'nos sbef saft sbeaf mid "qnos" "qsbef" "qsaft" "qsbeaf" "mid mid"\n1 2 3 4 5 6 7 8 9 10\n'
    )
aSample = ot.Sample.ImportFromTextFile(fname)
print("import with spaces:", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (1, 10), "spaces"

# spaces in description / semicolon
with open(fname, "w") as f:
    f.write(
        'nos; sbef;saft ; sbeaf ;mid mid;"qnos"; "qsbef";"qsaft"; "qsbeaf" ;"mid mid"\n1;2;3;4;5;6;7;8;9;10\n'
    )
aSample = ot.Sample.ImportFromTextFile(fname, ";")
print("import with spaces semicolon:", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (1, 10), "spaces/semicolon"

# 3rd sample
with open(fname, "w") as f:
    f.write("X1 X2 X3 X4\n-1.2 2.3 3.4 -4.5\n5.6 -6.7 7.8 8.9\n-0.1 3.2 5.1 7.5\n")

aSample = ot.Sample.ImportFromTextFile(fname)
aSample.setName("a good sample with spaces")
print("aSample=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "good/spaces"

# 4th sample
with open(fname, "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\n5.6;-xxx;7.8;8.9\n-0.1;3.2;..1;7.5\n0.9;9.8;8.4;5.4\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a sample with bad entries")
print("aSample with bad entries (see log)=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (4, 4), "bad/csv"

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a sample with bad entries")
print("aSample with bad entries (see log)=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (4, 4), "bad/txt"

# 5th sample
with open(fname, "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8\n-0.1;3.2;;7.5\n6.5;9.0;7.3;-3.7\n")
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a sample with missing entries")
print("aSample with missing entries (see log)=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (4, 4), "missing/csv"

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a sample with missing entries")
print("aSample with missing entries (see log)=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (4, 4), "missing/txt"

# 6th sample
with open(fname, "w") as f:
    f.write(
        '"X1;";"X2!()#{}%&<=>^$+-*./:\\|`?";"X3 [unit]"\n5.6;-6.7;7.8\n-0.1;3.2;7.5 \n'
    )
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
aSample.setName("a sample with special chars")
print("aSample with special chars (see log)=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (2, 3), "special/csv"

aSample = ot.Sample.ImportFromTextFile(fname, ";")
aSample.setName("a sample with special chars")
print("aSample with special chars (see log)=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (2, 3), "special/txt"

# text file with comments
with open(fname, "w") as f:
    f.write(
        "# hello\n\nh1 h2 h3 h4\n-1.2 2.3 3.4 -4.5 \n#spock\n5.6 -6.7 7.8 8.9 \n-0.1 3.2 5.1 7.5 "
    )
aSample = ot.Sample.ImportFromTextFile(fname)
print("aSample with comments=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "comments"

# DOS EOLs
with open(fname, "w") as f:
    f.write("-1.2;2.3;3.4;-4.5\r\n5.6;-6.7;7.8;8.9\r\n-0.1;3.2;7.5;4.6\r\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";")
print("aSample from CRLF file=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "dos"

# skip lines
aSample = ot.Sample.ImportFromTextFile(fname, ";", 1)
print("import with skipped lines=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (2, 4), "skipped"

# skip lines, with desc
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;7.5;4.6\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";", 1)
print("import with skipped lines, desc=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "skipped/desc"

# description with an empty component
with open(fname, "w") as f:
    f.write("a,,c\n1.0,2.0,3.0")
aSample = ot.Sample.ImportFromCSVFile(fname, ",")
print("aSample description empty component=", aSample)
assert (aSample.getSize(), aSample.getDimension()) == (1, 3), "empty/csv"
assert aSample.getDescription()[1].startswith("Unnamed")
aSample = ot.Sample.ImportFromTextFile(fname, ",")
print("aSample description empty component=", aSample)
assert (aSample.getSize(), aSample.getDimension()) == (1, 3), "empty/txt"
assert aSample.getDescription()[1].startswith("Unnamed")

# check export with nan/inf
values = ["-2.4e-08", "nan", "NaN", "inf", "Inf", "-inf"]
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
assert (aSample.getSize(), aSample.getDimension()) == (6, 1), "nan/csv"
for i in range(len(values)):
    assert str(aSample[i, 0]) == str(float(values[i])), f"{aSample[i, 0]} {values[i]}"
aSample = ot.Sample.ImportFromCSVFile(fname)
print("aSample with nan/inf  (csv)=", repr(aSample).replace("(ind)", ""))
assert (aSample.getSize(), aSample.getDimension()) == (6, 1), "nan/txt"
for i in range(len(values)):
    assert str(aSample[i, 0]) == str(float(values[i])), f"{aSample[i, 0]} {values[i]}"

# "Le French way"
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1,2;2,3;3,4;-4,5\n5,6;-6,7;7,8;8,9\n-0,1;3,2;5,1;7,5\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";", 0, ",")
print("aSample from fr_FR.utf-8 file=", repr(aSample))
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "fr/txt"
assert aSample[0, 1] == 2.3, f"fr {aSample[0, 1]}"

# export in fr too
aSample.exportToCSVFile(fname, ";", ",")
with open(fname, "r") as f:
    print("export fr=", f.readlines()[1])

# unicode description
with open(fname, "w", encoding="utf-8") as f:
    f.write(
        "Pmax (bar);Tmax (°C)\n1.4138771410089075e+02;6.1158681591754430e+01\n3.3836092350059448e+02;1.0205155574810637e+02\n"
    )
aSample = ot.Sample.ImportFromCSVFile(fname, ";")
assert (aSample.getSize(), aSample.getDimension()) == (2, 2), "unicode desc"
assert aSample.getDescription()[1] == "Tmax (°C)"

os.remove(fname)

# unicode file name
fname = "Données"
with open(fname, "w") as f:
    f.write("X1;X2;X3;X4\n-1.2;2.3;3.4;-4.5\n5.6;-6.7;7.8;8.9\n-0.1;3.2;5.1;7.5\n")
aSample = ot.Sample.ImportFromTextFile(fname, ";")
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "unicode/text"

aSample.exportToCSVFile(fname)
aSample = ot.Sample.ImportFromCSVFile(fname)
assert (aSample.getSize(), aSample.getDimension()) == (3, 4), "unicode/csv"

# empty components + non-empty parsable values should not land in the header
with open(fname, "w") as f:
    f.write(";;8\n;;2\n;;0")
aSample = ot.Sample.ImportFromCSVFile(fname)
assert (aSample.getSize(), aSample.getDimension()) == (3, 3), "parsable"

# reject nan in header
with open(fname, "w") as f:
    f.write("1;2;nan\n1;2;3\n4;5;6")
aSample = ot.Sample.ImportFromCSVFile(fname)
assert (aSample.getSize(), aSample.getDimension()) == (3, 3), "nan header"

# all-nan is ok
with open(fname, "w") as f:
    f.write("X1;X2;X3\nnan;nan;nan\n")
aSample = ot.Sample.ImportFromCSVFile(fname)
assert (aSample.getSize(), aSample.getDimension()) == (1, 3), "all nan"

os.remove(fname)
