"""
Sliced inverse regression
=========================
In this example we use the
:class:`~openturns.experimental.SlicedInverseRegression` class to find the
effective dimension reduction (EDR) space of a 2-d input. Then, on a
50-d model, we compare SIR with a principal component analysis (PCA)
computed manually with :class:`~openturns.Matrix` primitives only.
"""

# %%
# Introduction
# ~~~~~~~~~~~~
#
# We consider the following linear model:
#
# .. math::
#
#     Y = 4 (X_1 + 2 X_2) + 2 + \varepsilon
#
# where :math:`X = (X_1, X_2)` is a 2-d random vector and
# :math:`\varepsilon \sim \cN(0, 0.2^2)` is the noise.
#
# The response :math:`Y` depends on :math:`X` only through the single
# direction :math:`\beta = (1, 2)^T`. This is the effective dimension
# reduction (EDR) direction.
#
# The SIR algorithm attempts to recover this direction from the data.

# %%
import openturns as ot
import openturns.experimental as otexp
import matplotlib.pyplot as plt
import math

# %%
# Create the 2-d data
# ~~~~~~~~~~~~~~~~~~~
#
# We generate :math:`n = 100` observations of the input. The input is
# a mixture of a random normal component and a deterministic drift
# along the direction :math:`(1, 2)`:

ot.RandomGenerator.SetSeed(0)
N = 100
X = ot.Normal([0.0] * 2, [0.1] * 2).getSample(N)
X += [[i / (N - 1), 2 * i / (N - 1)] for i in range(N)]
# X is centered internally by the SIR algorithm
f = ot.SymbolicFunction(["x1", "x2"], ["4*(x1+2*x2)+2"])
Y = f(X) + ot.Normal(0.0, 0.2).getSample(N)

# %%
# Plot the 2-d data
# ~~~~~~~~~~~~~~~~~
#
# The color of each point represents the output value :math:`Y`.
# The data varies predominantly along the :math:`(1, 2)` direction,
# which is also the direction that drives :math:`Y`:

plt.scatter(X.getMarginal(0), X.getMarginal(1), c=Y)
plt.xlabel('x1')
plt.ylabel('x2')
plt.axis('square')
plt.title("2D colored dataset")
plt.colorbar(label='Y')
plt.show()

# %%
# Run the SIR algorithm on the 2-d data
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# Run the algorithm and retrieve the estimated EDR directions:

algo = otexp.SlicedInverseRegression(X, Y)
algo.run()
result = algo.getResult()
transformation = result.getTransformation()

# %%
# Show SIR direction
# ~~~~~~~~~~~~~~~~~~
#
# The estimated direction (red line) should align with the true
# EDR direction :math:`\beta = (1, 2)^T` (black dashed line).
# The directions are stored in the columns of the matrix returned by
# :meth:`~openturns.experimental.SlicedInverseRegressionResult.getDirections`:

plt.scatter(X.getMarginal(0), X.getMarginal(1), c=Y)
plt.xlabel('x1')
plt.ylabel('x2')
plt.axis('square')
plt.title("First direction of SIR")
dir_sir = result.getDirections()
plt.plot([-dir_sir[0, 0] * 30, dir_sir[0, 0] * 30],
         [-dir_sir[1, 0] * 30, dir_sir[1, 0] * 30], c='red', label='SIR')
beta = ot.Point([1.0, 2.0]) / math.sqrt(5)
plt.plot([-beta[0] * 30, beta[0] * 30],
         [-beta[1] * 30, beta[1] * 30], 'k--', label='True')
plt.legend()
plt.show()

# %%
# Eigenvalues
# ~~~~~~~~~~~
#
# The eigenvalues indicate the strength of each direction.
# A large gap between the first and second eigenvalue suggests that
# a single direction is sufficient to capture the dependence on Y:

eigenvalues = result.getEigenvalues()
print("Eigenvalues:", eigenvalues)

# %%
# Projection onto the EDR direction
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# We apply the estimated transformation to project the data onto
# the EDR direction and plot it against :math:`Y`.
# A clear linear relationship confirms that SIR has correctly
# identified the direction:

X_sir = transformation(X)
plt.scatter(X_sir.getMarginal(0), Y, alpha=0.7)
plt.xlabel('First SIR component')
plt.ylabel('Y')
plt.title('Projection onto SIR direction vs Y')
plt.show()

# %%
# SIR vs PCA
# ~~~~~~~~~~
#
# We now consider the following model with :math:`d = 50`:
#
# .. math::
#
#     Y = \cos(\pi X^T e_1) + (X^T e_2)^3 + \varepsilon
#
# where :math:`X` is uniform on :math:`[0, 1]^d`,
# :math:`e_1, e_2` are two orthonormal vectors and
# :math:`\varepsilon \sim \cN(0, 0.01^2)` is the noise.
#
# The response :math:`Y` depends on :math:`X` only through the
# 2-dimensional EDR subspace :math:`\mathrm{span}(e_1, e_2)`.
# Since :math:`X` is isotropic, its variance carries no information
# about :math:`Y`: PCA, which is unsupervised and only looks at the
# dispersion of :math:`X`, cannot recover this subspace, while SIR,
# which uses :math:`Y` to slice the data, can.

# %%
# Create the 50-d data
# ~~~~~~~~~~~~~~~~~~~~
#
# We build two orthonormal EDR directions by Gram-Schmidt
# orthogonalization of two uniform random vectors,
# then draw :math:`n = 200` observations of :math:`X`:

