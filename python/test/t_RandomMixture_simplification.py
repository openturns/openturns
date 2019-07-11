#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import sqrt, pi, exp

TESTPREAMBLE()
RandomGenerator.SetSeed(0)
ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)
TTY.ShowColors(False)
try:
    # Test fusion of Dirac with no other atoms: should be a unique Dirac(5.8)
    d = RandomMixture(
        [Dirac(1.0), Dirac(2.0), Dirac(3.0)], [0.5, 0.6, 0.7], 2.0)
    print("d=", d)
    # Test fusion of Dirac with other atoms: the Dirac should be merged into
    # the constant: 5 + Exponential(lambda=1,gamma=0)
    d = RandomMixture([Dirac(1.0), Dirac(2.0), Exponential()], 2.0)
    print("d=", d)
    # Test flatten RandomMixture atoms: the RandomMixture should have 4 atoms.
    d = RandomMixture(
        [Gumbel(), RandomMixture([Logistic(), WeibullMin()], [0.5, 1.5], 3.0), Frechet(1.0, 4.0)], [2.0, 3.0, 4.0], 2.0)
    print("d=", d)
    # Test merge of Normal atoms:
    d = RandomMixture(
        [Normal(1.0, 8.0), Logistic(), Normal(2.0, 1.0)], [0.5, 2.5, 3.0], 2.0)
    print("d=", d)
    # Test merge of Exponential, Gamma and ChiSquare atoms
    d = RandomMixture([Exponential(1.0, 1.0), Exponential(1.5, -1.0), Exponential(2.0, 1.0), Gamma(
        4.0, 2.0, -1.0), Gamma(3.0, 1.0, 3.0), ChiSquare(4.0)], [1.0, 1.5, 1.0, 2.0, 2.0, 0.5], 2.0)
    print("d=", d)
    # Test merge of Uniform atoms
    d = RandomMixture([Uniform(0.0, 1.0), Uniform(0.0, 1.0), Uniform(1.0, 3.0),
                       Uniform(-1.0, 4.0), Uniform(2.0, 3.0)], [1.0, 1.0, 2.0, 3.0, 4.0], 2.0)
    print("d=", d)
    # Test merge of Bernoulli and Binomial atoms
    # Deactivate the fusion of discrete atoms
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 0)
    d = RandomMixture([Bernoulli(0.5), Bernoulli(0.5), Bernoulli(0.1), Binomial(
        4, 0.5), Binomial(6, 0.5), Binomial(3, 0.1)], [1.0, 1.5, 2.0, 1.0, 4.0, 2.0], 2.0)
    print("d=", d)
    # Test merge of Poisson atoms
    # Deactivate the fusion of discrete atoms
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 0)
    d = RandomMixture([Poisson(3.0), Poisson(2.0), Poisson(6.0),
                       Poisson(10.0), Poisson(4.0)], [1.0, 2.0, 3.0, 2.0, 1.0], 2.0)
    print("d=", d)
    # Test merge of discrete and continuous atoms
    # Deactivate the fusion of discrete atoms
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 0)
    # more continuous atoms
    d = RandomMixture(
        [Logistic(), Binomial(2, 0.5), Uniform()], [1.0, 2.0, 3.0], 2.0)
    print("d=", d)
    # more discrete atoms
    d = RandomMixture(
        [Bernoulli(0.1), Binomial(2, 0.5), Uniform()], [1.0, 2.0, 3.0], 2.0)
    print("d=", d)
    # same number of continuous and discrete atoms
    d = RandomMixture(
        [Logistic(), Bernoulli(0.1), Binomial(2, 0.5), Uniform()], [1.0, 2.0, 3.0, 4.0], 2.0)
    print("d=", d)
    # Test the fusion of discrete atoms
    Log.Show(Log.DBG)
    print("\n")
    # All the atoms have a too large support
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 1)
    d = RandomMixture(
        [Binomial(2, 0.1), Binomial(3, 0.5), Poisson(), Geometric()], [1.0, 2.0, 3.0, 4.0], 2.0)
    print("d=", d)
    print("\n")
    # Some atoms have a too large support, no pending aggregated discrete
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 15)
    d = RandomMixture(
        [Binomial(2, 0.1), Binomial(3, 0.5), Poisson(), Geometric()], [1.0, 2.0, 3.0, 4.0], 2.0)
    print("d=", d)
    print("\n")
    # Some atoms have a too large support, a pending aggregated discrete
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 15)
    d = RandomMixture([Binomial(2, 0.1), Binomial(3, 0.5), Poisson(),
                       Binomial(2, 0.1), Binomial(3, 0.5)], [1.0, 2.0, 3.0, 4.0, 5.0], 2.0)
    print("d=", d)
    print("\n")
    # All the atoms can be merged
    ResourceMap.SetAsUnsignedInteger("RandomMixture-MaximumSupportSize", 100)
    d = RandomMixture(
        [Bernoulli(0.1), Bernoulli(0.2), Bernoulli(0.3), Bernoulli(0.4)], [1.0, 2.0, 3.0, 4.0], 2.0)
    print("d=", d)
except:
    import sys
    print("t_RandomMixture_simplification.py",
          sys.exc_info()[0], sys.exc_info()[1])
