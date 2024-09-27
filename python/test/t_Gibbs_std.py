#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)

# Test the Metropolis within Gibbs algorithm to simulate a correlated 2D Gaussian target :

# Define the target law
mean_target = [0.0, 0.0]
std_target = [1.0, 1.0]
rho_target = 0.9
target_normal = ot.Normal(
    mean_target,
    std_target,
    ot.CorrelationMatrix([[1.0, rho_target], [rho_target, 1.0]]),
)

# Initial state of the Gibbs sampler
initialState = mean_target

# Define RWMH sampler for first coordinate
proposal_std = 0.1
proposal = ot.Normal(0.0, proposal_std)
rwmh_sampler = ot.RandomWalkMetropolisHastings(
    target_normal, initialState, proposal, [0]
)

# Define exacte sampler for second coordinate
randomVector = ot.RandomVector(ot.Normal())


# Define link function, implementing the Gaussian conditioning formulae
# here a Python link function is used. TODO: use a symbolic link function instead
def py_link_function(x):
    cond_mean = (
        mean_target[1]
        + rho_target * std_target[1] * (x[0] - mean_target[0]) / std_target[0]
    )
    cond_std = std_target[1] * math.sqrt(1.0 - rho_target**2)
    return [cond_mean, cond_std]


plf = ot.PythonFunction(2, 2, py_link_function)
rv_sampler = ot.RandomVectorMetropolisHastings(randomVector, initialState, [1], plf)

# Create Gibbs Sampler
gibbs = ot.Gibbs([rwmh_sampler, rv_sampler])

# Generate posterior distribution sample
sampleSize = 10000
xSample = gibbs.getSample(sampleSize + rwmh_sampler.getBurnIn())[rwmh_sampler.getBurnIn():]

# Compare empirical to theoretical moments

ott.assert_almost_equal(
    xSample.computeMean(), mean_target, 0.0, 10.0 / math.sqrt(sampleSize)
)
ott.assert_almost_equal(
    xSample.computeStandardDeviation(), std_target, 0.0, 10.0 / math.sqrt(sampleSize)
)
ott.assert_almost_equal(
    xSample.computeLinearCorrelation()[0, 1],
    rho_target,
    0.0,
    10.0 / math.sqrt(sampleSize),
)

# This analytical example is taken from p. 9 of "Bayesian Modeling Using WinBUGS" - Ioannis Ntzoufras
# 1.5.3: Inference for the mean or normal data with known variance

# Variable of interest: Y=N(mu, sigma)
# Prior for mu: Normal(mu0, sigma0), sigma is known
# Posterior for mu: E(mu|y)=w*y_mean+(1-w)*mu0, and Var(mu|y)=w*(sigmay^2)/n
# => weighted average of the prior an the sample mean
# with w = n*sigma0^2 / (n*sigma0^2 + sigma^2)

# observations
size = 10
realDist = ot.Normal(31.0, 1.2)

data = realDist.getSample(size)

# instrumental distribution
mean_instrumental = ot.Uniform(-2.0, 2.0)
std_instrumental = ot.Uniform(-2.0, 2.0)

# prior distribution
mu0 = 25.0

sigma0s = [0.1, 1.0]
# sigma0s.append(2.0)

# play with the variance of the prior:
# if the prior variance is low (information concernig the mu parameter is strong)
# then the posterior mean will be equal to the prior mean
# if large, the posterior distribution is equivalent to the
# distribution of the sample mean
for i in range(len(sigma0s)):
    sigma0 = sigma0s[i]
    mean_prior = ot.Normal(mu0, sigma0)
    std_prior = ot.Dirac(2.0)  # standard dev is known
    prior = ot.JointDistribution([mean_prior, std_prior])
    # choose the initial state within the prior
    initialState = prior.getRealization()

    # conditional distribution
    conditional = ot.Normal()

    # create a Gibbs sampler
    mean_sampler = ot.RandomWalkMetropolisHastings(
        prior, initialState, mean_instrumental, [0]
    )
    mean_sampler.setLikelihood(conditional, data)
    std_sampler = ot.RandomWalkMetropolisHastings(
        prior, initialState, std_instrumental, [1]
    )
    std_sampler.setLikelihood(conditional, data)
    sampler = ot.Gibbs([mean_sampler, std_sampler])
    realization = sampler.getRealization()

    sigmay = ot.DeconditionedDistribution(ot.Normal(), prior).getStandardDeviation()[0]
    w = size * sigma0**2.0 / (size * sigma0**2.0 + sigmay**2.0)

    print("prior variance= %.12g" % (sigma0**2.0))
    print("  realization=", realization)

    print("  w= %.12g" % w)

    # the posterior for mu is analytical
    mu_exp = w * data.computeMean()[0] + (1.0 - w) * mu0
    sigma_exp = (w * sigmay**2.0 / size) ** 0.5
    print("  expected posterior ~N( %.6g" % mu_exp, ",  %.6g" % sigma_exp, ")")

    # try to generate a sample
    sample = sampler.getSample(1000)
    x_mu = sample.computeMean()[0]

    x_sigma = sample.computeStandardDeviation()[0]
    print("  obtained posterior ~N( %.6g" % x_mu, ",  %.6g" % x_sigma, ")")
    ott.assert_almost_equal(x_mu, mu_exp, 2e-1, 1e-1)
    ott.assert_almost_equal(x_sigma, sigma_exp, 1e-2, 1e-1)

    print(
        "acceptance rate=",
        [mh.getAcceptanceRate() for mh in sampler.getMetropolisHastingsCollection()],
    )


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
        return ot.Interval([0, 0], [1, 1], [True] * 2, [False, True])

    def computeLogPDF(self, x):
        if not (self.alpha > 0.0 and self.beta > 0.0):
            return float("-inf")
        log_pdf = -((x[0] / self.beta) ** self.alpha)
        log_pdf += (self.alpha - 1) * math.log(x[0] / self.beta) * x[1]
        log_pdf += math.log(self.alpha / self.beta) * x[1]
        return log_pdf

    def setParameter(self, parameter):
        self.beta = parameter[0]
        self.alpha = parameter[1]

    def getParameter(self):
        return [self.beta, self.alpha]


