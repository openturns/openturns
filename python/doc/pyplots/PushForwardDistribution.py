import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
import math

ot.RandomGenerator.SetSeed(0)

# Target manifold: the unit disk in R^2, drawn as reference
theta = [2.0 * math.pi * i / 200 for i in range(201)]
circle = ot.Sample([[math.cos(t), math.sin(t)] for t in theta])

fig = plt.figure(figsize=(10, 8))
ax1 = fig.add_subplot(221)
ax2 = fig.add_subplot(222)
ax3 = fig.add_subplot(223)
ax4 = fig.add_subplot(224)

# Case p > n, here n=1, p=2: antecedent Uniform(-1, 1) mapped onto a parabola
# inside the disk. The image is a 1D manifold (curve) and the PDF is a line
# density, obtained by summing the contributions of the preimages.
antecedent1 = ot.Uniform(-1.0, 1.0)
f1 = ot.SymbolicFunction(["x"], ["0.8 * x", "0.4 * (x^2 - 0.5)"])
dist1 = otexp.PushForwardDistribution(f1, antecedent1)
xs = [-1.0 + 2.0 * i / 100 for i in range(101)]
curve = ot.Sample([[0.8 * x, 0.4 * (x * x - 0.5)] for x in xs])
pdf1 = [dist1.computePDF(p) for p in curve]
ax1.plot([p[0] for p in circle], [p[1] for p in circle], "k--", lw=1.0, label="unit disk")
sc = ax1.scatter([p[0] for p in curve], [p[1] for p in curve], c=pdf1, cmap="viridis", s=8)
ax1.set_aspect("equal", adjustable="box")
ax1.set_title("p > n: PDF on a curve (n=1, p=2)")
ax1.set_xlabel(r"$y_0$")
ax1.set_ylabel(r"$y_1$")
plt.colorbar(sc, ax=ax1, label="PDF")

# Case p = n, here n=2, p=2: antecedent Normal(2) mapped by f(x) = x / (1 + |x|^2)
# onto the open disk of radius 1/2. The map is 2-to-1 (radii r and 1/r share the
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
ax2.set_title("p = n: PDF on a disk (n=2, p=2)")
ax2.set_xlabel(r"$y_0$")
ax2.set_ylabel(r"$y_1$")
plt.colorbar(im, ax=ax2, label="PDF")

# Case p < n, here n=2, p=1 (co-area formula): the fibre of the level y is the
# straight line {x : x0 + x1 = y}. The PDF at y is the integral of the
# antecedent density along that fibre, divided by the Jacobian |grad f| = sqrt(2).
antecedent3 = ot.Normal(2)
f3 = ot.SymbolicFunction(["x0", "x1"], ["x0 + x1"])
dist3 = otexp.PushForwardDistribution(f3, antecedent3)
halfWidth = 3.5
# Level lines x0 + x1 = y
for level in [-2.0, -1.0, 1.0, 2.0]:
    x0Range = [max(-halfWidth, level - halfWidth), min(halfWidth, level + halfWidth)]
    ax3.plot(x0Range, [level - s for s in x0Range], "-", color="0.8", lw=1.0)
# Fibre of the level y = 0, parameterized by its first coordinate: each of its
# points carries the value of the integrand p_X(x) / |grad f|
sValues = [-halfWidth + 2.0 * halfWidth * i / 100 for i in range(101)]
fibre = [[s, -s] for s in sValues]
integrand = [antecedent3.computePDF(x) / math.sqrt(2.0) for x in fibre]
ax3.plot([p[0] for p in fibre], [p[1] for p in fibre], "-", color="0.4", lw=1.0)
sc3 = ax3.scatter([p[0] for p in fibre], [p[1] for p in fibre], c=integrand, cmap="viridis", s=16)
ax3.text(0.04, 0.04, r"$f_Y(y) = \int_{f^{-1}(y)} p_X(x) / \sqrt{2}\ \mathrm{d}H^1$",
         transform=ax3.transAxes, fontsize=10)
ax3.set_xlim(-halfWidth - 0.3, halfWidth + 0.3)
ax3.set_ylim(-halfWidth - 0.3, halfWidth + 0.3)
ax3.set_aspect("equal", adjustable="box")
ax3.set_title("p < n: PDF as a fibre integral (n=2, p=1)")
ax3.set_xlabel(r"$x_0$")
ax3.set_ylabel(r"$x_1$")
plt.colorbar(sc3, ax=ax3, label="integrand")

# The co-area density is the distribution of the sum of two independent standard
# normals: it matches Normal(0, sqrt(2)) to machine precision, as well as the
# histogram of a sample of the push-forward distribution
reference = ot.Normal(0.0, math.sqrt(2.0))
values = [-4.0 + 8.0 * i / 160 for i in range(161)]
pdf3 = [dist3.computePDF([y]) for y in values]
pdfReference = [reference.computePDF([y]) for y in values]
difference = max(abs(a - b) for a, b in zip(pdf3, pdfReference))
ax4.hist([x[0] for x in dist3.getSample(10000)],
         bins=60, density=True, alpha=0.3, color="0.6", label="sample histogram")
ax4.plot(values, pdf3, "-", color="C0", lw=2.0, label="PushForwardDistribution")
ax4.plot(values, pdfReference, "k--", lw=1.0, label=r"reference $\mathcal{N}(0, \sqrt{2})$")
ax4.set_title(f"p < n: co-area PDF, max |diff| = {difference:.1e} (n=2, p=1)")
ax4.set_xlabel(r"$y$")
ax4.set_ylabel("PDF")
ax4.legend(loc="upper right", fontsize="small")

fig.suptitle("PushForwardDistribution: image of dimension p from an antecedent of dimension n")
fig.tight_layout()
