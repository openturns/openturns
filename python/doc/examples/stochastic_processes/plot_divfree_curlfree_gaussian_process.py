"""
Curl-free and divergence-free Gaussian processes
================================================

This example presents two experimental covariance models built from a given
scalar covariance model:

- :class:`~openturns.experimental.CurlFreeModel` extracts its curl-free part, so that the
  realizations of the associated Gaussian process are approximately curl-free;
- :class:`~openturns.experimental.DivFreeModel` extracts its divergence-free part, so that the
  realizations of the associated Gaussian process are approximately divergence-free.

We start from an isotropic :class:`~openturns.MaternModel` covariance model with smoothness
parameter :math:`\nu=5/2` and correlation length :math:`0.1`.
"""

# %%
import numpy as np
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

# sphinx_gallery_thumbnail_number = 2

# %%
# Build the covariance models
# ---------------------------
#
# The underlying isotropic scalar covariance model is built from a
# one-dimensional :class:`~openturns.MaternModel` kernel transformed by
# :class:`~openturns.IsotropicCovarianceModel`.

# %%
kernel = ot.MaternModel([0.1], [1.0], 2.5)
scalar_model = ot.IsotropicCovarianceModel(kernel, 2)
print(scalar_model)

# %%
# Extract its curl-free and divergence-free parts.

# %%
curl_free = otexp.CurlFreeModel(scalar_model)
div_free = otexp.DivFreeModel(scalar_model)
print(curl_free)
print(div_free)

# %%
# Discretize the domain with a regular grid
# ------------------------------------------
#
# We discretize the domain :math:`\mathcal{D}=[-0.1,0.1]^2` with a regular grid,
# using the class :class:`~openturns.IntervalMesher`. The mesh step is small
# compared to the correlation length, so that the central finite differences
# used below accurately estimate the differential operators.

# %%
mesh = ot.IntervalMesher([41, 41]).build(ot.Interval([-0.1, -0.1], [0.1, 0.1]))
print("Number of vertices:", mesh.getVerticesNumber())

# %%
# Sample realizations of the two Gaussian processes
# =================================================
#
# The sampling uses the exact dense factorization of the covariance matrix on
# the 1764 mesh vertices, so the realization is the exact restriction of a
# curl-free, resp. divergence-free, Gaussian field to the grid.

# %%
ot.RandomGenerator.SetSeed(6)
curl_free_process = ot.GaussianProcess(curl_free, mesh)
div_free_process = ot.GaussianProcess(div_free, mesh)
curl_free_field = curl_free_process.getRealization()
div_free_field = div_free_process.getRealization()

# %%
# Draw the vector fields with arrows fixed on the vertices of the mesh. To keep
# the figures readable we display only every third vertex of the sampled grid.

# %%
side = int(np.sqrt(div_free_field.getValues().getSize()))


def decimate(field, ratio):
    """Keep only one vertex out of 'ratio' in each direction."""
    values = np.asarray(field.getValues()).reshape(side, side, 2)[::ratio, ::ratio]
    vertices = np.asarray(field.getMesh().getVertices()).reshape(side, side, 2)
    vertices = vertices[::ratio, ::ratio].reshape(-1, 2)
    return ot.Field(ot.Mesh(ot.Sample(vertices)), ot.Sample(values.reshape(-1, 2)))


curl_free_display = decimate(curl_free_field, 3)
div_free_display = decimate(div_free_field, 3)

graph_curl = curl_free_display.draw()
graph_curl.setTitle("Curl-free realization")
graph_curl.setXTitle(r"$x_0$")
graph_curl.setYTitle(r"$x_1$")
graph_curl.setLegendPosition("")

graph_div = div_free_display.draw()
graph_div.setTitle("Divergence-free realization")
graph_div.setXTitle(r"$x_0$")
graph_div.setYTitle(r"$x_1$")
graph_div.setLegendPosition("")

grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, graph_curl)
grid.setGraph(0, 1, graph_div)
view = otv.View(grid)

# %%
# Verify the divergence-free and curl-free properties
# ---------------------------------------------------
#
# The divergence :math:`\mathrm{div}\ \mathbf{u} = \partial u_1/\partial x_0 +
# \partial u_2/\partial x_1` and the (scalar) curl
# :math:`\mathrm{curl}\ \mathbf{u} = \partial u_2/\partial x_0 -
# \partial u_1/\partial x_1` of each field are estimated by central finite
# differences on the regular grid. For a divergence-free field, the finite
# difference estimate of the divergence should be much smaller than the one of
# the curl, and conversely for a curl-free field.

# %%
step = mesh.getVertices()[1, 0] - mesh.getVertices()[0, 0]


def divergence_and_curl(field, mesh_step):
    """Return the maximum of the absolute finite-difference divergence and curl."""
    side = int(np.sqrt(field.getValues().getSize()))
    values = np.asarray(field.getValues()).reshape(side, side, 2)
    u1, u2 = values[:, :, 0], values[:, :, 1]
    du1 = (u1[1:-1, 2:] - u1[1:-1, :-2]) / (2.0 * mesh_step)
    du2 = (u1[2:, 1:-1] - u1[:-2, 1:-1]) / (2.0 * mesh_step)
    dv1 = (u2[1:-1, 2:] - u2[1:-1, :-2]) / (2.0 * mesh_step)
    dv2 = (u2[2:, 1:-1] - u2[:-2, 1:-1]) / (2.0 * mesh_step)
    divergence = np.max(np.abs(du1 + dv2))
    curl = np.max(np.abs(dv1 - du2))
    return divergence, curl


max_div, max_curl = divergence_and_curl(curl_free_field, step)
print("Curl-free field: max |divergence| =", max_div, ", max |curl| =", max_curl)
max_div, max_curl = divergence_and_curl(div_free_field, step)
print("Divergence-free field: max |divergence| =", max_div, ", max |curl| =", max_curl)

# %%
# The finite-difference divergence of the divergence-free field is about
# eighteen times smaller than its finite-difference curl, and conversely the
# finite-difference curl of the curl-free field is about fourteen times smaller
# than its finite-difference divergence. Both ratios improve when the mesh is
# refined: 22 and 20 on a :math:`45 \times 45` grid, and about 30 and 22 on a
# finer :math:`101 \times 101` grid, on which the peak divergence magnitude of
# the divergence-free field drops to about 86. Because the sampling routine
# factors the dense covariance matrix, whose memory footprint grows as the
# square of the number of vertices, the lighter :math:`41 \times 41` grid keeps
# this example fast enough for the online documentation.

# %%
# Display all figures
otv.View.ShowAll()
