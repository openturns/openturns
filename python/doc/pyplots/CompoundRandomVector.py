import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
distXgivenT = ot.Exponential()
distGamma = ot.Uniform(1.0, 2.0)
distLambda = ot.Uniform(0.0, 1)
distTheta = ot.JointDistribution([distLambda, distGamma])
rvTheta = ot.RandomVector(distTheta)

rvX = ot.CompoundRandomVector(distXgivenT, rvTheta)
sampleX = rvX.getSample(1000)

histX = ot.HistogramFactory().build(sampleX)
graph = histX.drawPDF()
graph.setXTitle("x")
graph.setYTitle("pdf")
graph.setTitle(
    r"Conditional Random Vector: Exp( $\lambda$, $\gamma$), $\lambda \sim \mathcal{U}(0,1)$, $\gamma \sim \mathcal{U}(1,2)$"
)

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)

View(graph, figure=fig, axes=[axis], add_legend=False)
