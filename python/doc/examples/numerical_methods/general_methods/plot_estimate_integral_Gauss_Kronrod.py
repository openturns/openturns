"""
Integrate a function with Gauss-Kronrod algorithm
=================================================
"""

# %%
import openturns as ot
import numpy as np
import openturns.viewer as otv

# %%
# References :
#
# - [davis1975]_ section "2.7.1.1 "The Kronrod scheme", p. 82 and section 6.2.0 "An iterative nonadaptive scheme based on Kronrod formulas" p.321.
# - [dahlquist2008]_ p.575.

# %%
# Introduction
# ============
#
# In this example, we present the :class:`~openturns.GaussKronrod` algorithm for one dimensional integration. That is, the algorithm can approximate the integral:
#
# .. math::
#    \int_a^b f(x) dx
#
# where :math:`f:[a,b] \rightarrow \mathbb{R}^p` is a function,
# :math:`[a,b] \subset \mathbb{R}` with :math:`a\leq b` is a one dimensional interval,
# :math:`p` is the dimension of the output.
# Notice that the dimension of the input must be equal to 1, but the number of outputs can be greater than 1.
#
# Suppose that we have estimated the integral with Gaussian quadrature and :math:`m` quadrature nodes.
# If we want to improve the accuracy and use more nodes, the issue is that the new nodes do not correspond to the old ones: therefore, we cannot reuse the function evaluations.
#
# The Gauss-Kronrod algorithm improves the situation by using two different methods:
#
# - a Gaussian quadrature rule with :math:`m` nodes,
# - a Kronrod extension with :math:`2m+1` nodes.
#
# The rule :math:`(G_m,K_{2m+1})` is called a Gauss-Kronrod pair. In the Kronrod extension, the first :math:`m` nodes are equal to the nodes in Gaussian quadrature.
#
# The Gaussian quadrature rule with :math:`m` nodes is exact for polynomials of degree :math:`2m-1`.
# The Kronrod extension with :math:`2m+1` nodes is designed to be exact for polynomials of degree :math:`3m+1`.
#
# The choice of the weight function :math:`w(x)` determines the nodes.
# We consider the weight :math:`w(x)=1` and the interval :math:`[a,b]=[-1,1]` (it is straightforward to generalize this for an arbitrary interval :math:`[a,b]`).
# In this case, the new :math:`m+1` nodes of the Kronrod extension interlaces with the Gaussian nodes.
# The weights are guaranteed to be positive (an essential property for numerical stability).

# %%
# Example
# =======
# The following example is from [davis1975]_ p.325:
#
# .. math::
#    \int_0^1 \frac{2}{2 + \sin(10 \pi x)} dx = \frac{2}{\sqrt{3}} = 1.154700538379251529.

# %%
# We first define the function as a :class:`~openturns.SymbolicFunction`.
integrand = ot.SymbolicFunction(["x"], ["2 / (2 + sin(10 * pi_ * x))"])
integrand.setOutputDescription([r"$\frac{2}{2 + sin(10 \pi x)}$"])
graph = integrand.draw(0.0, 1.0, 200)
otv.View(graph)

# %%
# We see that regular spikes and valleys will make this function difficult to integrate, because of the large curvatures implied at these points.
#
# We will later count the number of function evaluations.
# But a small amount of function evaluations has already been used for the plot and this is why we must take it into account.
before_evaluation_number = integrand.getEvaluationCallsNumber()
before_evaluation_number

# %%
# Basic use
# =========
# We first choose the Gauss-Kronrod rule. Six methods are available: we select the "G11K23" rule.
# It uses 11 nodes from a Gauss rule and 23 nodes from a Kronrod rule, re-using the nodes from the Gauss rule.
quadrature_rule = ot.GaussKronrodRule(ot.GaussKronrodRule.G11K23)

# %%
# We set the maximum number of sub-intervals and the maximum absolute error.
maximumSubIntervals = 100
maximumError = 1.0e-8
algo = ot.GaussKronrod(maximumSubIntervals, maximumError, quadrature_rule)
interval = ot.Interval(0.0, 1.0)
computed = algo.integrate(integrand, interval)
computed[0]

