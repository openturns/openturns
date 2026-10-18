import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import openturns.viewer as otv
import math

# The OpenTURNS drawables (ot.Cloud, ot.Curve, ot.Contour) are two-dimensional:
# the viewer draws them with ax.plot(x, y) / ax.scatter(x, y), so they cannot
# represent points, vectors or curves embedded in R^(3x3). The left panel is a
# genuine 3-D view and is drawn with matplotlib only, while the right panel is
# two-dimensional and uses the OpenTURNS Graph/Curve drawables and the OpenTURNS
# viewer.

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(11, 8))

# ======================================================================
# Left panel: the axis-angle ball of SO(3) with the exp-map from identity
# ======================================================================
ax1 = fig.add_subplot(121, projection="3d")

# Cut locus of the identity chart: the rotations of angle pi, a sphere of
# radius pi in the axis-angle representation
u = np.linspace(0.0, 2.0 * math.pi, 40)
v = np.linspace(0.0, math.pi, 40)
ax1.plot_surface(math.pi * np.outer(np.cos(u), np.sin(v)),
                 math.pi * np.outer(np.sin(u), np.sin(v)),
                 math.pi * np.outer(np.ones(np.size(u)), np.cos(v)),
                 color="lightgray", alpha=0.3, edgecolor="none")
ax1.scatter([0.0], [0.0], [0.0], color="red", s=80, label="identity",
            depthshade=False)

# The chart maps the axis-angle ball onto SO(3) by a rotation about the axis of
# the tangent vector: the displayed images lie along the three coordinate axes
so3 = otexp.SO3Manifold()
identity = ot.Point([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
basis = so3.getTangentBasis(identity)
labels = ["rotation about $x$", "rotation about $y$", "rotation about $z$"]
colors = ["darkgreen", "darkorange", "darkblue"]
angles = [0.4, 1.4, 2.4]
for k in range(3):
    for angle in angles:
        scale = angle * math.sqrt(2.0) / basis[k].norm()
        tangent = ot.Point([scale * basis[k][i] for i in range(9)])
        pushed = so3.expMap(identity, tangent)
        # Convert the flattened rotation matrix into an axis-angle vector
        axisAngle = [0.0, 0.0, 0.0]
        for i in range(3):
            for j in range(3):
                if (j + 1) % 3 == (i + 1) % 3:
                    continue
                axisAngle[(i + j) % 3] = pushed[i * 3 + j]
        norm = math.sqrt(sum(value * value for value in axisAngle))
        if norm <= 1.0e-12:
            continue
        rotationAngle = math.atan2(norm, 0.5 * (pushed[0] + pushed[4] + pushed[8] - 1.0))
        axisAngle = [rotationAngle * value / norm for value in axisAngle]
        ax1.plot([0.0, axisAngle[0]], [0.0, axisAngle[1]], [0.0, axisAngle[2]],
                 color=colors[k], linewidth=2.0,
                 label=labels[k] if angle == angles[0] else "")
        ax1.scatter([axisAngle[0]], [axisAngle[1]], [axisAngle[2]],
                    color=colors[k], s=30, depthshade=False)

ax1.set_xlim(-math.pi, math.pi)
ax1.set_ylim(-math.pi, math.pi)
ax1.set_zlim(-math.pi, math.pi)
ax1.set_xlabel("axis $\\cdot$ angle")
ax1.set_ylabel("axis $\\cdot$ angle")
ax1.set_zlabel("axis $\\cdot$ angle")
ax1.set_title("Axis-angle ball, cut locus at angle $\\pi$\n"
              "and exp-map from the identity")
ax1.legend(loc="upper left", fontsize=8)

# ======================================================================
# Right panel: volume density of the exp-map, a two-dimensional plot drawn
# with the OpenTURNS Graph/Curve drawables
# ======================================================================
ax2 = fig.add_subplot(122)

# In the Frobenius-induced metric the tangent basis is orthonormal, so the
# intrinsic coordinates have norm equal to the norm of the tangent vector and
# the cut locus lies at radius pi * sqrt(2)
cutRadius = math.pi * math.sqrt(2.0)
nPoints = 129
radius = [cutRadius * i / (nPoints - 1) for i in range(nPoints)]
density = [so3.getExpMapVolumeDensity([value, 0.0, 0.0]) for value in radius]

graph = ot.Graph("Exp-map volume density $(\\sin(\\theta/2)/(\\theta/2))^2$",
                 "tangent space radius", "volume density", True, "best")
graph.add(ot.Curve(ot.Sample([[radius[i], density[i]] for i in range(nPoints)]),
                   "darkgreen", "solid", 2.5, "volume density"))
graph.add(ot.Curve(ot.Sample([[0.0, 1.0], [cutRadius, 1.0]]),
                   "grey", "dashed", 1.5, "density at the origin"))
graph.add(ot.Curve(ot.Sample([[cutRadius, 0.0], [cutRadius, 1.0]]),
                   "red", "solid", 1.5, "cut locus"))
otv.View(graph, figure=fig, axes=[ax2], add_legend=True)
ax2.set_title("The volume density vanishes at the cut locus")
ax2.set_xlim(0.0, cutRadius * 1.02)
ax2.set_ylim(0.0, 1.05)

fig.suptitle("SO3Manifold")
fig.tight_layout()