ot.RandomGenerator.SetSeed(0)
d_hd = 50
n_hd = 200

edr1 = ot.Uniform(0.0, 1.0).getSample(d_hd).asPoint()
edr1 /= edr1.norm()
edr2 = ot.Uniform(0.0, 1.0).getSample(d_hd).asPoint()
edr2 -= edr1.dot(edr2) * edr1
edr2 /= edr2.norm()

X_hd = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * d_hd).getSample(n_hd)

# Evaluate the model. The matrix-vector products
# :math:`X e_1` and :math:`X e_2` are the coordinates
# of :math:`X` on the true EDR directions:
Xmat_hd = ot.Matrix(X_hd)
z1 = Xmat_hd * edr1
z2 = Xmat_hd * edr2
noise_hd = ot.Normal(0.0, 0.01).getSample(n_hd)
Y_hd = ot.Sample(n_hd, 1)
for i in range(n_hd):
    Y_hd[i, 0] = math.cos(math.pi * z1[i]) + z2[i] ** 3 + noise_hd[i, 0]
colors_hd = list(Y_hd.asPoint())

# %%
# Reference projection on the true EDR space
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# The 2-d projection of :math:`X` on :math:`(e_1, e_2)`,
# colored by :math:`Y`, shows a clear structure:
# :math:`Y` can be predicted from these two coordinates.

X_true = ot.Sample(n_hd, 2)
for i in range(n_hd):
    X_true[i, 0] = z1[i]
    X_true[i, 1] = z2[i]

plt.scatter([X_true[i, 0] for i in range(n_hd)],
            [X_true[i, 1] for i in range(n_hd)], c=colors_hd, cmap="viridis")
plt.xlabel("True coordinate 1")
plt.ylabel("True coordinate 2")
plt.axis("square")
plt.title("Projection on true EDR space")
plt.colorbar(label="Y")
plt.show()

# %%
# Run SIR on the 50-d data
# ~~~~~~~~~~~~~~~~~~~~~~~~
#
# We run SIR with 50 slices (4 points per slice on average)
# and retain 2 directions:

algo_hd = otexp.SlicedInverseRegression(X_hd, Y_hd, 50)
algo_hd.setNbModes(2)
algo_hd.run()
result_hd = algo_hd.getResult()
print("SIR eigenvalues:", result_hd.getEigenvalues())
directions_hd = result_hd.getDirections()
for k in range(2):
    dk = ot.Point([directions_hd[i, k] for i in range(d_hd)])
    print(f"direction {k}: |<d, e1>|={abs(dk.dot(edr1)):.3f} "
          f"|<d, e2>|={abs(dk.dot(edr2)):.3f}")

# %%
# Projection on the SIR space
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# The 2-d SIR projection also shows a clear structure driven by :math:`Y`,
# close to the reference projection above: SIR has recovered
# predictive directions.

X_sir_hd = result_hd.getTransformation()(X_hd)
plt.scatter([X_sir_hd[i, 0] for i in range(n_hd)],
            [X_sir_hd[i, 1] for i in range(n_hd)], c=colors_hd, cmap="viridis")
plt.xlabel("SIR coordinate 1")
plt.ylabel("SIR coordinate 2")
plt.axis("square")
plt.title("Projection on SIR space")
plt.colorbar(label="Y")
plt.show()

# %%
# PCA computed manually with Matrix primitives
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# PCA looks for the directions of largest variance of :math:`X`.
# We compute it manually: center :math:`X`, take the singular value
# decomposition (SVD) of the centered :math:`n \times d` matrix
#
# .. math::
#
#     X_c = U \Sigma V^T
#
# The columns of :math:`V` (rows of :math:`V^T`) are the principal
# axes ordered by decreasing singular values, and
# :math:`X_c V_2` holds the coordinates on the first two axes.

Xc_hd = X_hd - X_hd.computeMean()
Mc_hd = ot.Matrix(Xc_hd)
singularValues, U_hd, VT_hd = Mc_hd.computeSVD()
print("PCA singular values (first 5):",
      [singularValues[i] for i in range(5)])
V_hd = VT_hd.transpose()
V2_hd = ot.Matrix(d_hd, 2)
for i in range(d_hd):
    for j in range(2):
        V2_hd[i, j] = V_hd[i, j]
for k in range(2):
    vk = ot.Point([V_hd[i, k] for i in range(d_hd)])
    print(f"PCA axis {k}: |<v, e1>|={abs(vk.dot(edr1)):.3f} "
          f"|<v, e2>|={abs(vk.dot(edr2)):.3f}")
Mpca_hd = Mc_hd * V2_hd
X_pca_hd = ot.Sample(n_hd, 2)
for i in range(n_hd):
    for j in range(2):
        X_pca_hd[i, j] = Mpca_hd[i, j]

# %%
# Projection on the PCA space
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# In contrast with SIR, the 2-d PCA projection shows no structure
# with respect to :math:`Y`: the colors are mixed because the first
# variance directions of the isotropic input :math:`X` are unrelated
# to the response. The flat spectrum of singular values confirms that
# no variance direction dominates.

plt.scatter([X_pca_hd[i, 0] for i in range(n_hd)],
            [X_pca_hd[i, 1] for i in range(n_hd)], c=colors_hd, cmap="viridis")
plt.xlabel("PCA coordinate 1")
plt.ylabel("PCA coordinate 2")
plt.axis("square")
plt.title("Projection on PCA space")
plt.colorbar(label="Y")
plt.show()
