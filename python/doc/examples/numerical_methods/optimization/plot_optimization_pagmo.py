"""
Multi-objective optimization using Pagmo
========================================
"""
# %%
# In this example we are going to explore optimization using the `Pagmo <https://esa.github.io/pagmo2/>`_ solver.

# %%
import openturns as ot
from openturns.viewer import View

ot.Log.Show(ot.Log.NONE)
ot.RandomGenerator.SetSeed(0)

# %%
# List available algorithms
for algo in ot.Pagmo.GetAlgorithmNames():
    print(algo)

# %%
# More details on Pagmo algorithms are available `here <https://esa.github.io/pagmo2/overview.html#list-of-algorithms>`_ .

# %%
# Create the problem, from ZDT test suite
f = ot.SymbolicFunction(['x1', 'x2'], ['x1', 'var g := 1.0 + 9.0 * (x1 + x2); g * (1.0 - sqrt(x1 / g))'])
zdt1 = ot.OptimizationProblem(f)
zdt1.setBounds(ot.Interval([0.0] * 2, [1.0] * 2))

# %%
# We create the first generation of points by sampling into the bounding box
pop0 = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * 2).getSample(100)

# %%
# We create the algorithm that should evolve over 180 generations
algo = ot.Pagmo(zdt1, 'nsga2', pop0)
algo.setGenerationNumber(180)

# %%
# Benefit from parallel evaluations if the function allows it
algo.setBlockSize(8)

# %%
# We run the algo
algo.run()
pop1 = algo.getResult().getFinalPoints()

# %%
# We list the available Pareto fronts
fronts = algo.getResult().getParetoFrontsIndices()
len(fronts)

# %%
# We show the Pareto front
graph = ot.Graph("Pareto front", "X1", "X2", True, 'topright')
front = algo.getResult().getFinalPoints().select(fronts[0]).sortAccordingToAComponent(0)
data = ot.Sample(2*front.getSize()-1, 2)
for i in range(front.getSize()):
    data[2*i] = front[i]
    if i!= front.getSize()-1:
        data[2*i+1,0] = front[i,0]
        data[2*i+1,1] = front[i+1,1]
curve = ot.Curve(data)
curve.setColor('blue')
curve.setLegend(f"front {0}")
graph.add(curve)
graph.setGrid(True)
View(graph)

# %%
# We show the Pareto front from successive generations
fronts = []
for gen in range(5):
    algo = ot.Pagmo(zdt1, 'nsga2', pop0)
    algo.setGenerationNumber(gen)
    algo.run()
    front0 = algo.getResult().getParetoFrontsIndices()[0]
    fronts.append(algo.getResult().getFinalPoints().select(front0))
graph = ot.Graph("Successive fronts", "X1", "X2", True, 'topright')
palette = ot.Drawable.BuildDefaultPalette(len(fronts))
for k in range(len(fronts)):
    front = fronts[k].sortAccordingToAComponent(0)
    print(front)
    data = ot.Sample(2*front.getSize()-1, 2)
    for i in range(front.getSize()):
        data[2*i] = front[i]
        if i!= front.getSize()-1:
            data[2*i+1,0] = front[i,0]
            data[2*i+1,1] = front[i+1,1]
    curve = ot.Curve(data)
    curve.setColor(palette[k])
    curve.setLegend(f"generation {k}")
    graph.add(curve)
graph.setGrid(True)
View(graph)


View.ShowAll()
