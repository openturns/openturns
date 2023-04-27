#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.RandomGenerator.SetSeed(0)

ot.TESTPREAMBLE()


chainDim = 3
obsDim = 1
outputDim = 1

# observations
obsSize = 10
y_obs = ot.Sample(obsSize, obsDim)
y_obs[0, 0] = -9.50794871493506
y_obs[1, 0] = -3.83296694500105
y_obs[2, 0] = -2.44545713047953
y_obs[3, 0] = 0.0803625289211318
y_obs[4, 0] = 1.01898069723583
y_obs[5, 0] = 0.661725805623086
y_obs[6, 0] = -1.57581204592385
y_obs[7, 0] = -2.95308465670895
y_obs[8, 0] = -8.8878164296758
y_obs[9, 0] = -13.0812290405651
print("y_obs=", y_obs)

p = ot.Sample(obsSize, chainDim)
for i in range(obsSize):
    for j in range(chainDim):
        p[i, j] = (-2 + 5.0 * i / 9.0) ** j
print("p=", p)

fullModel = ot.SymbolicFunction(
    ["p1", "p2", "p3", "x1", "x2", "x3"], ["p1*x1+p2*x2+p3*x3", "1.0"]
)
linkFunction = ot.ParametricFunction(fullModel, range(chainDim), [0.0] * chainDim)

# instrumental distribution
instrumental = ot.Uniform(-1.0, 1.0)

# prior distribution
sigma0 = [10.0] * chainDim
Q0 = ot.CorrelationMatrix(chainDim)  # precision matrix
Q0_inv = ot.CorrelationMatrix(chainDim)  # variance matrix
for i in range(chainDim):
    Q0_inv[i, i] = sigma0[i] * sigma0[i]
    Q0[i, i] = 1.0 / Q0_inv[i, i]
print("Q0=", Q0)

mu0 = [0.0] * chainDim
prior = ot.Normal(mu0, Q0_inv)  # x0 ~ N(mu0, sigma0)
print("x~", prior)

# start from te mean x0=(0.,0.,0.)
print("x0=", mu0)

# conditional distribution y~N(z, 1.0)
conditional = ot.Normal()
print("y~", conditional)

# create a gibbs sampler
mh_coll = [
    ot.RandomWalkMetropolisHastings(prior, mu0, instrumental, [i])
    for i in range(chainDim)
]
for mh in mh_coll:
    mh.setLikelihood(conditional, y_obs, linkFunction, p)
sampler = ot.Gibbs(mh_coll)

# get a realization
realization = sampler.getRealization()
print("y1=", realization)

# try to generate a sample
sampleSize = 10000
sample = sampler.getSample(sampleSize)

x_mu = sample.computeMean()
x_sigma = sample.computeStandardDeviation()

# compute covariance
x_cov = sample.computeCovariance()
P = ot.Matrix(obsSize, chainDim)
for i in range(obsSize):
    for j in range(chainDim):
        P[i, j] = p[i, j]
Qn = P.transpose() * P + Q0
Qn_inv = ot.SquareMatrix(chainDim)
for j in range(chainDim):
    I_j = [0] * chainDim
    I_j[j] = 1.0
    Qn_inv_j = Qn.solveLinearSystem(I_j)
    for i in range(chainDim):
        Qn_inv[i, j] = Qn_inv_j[i]

sigma_exp = [0] * chainDim
for i in range(chainDim):
    sigma_exp[i] = m.sqrt(Qn_inv[i, i])
y_vec = [0] * obsSize
for i in range(obsSize):
    y_vec[i] = y_obs[i, 0]

x_emp = Qn.solveLinearSystem(P.transpose() * y_vec)
mu_exp = Qn.solveLinearSystem((P.transpose() * P) * x_emp + ot.Matrix(Q0) * mu0)

print("sample mean=", x_mu)
print("expected mean=", mu_exp)
ott.assert_almost_equal(x_mu, mu_exp, 1e-1, 1e-1)

print("covariance=", x_cov)
print("expected covariance=", Qn_inv)
ott.assert_almost_equal(x_cov, Qn_inv, 1e-1, 2e-1)
