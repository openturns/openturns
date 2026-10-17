import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import math

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(12, 5))

# ---- Left: samples pushed onto the sphere ----
ax1 = fig.add_subplot(121, projection='3d')

# Unit sphere grid (shared with PDF evaluation)
n_lat = 30
n_lon = 60
lat_vals = np.linspace(0, math.pi, n_lat)
lon_vals = np.linspace(0, 2 * math.pi, n_lon)
x = np.outer(np.cos(lon_vals), np.sin(lat_vals))
y = np.outer(np.sin(lon_vals), np.sin(lat_vals))
z = np.outer(np.ones(np.size(lon_vals)), np.cos(lat_vals))
ax1.plot_wireframe(x, y, z, color='lightgray', alpha=0.3)

# Build the manifold mapped distribution
manifold = otexp.Manifold(otexp.SphereManifold(3))
background = ot.Normal(2)
chart_center = [0.0, 0.0, 1.0]
distribution = otexp.ManifoldMappedDistribution(background, manifold, chart_center)

# Draw background samples and push them onto the sphere
n_samples = 200
background_sample = background.getSample(n_samples)
pushed = []
for i in range(n_samples):
    pt = distribution.getRealization()
    pushed.append([pt[0], pt[1], pt[2]])
pushed = np.array(pushed)

ax1.scatter(pushed[:, 0], pushed[:, 1], pushed[:, 2],
            c='darkred', s=20, alpha=0.6, label='pushed samples', depthshade=False)

# Chart center
ax1.scatter([0], [0], [1], color='red', s=80, label='chart center', depthshade=False)

ax1.set_xlim([-1.1, 1.1])
ax1.set_ylim([-1.1, 1.1])
ax1.set_zlim([-1.1, 1.1])
ax1.set_xlabel('$x$')
ax1.set_ylabel('$y$')
ax1.set_zlabel('$z$')
ax1.set_title('ManifoldMappedDistribution: samples pushed\nonto sphere via exp-map')
ax1.legend(loc='upper left', fontsize=8)

# ---- Right: PDF heatmap on sphere surface ----
ax2 = fig.add_subplot(122, projection='3d')

# Evaluate PDF on a grid of sphere points (same grid as left panel)
pdf_vals = np.zeros((n_lat, n_lon))
for i, lat in enumerate(lat_vals):
    for j, lon in enumerate(lon_vals):
        pt = ot.Point([math.sin(lat) * math.cos(lon),
                       math.sin(lat) * math.sin(lon),
                       math.cos(lat)])
        pdf_vals[i, j] = distribution.computePDF(pt)

# Scatter sphere points colored by PDF value
pdf_flat = pdf_vals.flatten()
x_flat = x.flatten()
y_flat = y.flatten()
z_flat = z.flatten()
sc = ax2.scatter(x_flat, y_flat, z_flat, c=pdf_flat,
                 cmap=plt.cm.viridis, s=20, alpha=0.8, depthshade=False)
fig.colorbar(sc, ax=ax2, shrink=0.5, label='PDF')

ax2.set_xlim([-1.1, 1.1])
ax2.set_ylim([-1.1, 1.1])
ax2.set_zlim([-1.1, 1.1])
ax2.set_xlabel('$x$')
ax2.set_ylabel('$y$')
ax2.set_zlabel('$z$')
ax2.set_title('ManifoldMappedDistribution: PDF on sphere\n(Riemannian volume)')

fig.suptitle(r'ManifoldMappedDistribution on $\mathcal{S}^2$')
fig.tight_layout()
