import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401
import numpy as np
import openturns.viewer as otv
import math

ot.RandomGenerator.SetSeed(0)

fig = plt.figure(figsize=(12, 5))

# ======================================================================
# Left panel: samples pushed onto the sphere via exp-map
# ======================================================================
ax1 = fig.add_subplot(121, projection='3d')

# Unit sphere wireframe (matplotlib - OpenTURNS has no surface drawable)
u = np.linspace(0, 2 * math.pi, 40)
v = np.linspace(0, math.pi, 40)
x_sphere = np.outer(np.cos(u), np.sin(v))
y_sphere = np.outer(np.sin(u), np.sin(v))
z_sphere = np.outer(np.ones(np.size(u)), np.cos(v))
ax1.plot_wireframe(x_sphere, y_sphere, z_sphere, color='lightgray', alpha=0.3)

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

# Push samples onto sphere as OpenTURNS Cloud (2D projection on xy plane)
# The 3D points are drawn with matplotlib; OpenTURNS Cloud renders xy projection
pushed_sample = ot.Sample(pushed)
cloud_samples = ot.Cloud(pushed_sample.getMarginal([0, 1]),
                         "darkred", "fsquare", "pushed samples")
graph_samples = ot.Graph()
graph_samples.add(cloud_samples)
otv.View(graph_samples, figure=fig, axes=[ax1], add_legend=False)

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

# ======================================================================
# Right panel: PDF on sphere surface
# ======================================================================
ax2 = fig.add_subplot(122, projection='3d')

# Evaluate PDF on a grid of sphere points (lat/lon domain)
# Use a single consistent grid for both sphere surface and PDF
n_lat = 30
n_lon = 60
lat_vals = np.linspace(0, math.pi, n_lat)
lon_vals = np.linspace(0, 2 * math.pi, n_lon)
pdf_vals = np.zeros((n_lat, n_lon))
for i, lat in enumerate(lat_vals):
    for j, lon in enumerate(lon_vals):
        pt = ot.Point([math.sin(lat) * math.cos(lon),
                       math.sin(lat) * math.sin(lon),
                       math.cos(lat)])
        pdf_vals[i, j] = distribution.computePDF(pt)

# Sphere surface using the same lat/lon grid
x_sphere = np.outer(np.cos(lon_vals), np.sin(lat_vals))
y_sphere = np.outer(np.sin(lon_vals), np.sin(lat_vals))
z_sphere = np.outer(np.ones(np.size(lon_vals)), np.cos(lat_vals))

# OpenTURNS Contour: PDF as function of (lon, lat) on the sphere domain
# lon varies fastest (inner loop), lat varies slowest (outer loop)
pdf_data = ot.Sample([[pdf_vals[i, j]] for i in range(n_lat)
                      for j in range(n_lon)])
contour = ot.Contour(n_lon, n_lat, pdf_data)
contour.setLevels([0.1, 0.3, 0.5, 0.7, 0.9])
contour.setColorMap("viridis")
graph_contour = ot.Graph("PDF on sphere (Riemannian volume)",
                         "longitude", "latitude")
graph_contour.add(contour)

# Render OpenTURNS contour on lat/lon plane (projected onto sphere)
otv.View(graph_contour, figure=fig, axes=[ax2], add_legend=False)

# Also scatter the raw PDF values as colored points on sphere surface
pdf_flat = pdf_vals.flatten()
x_flat = x_sphere.flatten()
y_flat = y_sphere.flatten()
z_flat = z_sphere.flatten()
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
plt.show()
