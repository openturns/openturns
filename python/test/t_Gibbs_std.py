#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


# this analytical example is taken from "Bayesian Modeling Using WinBUGS" - Ioannis Ntzoufras
# 1.5.3: Inference for the mean or normal data with known variance

# Variable of interest: Y=N(mu, sigma)
# Prior for mu: Normal(mu0, sigma0), sigma is known
# Posterior for mu: E(mu|y)=w*y_mean+(1-w)*mu0, and Var(mu|y)=w*(sigmay^2)/n
# => weighted average of the prior an the sample mean
# with w = n*sigma0^2 / (n*sigma0^2 + sigma^2)

# observations
size = 10
realDist = ot.Normal(31., 1.2)

data = realDist.getSample(size)

# instrumental distribution
mean_instrumental = ot.Uniform(-2.0, 2.0)
std_instrumental = ot.Uniform(-2.0, 2.0)

# prior distribution
mu0 = 25.

sigma0s = [0.1, 1.0]
# sigma0s.append(2.0)

# play with the variance of the prior:
# if the prior variance is low (information concernig the mu parameter is strong)
# then the posterior mean will be equal to the prior mean
# if large, the the posterior distribution is equivalent to the
# distribution of the sample mean
for i in range(len(sigma0s)):

    sigma0 = sigma0s[i]
    mean_prior = ot.Normal(mu0, sigma0)
    std_prior = ot.Dirac(2.0)  # standard dev is known
    prior = ot.ComposedDistribution([mean_prior, std_prior])
    # choose the initial state within the prior
    initialState = prior.getRealization()

    # conditional distribution
    conditional = ot.Normal()

    # create a Gibbs sampler
    mean_sampler = ot.RandomWalkMetropolisHastings(prior, initialState, mean_instrumental, [0])
    mean_sampler.setLikelihood(conditional, data)
    std_sampler  = ot.RandomWalkMetropolisHastings(prior, initialState, std_instrumental, [1])
    std_sampler.setLikelihood(conditional, data)
    sampler = ot.Gibbs([mean_sampler, std_sampler])
    sampler.setThinning(2)
    sampler.setBurnIn(500)
    realization = sampler.getRealization()

    sigmay = ot.ConditionalDistribution(ot.Normal(), prior).getStandardDeviation()[0]
    w = size * sigma0 ** 2. / (size * sigma0 ** 2. + sigmay ** 2.0)

    print("prior variance= %.12g" % (sigma0 ** 2.))
    print("  realization=", realization)

    print("  w= %.12g" % w)

    # the posterior for mu is analytical
    mu_exp = (w * data.computeMean()[0] + (1. - w) * mu0)
    sigma_exp = ((w * sigmay ** 2.0 / size) ** 0.5)
    print("  expected posterior ~N( %.6g" % mu_exp, ",  %.6g" % sigma_exp, ")")

    # try to generate a sample
    sample = sampler.getSample(1000)
    x_mu = sample.computeMean()[0]

    x_sigma = sample.computeStandardDeviation()[0]
    print("  obtained posterior ~N( %.6g" % x_mu, ",  %.6g" % x_sigma, ")")
    ott.assert_almost_equal(x_mu, mu_exp, 2e-1, 1e-1)
    ott.assert_almost_equal(x_sigma, sigma_exp, 1e-2, 1e-1)

    print('acceptance rate=', [mh.getAcceptanceRate() for mh in sampler.getMetropolisHastingsCollection()])


# improper prior
class CensoredWeibull(ot.PythonDistribution):
    """
    Right-censored Weibull log-PDF calculation
    Each data point x is assumed 2D, with:
        x[0]: observed functioning time
        x[1]: nature of x[0]:
            if x[1]=0: x[0] is a censoring time
            if x[1]=1: x[0] is a time-to failure
    """
    def __init__(self, beta=5000.0, alpha=2.0):
        super(CensoredWeibull, self).__init__(2)
        self.beta = beta
        self.alpha = alpha

    def getRange(self):
        return ot.Interval([0, 0], [1, 1], [True]*2, [False, True])

    def computeLogPDF(self, x):
        if not (self.alpha>0.0 and self.beta>0.0):
            return float('-inf')
        log_pdf = -( x[0] / self.beta )**self.alpha
        log_pdf += ( self.alpha - 1 ) * m.log( x[0] / self.beta ) * x[1]
        log_pdf += m.log( self.alpha / self.beta ) * x[1]
        return log_pdf

    def setParameter( self, parameter ):
        self.beta = parameter[0]
        self.alpha = parameter[1]

    def getParameter( self ):
        return [self.beta, self.alpha]

conditional = ot.Distribution(CensoredWeibull())
x = ot.Sample([[4380, 1], [1791,1], [1611,1], [1291,1], [6132,0], [5694,0], [5296,0], [4818,0], [4818,0], [4380,0]])
logpdf = ot.SymbolicFunction(['beta', 'alpha'], ['-log(beta)'])
support = ot.Interval([0] * 2, [1] * 2)
support.setFiniteUpperBound([False] * 2)
initialState = [1.0, 1.0]
rwmh_beta = ot.RandomWalkMetropolisHastings(logpdf, support, initialState, ot.Normal(0., 10000.0 ), [0])
rwmh_beta.setLikelihood(conditional, x)
rwmh_alpha = ot.RandomWalkMetropolisHastings(logpdf, support, initialState, ot.Normal(0., 0.5 ), [1])
rwmh_alpha.setLikelihood(conditional, x)
gibbs = ot.Gibbs([rwmh_beta, rwmh_alpha])
sample = gibbs.getSample(1000)
print('mu=', sample.computeMean())
print('sigma=', sample.computeStandardDeviation())


# check recompute indices, update bug
initial_state = [0.0, 0.0, 20.0]
target = ot.Normal(3)
weird_target = ot.ComposedDistribution([ot.Normal(), ot.Normal(), ot.Dirac(20.0)])
normal0_rwmh = ot.RandomWalkMetropolisHastings(target, initial_state, ot.Uniform(-10, 10), [0]) # samples from Normal(0,1)
normal1_rwmh = ot.RandomWalkMetropolisHastings(target, initial_state, ot.Uniform(-10, 10), [1]) # samples from Normal(0,1)
dirac_rwmh = ot.RandomWalkMetropolisHastings(weird_target, initial_state, ot.Normal(), [2])     # samples from Dirac(20)
gibbs = ot.Gibbs([normal0_rwmh, normal1_rwmh, dirac_rwmh]) # samples from Normal(0,1) x Normal(0,1) x Dirac(20)
sample = gibbs.getSample(1000)
recompute = gibbs.getRecomputeLogPosterior()
print(recompute)
assert recompute == ot.Indices([1, 0, 1]), "wrong recompute indices"
mean = sample.computeMean()
stddev = sample.computeStandardDeviation()
print(mean, stddev)
ott.assert_almost_equal(mean, [-0.015835,0.169951,20])
ott.assert_almost_equal(stddev, [0.956516,1.05469,0])
