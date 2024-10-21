#! /usr/bin/env python

import openturns as ot
import math
import openturns.testing as ott

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(["x"], ["sin(x)"])
a = -2.5
b = 4.5
# Default parameters
algo = ot.GaussLegendre()
print("Algo=", algo)
# High-level interface
value, adaptedNodes = algo.integrateWithNodes(f, ot.Interval(a, b))
ref = math.cos(a) - math.cos(b)
print("value=%.6f" % value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)
# Low-level interface
algo = ot.GaussLegendre([20])
value, adaptedNodes = algo.integrateWithNodes(f, ot.Interval(a, b))
print("value=%.6f" % value[0], ", ref=%.6f" % ref, ", adaptedNodes=", adaptedNodes)


# Check polynomial degree of exactness
def checkPolynomialExactness(
    marginalDegrees,
    marginalSizes,
    lowerBound=0.0,
    upperBound=1.0,
    rtol=1.0e-15,
    atol=0.0,
    verbose=False,
):
    """
    Check polynomial exactness of GaussLegendre quadrature

    Parameters
    ----------
    marginalDegrees : list of int
        The polynomial degree of the marginal polynomials to integrate
    marginalSizes : list of int
        The number of nodes on each marginal axis.
    lowerBound : float
        The lower bound of quadrature
    upperBound : float
        The upper bound of quadrature
    rtol : float, > 0
        The relative tolerance
    atol : float, > 0
        The absolute tolerance
    verbose : bool
        Set to True to print intermediate messages

    Examples
    --------
    marginalDegrees = [5, 3, 7]
    marginalSizes = [3, 2, 4]  # Polynomial exactness space = P5 x P3 x P7
    checkPolynomialExactness(marginalDegrees, marginalSizes)
    """
    dimension = len(marginalDegrees)
    if len(marginalSizes) != dimension:
        raise ValueError(
            f"Number of marginal degrees is {dimension} "
            f"but number of marginal sizes is {len(marginalSizes)}"
        )

    # Set bounds
    bounds = ot.Interval([lowerBound] * dimension, [upperBound] * dimension)

    # Create polynomial
    polynomialCollection = ot.PolynomialCollection()
    for i in range(dimension):
        coefficients = [0.0] * (1 + marginalDegrees[i])
        coefficients[-1] = 1
        polynomial = ot.UniVariatePolynomial(coefficients)
        polynomialCollection.add(polynomial)

    productPoly = ot.ProductPolynomialEvaluation(polynomialCollection)

    # Evaluate integral
    algo = ot.GaussLegendre(marginalSizes)
    computedIntegral = algo.integrate(ot.Function(productPoly), bounds)[0]

    # Expected integral
    lowerBoundPoint = bounds.getLowerBound()
    upperBoundPoint = bounds.getUpperBound()
    expectedIntegral = 1.0
    for i in range(dimension):
        marginalIntegral = (
            upperBoundPoint[i] ** (1 + marginalDegrees[i])
            - lowerBoundPoint[i] ** (1 + marginalDegrees[i])
        ) / (1 + marginalDegrees[i])
        expectedIntegral *= marginalIntegral
    absoluteError = abs(computedIntegral - expectedIntegral)
    if verbose:
        print(
            f"Polynomial : {str(productPoly):20s}, "
            f"  integral computed  = {computedIntegral:.7f}, "
            f"  expected = {expectedIntegral:.7f}, "
            f"  absolute error = {absoluteError:.3e}"
        )
    ott.assert_almost_equal(expectedIntegral, computedIntegral, rtol, atol)


# Test different polynomials, up to the maximum
marginalSizes = [3, 2, 4]  # Polynomial exactness space = P5 x P3 x P7
maximumMarginalDegrees = [2 * n for n in marginalSizes]
experiment = ot.Tuples(maximumMarginalDegrees)
marginalDegreesList = experiment.generate()
for i in range(marginalDegreesList.getSize()):
    marginalDegrees = marginalDegreesList[i]
    checkPolynomialExactness(
        marginalDegrees, marginalSizes, rtol=1.0e-14, verbose=False
    )
