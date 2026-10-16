import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
import math

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(10, 5))
ax1 = fig.add_subplot(121)
ax2 = fig.add_subplot(122)

# Target manifold: the unit disk in R^2
theta = [2.0 * math.pi * i / 200 for i in range(201)]
circleX = [math.cos(t) for t in theta]
circleY = [math.sin(t) for t in theta]

# Case n=1, p=2: reference segment [-1, 1] mapped by a P1 field onto a
# parabola inside the disk. The image mesh is a polyline (1D manifold).
nSeg = 40
refVertices1 = ot.Sample([[-1.0 + 2.0 * i / nSeg] for i in range(nSeg + 1)])
refSimplices1 = ot.IndicesCollection([[i, i + 1] for i in range(nSeg)])
refMesh1 = ot.Mesh(refVertices1, refSimplices1)
imgVertices1 = ot.Sample(
    [[0.8 * v[0], 0.4 * (v[0] * v[0] - 0.5)] for v in refVertices1]
)
field1 = ot.Field(refMesh1, imgVertices1)
dist1 = otexp.PushForwardOverMesh(field1)
# Evaluate the PDF at the mapped segment midpoints: interior vertices are
# shared by two segments and their first-match value depends on segment
# order, while midpoints lie strictly inside one image segment each.
midpoints1 = ot.Sample(
    [[0.5 * (imgVertices1[i][0] + imgVertices1[i + 1][0]),
      0.5 * (imgVertices1[i][1] + imgVertices1[i + 1][1])]
     for i in range(nSeg)]
)
pdf1 = [dist1.computePDF(p) for p in midpoints1]
ax1.plot(circleX, circleY, "k--", lw=1.0, label="unit disk")
sc1 = ax1.scatter(
    [p[0] for p in midpoints1], [p[1] for p in midpoints1], c=pdf1, cmap="viridis", s=12
)
ax1.set_aspect("equal", adjustable="box")
ax1.set_title("n=1, p=2: PDF on curve mesh")
ax1.set_xlabel(r"$y_0$")
ax1.set_ylabel(r"$y_1$")
plt.colorbar(sc1, ax=ax1, label="PDF")

# Case n=2, p=2: reference mesh of the unit disk (LevelSetMesher) mapped by
# the identity P1 field. The push-forward reduces to the uniform distribution
# over the disk mesh; the PDF is constant 1/area.
level = ot.LevelSet(ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1^2 - 1.0"]), ot.Less(), 0.0)
box = ot.Interval([-1.0, -1.0], [1.0, 1.0])
refMesh2 = ot.LevelSetMesher([20, 20]).build(level, box, False)
imgMesh2 = refMesh2
field2 = ot.Field(refMesh2, refMesh2.getVertices())
dist2 = otexp.PushForwardOverMesh(field2)
verts2 = imgMesh2.getVertices()
pdf2 = dist2.computePDF(verts2[0])
ax2.plot(circleX, circleY, "k--", lw=1.0)
# Draw the mesh support lightly and annotate the constant PDF
meshGraph = imgMesh2.draw()
# Use a scatter colored by the (constant) PDF for the legend
sc2 = ax2.scatter([v[0] for v in verts2], [v[1] for v in verts2], c=[pdf2] * len(verts2), cmap="viridis", s=4)
ax2.set_aspect("equal", adjustable="box")
ax2.set_title(f"n=2, p=2: PDF on disk mesh (PDF={pdf2:.3f})")
ax2.set_xlabel(r"$y_0$")
ax2.set_ylabel(r"$y_1$")
plt.colorbar(sc2, ax=ax2, label="PDF")

# Note: the case n=3, p=2 (co-area formula) is not implemented yet, see the
# class documentation.

fig.suptitle("PushForwardOverMesh on the unit disk")
fig.tight_layout()
