"""
openturns<=1.13 compatibility shims
"""

import openturns as ot
from openturns import *


def Frechet(alpha=1.0, beta=1.0, gamma=0.0):
    """
    Frechet compatibility shim.
    """
    return ot.Frechet(beta, alpha, gamma)


def Gumbel(alpha=1.0, gamma=0.0):
    """
    Gumbel compatibility shim.
    """
    return ot.ParametrizedDistribution(ot.GumbelLambdaGamma(alpha, gamma))


def Beta(r=2.0, t=4.0, a=-1.0, b=1.0):
    """
    InverseGamma compatibility shim.
    """
    return ot.Beta(r, t - r, a, b)


def InverseGamma(lambda_=1.0, k=1.0):
    """
    InverseGamma compatibility shim.
    """
    return ot.InverseGamma(k, lambda_)


def InverseNormal(lambda_=1.0, mu=1.0):
    """
    InverseNormal compatibility shim.
    """
    return ot.InverseNormal(mu, lambda_)


def Laplace(lambda_=1.0, mu=0.0):
    """
    Laplace compatibility shim.
    """
    return ot.Laplace(mu, lambda_)
