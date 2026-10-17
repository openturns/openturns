import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import math

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(10, 8))

# ---- Left: sphere with tangent basis at north pole ----
ax1 = fig.add_subplot(121, projection='3d')

# Draw the unit sphere
u = np.linspace(0, 2 * math.pi, 60)
v = np.linspace(0, math.pi, 60)
x = np.outer(np.cos(u), np.sin(v))
y = np.outer(np.sin(u), np.sin(v))
z = np.outer(np.ones(np.size(u)), np.cos(v))
ax1.plot_surface(x, y, z, color='lightblue', alpha=0.3, edgecolor='none')

# North pole (chart center)
north = ot.Point([0.0, 0.0, 1.0])
ax1.scatter([north[0]], [north[1]], [north[2]], color='red', s=60, label='chart center', depthshade=False)

# Tangent basis at north pole
sphere = otexp.SphereManifold(3)
basis = sphere.getTangentBasis(north)
for k in range(basis.getSize()):
    b = basis[k]
    # Draw tangent vector as arrow in the tangent plane
    ax1.quiver(north[0], north[1], north[2],
               b[0], b[1], b[2],
               color='green', arrow_length_ratio=0.15, linewidth=2,
               label='tangent basis' if k == 0 else "")

# Exponential map: push tangent vectors onto the sphere
for k in range(basis.getSize()):
    b = basis[k]
    # expMap of tangent vector scaled by angle
    for scale in [0.3, 0.7, 1.2]:
        tangent = ot.Point([b[0] * scale, b[1] * scale, b[2] * scale])
        pushed = sphere.expMap(north, tangent)
        ax1.plot([north[0], pushed[0]], [north[1], pushed[1]], [north[2], pushed[2]],
                 color='darkgreen', linewidth=1.5, alpha=0.7)
        ax1.scatter([pushed[0]], [pushed[1]], [pushed[2]], color='darkgreen', s=30, depthshade=False)

ax1.set_xlim([-1.1, 1.1])
ax1.set_ylim([-1.1, 1.1])
ax1.set_zlim([-1.1, 1.1])
ax1.set_xlabel('$x$')
ax1.set_ylabel('$y$')
ax1.set_zlabel('$z$')
ax1.set_title(r'SphereManifold ($\mathcal{S}^2$): tangent basis at north pole,\nexp-map pushing onto great circles')
ax1.legend(loc='upper left', fontsize=8)

# ---- Right: geodesic distance between two points ----
ax2 = fig.add_subplot(122, projection='3d')
ax2.plot_surface(x, y, z, color='lightblue', alpha=0.3, edgecolor='none')

p = ot.Point([1.0, 0.0, 0.0])
q = ot.Point([0.0, 1.0, 0.0])
ax2.scatter([p[0]], [p[1]], [p[2]], color='red', s=80, label='$p$', depthshade=False)
ax2.scatter([q[0]], [q[1]], [q[2]], color='blue', s=80, label='$q$', depthshade=False)

# Geodesic arc (great circle) between p and q
angle = sphere.getGeodesicDistance(p, q)
t_vals = np.linspace(0, 1, 100)
arc_x = []
arc_y = []
arc_z = []
for t in t_vals:
    # Spherical interpolation
    w = math.sin((1 - t) * angle) / math.sin(angle)
    w2 = math.sin(t * angle) / math.sin(angle)
    pt = ot.Point([w * p[i] + w2 * q[i] for i in range(3)])
    arc_x.append(pt[0])
    arc_y.append(pt[1])
    arc_z.append(pt[2])
ax2.plot(arc_x, arc_y, arc_z, color='darkred', linewidth=2.5, label='geodesic')

# Tangent vector at p pointing to q (log map)
log_pq = sphere.logMap(p, q)
ax2.quiver(p[0], p[1], p[2],
           log_pq[0], log_pq[1], log_pq[2],
           color='darkgreen', arrow_length_ratio=0.15, linewidth=2,
           label='log map at $p$')

ax2.set_xlim([-1.1, 1.1])
ax2.set_ylim([-1.1, 1.1])
ax2.set_zlim([-1.1, 1.1])
ax2.set_xlabel('$x$')
ax2.set_ylabel('$y$')
ax2.set_zlabel('$z$')
ax2.set_title(r'SphereManifold ($\mathcal{S}^2$): geodesic distance = ' + f'{angle:.3f}' + ' rad')
ax2.legend(loc='upper left', fontsize=8)

fig.suptitle(r'SphereManifold ($\mathcal{S}^2$)')
fig.tight_layout()
