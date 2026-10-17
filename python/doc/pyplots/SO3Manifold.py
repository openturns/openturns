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
# Left panel: axis-angle ball of SO(3) with exp-map from identity
# ======================================================================
ax1 = fig.add_subplot(121, projection='3d')

# Cut-locus sphere (radius = pi) - matplotlib surface
u = np.linspace(0, 2 * np.pi, 40)
v = np.linspace(0, np.pi, 40)
xs = np.pi * np.outer(np.cos(u), np.sin(v))
ys = np.pi * np.outer(np.sin(u), np.sin(v))
zs = np.pi * np.outer(np.ones(np.size(u)), np.cos(v))
ax1.plot_surface(xs, ys, zs, color='lightgray', alpha=0.3, edgecolor='none',
                 label='cut locus (angle = $\\pi$)')

# Identity at origin
ax1.scatter([0], [0], [0], color='red', s=80, label='identity', depthshade=False)

so3 = otexp.SO3Manifold()
identity = ot.Point([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
basis = so3.getTangentBasis(identity)

# Tangent basis vectors from identity using OpenTURNS Curve
# Each basis vector is a 9-dim flattened matrix; extract axis-angle for visualization
colors = ['darkgreen', 'darkorange', 'darkblue']
labels = ['rot about $x$', 'rot about $y$', 'rot about $z$']
for k in range(3):
    tangent = ot.Point(basis[k])
    scale = math.pi / 2
    tangent_scaled = ot.Point([tangent[i] * scale / tangent.norm() for i in range(9)])
    pushed = so3.expMap(identity, tangent_scaled)

    # Extract axis-angle from pushed rotation matrix
    rot = ot.SquareMatrix(3)
    for i in range(3):
        for j in range(3):
            rot[i, j] = pushed[i * 3 + j]
    trace = rot[0, 0] + rot[1, 1] + rot[2, 2]
    angle = math.acos(max(-1.0, min(1.0, (trace - 1.0) / 2.0)))
    axis = ot.Point([rot[2, 1] - rot[1, 2], rot[0, 2] - rot[2, 0], rot[1, 0] - rot[0, 1]])
    axis_norm = axis.norm()
    if axis_norm > 1e-10:
        axis = ot.Point([axis[i] / axis_norm for i in range(3)])
    end = ot.Point([axis[i] * angle for i in range(3)])

    # OpenTURNS Curve: axis-angle vector in 3D tangent space (use first 3 comps)
    # The tangent vector in axis-angle space is 3D; we plot its 2D projection
    # using OpenTURNS Curve for the x-y plane
    axis_angle_curve = ot.Curve(ot.Sample([[0.0, 0.0], [axis[0] * angle, axis[1] * angle]]),
                                colors[k], "solid", 2.5,
                                labels[k] if k == 0 else "")
    graph_axis = ot.Graph()
    graph_axis.add(axis_angle_curve)
    otv.View(graph_axis, figure=fig, axes=[ax1], add_legend=False)

    # Also draw as matplotlib 3D line for proper 3D appearance
    ax1.plot([0, end[0]], [0, end[1]], [0, end[2]],
             color=colors[k], linewidth=2.5, label=labels[k] if k == 0 else "")
    ax1.scatter([end[0]], [end[1]], [end[2]],
                color=colors[k], s=50, depthshade=False)

ax1.set_xlim([-math.pi, math.pi])
ax1.set_ylim([-math.pi, math.pi])
ax1.set_zlim([-math.pi, math.pi])
ax1.set_xlabel('axis $\\cdot$ angle')
ax1.set_ylabel('axis $\\cdot$ angle')
ax1.set_zlabel('axis $\\cdot$ angle')
ax1.set_title('SO(3): axis-angle ball (radius $\\pi$),\nexp-map from identity')
ax1.legend(loc='upper left', fontsize=8)

# ======================================================================
# Right panel: SO(3) exp-map volume density on tangent ball
# ======================================================================
ax2 = fig.add_subplot(122)

# Volume density: (sin(theta/2)/(theta/2))^2 where theta = |coordinates|/sqrt(2)
# Build OpenTURNS Curve data: (radius, density) pairs
r_max = math.pi * math.sqrt(2)  # max coordinate norm (cut locus at theta=pi)
n_points = 100
radii_data = ot.Sample([[r_max * i / (n_points - 1)] for i in range(n_points)])
density_data = ot.Sample()
for i in range(n_points):
    r = r_max * i / (n_points - 1)
    theta = r / math.sqrt(2)
    if theta <= 1e-10:
        density = 1.0
    else:
        ratio = math.sin(theta / 2) / (theta / 2)
        density = ratio * ratio
    density_data.add([density])

# Combine into (radius, density) Sample for OpenTURNS Curve
curve_data = ot.Sample([[radii_data[i, 0], density_data[i, 0]] for i in range(n_points)])
density_curve = ot.Curve(curve_data, "darkgreen", "solid", 2.5,
                         "volume density")
graph_density = ot.Graph("Volume density of exp-map on SO(3)",
                         "$|\\text{coordinates}|$ (tangent space radius)",
                         "volume density $(\\sin(\\theta/2)/(\\theta/2))^2$")
graph_density.add(density_curve)

otv.View(graph_density, figure=fig, axes=[ax2], add_legend=False)
ax2.set_xlabel('$|\\text{coordinates}|$ (tangent space radius)')
ax2.set_ylabel('volume density $(\\sin(\\theta/2)/(\\theta/2))^2$')
ax2.set_title('SO(3): exp-map volume density')
ax2.grid(True, alpha=0.3)

fig.suptitle('SO(3) rotation group')
fig.tight_layout()
plt.show()
