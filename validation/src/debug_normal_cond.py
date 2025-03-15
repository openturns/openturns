from openturns import *
from openturns.experimental import *

R = CorrelationMatrix(3)
R[0, 1]=0.1
R[0, 2]=0.2
R[1, 2]=0.3
d = Normal([0.0]*3, [1.5, 2.0, 3.0], R)
# Test conditional methods
y = [1.2, 2.3]
x = 1.5
pc = PointConditionalDistribution(d, [0, 1], y)
cdf1 = pc.computeCDF(x)
print(cdf1, pc.computeQuantile(cdf1)[0])
cdf2 = d.computeConditionalCDF(x, y)
print(cdf2, d.computeConditionalQuantile(cdf2, y))

# Test sequential conditional methods
# First, check if conditionalCDF and conditionalQuantile are inverse
xSeq = Point([2.1, 2.2, 2.3])
cdfSeq = d.computeSequentialConditionalCDF(xSeq)
qSeq = d.computeSequentialConditionalQuantile(cdfSeq)
print("xSeq=", xSeq, "cdfSeq=", cdfSeq, "qSeq=", qSeq)
uSeq = Point([0.1, 0.2, 0.3])
qSeq = d.computeSequentialConditionalQuantile(uSeq)
cdfSeq = d.computeSequentialConditionalCDF(qSeq)
print("uSeq=", uSeq, "qSeq=", qSeq, "cdfSeq=", cdfSeq)
# Second, check if the sequential versions match the pointwise methods
print("#"*50)
print("computeConditionalCDF vs computeSequentialConditionalCDF")
for i in range(3):
    y = qSeq[0:i]
    x = qSeq[i]
    ind = list(range(i+1))
    indred = list(range(i))
    pc = PointConditionalDistribution(d.getMarginal(ind), indred, y)
    print(d.computeConditionalCDF(x, y), cdfSeq[i], pc.computeCDF(x))
print("#"*50)
print("computeConditionalQuantile vs computeSequentialConditionalQuantile")
for i in range(3):
    y = qSeq[0:i]
    p = cdfSeq[i]
    ind = list(range(i+1))
    indred = list(range(i))
    pc = PointConditionalDistribution(d.getMarginal(ind), indred, y)
    print(d.computeConditionalQuantile(p, y), qSeq[i], pc.computeQuantile(p)[0])
# Third, check if the sequential versions match the pointwise methods
print("#"*50)
pdfSeq = d.computeSequentialConditionalPDF(qSeq)
print("computeConditionalPDF vs computeSequentialConditionalPDF")
for i in range(3):
    y = qSeq[0:i]
    x = qSeq[i]
    ind = list(range(i+1))
    indred = list(range(i))
    pc = PointConditionalDistribution(d.getMarginal(ind), indred, y)
    print(d.computeConditionalPDF(x, y), pdfSeq[i], pc.computePDF(x))
