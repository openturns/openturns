import openturns as ot
from math import pi

ot.TESTPREAMBLE()
ot.Log.Show(0)
ot.PlatformInfo.SetNumericalPrecision(3)


dim = 2
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.5
src = [
    ot.JointDistribution([ot.Uniform(-pi, pi)] * dim),
    ot.JointDistribution([ot.Normal(4.0, 2.0)] * dim),
    ot.JointDistribution([ot.Gamma()] * dim),
    ot.JointDistribution([ot.Gamma(1.5, 2.5, -0.5)] * dim),
    ot.JointDistribution([ot.Arcsine(5.2, 11.6)] * dim),
    ot.Normal([3.0] * dim, [2.0] * dim, R),
]

for sd in src:
    sample = sd.getSample(2000)
    d = ot.MetaModelAlgorithm.BuildDistribution(sample)
    print(d)

sample = ot.Sample(
    [[0], [142.857], [285.714], [428.571], [571.429], [714.286], [857.143], [1000.0]]
)
distribution = ot.MetaModelAlgorithm.BuildDistribution(sample)
print(distribution)
print("distribution (HTML)")
print(distribution._repr_html_())
print("distribution (Markdown)")
print(distribution.__repr_markdown__())
