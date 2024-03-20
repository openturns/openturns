#! /usr/bin/env python

import openturns as ot

# ot.Log.Show(ot.Log.ALL)

coll = []

# case 1: no transformation
coll.append([ot.Normal(), ot.Normal()])

# case 2: same copula
left = ot.JointDistribution([ot.Normal(), ot.Gumbel()], ot.IndependentCopula(2))
right = ot.JointDistribution([ot.Triangular()] * 2, ot.IndependentCopula(2))
coll.append([left, right])

# case 3: same standard space
left = ot.JointDistribution([ot.Normal(), ot.Gumbel()], ot.IndependentCopula(2))
right = ot.JointDistribution([ot.Triangular()] * 2, ot.GumbelCopula())
coll.append([left, right])

# TODO case 4: different standard space

for left, right in coll:
    transformation = ot.DistributionTransformation(left, right)
    print("left=", left)
    print("right=", right)
    print("transformation=", transformation)
    inverseTransformation = transformation.inverse()
    print("inverseTransformation=", inverseTransformation)
    print("-" * 100)

# with marginaltransformation
copula = ot.BlockIndependentCopula([ot.IndependentCopula(2), ot.IndependentCopula(2)])
distribution = ot.JointDistribution([ot.Normal()] * 4, copula)
print(
    ot.DistributionTransformation(
        ot.IndependentCopula(4), distribution.getMarginal([3, 1, 2, 0])
    )
)
