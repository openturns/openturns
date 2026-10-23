import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import openturns.viewer as otv
import math

# The OpenTURNS drawables (ot.Cloud, ot.Curve, ot.Contour) are two-dimensional:
# the viewer draws them with ax.plot(x, y) / ax.scatter(x, y), so they cannot
# represent points of the sphere embedded in R^3. The left panel is a genuine
# 3-D view and is drawn with matplotlib only, while the right panel is
# two-dimensional and uses the OpenTURNS Graph/Curve drawables and the OpenTURNS
# viewer.

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(11, 5))

# ======================================================================
# Left panel: realizations pushed onto the sphere through the exp-map
# ======================================================================
ax1 = fig.add_subplot(121, projection="3d")

# Unit sphere wireframe
u = np.linspace(0.0, 2.0 * math.pi, 40)
v = np.linspace(0.0, math.pi, 40)
ax1.plot_wireframe(np.outer(np.cos(u), np.sin(v)),
                   np.outer(np.sin(u), np.sin(v)),
                   np.outer(np.ones(np.size(u)), np.cos(v)),
                   color="lightgray", alpha=0.3, linewidth=0.5)

manifold = otexp.Manifold(otexp.SphereManifold(3))
background = ot.Normal(2)
chartCenter = [0.0, 0.0, 1.0]
distribution = otexp.ManifoldMappedDistribution(background, manifold, chartCenter)

realizations = np.array(distribution.getSample(300))
ax1.scatter(realizations[:, 0], realizations[:, 1], realizations[:, 2],
            color="darkred", s=6, alpha=0.5, label="realizations",
            depthshade=False)
ax1.scatter([chartCenter[0]], [chartCenter[1]], [chartCenter[2]],
            color="red", s=90, marker="+", label="chart center",
            depthshade=False)

ax1.set_xlim(-1.1, 1.1)
ax1.set_ylim(-1.1, 1.1)
ax1.set_zlim(-1.1, 1.1)
ax1.set_xlabel("$x$")
ax1.set_ylabel("$y$")
ax1.set_zlabel("$z$")
ax1.set_title("Realizations of a $\\mathcal{N}(2)$ background\n"
              "pushed onto $S^2$ through the exp-map")
ax1.legend(loc="upper left", fontsize=8)

# ======================================================================
# Right panel: density along the great circle through the chart center,
# a two-dimensional plot drawn with the OpenTURNS Graph/Curve drawables
# ======================================================================
ax2 = fig.add_subplot(122)

# theta is the geodesic distance from the chart center; the density diverges at
# the antipode, where the chart collapses, hence the truncated range
nPoints = 129
theta = [0.999 * math.pi * i / (nPoints - 1) for i in range(nPoints)]
pdf = [distribution.computePDF([math.sin(value), 0.0, math.cos(value)])
       for value in theta]

graph = ot.Graph("Density is not uniform on the sphere",
                 "geodesic distance $\\theta$ from the chart center",
                 "density with respect to the Riemannian volume", True, "best")
graph.add(ot.Curve(ot.Sample([[theta[i], pdf[i]] for i in range(nPoints)]),
                   "darkgreen", "solid", 2.5, "pushed-forward density"))
# The background density at the same radius: the ratio of the two curves is the
# chord-to-area correction $\theta/\sin(\theta)$ of the exp-map
backgroundPdf = [background.computePDF([value, 0.0]) for value in theta]
graph.add(ot.Curve(ot.Sample([[theta[i], backgroundPdf[i]]
                             for i in range(nPoints)]),
                   "grey", "dashed", 1.5, "background density"))
otv.View(graph, figure=fig, axes=[ax2], add_legend=True)
ax2.set_title("The density grows like $\\theta/\\sin(\\theta)$ towards the antipode")
ax2.set_xlim(0.0, 0.999 * math.pi)
ax2.set_ylim(0.0, 1.05 * max(pdf))

fig.suptitle("ManifoldMappedDistribution on $S^2$")
fig.tight_layout()
