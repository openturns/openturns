import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
distXgivenT = ot.Exponential()
distGamma = ot.Uniform(1.0, 2.0)
distAlpha = ot.Uniform(0.0, 0.1)
distTheta = ot.JointDistribution([distGamma, distAlpha])
rvTheta = ot.RandomVector(distTheta)

rvX = ot.DeconditionedRandomVector(distXgivenT, rvTheta)
sampleX = rvX.getSample(1000)

histX = ot.HistogramFactory().build(sampleX)
graph = histX.drawPDF()
graph.setXTitle("x")
graph.setYTitle("pdf")
graph.setTitle(
    r"Conditional Random Vector: Exp($\gamma$, $\lambda$), $\gamma \sim \mathcal{U}(1,2)$, $\lambda \sim \mathcal{U}(0,1)$"
)

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)

View(graph, figure=fig, axes=[axis], add_legend=False)
