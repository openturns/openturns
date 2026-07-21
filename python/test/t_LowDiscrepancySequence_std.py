#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# SobolSequence
assert ot.SobolSequence(3) == ot.SobolSequence(3), "Sobol==Sobol"
assert ot.SobolSequence(3) != ot.SobolSequence(5), "Sobol!=Sobol dim"

# HaltonSequence
assert ot.HaltonSequence(3) == ot.HaltonSequence(3), "Halton==Halton"
assert ot.HaltonSequence(3) != ot.HaltonSequence(5), "Halton!=Halton dim"

# ReverseHaltonSequence
assert ot.ReverseHaltonSequence(3) == ot.ReverseHaltonSequence(3), "RHS==RHS"
assert ot.ReverseHaltonSequence(3) != ot.ReverseHaltonSequence(5), "RHS!=RHS dim"

# HaselgroveSequence
assert ot.HaselgroveSequence(3) == ot.HaselgroveSequence(3), "HG==HG"
assert ot.HaselgroveSequence(3) != ot.HaselgroveSequence(5), "HG!=HG dim"

# FaureSequence
assert ot.FaureSequence(3) == ot.FaureSequence(3), "Faure==Faure"
assert ot.FaureSequence(3) != ot.FaureSequence(5), "Faure!=Faure dim"

# Cross-type inequality
assert ot.SobolSequence(3) != ot.HaltonSequence(3), "Sobol!=Halton"

# Handle equality
l1 = ot.LowDiscrepancySequence(ot.SobolSequence(3))
l2 = ot.LowDiscrepancySequence(ot.SobolSequence(3))
assert l1 == l2, "LDS handle =="
assert l1 != ot.LowDiscrepancySequence(), "LDS handle !="

# Self equality
s = ot.SobolSequence(3)
assert s == s, "self =="
assert not s != s, "self !="
