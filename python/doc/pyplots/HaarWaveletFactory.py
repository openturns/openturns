import openturns as ot
import openturns.viewer as otv
import matplotlib.pyplot as plt

grid = ot.GridLayout(2, 4)
for i in range(8):
    fct = ot.HaarWaveletFactory().build(i)
    g = fct.draw(0.0, 1.0, 256)
    g.setLegends([r'$\varphi_' + str(i) + '$'])
    g.setXTitle('x')
    g.setYTitle(r'$\varphi_' + str(i) + '(x)$')
    g.setTitle('')
    grid.setGraph(i // 4, i % 4, g)

v = otv.View(grid, figure_kw={"figsize": (8.0, 3.0)})
plt.subplots_adjust(right=0.8, bottom=0.2, wspace=0.3)
fig = v.getFigure()
fig.axes[1].legend(loc="best")
