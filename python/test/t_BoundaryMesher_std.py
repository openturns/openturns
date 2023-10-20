import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp


def compute_length(mesh2D):
    length = 0.0
    for ind in mesh2D.getSimplices():
        length += (mesh2D.getVertex(ind[0]) - mesh2D.getVertex(ind[1])).norm()
    return length


# Define the vertices of the mesh
vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [1.5, 1.0], [3.0, 1.5], [0.5, 1.5]]
# Define the simplices of the mesh
simplices = [[0, 1, 2], [1, 2, 3], [2, 3, 4], [2, 4, 5], [0, 2, 5]]
# Create the Mesh
mesh2D = ot.Mesh(vertices, simplices)
# Build the Mesh boundary
mesh2DBoundary = otexp.BoundaryMesher().build(mesh2D)
volume = mesh2DBoundary.getVolume()
ott.assert_almost_equal(volume, 0.0)
length = compute_length(mesh2DBoundary)
ott.assert_almost_equal(length, 7.780311648918275)
# Build a thick outside Mesh boundary
mesh2DBoundaryOutside = otexp.BoundaryMesher().build(mesh2D, 0.05)
volume = mesh2DBoundaryOutside.getVolume()
ott.assert_almost_equal(volume, 0.194508)
# Build a thick inside Mesh boundary
mesh2DBoundaryInside = otexp.BoundaryMesher().build(mesh2D, -0.05)
volume = mesh2DBoundaryInside.getVolume()
ott.assert_almost_equal(volume, 0.194508)
