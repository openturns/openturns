import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
import math

ot.RandomGenerator.SetSeed(0)

# Target manifold: the unit disk in R^2, drawn as reference
theta = [2.0 * math.pi * i / 200 for i in range(201)]
circle = ot.Sample([[math.cos(t), math.sin(t)] for t in theta])

fig = plt.figure(figsize=(10, 5))
ax1 = fig.add_subplot(121)
ax2 = fig.add_subplot(122)

# Case n=1, p=2: antecedent Uniform(-1, 1) mapped onto a parabola inside the disk.
# The image is a 1D manifold (curve) and the PDF is a line density.
antecedent1 = ot.Uniform(-1.0, 1.0)
f1 = ot.SymbolicFunction(["x"], ["0.8 * x", "0.4 * (x^2 - 0.5)"])
dist1 = otexp.PushForwardDistribution(f1, antecedent1)
xs = [-1.0 + 2.0 * i / 100 for i in range(101)]
curve = ot.Sample([[0.8 * x, 0.4 * (x * x - 0.5)] for x in xs])
pdf1 = [dist1.computePDF(p) for p in curve]
ax1.plot([p[0] for p in circle], [p[1] for p in circle], "k--", lw=1.0, label="unit disk")
sc = ax1.scatter([p[0] for p in curve], [p[1] for p in curve], c=pdf1, cmap="viridis", s=8)
ax1.set_aspect("equal", adjustable="box")
ax1.set_title("n=1, p=2: PDF on curve")
ax1.set_xlabel(r"$y_0$")
ax1.set_ylabel(r"$y_1$")
plt.colorbar(sc, ax=ax1, label="PDF")

# Case n=2, p=2: antecedent Normal(2) mapped by f(x) = x / (1 + |x|^2) onto
# the open disk of radius 1/2. The map is 2-to-1 (radii r and 1/r share the
# same image) and singular on |x| = 1, so the PDF sums two preimage
# contributions and diverges at the boundary of the disk.
f2 = ot.SymbolicFunction(
    ["x0", "x1"],
    ["x0 / (1 + x0^2 + x1^2)", "x1 / (1 + x0^2 + x1^2)"],
)
dist2 = otexp.PushForwardDistribution(f2, ot.Normal(2))
nGrid = 21
xs = [-0.45 + 0.9 * i / (nGrid - 1) for i in range(nGrid)]
ys = [-0.45 + 0.9 * j / (nGrid - 1) for j in range(nGrid)]
pdfGrid = []
for y in ys:
    row = []
    for x in xs:
        if x * x + y * y < 0.25:
            row.append(dist2.computePDF([x, y]))
        else:
            row.append(float("nan"))
    pdfGrid.append(row)
im = ax2.imshow(
    pdfGrid,
    extent=[-0.45, 0.45, -0.45, 0.45],
    origin="lower",
    cmap="viridis",
)
ax2.plot([p[0] for p in circle], [p[1] for p in circle], "k--", lw=1.0)
ax2.set_aspect("equal", adjustable="box")
ax2.set_title("n=2, p=2: PDF on disk of radius 1/2")
ax2.set_xlabel(r"$y_0$")
ax2.set_ylabel(r"$y_1$")
plt.colorbar(im, ax=ax2, label="PDF")

fig.suptitle("PushForwardDistribution on the unit disk")
fig.tight_layout()
