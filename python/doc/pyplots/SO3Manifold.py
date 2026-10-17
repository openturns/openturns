import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import math

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(10, 8))

# ---- Left: axis-angle representation of SO(3) ----
# A rotation is encoded as axis * angle, forming a ball of radius pi.
# Identity at origin, cut locus at boundary (angle = pi).
ax1 = fig.add_subplot(121, projection='3d')

# Draw the cut-locus sphere (radius = pi)
u = np.linspace(0, 2 * np.pi, 40)
v = np.linspace(0, np.pi, 40)
xs = np.pi * np.outer(np.cos(u), np.sin(v))
ys = np.pi * np.outer(np.sin(u), np.sin(v))
zs = np.pi * np.outer(np.ones(np.size(u)), np.cos(v))
ax1.plot_surface(xs, ys, zs, color='lightgray', alpha=0.3, edgecolor='none',
                 label='cut locus (angle = $\\pi$)')

# Origin = identity
ax1.scatter([0], [0], [0], color='red', s=80, label='identity', depthshade=False)

# Tangent basis at identity (Lie algebra so(3))
so3 = otexp.SO3Manifold()
identity = ot.Point([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
basis = so3.getTangentBasis(identity)
# Basis vectors are flattened 3x3 matrices (9-dim ambient vectors)
colors = ['darkgreen', 'darkorange', 'darkblue']
labels = ['rot about $x$', 'rot about $y$', 'rot about $z$']
for k in range(3):
    # expMap from identity along tangent direction k, scaled by angle
    tangent = ot.Point(basis[k])
    scale = math.pi / 2
    tangent_scaled = ot.Point([tangent[i] * scale / tangent.norm() for i in range(9)])
    pushed = so3.expMap(identity, tangent_scaled)
    # Extract axis-angle from the pushed rotation matrix
    rot = ot.SquareMatrix(3)
    for i in range(3):
        for j in range(3):
            rot[i, j] = pushed[i * 3 + j]
    # Angle from trace
    trace = rot[0, 0] + rot[1, 1] + rot[2, 2]
    angle = math.acos(max(-1.0, min(1.0, (trace - 1.0) / 2.0)))
    # Axis from skew part
    axis = ot.Point([rot[2, 1] - rot[1, 2], rot[0, 2] - rot[2, 0], rot[1, 0] - rot[0, 1]])
    axis_norm = axis.norm()
    if axis_norm > 1e-10:
        axis = ot.Point([axis[i] / axis_norm for i in range(3)])
    end = ot.Point([axis[i] * angle for i in range(3)])
    ax1.plot([0, end[0]], [0, end[1]], [0, end[2]],
             color=colors[k], linewidth=2.5, label=labels[k] if k == 0 else "")
    ax1.scatter([end[0]], [end[1]], [end[2]], color=colors[k], s=50, depthshade=False)

ax1.set_xlim([-math.pi, math.pi])
ax1.set_ylim([-math.pi, math.pi])
ax1.set_zlim([-math.pi, math.pi])
ax1.set_xlabel('axis $\\cdot$ angle')
ax1.set_ylabel('axis $\\cdot$ angle')
ax1.set_zlabel('axis $\\cdot$ angle')
ax1.set_title('SO(3): axis-angle ball (radius $\\pi$),\nexp-map from identity')
ax1.legend(loc='upper left', fontsize=8)

# ---- Right: volume density on SO(3) tangent ball ----
ax2 = fig.add_subplot(122, projection='3d')
# Volume density: (sin(theta/2)/(theta/2))^2 where theta = |coordinates|/sqrt(2)
# Plot density as function of radius in tangent space
r_max = math.pi * math.sqrt(2)  # max coordinate norm (cut locus at theta=pi)
radii = np.linspace(0, r_max, 50)
densities = []
for r in radii:
    theta = r / math.sqrt(2)
    if theta <= 1e-10:
        densities.append(1.0)
    else:
        ratio = math.sin(theta / 2) / (theta / 2)
        densities.append(ratio * ratio)

ax2.plot(radii, densities, color='darkgreen', linewidth=2.5)
ax2.set_xlabel('$|\\text{coordinates}|$ (tangent space radius)')
ax2.set_ylabel('volume density $(\\sin(\\theta/2)/(\\theta/2))^2$')
ax2.set_title('SO(3): exp-map volume density')
ax2.grid(True, alpha=0.3)

fig.suptitle('SO(3) rotation group')
fig.tight_layout()