conditional = ot.Distribution(CensoredWeibull())
x = ot.Sample(
    [
        [4380, 1],
        [1791, 1],
        [1611, 1],
        [1291, 1],
        [6132, 0],
        [5694, 0],
        [5296, 0],
        [4818, 0],
        [4818, 0],
        [4380, 0],
    ]
)
logpdf = ot.SymbolicFunction(["beta", "alpha"], ["-log(beta)"])
support = ot.Interval([0] * 2, [1] * 2)
support.setFiniteUpperBound([False] * 2)
initialState = [1.0, 1.0]
rwmh_beta = ot.RandomWalkMetropolisHastings(
    logpdf, support, initialState, ot.Normal(0.0, 10000.0), [0]
)
rwmh_beta.setLikelihood(conditional, x)
rwmh_alpha = ot.RandomWalkMetropolisHastings(
    logpdf, support, initialState, ot.Normal(0.0, 0.5), [1]
)
rwmh_alpha.setLikelihood(conditional, x)
gibbs = ot.Gibbs([rwmh_beta, rwmh_alpha])
sample = gibbs.getSample(2000)[rwmh_alpha.getBurnIn() :]
print("mu=", sample.computeMean())
print("sigma=", sample.computeStandardDeviation())

# check recompute indices, update bug
initial_state = [0.0, 0.0, 20.0]
target = ot.Normal(3)
weird_target = ot.JointDistribution([ot.Normal(), ot.Normal(), ot.Dirac(20.0)])
normal0_rwmh = ot.RandomWalkMetropolisHastings(
    target, initial_state, ot.Uniform(-10, 10), [0]
)  # samples from Normal(0,1)
normal1_rwmh = ot.RandomWalkMetropolisHastings(
    target, initial_state, ot.Uniform(-10, 10), [1]
)  # samples from Normal(0,1)
dirac_rwmh = ot.RandomWalkMetropolisHastings(
    weird_target, initial_state, ot.Normal(), [2]
)  # samples from Dirac(20)
# samples from Normal(0,1) x Normal(0,1) x Dirac(20)
gibbs = ot.Gibbs([normal0_rwmh, normal1_rwmh, dirac_rwmh])
sample = gibbs.getSample(5000)
recompute = gibbs.getRecomputeLogPosterior()
print(recompute)
assert recompute == ot.Indices([1, 0, 1]), "wrong recompute indices"
mean = sample.computeMean()
stddev = sample.computeStandardDeviation()
print(mean, stddev)
ott.assert_almost_equal(mean, [0.0, 0.0, 20.0], 0.0, 0.1)
ott.assert_almost_equal(stddev, [1.0, 1.0, 0], 0.0, 0.1)

# check log-pdf is recomputed by the correct blocks
initialState = [0.5] * 4
rvmh1 = ot.RandomVectorMetropolisHastings(
    ot.RandomVector(ot.Dirac([0.5] * 2)), initialState, [0, 1]
)
rvmh2 = ot.RandomVectorMetropolisHastings(
    ot.RandomVector(ot.Uniform(0.0, 1.0)), initialState, [2]
)
rwmh = ot.RandomWalkMetropolisHastings(
    ot.SymbolicFunction(["x", "y", "z", "t"], ["1"]),
    ot.Interval(4),
    initialState,
    ot.Uniform(),
    [3],
)
rwmh.setBurnIn(0)  # otherwise burn-in distorts the test

gibbs = ot.Gibbs([rvmh1, rvmh2, rwmh])
gibbs.getRealization()
assert gibbs.getRecomputeLogPosterior() == [1, 0, 1]
gibbs.setUpdatingMethod(ot.Gibbs.RANDOM_UPDATING)
gibbs.getRealization()
assert gibbs.getRecomputeLogPosterior() == [1, 1, 1]

# Check all blocks are called equally often under the random order option.
# Here there are 3 blocks:
# 1) a Dirac RandomVectorMetropolisHastings -- never moves
# 2) a Uniform RandomVectorMetropolisHastings -- always moves
# 3) a RandomWalkMetropolisHastings with average acceptance probability 1/2
# If 1) is selected or 3) is selected and the proposal is rejected, the chain does not move
# This happens with probability 1/3 + 1/3 * 1/2 = 1/2.
sample = gibbs.getSample(2000)
diffs = sample[1:] - sample[:-1]
zeros = ot.Point(4)
null_diffs = [point == zeros for point in diffs]
frequency_nomove = sum(null_diffs) / len(null_diffs)
ott.assert_almost_equal(frequency_nomove, 0.5, 0.05, 0.0)
