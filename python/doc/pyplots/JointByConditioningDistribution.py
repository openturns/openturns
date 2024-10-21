import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

conditioningDist = ot.Normal(0.0, 1.0)
link = ot.SymbolicFunction(["y"], ["y", "0.1+y^2"])
conditionedDist = ot.Normal()
distribution = ot.JointByConditioningDistribution(
    conditionedDist, conditioningDist, link
)
distribution.setDescription(["$Y$", "$X$"])

pdf_graph = distribution.drawPDF([-1.0] * 2, [1.0] * 2)
pdf_graph.setTitle(str(distribution))
fig = plt.figure(figsize=(10, 5))
pdf_axis = fig.add_subplot(111)
View(pdf_graph, figure=fig, axes=[pdf_axis], add_legend=False)
