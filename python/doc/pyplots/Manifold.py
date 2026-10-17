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
# Abstract Manifold interface illustrated on SphereManifold
# ======================================================================
ax = fig.add_subplot(111, projection='3d')

# Unit sphere wireframe (matplotlib - OpenTURNS has no surface drawable)
u = np.linspace(0, 2 * math.pi, 40)
v = np.linspace(0, math.pi, 40)
x_sphere = np.outer(np.cos(u), np.sin(v))
y_sphere = np.outer(np.sin(u), np.sin(v))
z_sphere = np.outer(np.ones(np.size(u)), np.cos(v))
ax.plot_wireframe(x_sphere, y_sphere, z_sphere, color='lightblue', alpha=0.3)

sphere = otexp.SphereManifold(3)

# Base point and tangent vector
base = ot.Point([0.0, 0.0, 1.0])
tangent = ot.Point([0.5, 0.5, 0.0])
tangent = ot.Point([tangent[i] / tangent.norm() for i in range(3)])  # normalize

ax.scatter([base[0]], [base[1]], [base[2]], color='red', s=80,
           label='base point', depthshade=False)

# Tangent vector at base point - drawn with OpenTURNS Curve on tangent plane
# (projected to xy-plane for OpenTURNS, drawn in 3D with matplotlib)
tangent_curve_2d = ot.Curve(ot.Sample([[0.0, 0.0], [tangent[0], tangent[1]]]),
                            "darkgreen", "solid", 2.5, "tangent vector")
graph_tangent = ot.Graph()
graph_tangent.add(tangent_curve_2d)
otv.View(graph_tangent, figure=fig, axes=[ax], add_legend=False)

# Also draw as matplotlib 3D quiver for proper 3D appearance
ax.quiver(base[0], base[1], base[2],
          tangent[0], tangent[1], tangent[2],
          color='darkgreen', arrow_length_ratio=0.15, linewidth=2.5)

# expMap: push tangent vector onto sphere
pushed = sphere.expMap(base, tangent)
ax.plot([base[0], pushed[0]], [base[1], pushed[1]], [base[2], pushed[2]],
        color='darkgreen', linewidth=2, label='exp-map')
ax.scatter([pushed[0]], [pushed[1]], [pushed[2]], color='darkgreen', s=50,
           depthshade=False)

# OpenTURNS Curve: expMap path in tangent plane coordinates
exp_curve_2d = ot.Curve(ot.Sample([[0.0, 0.0], [pushed[0], pushed[1]]]),
                        "darkgreen", "solid", 2, "")
graph_exp = ot.Graph()
graph_exp.add(exp_curve_2d)
otv.View(graph_exp, figure=fig, axes=[ax], add_legend=False)

# logMap: pull back from pushed point
pulled = sphere.logMap(base, pushed)
ax.quiver(base[0], base[1], base[2],
          pulled[0], pulled[1], pulled[2],
          color='purple', arrow_length_ratio=0.15, linewidth=2,
          label='log-map (inverse)')

# OpenTURNS Curve: logMap result in tangent plane coordinates
log_curve_2d = ot.Curve(ot.Sample([[0.0, 0.0], [pulled[0], pulled[1]]]),
                        "purple", "solid", 2, "")
graph_log = ot.Graph()
graph_log.add(log_curve_2d)
otv.View(graph_log, figure=fig, axes=[ax], add_legend=False)

# projectToManifold: project ambient point onto sphere
ambient = ot.Point([2.0, 0.5, 0.3])
projected = sphere.projectToManifold(ambient)
ax.scatter([ambient[0]], [ambient[1]], [ambient[2]], color='orange', s=60,
           label='ambient point', depthshade=False)
ax.scatter([projected[0]], [projected[1]], [projected[2]], color='orange',
           s=50, marker='x', depthshade=False)
ax.plot([ambient[0], projected[0]], [ambient[1], projected[1]],
        [ambient[2], projected[2]],
        color='orange', linestyle='--', linewidth=1.5,
        label='projectToManifold')

# OpenTURNS Curve: projection line in tangent plane coordinates
proj_curve_2d = ot.Curve(ot.Sample([[ambient[0], ambient[1]],
                                    [projected[0], projected[1]]]),
                         "orange", "dashed", 1.5, "")
graph_proj = ot.Graph()
graph_proj.add(proj_curve_2d)
otv.View(graph_proj, figure=fig, axes=[ax], add_legend=False)

ax.set_xlim([-1.1, 2.5])
ax.set_ylim([-1.1, 1.1])
ax.set_zlim([-1.1, 1.5])
ax.set_xlabel('$x$')
ax.set_ylabel('$y$')
ax.set_zlabel('$z$')
ax.set_title('Manifold interface: exp/log maps, projection\n'
             '(illustrated on $S^2$)')
ax.legend(loc='upper left', fontsize=8)

fig.suptitle('Manifold (abstract interface)')
fig.tight_layout()
plt.show()
