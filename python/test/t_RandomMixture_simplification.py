#! /usr/bin/env python

import openturns as ot
from math import sqrt, pi, exp

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)
ot.TTY.ShowColors(False)

# Test fusion of Dirac with no other atoms: should be a unique Dirac(5.8)
d = ot.RandomMixture(
    [ot.Dirac(1.0), ot.Dirac(2.0), ot.Dirac(3.0)], [0.5, 0.6, 0.7], 2.0)
print("d=", d)
# Test fusion of Dirac with other atoms: the Dirac should be merged into
# the constant: 5 + Exponential(lambda=1,gamma=0)
d = ot.RandomMixture([ot.Dirac(1.0), ot.Dirac(2.0), ot.Exponential()], 2.0)
print("d=", d)
# Test flatten RandomMixture atoms: the RandomMixture should have 4 atoms.
d = ot.RandomMixture(
    [ot.Gumbel(), ot.RandomMixture([ot.Logistic(), ot.WeibullMin()], [0.5, 1.5], 3.0), ot.Frechet(1.0, 4.0)], [2.0, 3.0, 4.0], 2.0)
print("d=", d)
# Test merge of Normal atoms:
d = ot.RandomMixture(
    [ot.Normal(1.0, 8.0), ot.Logistic(), ot.Normal(2.0, 1.0)], [0.5, 2.5, 3.0], 2.0)
print("d=", d)
# Test merge of Exponential, Gamma and ChiSquare atoms
d = ot.RandomMixture([ot.Exponential(1.0, 1.0), ot.Exponential(1.5, -1.0), ot.Exponential(2.0, 1.0), ot.Gamma(
    4.0, 2.0, -1.0), ot.Gamma(3.0, 1.0, 3.0), ot.ChiSquare(4.0)], [1.0, 1.5, 1.0, 2.0, 2.0, 0.5], 2.0)
print("d=", d)
# Test merge of Uniform atoms
d = ot.RandomMixture([ot.Uniform(0.0, 1.0), ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 3.0),
                    ot.Uniform(-1.0, 4.0), ot.Uniform(2.0, 3.0)], [1.0, 1.0, 2.0, 3.0, 4.0], 2.0)
print("d=", d)
# Test merge of Bernoulli and Binomial atoms
# Deactivate the fusion of discrete atoms
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 0)
d = ot.RandomMixture([ot.Bernoulli(0.5), ot.Bernoulli(0.5), ot.Bernoulli(0.1), ot.Binomial(
    4, 0.5), ot.Binomial(6, 0.5), ot.Binomial(3, 0.1)], [1.0, 1.5, 2.0, 1.0, 4.0, 2.0], 2.0)
print("d=", d)
# Test merge of Poisson atoms
# Deactivate the fusion of discrete atoms
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 0)
d = ot.RandomMixture([ot.Poisson(3.0), ot.Poisson(2.0), ot.Poisson(6.0),
                    ot.Poisson(10.0), ot.Poisson(4.0)], [1.0, 2.0, 3.0, 2.0, 1.0], 2.0)
print("d=", d)
# Test merge of discrete and continuous atoms
# Deactivate the fusion of discrete atoms
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 0)
# more continuous atoms
d = ot.RandomMixture(
    [ot.Logistic(), ot.Binomial(2, 0.5), ot.Uniform()], [1.0, 2.0, 3.0], 2.0)
print("d=", d)
# more discrete atoms
d = ot.RandomMixture(
    [ot.Bernoulli(0.1), ot.Binomial(2, 0.5), ot.Uniform()], [1.0, 2.0, 3.0], 2.0)
print("d=", d)
# same number of continuous and discrete atoms
d = ot.RandomMixture(
    [ot.Logistic(), ot.Bernoulli(0.1), ot.Binomial(2, 0.5), ot.Uniform()], [1.0, 2.0, 3.0, 4.0], 2.0)
print("d=", d)
# Test the fusion of discrete atoms
ot.Log.Show(ot.Log.DBG)
print("\n")
# All the atoms have a too large support
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 1)
d = ot.RandomMixture(
    [ot.Binomial(2, 0.1), ot.Binomial(3, 0.5), ot.Poisson(), ot.Geometric()], [1.0, 2.0, 3.0, 4.0], 2.0)
print("d=", d)
print("\n")
# Some atoms have a too large support, no pending aggregated discrete
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 15)
d = ot.RandomMixture(
    [ot.Binomial(2, 0.1), ot.Binomial(3, 0.5), ot.Poisson(), ot.Geometric()], [1.0, 2.0, 3.0, 4.0], 2.0)
print("d=", d)
print("\n")
# Some atoms have a too large support, a pending aggregated discrete
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 15)
d = ot.RandomMixture([ot.Binomial(2, 0.1), ot.Binomial(3, 0.5), ot.Poisson(),
                    ot.Binomial(2, 0.1), ot.Binomial(3, 0.5)], [1.0, 2.0, 3.0, 4.0, 5.0], 2.0)
print("d=", d)
print("\n")
# All the atoms can be merged
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 100)
d = ot.RandomMixture(
    [ot.Bernoulli(0.1), ot.Bernoulli(0.2), ot.Bernoulli(0.3), ot.Bernoulli(0.4)], [1.0, 2.0, 3.0, 4.0], 2.0)
print("d=", d)
