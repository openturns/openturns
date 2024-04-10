"""
Use the LOLA-Voronoi sequential design of experiment
====================================================
"""

# %%
# The LOLA-Voronoi sequential experiment helps to generate an optimized design allowing
# better approximations of functions taking into account empty regions and gradient values.

# %%
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

# %%
# Create an initial i/o design of experiment as starting state of the algorithm
f1 = ot.SymbolicFunction(
    ["a0", "a1"],
    ["-4 * exp((-25 / 8) * (a0^2 + a1^2)) + 7 * exp((-125 / 4) * (a0^2 + a1^2))"],
)
distribution = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 2)
x0 = ot.LowDiscrepancyExperiment(ot.HaltonSequence(), distribution, 100).generate()
y0 = f1(x0)


# %%
# Plot the gradient: high at the center of the design
def pyGradNorm(X):
    grad = f1.gradient(X)
    grad.squareElements()
    s = grad.computeSumElements()
    return [s**0.5]


gradNorm = ot.PythonFunction(f1.getInputDimension(), 1, pyGradNorm)

# %%
# Plot the initial input sample
graph = ot.Graph("LOLA-Voronoi", "x1", "x2", True)
cloud1 = ot.Cloud(x0)
cloud1.setPointStyle("fcircle")
cloud1.setColor("blue")
graph.add(cloud1)
graph.add(
    gradNorm.draw(
        distribution.getRange().getLowerBound(), distribution.getRange().getUpperBound()
    )
)
otv.View(graph)

# %%
# Instantiate the algorithm from the initial DOE and the distribution
algo = otexp.LOLAVoronoi(x0, y0, distribution)

# %%
# Iteratively generate new samples
for i in range(5):
    x = algo.generate(5)
    y = f1(x)
    algo.update(x, y)

    # Update the graph
    cloud2 = ot.Cloud(x)
    cloud2.setPointStyle("fcircle")
    cloud2.setColor("red")
    graph.add(cloud2)
    otv.View(graph)

# %%
otv.View.ShowAll()
