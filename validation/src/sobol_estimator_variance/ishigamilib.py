#
# A simple sensitivity analysis, with Sobol indices.
# "Introduction to sensitivity analysis with NISP
# Michael Baudin (EDF), Jean-Marc Martinez (CEA)"
#
# EDF R&D - 2012-2013 - Michael Baudin
#

from math import sin, pi
from openturns import ComposedDistribution, Uniform, SymbolicFunction


def ishigamiAB():
    a = 7.0
    b = 0.1
    return [a, b]


# Calculs exacts
def ishigamiSAExact(a, b):
    meanY = a / 2.0
    varY = 1.0 / 2.0 + a ** 2 / 8 + b * pi ** 4 / 5 + b ** 2 * pi ** 8 / 18
    S1 = (1.0 / 2.0 + b * pi ** 4 / 5 + b ** 2 * pi ** 8 / 50) / varY
    S2 = (a ** 2 / 8) / varY
    S3 = 0
    # S12 = 0;
    # S23 = 0;
    S13 = b ** 2 * pi ** 8 / 2.0 * (1.0 / 9.0 - 1.0 / 25.0) / varY
    # S123 = 0;
    ST1 = S1 + S13
    ST2 = S2
    ST3 = S3 + S13
    return [meanY, varY, S1, S2, S3, ST1, ST2, ST3]


# Define the model as a Python function
# Ishigami
def ishigamiG(x):
    a, b = ishigamiAB()
    y = sin(x[0]) + a * sin(x[1]) ** 2 + b * x[2] ** 4 * sin(x[0])
    return [y]


def ishigamiGSymbolic():
    formula = ["sin(X1)+7*sin(X2)*sin(X2)+0.1*X3*X3*X3*X3*sin(X1)"]
    model = SymbolicFunction(["X1", "X2", "X3"], formula)
    return model


def ishigamiDistribution():
    distribution = ComposedDistribution([Uniform(-pi, pi)] * 3)
    return distribution


if __name__ == "__main__":
    # Number of input parameters
    dim = 3

    # Total degree
    totdeg = 9

    # Definition des constantes
    a, b = ishigamiAB()

    [meanY, varY, S1, S2, S3, ST1, ST2, ST3] = ishigamiSAExact(a, b)

    print("Exact Results:")
    print("E(Y):", meanY)
    print("V(Y):", varY)
    print("S:", S1, S2, S3)
    print("ST:", ST1, ST2, ST3)

    #   expectation: 3.5
    #   var: 13.844588
    #   S: 0.3139052, 0.4424111, 0
    #   ST: 0.5575889, 0.4424111, 0.2436837
