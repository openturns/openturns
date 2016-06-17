import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


class GaussianConvolution(ot.OpenTURNSPythonDynamicalFunction):

    def __init__(self):
        super(GaussianConvolution, self).__init__(1, 1, 1)
        self.setInputDescription(["x"])
        self.setOutputDescription(["y"])
        self.outputGrid_ = ot.RegularGrid(-1.0, 0.02, 101)
        self.algo_ = ot.GaussKronrod(
            20, 1.0e-4, ot.GaussKronrodRule(ot.GaussKronrodRule.G7K15))

    def _exec(self, X):
        inputTG = X.getTimeGrid()
        inputValues = X.getValues()
        f = ot.NumericalMathFunction(ot.PiecewiseLinearEvaluationImplementation(
            [x[0] for x in inputTG.getVertices()], inputValues))
        outputValues = ot.NumericalSample(0, 1)
        for t in self.outputGrid_.getVertices():
            kernel = ot.Normal(t[0], 0.05)

            def pdf(X):
                return [kernel.computePDF(X)]
            weight = ot.NumericalMathFunction(ot.PythonFunction(1, 1, pdf))
            outputValues.add(self.algo_.integrate(
                weight * f, kernel.getRange()))
        return ot.Field(self.outputGrid_, outputValues)

N = 5
X = ot.TemporalNormalProcess(ot.GeneralizedExponential(
    [0.1], 1.0), ot.RegularGrid(-5.0, 0.1, 101))
f = ot.DynamicalFunction(GaussianConvolution())
Y = ot.CompositeProcess(f, X)
x_graph = X.getSample(N).drawMarginal(0)
y_graph = Y.getSample(N).drawMarginal(0)
fig = plt.figure(figsize=(10, 4))
plt.suptitle("Composite process")
x_axis = fig.add_subplot(121)
y_axis = fig.add_subplot(122)
View(x_graph, figure=fig, axes=[x_axis], add_legend=False)
View(y_graph, figure=fig, axes=[y_axis], add_legend=False)
