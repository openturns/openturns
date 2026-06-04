import openturns as ot
import openturns.viewer as otv

analytical = ot.SymbolicFunction(["x"], ["2*x-8"])
levelset = ot.LevelSet(analytical, ot.Less(), 0.0)
f = ot.IndicatorFunction(levelset)

graph = f.draw(0.0, 10.0)
graph.setTitle(r"$y=\mathbb{1}_{2x-8<0}$")
otv.View(graph, figure_kw={"figsize": (8, 4)}, add_legend=True)
