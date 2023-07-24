import openturns as ot
import openturns.experimental as otexp
from openturns.viewer import View

ot.ResourceMap.SetAsUnsignedInteger("Distribution-DefaultIntegrationNodesNumber", 64)
ot.ResourceMap.SetAsUnsignedInteger("Mesh-LargeSize", 0)

grid = ot.GridLayout(2, 2)
threshold = 0.2
inVars = ["x1", "x2"]
formula = "sin(x1)*sin(x2)"
f = ot.SymbolicFunction(inVars, [formula])
levelSet = ot.LevelSet(f, ot.Less(), threshold)
xMin = [-5.0] * 2
xMax = [5.0] * 2
box = ot.Interval(xMin, xMax)
mesh = ot.LevelSetMesher([51] * 2).build(levelSet, box, False)
pdf_2d = ot.Graph("Normal distribution over a mesh, PDF", "x1", "x2", True)
cdf_2d = ot.Graph("Normal distribution over a mesh, CDF", "x1", "x2", True)
dist = ot.Normal([0] * 2, [2] * 2)
distribution_2d = otexp.TruncatedOverMesh(dist, mesh)
mesh_g = mesh.draw()
mesh_g.setLegends([""])
mesh_g.setTitle("Mesh of domain D={" + formula + "<=" + str(threshold) + "}")
mesh_g.setXTitle("x1")
mesh_g.setYTitle("x2")

iso = f.draw(xMin, xMax)
iso.setTitle("Normal sample on D")
dr = iso.getDrawable(0)
dr.setLevels([threshold])
iso.setDrawables([dr])
iso.add(ot.Cloud(distribution_2d.getSample(1500)))
grid.setGraph(0, 0, mesh_g)
grid.setGraph(0, 1, iso)

pdf_2d.add(distribution_2d.drawPDF(xMin, xMax))
pdf_2d.setLegends([""])
cdf_2d.add(distribution_2d.drawCDF(xMin, xMax, [41] * 2))
cdf_2d.setLegends([""])
grid.setGraph(1, 0, pdf_2d)
grid.setGraph(1, 1, cdf_2d)
grid.setTitle("TruncatedOverMesh")
grid.setLegendPosition("topright")
v = View(grid)
fig = v.getFigure()
fig.axes[1].legend(loc="best")
