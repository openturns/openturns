import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import sqrt

mesh = ot.IntervalMesher([128]).build(ot.Interval(-1.0, 1.0))
threshold = 0.001
covariance = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, covariance, threshold)
algo.run()
process = ot.GaussianProcess(covariance, mesh)
sample = process.getSample(100)
validation = ot.KarhunenLoeveValidation(sample, algo.getResult())
g = validation.drawValidation()

fig = plt.figure(figsize=(6, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
