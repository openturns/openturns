import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv

# Create the mesher
mesher = ot.LevelSetMesher([50] * 2)

# Create a level set
function = ot.SymbolicFunction(["x0", "x1"], ["10*(x0^3+x1)^2+x0^2"])
level = 0.5
set = ot.LevelSet(function, ot.LessOrEqual(), level)

# Mesh the level set
mesh = mesher.build(set, ot.Interval([-1.0] * 2, [1.0] * 2))

# Intersection of two level sets: lens of two unit disks,
# meshed with the QEF (sharp-edge) placement
disk1 = ot.SymbolicFunction(["x", "y"], ["(x+0.5)^2+y^2"])
disk2 = ot.SymbolicFunction(["x", "y"], ["(x-0.5)^2+y^2"])
lens = [ot.LevelSet(disk1, ot.LessOrEqual(), 1.0), ot.LevelSet(disk2, ot.LessOrEqual(), 1.0)]
lensMesher = ot.LevelSetMesher([20] * 2)
lensMesher.setUseQEF(True)
lensMesh = lensMesher.build(lens, ot.Interval([-1.6] * 2, [1.6] * 2))

# Draw both meshes
graph = mesh.draw()
graph.setXTitle("$x_0$")
graph.setYTitle("$x_1$")
graph.setTitle("Mesh of a level set")
lensGraph = lensMesh.draw()
lensGraph.setXTitle("$x$")
lensGraph.setYTitle("$y$")
lensGraph.setTitle(f"Lens intersection (QEF), volume={lensMesh.getVolume():.4f}")

fig = plt.figure(figsize=(10, 4))
graph_axis = fig.add_subplot(121)
graph_axis.set_xlim(auto=True)
lens_axis = fig.add_subplot(122)
lens_axis.set_xlim(auto=True)

otv.View(graph, figure=fig, axes=[graph_axis], add_legend=True)
otv.View(lensGraph, figure=fig, axes=[lens_axis], add_legend=True)
