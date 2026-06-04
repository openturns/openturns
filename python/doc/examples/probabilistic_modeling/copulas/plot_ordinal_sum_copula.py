"""
Create the ordinal sum of copulas
=================================
"""

# %%
# In this example we are going to create an ordinal sum of copulas.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Create a collection of copulas
collection = [ot.GumbelCopula(2), ot.NormalCopula(2)]

# %%
# Merge the copulas
bounds = [0.3]
copula = ot.OrdinalSumCopula(collection, bounds)
print(copula)

# %%
# Draw PDF
graph = copula.drawPDF([512] * 2)
graph.setXTitle("x")
graph.setYTitle("y")
graph.setLegendPosition("")
view = otv.View(graph)

# %%
# Display all figures
otv.View.ShowAll()
