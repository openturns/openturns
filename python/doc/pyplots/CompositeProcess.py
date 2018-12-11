import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

mesh = ot.RegularGrid(-5.0, 0.1, 101)


class GaussianConvolution(ot.OpenTURNSPythonFieldFunction):

    def __init__(self):
        outputGrid = ot.RegularGrid(-1.0, 0.02, 101)
        super(GaussianConvolution, self).__init__(mesh, 1, outputGrid, 1)
        self.setInputDescription(["x"])
        self.setOutputDescription(["y"])
        self.algo_ = ot.GaussKronrod(
            20, 1.0e-4, ot.GaussKronrodRule(ot.GaussKronrodRule.G7K15))

    def _exec(self, X):
        f = ot.Function(ot.PiecewiseLinearEvaluation(
            [x[0] for x in self.getInputMesh().getVertices()], X))
        outputValues = ot.Sample(0, 1)
        for t in self.getOutputMesh().getVertices():
            kernel = ot.Normal(t[0], 0.05)

            def pdf(X):
                return [kernel.computePDF(X)]
            weight = ot.Function(ot.PythonFunction(1, 1, pdf))
            outputValues.add(self.algo_.integrate(
                weight * f, kernel.getRange()))
        return outputValues


N = 5
X = ot.GaussianProcess(ot.GeneralizedExponential(
    [0.1], 1.0), mesh)
f = ot.FieldFunction(GaussianConvolution())
Y = ot.CompositeProcess(f, X)
x_graph = X.getSample(N).drawMarginal(0)
y_graph = Y.getSample(N).drawMarginal(0)
fig = plt.figure(figsize=(10, 4))
x_axis = fig.add_subplot(121)
y_axis = fig.add_subplot(122)
View(x_graph, figure=fig, axes=[x_axis], add_legend=False)
View(y_graph, figure=fig, axes=[y_axis], add_legend=False)
fig.suptitle("Composite process")
