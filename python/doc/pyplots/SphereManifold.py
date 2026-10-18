import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import math

# The OpenTURNS drawables (ot.Cloud, ot.Curve, ot.Contour) are two-dimensional:
# the viewer draws them with ax.plot(x, y) / ax.scatter(x, y), so they cannot
# represent points, vectors or curves embedded in R^3. Both panels below are
# genuine 3-D views of S^2, hence they are drawn with matplotlib only. See
# SO3Manifold.py for the two-dimensional part of a manifold illustration.

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(10, 8))

# Unit sphere wireframe shared by both panels
u = np.linspace(0.0, 2.0 * math.pi, 40)
v = np.linspace(0.0, math.pi, 40)
x = np.outer(np.cos(u), np.sin(v))
y = np.outer(np.sin(u), np.sin(v))
z = np.outer(np.ones(np.size(u)), np.cos(v))

# ======================================================================
# Left panel: tangent basis at the north pole and exp-map
# ======================================================================
ax1 = fig.add_subplot(121, projection="3d")
ax1.plot_wireframe(x, y, z, color="lightblue", alpha=0.3, linewidth=0.5)

sphere = otexp.SphereManifold(3)
north = ot.Point([0.0, 0.0, 1.0])
ax1.scatter([north[0]], [north[1]], [north[2]], color="red", s=80,
            label="chart center", depthshade=False)

# The tangent space at the north pole is the plane z = 1: the tangent basis is
# an orthonormal pair of that plane
basis = sphere.getTangentBasis(north)
for k in range(basis.getSize()):
    direction = basis[k]
    ax1.quiver(north[0], north[1], north[2],
               direction[0], direction[1], direction[2],
               color="darkgreen", arrow_length_ratio=0.2, linewidth=2.5,
               label="tangent basis" if k == 0 else "")

# The exponential map pushes the tangent vectors onto great circles: scale the
# basis vectors by increasing norms and join the images to the base point
scales = [0.3, 0.7, 1.2]
for scale in scales:
    tangent = ot.Point([basis[0][i] * scale for i in range(3)])
    pushed = sphere.expMap(north, tangent)
    ax1.plot([north[0], pushed[0]], [north[1], pushed[1]],
             [north[2], pushed[2]],
             color="darkgreen", linewidth=1.5, alpha=0.7,
             label="exp-map" if scale == scales[0] else "")
    ax1.scatter([pushed[0]], [pushed[1]], [pushed[2]], color="darkgreen",
                s=30, depthshade=False)

ax1.set_xlim(-1.1, 1.1)
ax1.set_ylim(-1.1, 1.1)
ax1.set_zlim(-1.1, 1.1)
ax1.set_xlabel("$x$")
ax1.set_ylabel("$y$")
ax1.set_zlabel("$z$")
ax1.set_title("Tangent basis at the north pole\nand exp-map onto great circles")
ax1.legend(loc="upper left", fontsize=8)

# ======================================================================
# Right panel: geodesic distance between two points
# ======================================================================
ax2 = fig.add_subplot(122, projection="3d")
ax2.plot_wireframe(x, y, z, color="lightblue", alpha=0.3, linewidth=0.5)

p = ot.Point([1.0, 0.0, 0.0])
q = ot.Point([0.0, 1.0, 0.0])
ax2.scatter([p[0]], [p[1]], [p[2]], color="red", s=80, label="$p$",
            depthshade=False)
ax2.scatter([q[0]], [q[1]], [q[2]], color="blue", s=80, label="$q$",
            depthshade=False)

# The geodesic joining p and q is the great circle arc of the angle returned by
# getGeodesicDistance
angle = sphere.getGeodesicDistance(p, q)
arc = []
for t in np.linspace(0.0, 1.0, 100):
    weightP = math.sin((1.0 - t) * angle) / math.sin(angle)
    weightQ = math.sin(t * angle) / math.sin(angle)
    arc.append([weightP * p[i] + weightQ * q[i] for i in range(3)])
arc = np.array(arc)
ax2.plot(arc[:, 0], arc[:, 1], arc[:, 2], color="darkred", linewidth=2.5,
         label="geodesic")

# The logarithmic map pulls q back to the tangent space at p: the result is a
# tangent vector whose exp-map at p is q again
logMapVector = sphere.logMap(p, q)
ax2.quiver(p[0], p[1], p[2],
           logMapVector[0], logMapVector[1], logMapVector[2],
           color="darkgreen", arrow_length_ratio=0.2, linewidth=2.5,
           label="log-map at $p$")

ax2.set_xlim(-1.1, 1.1)
ax2.set_ylim(-1.1, 1.1)
ax2.set_zlim(-1.1, 1.1)
ax2.set_xlabel("$x$")
ax2.set_ylabel("$y$")
ax2.set_zlabel("$z$")
ax2.set_title("Geodesic distance $= {:.3f}$ rad".format(angle))
ax2.legend(loc="upper left", fontsize=8)

fig.suptitle("SphereManifold ($S^2$)")
fig.tight_layout()
