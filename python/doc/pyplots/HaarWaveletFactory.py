import openturns as ot
import openturns.viewer as otv
import matplotlib.pyplot as plt

grid = ot.GridLayout(2, 4)
palette = ot.Drawable.BuildDefaultPalette(8)
for i in range(8):
    fct = ot.HaarWaveletFactory().build(i)
    g = fct.draw(0.0, 1.0, 256)
    g.setColors([palette[i]])
    g.setTitle(r"$x \rightarrow \varphi_" + str(i) + "(x)$")
    g.setLegendPosition("")
    g.setBoundingBox(ot.Interval([0.0, -2.0], [1.0, 2.0]))
    if i == 0:
        g.setTickLocation(ot.GraphImplementation.TICKY)
    if i == 4:
        g.setTickLocation(ot.GraphImplementation.TICKXY)
    g.setXTitle("x")
    if i >= 1 and i < 4:
        g.setTickLocation(ot.GraphImplementation.TICKNONE)
    if i >= 5:
        g.setTickLocation(ot.GraphImplementation.TICKX)
    g.setXTitle("x")
    g.setYTitle("")
    grid.setGraph(i // 4, i % 4, g)

v = otv.View(grid, figure_kw={"figsize": (12.0, 4.5)})
plt.subplots_adjust(wspace=0.3, hspace=0.7, right=0.8)
fig = v.getFigure()
