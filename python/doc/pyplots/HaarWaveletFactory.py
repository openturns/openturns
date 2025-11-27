import openturns as ot
import openturns.viewer as otv

grid = ot.GridLayout(2, 4)
for i in range(8):
    fct = ot.HaarWaveletFactory().build(i)
    g = fct.draw(0.0, 1.0, 256)
    g.setLegends([r'$\varphi_' + str(i) + '$'])
    g.setXTitle('x')
    g.setYTitle(r'$\varphi_' + str(i) + '(x)$')
    g.setTitle('')
    grid.setGraph(i//4, i%4, g)

v = otv.View(grid)
fig = v.getFigure()
fig.axes[1].legend(loc="best")

