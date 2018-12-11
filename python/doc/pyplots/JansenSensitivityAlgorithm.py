import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
ot.RandomGenerator.SetSeed(0)


def flooding(X):
    Hd = 3.0
    Zb = 55.5
    L = 5.0e3
    B = 300.0
    Zd = Zb + Hd
    Q, Ks, Zv, Zm = X
    alpha = (Zm - Zv) / L
    H = (Q / (Ks * B * alpha**0.5))**0.6
    Zc = H + Zv
    S = Zc - Zd
    return [S]


myFunction = ot.PythonFunction(4, 1, flooding)
myParam = ot.GumbelAB(1013.0, 558.0)
Q = ot.ParametrizedDistribution(myParam)
Q = ot.TruncatedDistribution(Q, 0.0, ot.SpecFunc.MaxScalar)
Ks = ot.Normal(30.0, 7.5)
Ks = ot.TruncatedDistribution(Ks, 0.0, ot.SpecFunc.MaxScalar)
Zv = ot.Uniform(49.0, 51.0)
Zm = ot.Uniform(54.0, 56.0)
inputX = ot.ComposedDistribution([Q, Ks, Zv, Zm])
inputX.setDescription(["Q", "Ks", "Zv", "Zm"])

size = 5000
computeSO = True
inputDesign = ot.SobolIndicesExperiment(inputX, size, computeSO).generate()
outputDesign = myFunction(inputDesign)
sensitivityAnalysis = ot.JansenSensitivityAlgorithm(
    inputDesign, outputDesign, size)

graph = sensitivityAnalysis.draw()

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)
