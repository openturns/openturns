import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import sqrt

domain = ot.Interval(-1.0, 1.0)
basis = ot.OrthogonalProductFunctionFactory([ot.FourierSeriesFactory()])
basisSize = 10
experiment = ot.GaussProductExperiment(basis.getMeasure(), [20])
mustScale = False
threshold = 0.001
model = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveQuadratureAlgorithm(
    domain, model, experiment, basis, basisSize, mustScale, threshold)
algo.run()
ev = algo.getResult().getEigenValues()
functions = algo.getResult().getScaledModes()
g = ot.Graph()
g.setXTitle("$t$")
g.setYTitle("$\sqrt{\lambda_n}\phi_n$")
for i in range(functions.getSize()):
    g.add(functions.build(i).draw(-1.0, 1.0, 256))
g.setColors(ot.Drawable.BuildDefaultPalette(functions.getSize()))

fig = plt.figure(figsize=(6, 4))
plt.suptitle("Quadrature approx. of KL expansion for $C(s,t)=e^{-|s-t|}$")
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
