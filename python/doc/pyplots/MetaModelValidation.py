import openturns as ot
import openturns.viewer as otv

# Define the model
dist = ot.Uniform(-3.0, 3.0)
model = ot.SymbolicFunction(["x"], ["sin(x)"])

# Build the metamodel
x_train = dist.getSample(25)
y_train = model(x_train)
total_degree = 3
polynomialCollection = ["x^%d" % (degree) for degree in range(1, total_degree + 1)]
basis = ot.SymbolicFunction(["x"], polynomialCollection)
designMatrix = basis(x_train)
algo = ot.LinearLeastSquares(designMatrix, y_train)
algo.run()
leastSquaresModel = algo.getResult().getMetaModel()
metaModel = ot.ComposedFunction(leastSquaresModel, basis)

# Validate the metamodel
x_test = dist.getSample(100)
y_test = model(x_test)
metamodelPredictions = metaModel(x_test)
val = ot.MetaModelValidation(y_test, metamodelPredictions)
graph = val.drawValidation()
otv.View(graph)
