import openturns as ot
from openturns.viewer import View

N = 100
M = 1000
P = 10
mean = ot.SymbolicFunction("x", "sign(x)")
cov = ot.SquaredExponential([1.0], [0.1])
mesh = ot.IntervalMesher([N]).build(ot.Interval(-2.0, 2.0))
process = ot.GaussianProcess(ot.TrendTransform(mean, mesh), cov, mesh)
sample = process.getSample(M)
algo = ot.KarhunenLoeveSVDAlgorithm(sample, 1e-6)
algo.run()
result = algo.getResult()
trend = ot.TrendTransform(ot.P1LagrangeEvaluation(sample.computeMean()), mesh)
sample2 = process.getSample(P)
sample2.setName("reduction of sign(x) w/o trend")
reduced1 = ot.KarhunenLoeveReduction(result)(sample2)
reduced2 = ot.KarhunenLoeveReduction(result, trend)(sample2)
g = sample2.drawMarginal(0)
g.setColors(["red"])
g1 = reduced1.drawMarginal(0)
g1.setColors(["blue"])
drs = g1.getDrawables()
for i, d in enumerate(drs):
    d.setLineStyle("dashed")
    drs[i] = d
g1.setDrawables(drs)
g.add(g1)
g2 = reduced2.drawMarginal(0)
g2.setColors(["green"])
drs = g2.getDrawables()
for i, d in enumerate(drs):
    d.setLineStyle("dotted")
    drs[i] = d
g2.setDrawables(drs)
g.add(g2)
View(g)
