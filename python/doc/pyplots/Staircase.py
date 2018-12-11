import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

distribution = ot.Normal([0.5], [2.], ot.CorrelationMatrix(1))
sample = distribution.getSample(200)

# Construct empirical CDF for the sample
data = ot.Sample(20, 2)
cursor = ot.Point(2)
for i in range(20):
    cursor[0] = 13. * i / 20 - 6.5
    count = 0.
    for j in range(200):
        if(sample[j, 0] < cursor[0]):
            count += 1.
    cursor[1] = count / 200
    data[i] = cursor

# Create an empty graph
graph = ot.Graph("Staircase example", "x1", "x2", True, "topleft")

# Create the staircase
staircase = ot.Staircase(data, "blue", "solid", "s", "Normal CDF")

# Then, draw it
graph.add(staircase)

fig = plt.figure(figsize=(4.5, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)
