#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# CharlierFactory
assert ot.CharlierFactory(5.0) == ot.CharlierFactory(5.0), "Charlier==Charlier"
assert ot.CharlierFactory(5.0) != ot.CharlierFactory(10.0), "Charlier!=Charlier lambda"

# ChebychevFactory
assert ot.ChebychevFactory() == ot.ChebychevFactory(), "Chebychev==Chebychev"
assert ot.ChebychevFactory() != ot.CharlierFactory(5.0), "Chebychev!=Charlier"

# HermiteFactory
assert ot.HermiteFactory() == ot.HermiteFactory(), "Hermite==Hermite"

# JacobiFactory
assert ot.JacobiFactory(0.5, 1.5) == ot.JacobiFactory(0.5, 1.5), "Jacobi==Jacobi"
assert ot.JacobiFactory(0.5, 1.5) != ot.JacobiFactory(0.5, 2.0), "Jacobi!=Jacobi beta"

# KrawtchoukFactory
assert ot.KrawtchoukFactory(10, 0.5) == ot.KrawtchoukFactory(10, 0.5), "Krawtchouk==Krawtchouk"
assert ot.KrawtchoukFactory(10, 0.5) != ot.KrawtchoukFactory(10, 0.7), "Krawtchouk!=Krawtchouk p"

# LaguerreFactory
assert ot.LaguerreFactory(1.5) == ot.LaguerreFactory(1.5), "Laguerre==Laguerre"
assert ot.LaguerreFactory(1.5) != ot.LaguerreFactory(2.5), "Laguerre!=Laguerre k"

# LegendreFactory
assert ot.LegendreFactory() == ot.LegendreFactory(), "Legendre==Legendre"

# MeixnerFactory
assert ot.MeixnerFactory(2.0, 0.5) == ot.MeixnerFactory(2.0, 0.5), "Meixner==Meixner"
assert ot.MeixnerFactory(2.0, 0.5) != ot.MeixnerFactory(3.0, 0.5), "Meixner!=Meixner r"

# StandardDistributionPolynomialFactory
assert ot.StandardDistributionPolynomialFactory(ot.Normal()) == ot.StandardDistributionPolynomialFactory(ot.Normal()), "SDPF==SDPF"
assert ot.StandardDistributionPolynomialFactory(ot.Normal()) != ot.StandardDistributionPolynomialFactory(ot.Uniform()), "SDPF!=SDPF dist"

# Handle equality
f1 = ot.OrthogonalUniVariatePolynomialFamily(ot.HermiteFactory())
f2 = ot.OrthogonalUniVariatePolynomialFamily(ot.HermiteFactory())
assert f1 == f2, "OUPF handle =="
assert f1 != ot.OrthogonalUniVariatePolynomialFamily(ot.LegendreFactory()), "OUPF handle !="

# Self equality
h = ot.HermiteFactory()
assert h == h, "self =="
assert not h != h, "self !="
