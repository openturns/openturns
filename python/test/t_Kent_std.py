#! /usr/bin/env python

import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
gamma1 = [1.0, 0.0, 0.0]
gamma2 = [0.0, 1.0, 0.0]
gamma3 = [0.0, 0.0, 1.0]
distribution = otexp.Kent(25.0, 0.5, gamma1, gamma2, gamma3)
print("Distribution ", distribution)

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
assert oneRealization.getDimension() == 3
assert abs(oneRealization.norm() - 1.0) < 1e-15

# Define a point on the sphere
point = [1.0, 0.0, 0.0]

# Show PDF and log-PDF of point
LPDF = distribution.computeLogPDF(point)
PDF = distribution.computePDF(point)
assert PDF > 0.0
ott.assert_almost_equal(PDF, 24.9823, 1e-3, 0.0)
ott.assert_almost_equal(LPDF, 3.21817, 1e-4, 0.0)

# PDF at a point off the sphere should be 0
point_off = [0.5, 0.0, 0.0]
assert distribution.computePDF(point_off) == 0.0

# PDF at an orthogonal point
point_orth = [0.0, 1.0, 0.0]
pdf_orth = distribution.computePDF(point_orth)
assert pdf_orth > 0.0
# Ratio should match exp(beta - kappa)
ratio = pdf_orth / PDF
ott.assert_almost_equal(ratio, math.exp(-24.5), 1e-12, 0.0)

# Get parameters
param = distribution.getParameter()
assert param.getDimension() == 11
assert param[0] == 25.0

# Get individual parameters
assert distribution.getKappa() == 25.0
assert distribution.getBeta() == 0.5
assert distribution.getGamma1() == gamma1
assert distribution.getGamma2() == gamma2
assert distribution.getGamma3() == gamma3

# Range
range_ = distribution.getRange()
ott.assert_almost_equal(range_.getLowerBound(), [-1.0] * 3)
ott.assert_almost_equal(range_.getUpperBound(), [1.0] * 3)

# Clone via Distribution wrapper
dist = ot.Distribution(distribution)
assert dist.getImplementation().getClassName() == "Kent"

# Test with beta=0 (von Mises-Fisher)
vmf = otexp.Kent(10.0, 0.0, gamma1, gamma2, gamma3)
pdf_vmf = vmf.computePDF([1.0, 0.0, 0.0])
# For von Mises-Fisher on S^2: PDF at mode = kappa / (2*pi)
ott.assert_almost_equal(pdf_vmf, 10.0 / (2.0 * math.pi), 1e-4, 0.0)

# Default constructor
default = otexp.Kent()
assert default.getKappa() == 1.0
assert default.getBeta() == 0.25
assert default.getGamma1() == gamma1

# Test parameter validation: kappa <= 2*beta should fail
caught = False
try:
    invalid = otexp.Kent(1.0, 1.0, gamma1, gamma2, gamma3)
except TypeError:
    caught = True
assert caught, "should reject kappa <= 2*beta"

# Test non-unit vector validation
caught = False
try:
    invalid = otexp.Kent(1.0, 0.25, [2.0, 0.0, 0.0], gamma2, gamma3)
except TypeError:
    caught = True
assert caught, "should reject non-unit gamma vectors"
