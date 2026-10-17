import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import openturns.viewer as otv
import math

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(10, 8))

# ======================================================================
# Left panel: tangent basis at north pole + exp-map great circles
# ======================================================================
ax1 = fig.add_subplot(121, projection='3d')

# Unit sphere wireframe (matplotlib - OpenTURNS has no surface drawable)
u = np.linspace(0, 2 * math.pi, 40)
v = np.linspace(0, math.pi, 40)
x = np.outer(np.cos(u), np.sin(v))
y = np.outer(np.sin(u), np.sin(v))
z = np.outer(np.ones(np.size(u)), np.cos(v))
ax1.plot_wireframe(x, y, z, color='lightblue', alpha=0.3, edgecolor='gray', linewidth=0.5)

sphere = otexp.SphereManifold(3)
north = ot.Point([0.0, 0.0, 1.0])

# North pole as OpenTURNS Cloud (projected to 2D for OpenTURNS, drawn with matplotlib for 3D)
ax1.scatter([north[0]], [north[1]], [north[2]], color='red', s=80,
            label='chart center', depthshade=False)

# Tangent basis at north pole - draw as OpenTURNS Curve on a 2D tangent plane projection
# The tangent plane at north pole is the xy-plane (z=0), so Curve works natively in 2D
basis = sphere.getTangentBasis(north)
for k in range(basis.getSize()):
    b = basis[k]
    # Tangent vector from origin in tangent plane (2D OpenTURNS Curve)
    tangent_curve = ot.Curve(ot.Sample([[0.0, 0.0], [b[0], b[1]]]),
                             "darkgreen", "solid", 2,
                             f"tangent {k + 1}" if k == 0 else "")
    graph_tangent = ot.Graph()
    graph_tangent.add(tangent_curve)
    # Render on the 3D axes using OpenTURNS viewer
    otv.View(graph_tangent, figure=fig, axes=[ax1], add_legend=False)

    # Also draw as matplotlib 3D quiver for proper 3D appearance
    ax1.quiver(0.0, 0.0, 1.0, b[0], b[1], b[2],
               color='darkgreen', arrow_length_ratio=0.12, linewidth=2.5,
               label='tangent basis' if k == 0 else "")

# expMap: push tangent vectors onto sphere - draw curves using OpenTURNS Curve
# The expMap of tangent vectors at north pole traces great circles on the sphere.
# We project the 3D curve onto the tangent plane for OpenTURNS Curve, then also
# draw the true 3D curve with matplotlib.
for k in range(basis.getSize()):
    b = basis[k]
    for scale in [0.3, 0.7, 1.2]:
        tangent = ot.Point([b[0] * scale, b[1] * scale, b[2] * scale])
        pushed = sphere.expMap(north, tangent)

        # OpenTURNS Curve: preimage line in tangent plane (2D) + pushed curve
        # Project pushed point to tangent plane coordinates for OpenTURNS
        pushed_curve = ot.Curve(ot.Sample([[0.0, 0.0], [pushed[0], pushed[1]]]),
                                "darkgreen", "solid", 1.5, "")
        graph_push = ot.Graph()
        graph_push.add(pushed_curve)
        otv.View(graph_push, figure=fig, axes=[ax1], add_legend=False)

        # True 3D curve using matplotlib
        ax1.plot([0.0, pushed[0]], [0.0, pushed[1]], [1.0, pushed[2]],
                 color='darkgreen', linewidth=1.5, alpha=0.7)
        ax1.scatter([pushed[0]], [pushed[1]], [pushed[2]],
                    color='darkgreen', s=30, depthshade=False)

ax1.set_xlim([-1.1, 1.1])
ax1.set_ylim([-1.1, 1.1])
ax1.set_zlim([-1.1, 1.1])
ax1.set_xlabel('$x$')
ax1.set_ylabel('$y$')
ax1.set_zlabel('$z$')
ax1.set_title('SphereManifold: tangent basis at north pole,\nexp-map onto great circles')
ax1.legend(loc='upper left', fontsize=8)

# ======================================================================
# Right panel: geodesic distance between two points
# ======================================================================
ax2 = fig.add_subplot(122, projection='3d')
ax2.plot_wireframe(x, y, z, color='lightblue', alpha=0.3, edgecolor='gray', linewidth=0.5)

p = ot.Point([1.0, 0.0, 0.0])
q = ot.Point([0.0, 1.0, 0.0])
ax2.scatter([p[0]], [p[1]], [p[2]], color='red', s=80, label='$p$', depthshade=False)
ax2.scatter([q[0]], [q[1]], [q[2]], color='blue', s=80, label='$q$', depthshade=False)

# Geodesic arc using OpenTURNS Curve (projected to xy-plane for OpenTURNS,
# drawn in 3D with matplotlib)
angle = sphere.getGeodesicDistance(p, q)
arc_points = []
t_vals = np.linspace(0, 1, 100)
for t in t_vals:
    w = math.sin((1 - t) * angle) / math.sin(angle)
    w2 = math.sin(t * angle) / math.sin(angle)
    pt = ot.Point([w * p[i] + w2 * q[i] for i in range(3)])
    arc_points.append([pt[0], pt[1]])
arc_curve = ot.Curve(ot.Sample(arc_points), "darkred", "solid", 2.5, "geodesic")
graph_arc = ot.Graph()
graph_arc.add(arc_curve)
otv.View(graph_arc, figure=fig, axes=[ax2], add_legend=False)

# True 3D geodesic arc with matplotlib
arc_x, arc_y, arc_z = [], [], []
for t in t_vals:
    w = math.sin((1 - t) * angle) / math.sin(angle)
    w2 = math.sin(t * angle) / math.sin(angle)
    pt = ot.Point([w * p[i] + w2 * q[i] for i in range(3)])
    arc_x.append(pt[0])
    arc_y.append(pt[1])
    arc_z.append(pt[2])
ax2.plot(arc_x, arc_y, arc_z, color='darkred', linewidth=2.5, label='geodesic')

# Tangent vector at p (log map) - drawn with OpenTURNS Curve in tangent plane
log_pq = sphere.logMap(p, q)
log_curve = ot.Curve(ot.Sample([[0.0, 0.0], [log_pq[0], log_pq[1]]]),
                     "darkgreen", "solid", 2, "log map at p")
graph_log = ot.Graph()
graph_log.add(log_curve)
otv.View(graph_log, figure=fig, axes=[ax2], add_legend=False)

# Also draw as matplotlib 3D quiver for proper 3D appearance
ax2.quiver(p[0], p[1], p[2], log_pq[0], log_pq[1], log_pq[2],
           color='darkgreen', arrow_length_ratio=0.15, linewidth=2,
           label='log map at $p$')

ax2.set_xlim([-1.1, 1.1])
ax2.set_ylim([-1.1, 1.1])
ax2.set_zlim([-1.1, 1.1])
ax2.set_xlabel('$x$')
ax2.set_ylabel('$y$')
ax2.set_zlabel('$z$')
ax2.set_title('SphereManifold: geodesic distance = {:.3f} rad'.format(angle))
ax2.legend(loc='upper left', fontsize=8)

fig.suptitle('SphereManifold ($\\mathcal{S}^2$)')
fig.tight_layout()
plt.show()
