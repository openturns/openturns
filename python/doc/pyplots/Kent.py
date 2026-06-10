import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
import openturns.viewer as otv
import math


# Mesh of the unit sphere
def icosahedron_sphere(refinements=3):
    t = (1.0 + math.sqrt(5.0)) / 2.0
    vertices = [
        [-1, t, 0], [1, t, 0], [-1, -t, 0], [1, -t, 0],
        [0, -1, t], [0, 1, t], [0, -1, -t], [0, 1, -t],
        [t, 0, -1], [t, 0, 1], [-t, 0, -1], [-t, 0, 1],
    ]
    norm = math.sqrt(1 + t * t)
    vertices = [[x / norm for x in v] for v in vertices]
    faces = [
        [0, 11, 5], [0, 5, 1], [0, 1, 7], [0, 7, 10], [0, 10, 11],
        [1, 5, 9], [5, 11, 4], [11, 10, 2], [10, 7, 6], [7, 1, 8],
        [3, 9, 4], [3, 4, 2], [3, 2, 6], [3, 6, 8], [3, 8, 9],
        [4, 9, 5], [2, 4, 11], [6, 2, 10], [8, 6, 7], [9, 8, 1],
    ]
    for _ in range(refinements):
        new_faces = []
        edge_mid = {}
        for f in faces:
            a, b, c = f
            ab = tuple(sorted((a, b)))
            bc = tuple(sorted((b, c)))
            ca = tuple(sorted((c, a)))
            for pair in (ab, bc, ca):
                if pair not in edge_mid:
                    i, j = pair
                    p = [(vertices[i][k] + vertices[j][k]) / 2.0 for k in range(3)]
                    norm = math.sqrt(p[0] ** 2 + p[1] ** 2 + p[2] ** 2)
                    p = [x / norm for x in p]
                    edge_mid[pair] = len(vertices)
                    vertices.append(p)
            ab_i = edge_mid[ab]
            bc_i = edge_mid[bc]
            ca_i = edge_mid[ca]
            new_faces.append([a, ab_i, ca_i])
            new_faces.append([b, bc_i, ab_i])
            new_faces.append([c, ca_i, bc_i])
            new_faces.append([ab_i, bc_i, ca_i])
        faces = new_faces
    return vertices, faces


ot.ResourceMap.SetAsScalar("Mesh-AmbientFactor", 0.1)
ot.ResourceMap.SetAsScalar("Mesh-DiffuseFactor", 0.8)
ot.ResourceMap.SetAsScalar("Mesh-Shininess", 50.0)
ot.ResourceMap.SetAsScalar("Mesh-SpecularFactor", 0.2)
ot.ResourceMap.SetAsUnsignedInteger("Drawable-DefaultPalettePhase", 10 ** 10)
ot.ResourceMap.SetAsString("Drawable-DefaultPaletteName", "Rainbow")
ot.ResourceMap.SetAsScalar("Mesh-DefaultThetaX", 0.0)
ot.ResourceMap.SetAsScalar("Mesh-DefaultThetaY", 0.0)
ot.ResourceMap.SetAsScalar("Mesh-DefaultThetaZ", 0.0)

vertices, faces = icosahedron_sphere(4)
simplices = [[a, b, c, c] for a, b, c in faces]
mesh = ot.Mesh(ot.Sample(vertices), simplices)

gamma1 = [0.0, 0.0, -1.0]
gamma2_list = [
    [1.0, 0.0, 0.0],
    [2.0 ** (-0.5), 2.0 ** (-0.5), 0.0],
]

gamma3_list = []
for g2 in gamma2_list:
    g3 = [
        gamma1[1] * g2[2] - gamma1[2] * g2[1],
        gamma1[2] * g2[0] - gamma1[0] * g2[2],
        gamma1[0] * g2[1] - gamma1[1] * g2[0],
    ]
    gamma3_list.append(g3)

grid = ot.GridLayout(2, 4)
grid.setTitle("Kent(kappa, beta, gamma1, gamma2, gamma3)")

kappa_vals = [5.0, 20.0]
beta_vals = [1.0, None]

fig = plt.figure(figsize=(16, 8))
index = 0
factor = 0.3

for kappa in kappa_vals:
    for j, beta in enumerate(beta_vals):
        if beta is None:
            beta = 0.45 * kappa
        for g2, g3 in zip(gamma2_list, gamma3_list):
            distribution = otexp.Kent(kappa, beta, gamma1, g2, g3)
            pdf = distribution.computePDF(ot.Sample(vertices))
            pdf_max = pdf.getMax()[0]
            defVertices = [
                [v[k] * (1.0 + factor * pdf[i, 0] / pdf_max) for k in range(3)]
                for i, v in enumerate(vertices)
            ]
            defMesh = ot.Mesh(ot.Sample(defVertices), simplices)
            field = ot.Field(defMesh, pdf)
            pdf_graph = field.draw()
            pdf_graph.setTitle(
                rf"$\kappa={kappa:.3g}$, $\beta={beta:.3g}$, "
                rf"$\gamma_2=({g2[0]:.3g},{g2[1]:.3g},{g2[2]:.3g})$"
            )
            pdf_graph.setXTitle(r"$x$")
            pdf_graph.setYTitle(r"$y$")
            row = index // 4
            col = index % 4
            grid.setGraph(row, col, pdf_graph)
            index += 1

otv.View(grid, figure=fig, add_legend=True, square_axes=True)
plt.subplots_adjust(wspace=0.3, hspace=0.3)
