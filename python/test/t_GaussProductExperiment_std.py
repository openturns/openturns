#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Test default constructor
print("experiment0=", repr(ot.GaussProductExperiment().generate()))

distribution = ot.JointDistribution([ot.Exponential(), ot.Triangular(-1.0, -0.5, 1.0)])
marginalSizes = [3, 6]
# Test the constructor based on marginal degrees
print("experiment1=", ot.GaussProductExperiment(marginalSizes))
# Test the constructor based on distribution
print("experiment2=", ot.GaussProductExperiment(distribution))
# Test the constructor based on marginal degrees and distribution
experiment = ot.GaussProductExperiment(distribution, marginalSizes)
print("experiment = ", experiment)
sample, weights = experiment.generateWithWeights()
print("sample = ", repr(sample))
print("weights = ", repr(weights))


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
    Check polynomial exactness of Gauss tensor product quadrature

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

    # Create Gauss tensor product quadrature
    lowerBoundPoint = bounds.getLowerBound()
    upperBoundPoint = bounds.getUpperBound()
    distribution = ot.JointDistribution(
        [ot.Uniform(lowerBoundPoint[i], upperBoundPoint[i]) for i in range(dimension)]
    )
    experiment = ot.GaussProductExperiment(distribution, marginalSizes)

    # Evaluate integral
    nodes, weights = experiment.generateWithWeights()
    values = productPoly(nodes).asPoint()
    computedIntegral = weights.dot(values)

    # Expected integral
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
    return


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