# %%
# Notice that the algorithm can integrate a function which has several outputs (but the number of inputs is restricted to 1).
# This is why we use the index `[0]` of `computed`, since :meth:`~openturns.GaussKronrod.integrate` returns a :class:`~openturns.Point`.
# In order to check this computation, we compute the log-relative error in base 10.
# In most cases (except when the exponent of the two numbers are different), this represents the number of correct digits in base 10.
expected = 1.154700538379251529
LRE_10 = -np.log10(abs(computed[0] - expected) / abs(expected))
LRE_10

# %%
# The method computes more than 14 digits correctly. Given that 17 digits is the best we can, this is an astonishing performance.
#
# We then compute the number of function evaluations.
after_evaluation_number = integrand.getEvaluationCallsNumber()
number_of_calls = after_evaluation_number - before_evaluation_number
number_of_calls

# %%
# Advanced use
# ============
#
# The Gauss-Kronrod algorithm strives to produce an approximated integral which actual error is less than the tolerance.
# The algorithm estimates the error, which may be used to guess the accuracy in the situation where the exact value is unknown (this is the general use case, of course).
#
# In order to get the error estimated by the algorithm, we use the third input argument of the :meth:`~openturns.GaussKronrod.integrate` method.
error = ot.Point()
computed = algo.integrate(integrand, interval, error)
computed[0]

# %%
# The variable `error` now contains the error estimate from the algorithm.
error[0]

# %%
# We see that the error estimate is a little lower than the tolerance, which indicates that the integral should be correctly approximated.
error = ot.Point()
lowerBound = 0.0
upperBound = 1.0
ai = ot.Point()
bi = ot.Point()
ei = ot.Point()
fi = ot.Sample()
computed = algo.integrate(
    integrand, lowerBound, upperBound, error, ai, bi, fi, ei)
computed[0]

# %%
# The error still contains the estimate of the error.
error[0]

# %%
# During the algorithm, a collection of subintegrals
#
# .. math::
#    \int_{a_i}^{b_i} g(x) dx
#
# are approximated.
# The outputs :math:`a_i` and :math:`b_i` contain the subintervals used in the algorithm.
print('ai:', ai)
print('bi:', bi)

# %%
# The corresponding value of the integrals are in :math:`f_i`.
# Since :math:`f` can be a multidimensional point, this is a :class:`~openturns.Sample`, which dimension corresponds to the output dimension of the function :math:`f`.
print('fi:', fi)

# %%
# The sum of these sub-integrals is the value of the integral:
sum(fi)[0]

# %%
# The estimated error of each integral is in :math:`e_i`:
print(ei)
number_of_intervals = ai.getDimension()
print('number of intervals:', number_of_intervals)
for i in range(number_of_intervals):
    print(
        "Integral #%d : [%.4f, %.4f] is %.4f with error %.3e"
        % (i, ai[i], bi[i], fi[i, 0], ei[i])
    )

# %%
# The quadrature rule
# ===================
# The object created by :class:`~openturns.GaussKronrodRule` defines the nodes and weights of the quadrature rule.
# We use the notations in the help page of :class:`~openturns.GaussKronrodRule`.
#
# The method :meth:`~openturns.GaussKronrodRule.getOrder` returns the parameter :math:`m`:
quadrature_rule.getOrder()

# %%
# We first analyze the Gaussian quadrature rule. The first parameter is the weight :math:`\omega_0` in front of :math:`f(0)`.
quadrature_rule.getZeroGaussWeight()

# %%
# The method :meth:`~openturns.GaussKronrodRule.getOtherGaussWeights` returns the weights :math:`\omega_k` in front of :math:`f(\xi_k) + f(-\xi_k)`.
quadrature_rule.getOtherGaussWeights()

# %%
# The method :meth:`~openturns.GaussKronrodRule.getOtherKronrodNodes` returns the nodes. The Kronrod nodes interlace the Gaussian nodes.
quadrature_rule.getOtherKronrodNodes()

# %%
# The method :meth:`~openturns.GaussKronrodRule.getOtherKronrodWeights` returns the weights :math:`\alpha_k` in the Kronrod extension.
quadrature_rule.getOtherKronrodWeights()
