"""
Estimate a probability using Line Sampling
==========================================
"""

# %%
# In this example, we estimate the probability that the output of a function exceeds a given threshold with the Line Sampling method.

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
import matplotlib.pyplot as plt

# %%
# Define the limit state function and the random vector
# -----------------------------------------------------
dim = 2
g_twoBranch = ot.SymbolicFunction(
    ["x1", "x2"],
    [
        "min(4 + 0.1 * (x1 - x2)^2 - (x1 + x2) / sqrt(2), 4 + 0.1 * (x1 - x2)^2 + (x1 + x2) / sqrt(2))"
    ],
)
X = ot.RandomVector(ot.Normal(dim))

# %%
# Define the event
# ----------------
Y_twoBranch = ot.CompositeRandomVector(g_twoBranch, X)
threshold = 1.5
event_twoBranch = ot.ThresholdEvent(Y_twoBranch, ot.Less(), threshold)

# %%
# Run FORM approximation
# ----------------------
optimAlgo = ot.Cobyla()
optimAlgo.setStartingPoint(X.getMean())
algo = ot.FORM(optimAlgo, event_twoBranch)
algo.run()
resultFORM = algo.getResult()


# %%
# Run Line Sampling
# -----------------

# %%
# The design point will define the initial important direction
alpha_twoBranch = resultFORM.getStandardSpaceDesignPoint()

# %%
# Define the root solver
solver = ot.Brent(1e-3, 1e-3, 1e-3, 5)
rootStrategy = ot.MediumSafe(solver)

# %%
# Create the algorithm
algo = otexp.LineSampling(event_twoBranch, alpha_twoBranch, rootStrategy)
algo.setMaximumOuterSampling(1000)
algo.setMaximumCoefficientOfVariation(5e-2)
# disable adaptive important direction to make plots easier to interpret
algo.setAdaptiveImportantDirection(False)

# %%
# Save the important direction history, and especially all root points
algo.setStoreHistory(True)

# %%
# Run the simulation
algo.run()
result = algo.getResult()
print(result)

# %%
# Retrieve important directions
alphas = algo.getAlphaHistory()

# %%
# Retrieve root points and root values
rootPoints = algo.getRootPointsHistory()
rootValues = algo.getRootValuesHistory()


# %%


def drawLines(algo, n=10):
    """Draw sampling lines and the corresponding roots."""
    rootPoints = algo.getRootPointsHistory()
    alphas = algo.getAlphaHistory()
    n = min(n, len(rootPoints))
    for i in range(n):
        # there can be several roots per sample
        n_roots = len(rootPoints[i])
        alpha = alphas[i]
        for j in range(n_roots):
            if i + j == 0:
                print(f"n_roots={n_roots}")
            dp = rootPoints[i][j]
            # project design point on the hyperplane orthogonal to alpha to get origin point
            uPoint = dp
            uDot = uPoint.dot(alpha)
            uPoint = uPoint - alpha * uDot
            # draw segment origin -> design point
            plt.plot(
                (uPoint[0], dp[0]),
                (uPoint[1], dp[1]),
                color="blue",
                linestyle="--",
                linewidth=0.75,
            )
            # draw origin
            plt.plot(uPoint[0], uPoint[1], "ro", markersize=3)
            # draw design point
            plt.plot(dp[0], dp[1], "bo", markersize=3, zorder=3)


# %%


def drawLSDesign(algo):
    """Draw sampling lines, roots, and the limit state function."""
    dmin = [-4.0] * 2
    dmax = [4.0] * 2
    graph1 = g_twoBranch.draw(dmin, dmax)
    contour_g = graph1.getDrawable(0).getImplementation()
    contour_g.setColorBarPosition("")
    contour_g.setColorMap("gray")
    graph1.setDrawable(0, contour_g)
    view1 = otv.View(graph1, square_axes=True)
    # now draw the the limit state on same figure
    graph2 = g_twoBranch.draw(dmin, dmax)
    contour_g = graph2.getDrawable(0).getImplementation()
    contour_g.setLevels([threshold])
    contour_g.setColor("red")
    graph2.setDrawable(0, contour_g)
    plt.axline([-1, 1], [1, -1], linestyle="dotted", color="gray")
    drawLines(algo)
    graph2.setTitle("Line Sampling")
    _ = otv.View(graph2, figure=view1.getFigure())


# %%
# Plot the limit state, a few design points and their origin
drawLSDesign(algo)

# %%
# Now we disable the opposite direction search to try to save function evaluations.
# This practice is incorrect in this case, however, as the algorithm misses half the probability of the event.
ot.RandomGenerator.SetSeed(0)
algo.setSearchOppositeDirection(False)
algo.run()
result = algo.getResult()
print(result)

# %%
# We plot the limit state, a few design points and their origin.
# This time we see each origin sample point in the orthogonal hyperplane yields only one design point.
drawLSDesign(algo)

# %%
# Now re-enable adaptive important direction search
ot.RandomGenerator.SetSeed(0)
algo.setAdaptiveImportantDirection(True)
algo.setSearchOppositeDirection(True)
algo.run()
result = algo.getResult()
print(result)

# %%
# Inspect important directions (without duplicates from history)
unique_alphas = ot.Sample(0, dim)
for alpha in algo.getAlphaHistory():
    if len(unique_alphas) == 0 or alpha != unique_alphas[-1]:
        unique_alphas.add(alpha)
print("unique alphas:")
print(unique_alphas)

# %%
otv.View.ShowAll()
