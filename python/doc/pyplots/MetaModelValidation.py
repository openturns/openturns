import openturns as ot
from openturns.viewer import View

dist = ot.Uniform(-3.0, 3.0)
model = ot.SymbolicFunction(["x"], ["sin(x)"])
metaModel = ot.SymbolicFunction(["x"], ["x - x^3 / 6.0 + x^5 / 120.0"])
x = dist.getSample(100)
y = model(x)
val = ot.MetaModelValidation(x, y, metaModel)
graph = val.drawValidation()
View(graph)
