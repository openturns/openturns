from openturns import *

data_ref=NumericalSample.ImportFromCSVFile("besselI1_maple.csv")
print data_ref, data_ref.getSize()
errMax = 0
err1 = 100
err2 = 100
for j in range(data_ref.getSize()):
    x = data_ref[j, 0]
    ref = data_ref[j, 1]
    val1 = SpecFunc.BesselI1(x)
    val2 = SpecFunc.BesselI2(x)
    err1 = abs(ref - val1)
    err2 = abs(ref - val2)
    if ref != 0.0:
        err1 /= abs(ref)
        err2 /= abs(ref)
    if err1 < err2:
        print "111111111 x=", x, "error 1=", err1, "error 2=", err2
    else:
        print "222222222 x=", x, "error 1=", err1, "error 2=", err2
